#ifndef HELIOS_H
#define HELIOS_H

#include <cerrno>
#include <cstdint>
#include <memory>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <vector>
#include <xcb/xcb.h>
#include <xcb/xcb_cursor.h>
#include <xcb/xcb_ewmh.h>
#include <xcb/xproto.h>
#include <xcb/shape.h>

#include <X11/keysym.h>

#include "config.h"
#include "key.h"
#include "spawn.h"

class WindowManager {
private:
  xcb_connection_t *conn;
  xcb_screen_t *screen;
  xcb_window_t root;
  std::vector<xcb_window_t> windows;
  Config config;
  std::shared_ptr<spdlog::logger> logger =
      spdlog::basic_logger_mt("Helios", "logs.txt");
  int current_window;
  xcb_cursor_t cursor;
  xcb_cursor_context_t *cursor_context;
  uint32_t event_mask =
      XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |
      XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_BUTTON_PRESS |
      XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION |
      XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_ENTER_WINDOW;
  std::unique_ptr<uint32_t[]> values;
  std::vector<xcb_atom_t> supported_atoms = {};
  std::unique_ptr<xcb_atom_t[]> atoms;

  xcb_ewmh_connection_t ewmh;

public:
  WindowManager();
  ~WindowManager();
  void run();

private:
  void handle_map_request(xcb_window_t window);
  void handle_unmap_request(xcb_window_t window);
  void handle_destroy_notify(xcb_window_t window);
  void handle_enter_notify(xcb_window_t window);
  void handle_key_press(xcb_key_press_event_t* key_press);
  void set_window_border_color(xcb_window_t window, uint32_t color);
  void set_focus(xcb_window_t window);
  void switch_workspace(uint32_t i);
  void tile_windows();
};

#endif