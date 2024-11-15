#include <cstdlib>
#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xproto.h>

#include <iostream>

inline void grab_key(xcb_connection_t *conn, uint16_t mod, uint32_t keysym,
                     xcb_window_t window) {
  auto *keysyms = xcb_key_symbols_alloc(conn);
  auto *keycode = xcb_key_symbols_get_keycode(keysyms, keysym);

  if (keycode) {
    xcb_grab_key(conn, 1, window, mod, *keycode, XCB_GRAB_MODE_ASYNC,
                 XCB_GRAB_MODE_ASYNC);
    xcb_flush(conn);
    free(keycode);
  } else {
    std::cerr << "Unable to find keycode for the given keysym." << std::endl;
    exit(EXIT_FAILURE);
  }

  xcb_key_symbols_free(keysyms);
}
