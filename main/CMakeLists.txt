idf_component_register(
        SRCS
        main.cpp
        system.cpp
        report.cpp
        INCLUDE_DIRS
        .
        ./include
        REQUIRES esp32_opencv_component spiffs esp_timer
)
