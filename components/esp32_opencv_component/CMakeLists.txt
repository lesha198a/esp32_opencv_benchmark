set(deps esp_rom vfs lwip pthread newlib)

idf_component_register(
    SRCS dummy.c
    PRIV_REQUIRES ${deps}
)

include(ExternalProject)

set(ESP32_OPENCV_INSTALL_DIR ${CMAKE_BINARY_DIR}/esp32_opencv_install)

set(idf_include_directories $<TARGET_PROPERTY:idf::${COMPONENT_NAME},INCLUDE_DIRECTORIES>)
set(includes "-I$<JOIN:${idf_include_directories}, -I>")
set(extra_defines "-D_DEFAULT_SOURCE -DESP32")
set(c_flags "${includes} ${extra_defines}")
set(cxx_flags "${includes} ${extra_defines} -DCV_INT32_T_IS_LONG_INT=ON -Dalloca=__builtin_alloca")

set(assert_flags "-DNDEBUG")
set(assert_flags "${assert_flags} -mlongcalls -mauto-litpools")

set(opt_flags "-O3 ${common_flags} ${assert_flags}")
set(opt_args -DCMAKE_BUILD_TYPE=Release
            -DCMAKE_C_FLAGS_RELEASE=${opt_flags}
            -DCMAKE_CXX_FLAGS_RELEASE=${opt_flags})

set(opencv_libraries
    calib3d features2d flann
    imgcodecs imgproc core
)

set(3rdparty_libraries
    zlib 
    libpng
)

set(all_libraries)
set(all_targets)
foreach(libname ${opencv_libraries})
    set(lib_path ${ESP32_OPENCV_INSTALL_DIR}/lib/libopencv_${libname}.a)
    list(APPEND all_libraries ${lib_path})
    list(APPEND all_targets opencv_${libname})
    add_library(opencv_${libname} STATIC IMPORTED GLOBAL)
    set_target_properties(opencv_${libname} PROPERTIES IMPORTED_LOCATION ${lib_path})
    set_target_properties(opencv_${libname} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${ESP32_OPENCV_INSTALL_DIR}/include/opencv4)
endforeach()
foreach(libname ${3rdparty_libraries})
    set(lib_path ${ESP32_OPENCV_INSTALL_DIR}/lib/opencv4/3rdparty/lib${libname}.a)
    list(APPEND all_libraries ${lib_path})
    list(APPEND all_targets ${libname})
    add_library(${libname} STATIC IMPORTED GLOBAL)
    set_target_properties(${libname} PROPERTIES IMPORTED_LOCATION ${lib_path})
    # set_target_properties(opencv_${libname} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${ESP32_OPENCV_INSTALL_DIR}/include/opencv4)
endforeach()

ExternalProject_Add(esp32_opencv
    PREFIX                  ${CMAKE_BINARY_DIR}/esp32_opencv
    INSTALL_DIR             ${ESP32_OPENCV_INSTALL_DIR}
    GIT_REPOSITORY  U:/vscode_workspaces/vscode_espidf/esp32-opencv
    GIT_TAG         wip_component
    GIT_SHALLOW     1
    CMAKE_ARGS
        -DCMAKE_C_FLAGS=${c_flags}
        -DCMAKE_CXX_FLAGS=${cxx_flags}
        ${opt_args}
        -DBUILD_SHARED_LIBS=OFF
        -DCMAKE_INSTALL_PREFIX=${ESP32_OPENCV_INSTALL_DIR}
        -DENABLE_CCACHE=${CCACHE_ENABLE}
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
        -DPYTHON_DEFAULT_EXECUTABLE=${python}
        -DCV_CPU_BASELINE_MODE=${CONFIG_IDF_TARGET}
        # -DCV_DISABLE_OPTIMIZATION=0
        # -DOPENCV_FORCE_3RDPARTY_BUILD=1
        -DCMAKE_SYSTEM_PROCESSOR=${CONFIG_IDF_TARGET_ARCH}
        # -DENABLE_PIC=0
        -DTARGET=${CONFIG_IDF_TARGET}
        -DBUILD_opencv_world=OFF
        -DBUILD_opencv_videoio=OFF
        -DBUILD_LIST=core,imgproc,imgcodecs,calib3d
        -DESP32=ON
        -DWITH_IPP=OFF
        -DWITH_TBB=OFF
        -DWITH_CUDA=OFF
        -DWITH_OPENCL=OFF
        -DWITH_JPEG=OFF
        -DWITH_PNG=ON
        -DWITH_WEBP=OFF
        -DWITH_EIGEN=OFF
        -DWITH_GSTREAMER=OFF
        -DWITH_GTK=OFF
        -DWITH_OPENMP=OFF
        -DWITH_ITT=OFF
        -DWITH_PROTOBUF=OFF
        -DWITH_TIFF=OFF
        -DWITH_JASPER=OFF
        -DWITH_QUIRC=OFF
        -DBUILD_TESTS=OFF
        -DBUILD_PERF_TESTS=OFF
        -DBUILD_DOCS=OFF
        -DWITH_ADE=OFF
        -DBUILD_ZLIB=ON
        -DBUILD_PNG=ON
        -DBUILD_TIFF=OFF
        -DBUILD_JASPER=OFF
        -DBUILD_QUIRC=OFF

    USES_TERMINAL_DOWNLOAD TRUE
    USES_TERMINAL_CONFIGURE TRUE
    USES_TERMINAL_BUILD TRUE
    BUILD_IN_SOURCE FALSE
    BUILD_ALWAYS TRUE

    BUILD_BYPRODUCTS ${all_libraries}
)

set_target_properties(${COMPONENT_LIB} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${ESP32_OPENCV_INSTALL_DIR}/include/opencv4)

add_dependencies(${COMPONENT_LIB} esp32_opencv)

set(IMGCODEC_DEP libpng zlib)
target_link_libraries(opencv_imgcodecs INTERFACE ${IMGCODEC_DEP})

set(CORE_DEP zlib)
target_link_libraries(opencv_core INTERFACE ${CORE_DEP})

set(OPENCV_DEP
        opencv_calib3d opencv_features2d opencv_flann
        opencv_imgcodecs opencv_imgproc opencv_core )

target_link_libraries(${COMPONENT_LIB} INTERFACE ${OPENCV_DEP})
