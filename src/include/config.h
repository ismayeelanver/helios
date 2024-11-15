#ifndef CONFIG_H
#define CONFIG_H

#include <cstdint>
#include <string>
#include <toml++/toml.h>
#include <vector>

namespace WMConfig {

// Structure to define border properties of windows
typedef struct Border {
  int width;      // Width of the border in pixels
  uint32_t color; // Color of the border in hexadecimal format
  int radius;     // Radius of border corners in pixels
} Border;

// Structure to define window properties
typedef struct Window {
  int gap; // Gap between windows in pixels
} Window;

// Enum for different types of actions that can be bound to keys
typedef enum class ActionType { run, ch, focus, close, toggle } ActionType;

// Structure to define an action associated with a keybind
typedef struct Action {
  std::string type;   // Type of action (using enum for type safety)
  std::string target; // Target for the action (e.g., program name or workspace)
} Action;

// Structure to define a keybind
typedef struct Keybind {
  uint16_t mod;    
  uint32_t keysym; 
  Action action;
} Keybind;

// Structure to encapsulate general configuration settings
typedef struct General {
  std::vector<std::string> startup; // Applications to start on launch
  Border border;                    // Border configuration
  Window window;                    // Window configuration
  std::vector<Keybind> bindings;    // List of keybindings
} General;

// Function to print debug information about the configuration
void debugConfig(const General &config);

} // namespace WMConfig

// Alias for the General configuration structure
using Config = WMConfig::General;

// Function prototype to load configuration from a file
Config loadConfig(const std::string &filename);

#endif // CONFIG_HPP
