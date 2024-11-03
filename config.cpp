#include "include/config.hpp"
/**
 * The function `loadConfig` reads a TOML configuration file and extracts
 * specific settings related to startup applications, border settings, window
 * settings, and keybindings into a structured data object.
 *
 * @param filename The `filename` parameter is a `std::string` type that
 * represents the name or path of the TOML configuration file that needs to be
 * loaded and parsed in the `loadConfig` function.
 *
 * @return The function `loadConfig` is returning an object of type
 * `WMConfig::General`, which contains various configurations loaded from a TOML
 * file.
 */
Config loadConfig(const std::string &filename) {
  // Load the TOML file
  toml::table config;
  try {
    config = toml::parse_file(filename);
  } catch (const std::exception &e) {
    std::cerr << "Error loading config file: " << e.what() << std::endl;
    throw;
  }

  WMConfig::General generalConfig;

  // Load startup applications
  if (config.contains("general") && config["general"].is_table()) {
    auto generalTable = config["general"].as_table();

    if (generalTable->contains("startup") &&
        generalTable->get("startup")->is_array()) {
      for (const auto &startup : *generalTable->get("startup")->as_array()) {
        generalConfig.startup.push_back(startup.as_string()->value_or(""));
      }
    }

    // Load border settings
    if (generalTable->contains("border") &&
        generalTable->get("border")->is_table()) {
      auto borderTable = generalTable->get("border")->as_table();
      generalConfig.border.width = borderTable->get("width")->value_or(1);
      generalConfig.border.color =
          borderTable->get("color")->value_or(0xFFFFFF);
      generalConfig.border.radius = borderTable->get("radius")->value_or(0);
    }

    // Load window settings
    if (generalTable->contains("window") &&
        generalTable->get("window")->is_table()) {
      auto windowTable = generalTable->get("window")->as_table();
      generalConfig.window.gap = windowTable->get("gap")->value_or(0);
    }

    // Load keybindings
    if (generalTable->contains("bindings") &&
        generalTable->get("bindings")->is_array()) {
      for (const auto &bind : *generalTable->get("bindings")->as_array()) {
        WMConfig::Keybind binding;
        binding.mod = bind.as_table()->get("mod")->as_string()->value_or("");
        binding.keysym =
            bind.as_table()->get("keysym")->as_string()->value_or("");

        // Load action
        if (bind.as_table()->contains("action")) {
          auto actionTable = bind.as_table()->get("action")->as_table();
          binding.action.type =
              actionTable->get("type")->as_string()->value_or("");
          binding.action.target =
              actionTable->get("target")->as_string()->value_or("");
        }

        generalConfig.bindings.push_back(binding);
      }
    }
  }

  return generalConfig;
}

/**
 * The debugConfig function outputs the configuration details of a window
 * manager to the console.
 *
 * @param config The `debugConfig` function is used to print out the
 * configuration settings stored in the `config` object of type
 * `WMConfig::General`.
 */
void WMConfig::debugConfig(const WMConfig::General &config) {
  std::cout << "Startup applications:\n";
  for (const auto &app : config.startup) {
    std::cout << " - " << app << '\n';
  }

  std::cout << "Border settings:\n"
            << " Width: " << config.border.width << '\n'
            << " Color: " << std::hex << config.border.color << std::dec << '\n'
            << " Radius: " << config.border.radius << '\n';

  std::cout << "Window settings:\n"
            << " Gap: " << config.window.gap << '\n';

  std::cout << "Keybindings:\n";
  for (const auto &bind : config.bindings) {
    std::cout << " - Mod: " << bind.mod << ", Keysym: " << bind.keysym
              << ", Action Type: " << bind.action.type
              << ", Target: " << bind.action.target << '\n';
  }
}