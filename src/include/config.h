#ifndef CONFIG_H
#define CONFIG_H

#include <cstdint> // For uint32_t, which is the type that xcb uses for window IDs.
#include <string> // For std::string, which is used for the command and target of an action.
#include <toml++/toml.h> // For toml::table, which is used to parse the config file.
#include <vector> // For std::vector, which is used to store the startup programs, border settings, window settings, and keybindings.
/**
 * @brief The namespace which holds all the structures and functions neccessary to generate a config for the WM.
 * 
 */
namespace WMConfig {

/**
 * @brief This struct represents the border settings. It contains the width of
 * the border, the color of the border, and the radius of the border.
 */
typedef struct Border {
  int width;
  uint32_t active_color;
  uint32_t inactive_color;
  int radius;
} Border;

/**
 * This struct represents the window settings. It contains the gap
 * between windows, in pixels.
 */
typedef struct Window {
  int gap; // The gap between windows, in pixels.
} Window;
/**
 * @breif This enum represents the type of action that can be performed when a
 * key is pressed. It can be one of the following:
 *   - run: Run a command.
 *   - ch: Change the currently focused window to the window with the specified
 *   target.
 *   - focus: Focus the window with the specified target.
 *   - close: Close the currently focused window.
 *   - toggle: Toggle the visibility of the window with the specified target.
 */
typedef enum class ActionType { run, ch, focus, close, toggle } ActionType;
/**
 * @brief This struct represents an action that can be performed when a key is
 * pressed. It contains the type of the action and the target of the action.
 */
typedef struct Action {
  std::string type;   // The type of the action, as a string. It can be "run",
                      // "ch", "focus", "close", or "toggle".
  std::string target; // The target of the action, as a string. It can be a
                      // window ID, a class name, or an instance name.
} Action;
/**
 * @brief This struct represents a keybinding. It contains the modifier keys
 * that must be pressed, the keysym that must be pressed, and the action that
 * will be performed when the key is pressed.
 */
typedef struct Keybind {
  uint16_t mod; // The modifier keys that must be pressed, as a bitfield. It can
                // be one of the following:
                //   - XCB_MOD_MASK_SHIFT
                //   - XCB_MOD_MASK_LOCK
                //   - XCB_MOD_MASK_CONTROL
                //   - XCB_MOD_MASK_1
                //   - XCB_MOD_MASK_2
                //   - XCB_MOD_MASK_3
                //   - XCB_MOD_MASK_4
                //   - XCB_MOD_MASK_5
                //   - XCB_MOD_MASK_ANY
  uint32_t keysym; // The keysym that must be pressed, as a guint32.
  Action action;   // The action that will be performed when the key is pressed.
} Keybind;

/**
* @brief This struct represents the general configuration of the window manager. It
* contains the startup programs, border settings, window settings, and
* keybindings.
*/
typedef struct General {
  std::vector<std::string>
      startup;   // The startup programs, as a vector of strings. Each string is
                 // the path to an executable.
  Border border; // The border settings, as a Border struct.
  Window window; // The window settings, as a Window struct.
  std::vector<Keybind>
      bindings; // The keybindings, as a vector of Keybind structs.
} General;

/**
 * @brief Function for showing the config
 * 
 * @param config the given config to be shown
 */
void debugConfig(const General &config);

} // namespace WMConfig

/**
 * @brief alias Cofig for WMConfig::General
 * 
 */
using Config = WMConfig::General;
/**
 * @brief This function gets the filename parses a new config from the file and set the parsed code in a structure and returns the structure
 * 
 * @param filename The specified filename
 * @return Config The structure of the config
 */
Config loadConfig(const std::string &filename);

#endif
