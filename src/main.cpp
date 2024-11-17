#include "include/helios.h"
#include <spdlog/sinks/basic_file_sink.h>
/**
 * @brief The main entry point of the application.
 *
 * Sets up a logger and starts the WindowManager.
 *
 * @return 0 on success.
 */
int main() {
  auto logger = spdlog::basic_logger_mt("Log", "logs.txt");
  logger->info("Stopped!");
  WindowManager* WM = new WindowManager();
  WM->run();

  return 0;
}