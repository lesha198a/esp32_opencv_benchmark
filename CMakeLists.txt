# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp32_opencv_calib3d)

spiffs_create_partition_image(storage data FLASH_IN_PROJECT)