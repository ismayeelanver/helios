#include "include/config.h"
#include <cstdint>
#include <iostream>

/**
 * Load the configuration from a file and return it as a General struct.
 *
 * @param filename The name of the file to load the configuration from.
 * @return The configuration loaded from the file.
 */
Config loadConfig(const std::string &filename) {
    toml::table config;
    try {
        // Attempt to parse the config file, and store the result in a toml::table
        config = toml::parse_file(filename);
    } catch (const std::exception &e) {
        // If the file can't be parsed, print an error and throw the exception
        std::cerr << "Error loading config file: " << e.what() << std::endl;
        throw;
    }

    // Create an empty GeneralConfig struct to store the loaded config
    WMConfig::General generalConfig;

    // Attempt to get the "general" table from the config, and store it in a variable
    if (auto generalTable = config["general"].as_table()) {

        // Attempt to get the "startup" array from the general table, and load it into a vector
        if (auto startupArray = generalTable->get("startup")->as_array()) {
            for (const auto &startup : *startupArray) {
                // Add each startup application to the startup vector
                generalConfig.startup.push_back(startup.value_or(""));
            }
        }

        // Attempt to get the "border" table from the general table, and store it in a variable
        if (auto borderTable = generalTable->get("border")->as_table()) {
            // Load the border settings from the table
            generalConfig.border.width = borderTable->get("width")->value_or(1);
            generalConfig.border.active_color = borderTable->get("active_color")->value_or(0xFFFFFF);
            generalConfig.border.inactive_color = borderTable->get("inactive_color")->value_or(0xFFFFFF);
            generalConfig.border.radius = borderTable->get("radius")->value_or(0);
        }

        // Attempt to get the "window" table from the general table, and store it in a variable
        if (auto windowTable = generalTable->get("window")->as_table()) {
            // Load the window gap from the table
            generalConfig.window.gap = windowTable->get("gap")->value_or(0);
        }

        // Attempt to get the "bindings" array from the general table, and load it into a vector
        if (auto bindingsArray = generalTable->get("bindings")->as_array()) {
            for (const auto &bind : *bindingsArray) {
                // Create a new Keybind struct and load each binding from the array
                WMConfig::Keybind binding;
                if (auto bindTable = bind.as_table()) {
                    // Load the modifier and keysym from the table
                    binding.mod = static_cast<uint16_t>(bindTable->get("mod")->value<uint64_t>().value_or(0));
                    binding.keysym = static_cast<uint32_t>(bindTable->get("keysym")->value<uint64_t>().value_or(0));

                    // Attempt to get the "action" table from the binding table, and store it in a variable
                    if (auto actionTable = bindTable->get("action")->as_table()) {
                        // Load the action type and target from the table
                        binding.action.type = actionTable->get("type")->value_or("");
                        binding.action.target = actionTable->get("target")->value_or("");
                    }
                }
                // Add the binding to the bindings vector
                generalConfig.bindings.push_back(binding);
            }
        }
    }

    // Return the loaded config
    return generalConfig;
}

/**
 * Print the configuration to the console.
 *
 * @param config The configuration to print.
 */
void WMConfig::debugConfig(const WMConfig::General &config) {
    // Print the startup applications
    std::cout << "Startup applications:\n";
    for (const auto &app : config.startup) {
        std::cout << " - " << app << '\n';
    }

    // Print the border settings
    std::cout << "Border settings:\n"
              << " Width: " << config.border.width << '\n'
              << "Active Color: " << std::hex << config.border.active_color << std::dec << '\n'
              << "Inactive Color: " << std::hex << config.border.inactive_color << std::dec << '\n'
              << " Radius: " << config.border.radius << '\n';

    // Print the window gap
    std::cout << "Window settings:\n"
              << " Gap: " << config.window.gap << '\n';

    // Print the keybindings
    std::cout << "Keybindings:\n";
    for (const auto &bind : config.bindings) {
        std::cout << " - Mod: " << bind.mod << ", Keysym: " << bind.keysym
                  << ", Action Type: " << bind.action.type
                  << ", Target: " << bind.action.target << '\n';
    }
}
