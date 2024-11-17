#include "include/helios.h"
#include <sys/types.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

/**
 * @brief Construct a new WindowManager object
 *
 * This function initializes a window manager, setting up the necessary
 * XCB connections, loading the configuration, and preparing the window
 * manager for use.
 *
 * @throw std::runtime_error if unable to connect to the X server
 * @throw std::runtime_error if unable to initialize ewmh cookie for connection
 * @throw std::runtime_error if unable to initialize ewmh connection with cookie
 * @throw std::runtime_error if cursor context initialization fails
 * @throw srd::runtime_error if cursor context creation fails
 */
WindowManager::WindowManager()
    : values(std::make_unique<uint32_t[]>(1)), atoms(nullptr) {
  logger->flush_on(spdlog::level::info);
  if (!(conn = xcb_connect(nullptr, nullptr))) {
    logger->error("Could not connect to the X server");
    throw std::runtime_error("X server connection failed");
  }

  config = loadConfig("config.toml");

  const xcb_setup_t *setup = xcb_get_setup(conn);

  if (xcb_connection_has_error(conn)) {
    logger->error(
        "The Connection has an error and other stuff could not be proceeded");
    exit(EXIT_FAILURE);
  }

  xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
  if (!(screen = iter.data)) {
    logger->error("Unable to access screen information. Ensure the X server is "
                  "running and is accessible");
  }

  xcb_flush(conn);
  root = screen->root;

  xcb_intern_atom_cookie_t *ewmh_cookie = new xcb_intern_atom_cookie_t;

  ewmh_cookie = xcb_ewmh_init_atoms(conn, &ewmh);
  if (!ewmh_cookie) {
    logger->error("EWMH cookie initialization failed");
    throw std::runtime_error("EWMH cookie initialization failed");
  }

  xcb_generic_error_t *error = nullptr;
  if (!xcb_ewmh_init_atoms_replies(&ewmh, ewmh_cookie, &error)) {
    if (error) {
      logger->error("EWMH initialization failed: {}", error->major_code);
      free(error);
    } else {
      logger->error(
          "EWMH initialization failed with no error details available");
    }
    throw std::runtime_error("EWMH connection initialization failed");
  }

  supported_atoms = {ewmh._NET_SUPPORTED,
                     ewmh._NET_SUPPORTING_WM_CHECK,
                     ewmh._NET_ACTIVE_WINDOW,
                     ewmh._NET_CLIENT_LIST,
                     ewmh._NET_CURRENT_DESKTOP,
                     ewmh._NET_DESKTOP_NAMES,
                     ewmh._NET_NUMBER_OF_DESKTOPS,
                     ewmh._NET_WM_NAME,
                     ewmh._NET_WM_STATE,
                     ewmh._NET_WM_STATE_FULLSCREEN,
                     ewmh._NET_WM_WINDOW_TYPE,
                     ewmh._NET_WM_WINDOW_TYPE_DIALOG,
                     ewmh._NET_WM_WINDOW_TYPE_DOCK,
                     ewmh._NET_WM_WINDOW_TYPE_DESKTOP};

  atoms = std::make_unique<xcb_atom_t[]>(supported_atoms.size());

  std::copy(supported_atoms.begin(), supported_atoms.end(), atoms.get());

  auto window = xcb_generate_id(conn);

  xcb_create_window(conn, XCB_WINDOW_CLASS_COPY_FROM_PARENT, window, root, 0, 0,
                    1, 1, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual,
                    {}, {});

  xcb_ewmh_set_supported(&ewmh, 0, supported_atoms.size(), atoms.get());
  xcb_ewmh_set_supporting_wm_check(&ewmh, root, window);
  xcb_ewmh_set_wm_name(&ewmh, root, strlen(WM_NAME), "" WM_NAME);
  xcb_ewmh_set_supporting_wm_check(&ewmh, root, root);
  xcb_ewmh_set_number_of_desktops(&ewmh, 0, 9);
  xcb_ewmh_set_current_desktop(&ewmh, 0, 1);
  xcb_ewmh_set_active_window(&ewmh, 0, root);

  windows = {};

  for (auto kbd : config.bindings) {
    grab_key(conn, kbd.mod, kbd.keysym, root);
  }

  for (auto wrkspce = 0; wrkspce < 10; ++wrkspce) {
    grab_key(conn, XK_Super_L, XK_0 + wrkspce, root);
  }

  *values.get() = event_mask;
  xcb_void_cookie_t event_mask_cookie =
      xcb_change_window_attributes(conn, root, XCB_CW_EVENT_MASK, values.get());
  xcb_generic_error_t *em_error = xcb_request_check(conn, event_mask_cookie);
  if (!em_error) {
    logger->info("Request Completed Successfully");
  } else {
    auto code = em_error->error_code;
    logger->error("Error in Request: Code {}", code);
  }
  xcb_flush(conn);

  WMConfig::debugConfig(config);

  for (std::string &program : config.startup) {
    spawn(program.c_str());
  }

  if (xcb_cursor_context_new(conn, screen, &cursor_context) != 0) {
    logger->error("Unable to create cursor context");
    throw std::runtime_error("Cursor context creation failed");
  }

  cursor = xcb_cursor_load_cursor(cursor_context, "left_ptr");
  if (cursor == XCB_CURSOR_NONE) {
    logger->error("Failed to load cursor");
    xcb_cursor_context_free(cursor_context);
    throw std::runtime_error("Unable to create cursor context");
  }

  xcb_void_cookie_t cursor_cookie =
      xcb_change_window_attributes(conn, root, XCB_CW_CURSOR, &cursor);
  xcb_generic_error_t *c_error = xcb_request_check(conn, cursor_cookie);
  if (!c_error) {
    logger->info("Request Completed Successfully");
  } else {
    auto code = c_error->error_code;
    logger->error("Error in Request: Code {}", code);
  }
  xcb_flush(conn);

  logger->info("WM initialized, ready to go!");
}

