cmake_minimum_required(VERSION 3.22)
project(DirektDSP_GUI VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── JUCE integration ─────────────────────────────────────────────────────────
# juce::* targets (and examples/StandaloneTest) exist only when this points at a
# JUCE checkout (the directory that contains JUCE's top-level CMakeLists.txt).
#
# Default: the git submodule at ./JUCE (clone with: git submodule update --init).
# Override any time: cmake -B build -DJUCE_DIR=/other/juce
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/JUCE/CMakeLists.txt")
    set(_JUCE_DEFAULT "${CMAKE_CURRENT_SOURCE_DIR}/JUCE")
else()
    set(_JUCE_DEFAULT "")
endif()
set(JUCE_DIR "${_JUCE_DEFAULT}" CACHE PATH "Path to JUCE root — required to build examples/StandaloneTest")

if(JUCE_DIR AND EXISTS "${JUCE_DIR}/CMakeLists.txt")
    add_subdirectory("${JUCE_DIR}" "${CMAKE_BINARY_DIR}/JUCE" EXCLUDE_FROM_ALL)
elseif(JUCE_DIR)
    message(WARNING "JUCE_DIR=\"${JUCE_DIR}\" does not contain CMakeLists.txt — JUCE targets disabled.")
endif()

# ── Source files ──────────────────────────────────────────────────────────────
# GLOB_RECURSE must not include the JUCE submodule or examples — they are separate
# CMake projects; pulling them in would add thousands of .cpp files to consumers.
file(GLOB_RECURSE DirektGUI_Sources CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
list(FILTER DirektGUI_Sources EXCLUDE REGEX "/JUCE/")
list(FILTER DirektGUI_Sources EXCLUDE REGEX "/examples/")
list(FILTER DirektGUI_Sources EXCLUDE REGEX "/build/")

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

# ── Example: standalone component gallery ───────────────────────────────────
if(TARGET juce::juce_gui_basics)
    add_subdirectory(examples/StandaloneTest)
else()
    message(NOTICE "")
    message(NOTICE "  StandaloneTest example skipped (JUCE not loaded). If this repo has the JUCE submodule:")
    message(NOTICE "    git submodule update --init")
    message(NOTICE "  Then reconfigure, or set JUCE explicitly: cmake -B build -DJUCE_DIR=/path/to/JUCE")
    message(NOTICE "  Build: cmake --build build --target StandaloneTest")
    message(NOTICE "")
endif()
