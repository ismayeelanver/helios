#ifndef CONFIG_H
#define CONFIG_H

#include <cstdint>
#include <string>
#include <toml++/toml.h>
#include <vector>

namespace WMConfig {

typedef struct Border {
  int width;      
  uint32_t color; 
  int radius;     
} Border;

typedef struct Window {
  int gap; 
} Window;

typedef enum class ActionType { run, ch, focus, close, toggle } ActionType;

typedef struct Action {
  std::string type;   
  std::string target; 
} Action;

typedef struct Keybind {
  uint16_t mod;    
  uint32_t keysym; 
  Action action;
} Keybind;

typedef struct General {
  std::vector<std::string> startup; 
  Border border;                    
  Window window;                    
  std::vector<Keybind> bindings;    
} General;

void debugConfig(const General &config);

} 

using Config = WMConfig::General;

Config loadConfig(const std::string &filename);

#endif 