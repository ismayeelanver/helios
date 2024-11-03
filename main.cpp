#include "include/helios.h"
#include <spdlog/sinks/basic_file_sink.h>
int main() {
    auto logger = spdlog::basic_logger_mt("Log", "logs.txt");
    logger->info("Started!");
    WindowManager* WM = new WindowManager;
    WM->run();

    return 0;
}