#include "include/helios.hpp"

WindowManager::WindowManager()
    : values(std::make_unique<uint32_t[]>(1)),
      atoms(std::make_unique<xcb_atom_t[]>(supported_atoms.size())) {
  logger->flush_on(spdlog::level::info);
  if (!(conn = xcb_connect(nullptr, nullptr))) {
    logger->error("Could not connect to the X server");
    throw std::runtime_error("X server connection failed");
  }

  const xcb_setup_t *setup = xcb_get_setup(conn);
  xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
  if (!(screen = iter.data)) {
    logger->error("Unable to access screen information. Ensure the X server is "
                  "running and is accessible");
  }

  xcb_flush(conn);
  root = screen->root; // Ensure root is assigned

  xcb_intern_atom_cookie_t *ewmh_cookie = new xcb_intern_atom_cookie_t;

  ewmh_cookie = xcb_ewmh_init_atoms(conn, &ewmh);
  if (!ewmh_cookie) { // Check if the cookie itself is valid
    logger->error("EWMH cookie initialization failed");
    throw std::runtime_error("EWMH cookie initialization failed");
  }

  xcb_generic_error_t *error = nullptr;
  if (!xcb_ewmh_init_atoms_replies(&ewmh, ewmh_cookie, &error)) {
    if (error) {
      logger->error("EWMH initialization failed: {}", error->major_code);
      free(error); // Free the error structure if it's allocated
    } else {
      logger->error(
          "EWMH initialization failed with no error details available");
    }
    throw std::runtime_error("EWMH connection initialization failed");
  }

  supported_atoms = {
      ewmh._NET_SUPPORTED,          ewmh._NET_SUPPORTING_WM_CHECK,
      ewmh._NET_ACTIVE_WINDOW,      ewmh._NET_CLIENT_LIST,
      ewmh._NET_CURRENT_DESKTOP,    ewmh._NET_DESKTOP_NAMES,
      ewmh._NET_NUMBER_OF_DESKTOPS, ewmh._NET_WM_NAME,
      ewmh._NET_WM_STATE,           ewmh._NET_WM_STATE_FULLSCREEN,
      ewmh._NET_WM_WINDOW_TYPE,     ewmh._NET_WM_WINDOW_TYPE_DIALOG};

  std::copy(supported_atoms.begin(), supported_atoms.end(), atoms.get());

  xcb_ewmh_set_supported(&ewmh, 0, supported_atoms.size(), atoms.get());
  xcb_ewmh_set_supporting_wm_check(&ewmh, root, root);
  xcb_ewmh_set_wm_name(&ewmh, root, strlen("helios"), "helios");

  windows = {};
  current_window = 0;

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
  logger->info("WM initialized, ready to go!");
}

void WindowManager::run() {
  for (;;) {
    xcb_generic_event_t *event = xcb_wait_for_event(conn);

    if (!event) {
      logger->error("Event is invalid");
      break;
    }

    switch (event->response_type & ~0x80) {
    case XCB_MAP_REQUEST: {
      auto *map_request = (xcb_map_request_event_t *)event;
      handle_map_request(map_request->window);
      break;
    }
    case XCB_UNMAP_NOTIFY: {
      auto *unmap_notify = (xcb_unmap_notify_event_t *)event;
      handle_unmap_request(unmap_notify->window);
      break;
    }
    }

    free(event);
  }
}

void WindowManager::tile_windows() {
  int num_windows = windows.size();
  if (num_windows == 0)
    return;

  int width = screen->width_in_pixels;
  int height = screen->height_in_pixels /
               num_windows; // Divide height equally among windows

  for (int i = 0; i < num_windows; ++i) {
    uint32_t values[4];
    values[0] = 0;          // x position (left edge of the screen)
    values[1] = i * height; // y position, staggered by window index
    values[2] = width;      // full width of the screen
    values[3] = height;     // calculated height for each window

    xcb_configure_window(conn, windows[i],
                         XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y |
                             XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT,
                         values);
  }

  xcb_flush(conn); // Apply all changes
}

void WindowManager::set_focus(xcb_window_t window) {
  xcb_set_input_focus(conn, XCB_INPUT_FOCUS_POINTER_ROOT, window,
                      XCB_CURRENT_TIME);
  xcb_flush(conn);
}

void WindowManager::handle_map_request(xcb_window_t window) {
  windows.push_back(window);
  xcb_map_window(conn, window);

  xcb_set_input_focus(conn, XCB_INPUT_FOCUS_POINTER_ROOT, window,
                      XCB_CURRENT_TIME);
  xcb_flush(conn);
  tile_windows();
}

void WindowManager::handle_unmap_request(xcb_window_t window) {
  windows.erase(std::remove(windows.begin(), windows.end(), window),
                windows.end());
  if (!windows.empty()) {
    // Select the next window to focus
    auto next_window =
        windows.back(); // Example: focus the last window in the list

    // Focus the next window (you would call your focus function here)
    set_focus(next_window);
  } else {
    // If no windows are left, set focus to root or reset it
    set_focus(root);
  }
  tile_windows();
}

WindowManager::~WindowManager() {
  supported_atoms.clear();
  windows.clear();
  xcb_free_cursor(conn, cursor);
  xcb_destroy_window(conn, root);
  xcb_disconnect(conn);

  if (cursor_context) {
    xcb_cursor_context_free(cursor_context); // only free if initialized
  }
  logger->info("WM stopped");
}