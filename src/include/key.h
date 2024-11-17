#include <cstdlib>
#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xproto.h>

#include <iostream>

/**
 * @brief Grabs a key so that it will not be passed to other X clients.
 * 
 * @details
 * This function will grab a key so that it will not be passed to other X clients.
 * It will use the XCB library to do this. The function takes four parameters: a
 * pointer to an XCB connection, a modifier mask, a keysym, and a window ID. The
 * function will return nothing.
 * 
 * The first thing the function does is to allocate a key symbols structure. This
 * structure will be used to look up the keycode associated with the given keysym.
 * The function does this by calling xcb_key_symbols_alloc(). The function then
 * looks up the keycode associated with the given keysym by calling
 * xcb_key_symbols_get_keycode(). If this call fails, the function will print an
 * error message and exit with a status of EXIT_FAILURE.
 * 
 * If the call succeeds, the function will then call xcb_grab_key() to grab the
 * key. The function passes 1 as the first parameter to xcb_grab_key() to specify
 * that the key should be grabbed for the given window. The second parameter is
 * the window ID, the third is the modifier mask, the fourth is the keycode, the
 * fifth is the grab mode, and the sixth is the grab mode. The function then calls
 * xcb_flush() to flush the XCB connection.
 * 
 * Finally, the function frees the key symbols structure by calling
 * xcb_key_symbols_free().
 * 
 * @param conn The XCB connection to use.
 * @param mod The modifier mask to use.
 * @param keysym The keysym to use.
 * @param window The window ID to use.
 * @exception exit(EXIT_FAILURE) if the system is unable to find keycode for a given keysym
 */
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