/**
 * @brief  Tile all windows in the list in a non-overlapping manner across the
 * screen. The tiling algorithm is as follows:
 *
 * 1. Split the screen in half horizontally.
 * 2. Place the first window in the top half of the screen.
 * 3. For each subsequent window:
 *    a. Find the window with the largest area.
 *    b. Split that window in half, either horizontally or vertically,
 *       depending on the previous split direction.
 *    c. Place the new window in the newly created space.
 *    d. Switch the split direction.
 */
void WindowManager::tile_windows() {
  int num_windows = windows.size();
  if (num_windows == 0)
    return;

  int screen_width = screen->width_in_pixels;
  int screen_height = screen->height_in_pixels;
  int gap = config.window.gap;
  int border_width = config.border.width;

  if (num_windows == 1) {
    uint32_t values[4] = {static_cast<uint32_t>(gap),
                          static_cast<uint32_t>(gap),
                          static_cast<uint32_t>(screen_width - 2 * gap),
                          static_cast<uint32_t>(screen_height - 2 * gap)};

    uint32_t border_values[1] = {static_cast<uint32_t>(border_width)};
    xcb_configure_window(conn, windows[0], XCB_CONFIG_WINDOW_BORDER_WIDTH,
                         border_values);
    xcb_configure_window(conn, windows[0],
                         XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y |
                             XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT,
                         values);
    xcb_set_input_focus(conn, XCB_INPUT_FOCUS_POINTER_ROOT, windows[0],
                        XCB_CURRENT_TIME);
    xcb_flush(conn);
    return;
  }

  struct WindowRect {
    int x, y, width, height;
  };
  std::vector<WindowRect> window_rects(num_windows);

  window_rects[0] = {gap, gap, screen_width - 2 * gap, screen_height - 2 * gap};

  bool split_horizontal = false;
  for (int i = 1; i < num_windows; i++) {

    int largest_idx = i - 1;
    int largest_area = 0;
    for (int j = 0; j < i; j++) {
      int area = window_rects[j].width * window_rects[j].height;
      if (area > largest_area) {
        largest_area = area;
        largest_idx = j;
      }
    }

    WindowRect &to_split = window_rects[largest_idx];
    WindowRect &new_window = window_rects[i];

    if (split_horizontal) {

      int new_height = (to_split.height - gap) / 2;
      new_window = {to_split.x, to_split.y + new_height + gap, to_split.width,
                    to_split.height - new_height - gap};
      to_split.height = new_height;
    } else {

      int new_width = (to_split.width - gap) / 2;
      new_window = {to_split.x + new_width + gap, to_split.y,
                    to_split.width - new_width - gap, to_split.height};
      to_split.width = new_width;
    }

    split_horizontal = !split_horizontal;
  }

  for (int i = 0; i < num_windows; i++) {
    const auto &rect = window_rects[i];
    uint32_t values[4] = {
        static_cast<uint32_t>(rect.x), static_cast<uint32_t>(rect.y),
        static_cast<uint32_t>(rect.width), static_cast<uint32_t>(rect.height)};

    uint32_t border_values[1] = {static_cast<uint32_t>(border_width)};
    xcb_configure_window(conn, windows[i], XCB_CONFIG_WINDOW_BORDER_WIDTH,
                         border_values);
    xcb_configure_window(conn, windows[i],
                         XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y |
                             XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT,
                         values);
    xcb_set_input_focus(conn, XCB_INPUT_FOCUS_POINTER_ROOT, windows[i],
                        XCB_CURRENT_TIME);
  }

  xcb_flush(conn);
}

