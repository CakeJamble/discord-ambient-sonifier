cmake_minimum_required(VERSION 3.22)
project(ambient_bot LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(JUCE_BUILD_EXAMPLES OFF)
set(JUCE_BUILD_EXTRAS OFF)
set(JUCE_BUILD_EXTRAS OFF CACHE BOOL "" FORCE)

add_compile_definitions(
    JUCE_HEADLESS_APPLICATION=1
)

# ================= JUCE =================
add_subdirectory(external/JUCE)

# Disable unused JUCE features (important for servers)
add_compile_definitions(
    JUCE_USE_CURL=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CAMERA=0
    JUCE_USE_CDREADER=0
    JUCE_USE_MEDIAFOUNDATION=0
    JUCE_USE_WINDOWS_MEDIA_FORMAT=0
)

# ================= Executable =================
add_executable(ambient_bot
    src/main.cpp
    src/AmbientPadSynth.cpp
)

target_include_directories(ambient_bot PRIVATE src)

# ================= JUCE modules =================
target_link_libraries(ambient_bot
    PRIVATE
        juce_core
        juce_audio_basics
        juce_audio_formats
        juce_audio_processors
        juce_dsp
)

# ================= Optimizations =================
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if (MSVC)
    target_compile_options(ambient_bot PRIVATE /O2)
else()
    target_compile_options(ambient_bot PRIVATE -O3)
endif()
