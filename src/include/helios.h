#ifndef HELIOS_H
#define HELIOS_H

#include <cerrno>
#include <cstdint>
#include <memory>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <vector>
#include <xcb/shape.h>
#include <xcb/xcb.h>
#include <xcb/xcb_cursor.h>
#include <xcb/xcb_ewmh.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xproto.h>

#include <X11/keysym.h>

#include "../wm.def.h"
#include "config.h"
#include "key.h"
#include "spawn.h"

/**
 * @class WindowManager
 *
 * This class implements a window manager. It is responsible for managing the
 * windows on the screen and handling events such as key presses.
 */
class WindowManager {
private:
  /**
   * @brief The XCB connection to the X server.
   *
   * This is the connection that the window manager uses to communicate with
   * the X server.
   */
  xcb_connection_t *conn;

  /**
   * @brief The screen structure for the screen that the window manager is
   *        managing.
   *
   * This structure contains information about the screen such as its width and
   * height.
   */
  xcb_screen_t *screen;

  /**
   * @brief The root window of the X server.
   *
   * The root window is the parent window of all other windows on the X server.
   */
  xcb_window_t root;

  /**
   * @brief A vector of window IDs that the window manager is currently
   *        managing.
   *
   * This vector contains the IDs of all windows that the window manager has
   * been asked to manage. The window manager will receive events for these
   * windows and will be able to control them.
   */
  std::vector<xcb_window_t> windows;

  /**
   * @brief The configuration for the window manager.
   *
   * This configuration contains information such as the colors to use for the
   * window manager and the fonts to use.
   */
  Config config;

  /**
   * @brief The logger for the window manager.
   *
   * The logger is a mechanism for logging messages. The window manager uses
   * the logger to log messages about what it is doing.
   */
  std::shared_ptr<spdlog::logger> logger =
      spdlog::basic_logger_mt("Helios", "logs.txt");

  /**
   * @brief The window that currently has focus.
   *
   * This is the window in the windows vector that currently has
   * focus.
   */
  xcb_window_t current_window = XCB_NONE;

  /**
   * @brief The cursor ID for the window manager's cursor.
   *
   * The cursor ID is an ID that is used to identify the window manager's
   * cursor.
   */
  xcb_cursor_t cursor;

  /**
   * @brief The context for the window manager's cursor.
   *
   * The context is a structure that contains information about the cursor such
   * as its shape and its color.
   */
  xcb_cursor_context_t *cursor_context;

  /**
   * @brief The event mask for the window manager.
   *
   * The event mask is a bit mask that specifies which events the window
   * manager should receive. The window manager will receive events for all
   * windows that have this mask.
   */
  const uint32_t event_mask =
      XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
      XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_ENTER_WINDOW |
      XCB_EVENT_MASK_LEAVE_WINDOW | XCB_EVENT_MASK_STRUCTURE_NOTIFY |
      XCB_EVENT_MASK_PROPERTY_CHANGE | XCB_EVENT_MASK_BUTTON_PRESS |
      XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_FOCUS_CHANGE |
      XCB_EVENT_MASK_KEY_PRESS;

  /**
   * @brief A unique pointer to an array of uint32_t values.
   *
   * This array is used to store values that are used by the window manager.
   */
  std::unique_ptr<uint32_t[]> values;

  /**
   * @brief A vector of supported atoms for the window manager.
   *
   * This vector contains the atoms that the window manager supports.
   */
  std::vector<xcb_atom_t> supported_atoms = {};

  /**
   * @brief A unique pointer to an array of xcb_atom_t values.
   *
   * This array is used to store the atoms that the window manager supports.
   */
  std::unique_ptr<xcb_atom_t[]> atoms;

  /**
   * @brief The EWMH connection for the window manager.
   *
   * The EWMH connection is a structure that contains information about the
   * window manager's connection to the X server.
   */
  xcb_ewmh_connection_t ewmh;

public:
  /**
   * @brief Constructs a new WindowManager instance.
   *
   * @details
   * This constructor does not take any parameters. It initializes the
   * window manager by connecting to the X server, setting up the event mask,
   * and setting up the window manager's logger.
   */
  WindowManager();

  /**
   * @brief Destructs the WindowManager instance.
   *
   * @details
   * This destructor is called when the WindowManager instance is no longer
   * needed. It is responsible for cleaning up after the window manager, which
   * includes disconnecting from the X server and freeing any allocated memory.
   */
  ~WindowManager();

  /**
   * @brief Starts the window manager.
   *
   * @details
   * This function is called when the window manager is started. It sets up the
   * event loop, which is the main loop of the window manager. This loop will
   * continue to run until the window manager is stopped.
   */
  void run();

private:
  /**
   @breif The function that updates the focus

   The `void update_focus(xcb_window_t window);` function in the `WindowManager`
   class is responsible for updating the focus to the specified window. When
   this function is called with a window ID as a parameter, it updates the focus
   of the window manager to that window. This means that the specified window
   will become the active window that receives input events such as key presses
   and mouse clicks.
   */
  void update_focus(xcb_window_t window);

  /**
   * @brief Handles a map request event for the given window.
   *
   * @param event The event to be handled.
   */
  void handle_map_request(xcb_map_request_event_t *event);

  /**
   * @brief Handles an unmap request event for the given window.
   *
   * @param event The event to be handled.
   */
  void handle_unmap_request(xcb_unmap_notify_event_t *event);

  /**
   * @brief Handles a destroy notify event for the given window.
   *
   * @param event The event to be handled.
   */
  void handle_destroy_notify(xcb_destroy_notify_event_t *event);

  /**
   * @brief Handles an enter notify event for the given window.
   *
   * @param event The event to be handled.
   */
  void handle_enter_notify(xcb_window_t window);

  /**
   * @brief Handles a key press event for the given key press event.
   *
   * @param event The event to be handled.
   */
  void handle_key_press(xcb_key_press_event_t *event);

  /**
   * @brief Sets the border color of the window to the given color.
   *
   * @param window The window whose border color to set.
   * @param color The color to set the border color to.
   */
  void set_window_border_color(xcb_window_t window, uint32_t color);

  /**
   * @brief Sets the focus of the window manager to the given window.
   *
   * @param window The window whose focus to set.
   */
  void set_focus(xcb_window_t window);

  /**
   * @brief Switches the workspace of the window manager to the given workspace.
   *
   * @param i The workspace number to switch to.
   */
  void switch_workspace(uint32_t i);

  /**
   * @brief Tiles all windows in the current workspace.
   */
  void tile_windows();
};

#endif