/**
 * Sets the border color of a window.
 *
 * @param window The X window for which to set the border color.
 * @param color The color to set the border to, in 32-bit ARGB format.
 */
void WindowManager::set_window_border_color(xcb_window_t window,
                                            uint32_t color) {
  xcb_change_window_attributes(conn, window, XCB_CW_BORDER_PIXEL, &color);
  xcb_flush(conn);
}

/**
 * Sets the focus to a given window.
 *
 * @param window The window to set the focus to.
 */
void WindowManager::set_focus(xcb_window_t window) {
  xcb_set_input_focus(conn, XCB_INPUT_FOCUS_POINTER_ROOT, window,
                      XCB_CURRENT_TIME);
  update_focus(window);
  xcb_flush(conn);
}

void WindowManager::update_focus(xcb_window_t window) {
  if (current_window != XCB_NONE && current_window != window) {
    set_window_border_color(current_window, config.border.inactive_color);
  }

  if (window != XCB_NONE) {
    current_window = window;
    set_window_border_color(window, config.border.active_color);
    xcb_set_input_focus(conn, XCB_INPUT_FOCUS_POINTER_ROOT, window,
                        XCB_CURRENT_TIME);
  }
}

/**
 * Switches to the workspace with the given index.
 *
 * @param i The index of the workspace to switch to.
 */
void WindowManager::switch_workspace(uint32_t i) {
  uint32_t data[] = {i};
  xcb_ewmh_set_current_desktop(&ewmh, 0, data[0]);
  xcb_flush(conn);
}

/**
 * @brief Handles an EnterNotify event.
 *
 * @param window The window which generated the EnterNotify event.
 *
 */
void WindowManager::handle_enter_notify(xcb_generic_event_t *ev) {
  auto event = (xcb_enter_notify_event_t *)ev;
  auto window = event->event;
  if (window == XCB_WINDOW_NONE)
    return;

  if (std::find(windows.begin(), windows.end(), window) == windows.end())
    return;

  if (current_window != window) {
    update_focus(window);
    xcb_flush(conn);
  }
}

/**
 * Handles a KeyPress event by switching to the workspace that corresponds
 * to the number key that was pressed (0-9). If the key press is not a number
 * key with the Mod4 modifier, the function does nothing.
 *
 * @param key_press The KeyPress event to handle.
 */
void WindowManager::handle_key_press(xcb_generic_event_t *ev) {
  auto key_press = (xcb_key_press_event_t *)ev;
  for (int i = 0; i <= 9; ++i) {
    if (key_press->state == XK_Alt_L && key_press->detail == XK_0 + i) {
      switch_workspace(i);
      return;
    }
  }
}

/**
 * Handles a MapRequest event by adding the window to the list of windows,
 * mapping the window, setting the border width and color, setting the input
 * focus to the window, and re-tiling the windows.
 *
 * @param window The window to handle.
 */
