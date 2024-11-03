#include <iostream>
#include <cstdint>
#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xproto.h>
inline void grab_key(xcb_connection_t* conn, uint32_t keysym, uint32_t modifiers,
              xcb_window_t window) {
  xcb_key_symbols_t *symbols = xcb_key_symbols_alloc(conn);
  xcb_keycode_t *keycode = xcb_key_symbols_get_keycode(symbols, keysym);

  if (!keycode) {
    std::cerr << "Keycode not found for the specified keysym." << std::endl;
    return;
  }

  xcb_void_cookie_t status = xcb_grab_key(conn, 1, window, modifiers, *keycode,
                            XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC);
    xcb_generic_error_t* error = xcb_request_check(conn, status);
  if (!error) {
    std::cerr << "Failed to grab key: " << status.sequence << std::endl;
  } else {
    std::cout << "Key grabbed successfully." << std::endl;
  }

  xcb_key_symbols_free(symbols);
}