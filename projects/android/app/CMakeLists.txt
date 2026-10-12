cmake_minimum_required(VERSION 3.10)
project(cuicui_app)

# your C++ files: every .cpp in app/ (add others here if needed)
file(GLOB APP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../../../app/*.cpp")

# the library name must stay "native-lib" (loaded by CuicuiActivity)
add_library(native-lib SHARED ${APP_SOURCES})

# cuicui core
include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cuicui_core/cuicui.cmake)
cuicui_add_sources(native-lib)

# modules, e.g.:
# target_sources(native-lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../modules/cuicui_biometricAuth/cuicui_biometricAuth.cpp)

# 16 KB page alignment (required to target Android 15+)
target_link_options(native-lib PRIVATE "-Wl,-z,max-page-size=16384" "-Wl,-z,common-page-size=16384")