void WindowManager::handle_map_request(xcb_generic_event_t *ev) {
  auto event = (xcb_map_request_event_t *)ev;
  auto window = event->window;
  windows.push_back(window);

  auto cookie = xcb_get_window_attributes(conn, window);
  xcb_generic_error_t **e = nullptr;
  auto _attr_reply = xcb_get_window_attributes_reply(conn, cookie, e);

  if (_attr_reply->override_redirect) {
    delete _attr_reply;
    delete e;
    return;
  }

  uint32_t values[] = {XCB_EVENT_MASK_ENTER_WINDOW |
                       XCB_EVENT_MASK_FOCUS_CHANGE |
                       XCB_EVENT_MASK_PROPERTY_CHANGE};
  xcb_change_window_attributes(conn, window, XCB_CW_EVENT_MASK, values);

  xcb_map_window(conn, window);

  auto border_width = static_cast<uint32_t>(config.border.width);
  uint32_t border_values[2] = {border_width, border_width};

  xcb_configure_window(conn, window, XCB_CONFIG_WINDOW_BORDER_WIDTH,
                       border_values);

  set_window_border_color(window, config.border.inactive_color);

  update_focus(window);

  xcb_flush(conn);
  tile_windows();

  delete _attr_reply;
  delete e;
}

/**
 * Handles a DestroyNotify event by destroying the window, removing it from
 * the list of windows, re-tiling the windows, and setting the input focus to
 * the last window in the list.
 *
 * @param window The window to handle.
 */
void WindowManager::handle_destroy_notify(xcb_generic_event_t *ev) {
  auto event = (xcb_destroy_notify_event_t *)ev;
  auto window = event->window;

  if (window == current_window) {
    current_window = XCB_NONE;
  }

  xcb_destroy_window(conn, window);
  auto new_end = std::remove(windows.begin(), windows.end(), window);
  windows.erase(new_end, windows.end());

  if (!windows.empty()) {
    update_focus(windows.back());
  }

  xcb_flush(conn);
  tile_windows();
}

/**
 * Handles an UnmapNotify event by un-mapping the window, setting the input
 * focus to the last window in the list, and re-tiling the windows.
 *
 * @param window The window to handle.
 */
void WindowManager::handle_unmap_request(xcb_generic_event_t *ev) {
  auto event = (xcb_unmap_notify_event_t *)ev;
  auto window = event->window;

  if (window == current_window) {
    current_window = XCB_NONE;
  }
  xcb_unmap_window(conn, window);

  if (!windows.empty()) {
    update_focus(windows.back());
  }

  xcb_flush(conn);
  tile_windows();
}

/**
 * Destructor for the WindowManager class.
 *
 * Cleans up and releases all resources associated with the window manager.
 * This includes clearing supported atoms, destroying all managed windows,
 * freeing the cursor, destroying the root window, disconnecting from the
 * X server, and freeing the cursor context if it exists. Logs a message
 * indicating that the window manager has stopped.
 */
WindowManager::~WindowManager() {
  supported_atoms.clear();
  for (auto wdow : windows) {
    xcb_destroy_window(conn, wdow);
  }
  windows.clear();
  xcb_free_cursor(conn, cursor);
  xcb_destroy_window(conn, root);
  xcb_disconnect(conn);

  if (cursor_context) {
    xcb_cursor_context_free(cursor_context);
  }
  logger->info("WM stopped");
}

/**
 * The main loop of the window manager.
 *
 * Waits for events and handles them accordingly. Events are handled in the
 * following order:
 *  1. MapRequest - Maps the window and adds it to the list of managed windows.
 *  2. UnmapNotify - Unmaps the window and removes it from the list of managed
 *     windows.
 *  3. DestroyNotify - Destroys the window and removes it from the list of
 *     managed windows.
 *  4. EnterNotify - Sets the input focus to the window that the pointer is
 *     currently over.
 *  5. KeyPress - Switches to the specified workspace when a number key with
 *     the Mod4 modifier is pressed.
 *
 * If the event is not one of the above, the window manager will log an error
 * message and break out of the main loop.
 *
 * The window manager will also log an error message if the event is invalid.
 */
void WindowManager::run() {
  for (;;) {
    xcb_generic_event_t *event = xcb_wait_for_event(conn);

    if (!event) {
      logger->error("Event is invalid");
      break;
    }

    auto ev = event->response_type & ~0x80;
    auto handler = evH.find(ev);
    if (handler != evH.end()) {
      handler->second((xcb_generic_event_t *)event);
    } else {
      logger->error("Invalid Event Type!");
    }
    delete event;
  }
}