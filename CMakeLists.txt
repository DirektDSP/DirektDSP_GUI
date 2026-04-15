cmake_minimum_required(VERSION 3.22)
project(DirektDSP_GUI VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── JUCE integration ─────────────────────────────────────────────────────────
# When JUCE_DIR is provided (e.g. by CI), add it as a subdirectory so that
# juce::* CMake targets become available.
if(DEFINED JUCE_DIR AND EXISTS "${JUCE_DIR}/CMakeLists.txt")
    add_subdirectory("${JUCE_DIR}" "${CMAKE_BINARY_DIR}/JUCE" EXCLUDE_FROM_ALL)
endif()

# ── Source files ──────────────────────────────────────────────────────────────
file(GLOB_RECURSE DirektGUI_Sources CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

# ── INTERFACE library (for embedding into plugins) ───────────────────────────
add_library(DirektDSP_GUI INTERFACE)
target_sources(DirektDSP_GUI INTERFACE ${DirektGUI_Sources})
target_include_directories(DirektDSP_GUI INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(DirektDSP_GUI INTERFACE cxx_std_17)

if(TARGET juce::juce_gui_basics)
    target_link_libraries(DirektDSP_GUI INTERFACE
        juce::juce_gui_basics
        juce::juce_audio_processors)
endif()

# ── Static-analysis helper target ────────────────────────────────────────────
# Compiles all translation units so that CMAKE_EXPORT_COMPILE_COMMANDS produces
# a populated compile_commands.json for clang-tidy.
add_library(DirektDSP_GUI_analysis OBJECT EXCLUDE_FROM_ALL)
target_sources(DirektDSP_GUI_analysis PRIVATE ${DirektGUI_Sources})
target_include_directories(DirektDSP_GUI_analysis PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(DirektDSP_GUI_analysis PRIVATE cxx_std_17)

if(TARGET juce::juce_gui_basics)
    target_link_libraries(DirektDSP_GUI_analysis PRIVATE
        juce::juce_gui_basics
        juce::juce_audio_processors)
endif()
