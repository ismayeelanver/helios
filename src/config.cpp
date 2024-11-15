#include "include/config.h"
#include <cstdint>
#include <iostream>

Config loadConfig(const std::string &filename) {
    toml::table config;
    try {
        config = toml::parse_file(filename);
    } catch (const std::exception &e) {
        std::cerr << "Error loading config file: " << e.what() << std::endl;
        throw;
    }

    WMConfig::General generalConfig;

    if (auto generalTable = config["general"].as_table()) {

        if (auto startupArray = generalTable->get("startup")->as_array()) {
            for (const auto &startup : *startupArray) {
                generalConfig.startup.push_back(startup.value_or(""));
            }
        }

        if (auto borderTable = generalTable->get("border")->as_table()) {
            generalConfig.border.width = borderTable->get("width")->value_or(1);
            generalConfig.border.color = borderTable->get("color")->value_or(0xFFFFFF);
            generalConfig.border.radius = borderTable->get("radius")->value_or(0);
        }

        if (auto windowTable = generalTable->get("window")->as_table()) {
            generalConfig.window.gap = windowTable->get("gap")->value_or(0);
        }

        if (auto bindingsArray = generalTable->get("bindings")->as_array()) {
            for (const auto &bind : *bindingsArray) {
                WMConfig::Keybind binding;
                if (auto bindTable = bind.as_table()) {
                    binding.mod = static_cast<uint16_t>(bindTable->get("mod")->value<uint64_t>().value_or(0));
                    binding.keysym = static_cast<uint32_t>(bindTable->get("keysym")->value<uint64_t>().value_or(0));

                    if (auto actionTable = bindTable->get("action")->as_table()) {
                        binding.action.type = actionTable->get("type")->value_or("");
                        binding.action.target = actionTable->get("target")->value_or("");
                    }
                }
                generalConfig.bindings.push_back(binding);
            }
        }
    }

    return generalConfig;
}

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