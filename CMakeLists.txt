cmake_minimum_required(VERSION 3.22)
project(DirektDSP_GUI VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── JUCE integration ─────────────────────────────────────────────────────────
# When embedded in a plugin repo, the parent must load JUCE once (see
# cmake-local/DirektDSPGUI.cmake in PluginTemplate / BandGate). Do not rely on
# the optional JUCE submodule in this folder for consumer builds.
#
# Standalone development of DirektDSP_GUI only:
#   cmake -B build -DJUCE_DIR=/path/to/JUCE
# or initialize the optional submodule:
#   git submodule update --init JUCE
if(TARGET juce::juce_gui_basics)
    message(STATUS "DirektDSP_GUI: using JUCE from parent project")
elseif(JUCE_DIR AND EXISTS "${JUCE_DIR}/CMakeLists.txt")
    if(NOT TARGET juce::juce_core)
        add_subdirectory("${JUCE_DIR}" "${CMAKE_BINARY_DIR}/DirektDSP_GUI_JUCE" EXCLUDE_FROM_ALL)
    endif()
    message(STATUS "DirektDSP_GUI: using JUCE from JUCE_DIR=${JUCE_DIR}")
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/JUCE/CMakeLists.txt")
    if(NOT TARGET juce::juce_core)
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/JUCE"
                           "${CMAKE_BINARY_DIR}/DirektDSP_GUI_JUCE" EXCLUDE_FROM_ALL)
    endif()
    message(STATUS "DirektDSP_GUI: using optional bundled JUCE submodule")
else()
    message(WARNING
        "DirektDSP_GUI: JUCE not found. GUI library headers still export; "
        "StandaloneTest and analysis targets that need JUCE are skipped.")
endif()

# ── Source files ──────────────────────────────────────────────────────────────
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
    message(NOTICE "  StandaloneTest skipped (no JUCE). Parent plugin builds are unaffected.")
    message(NOTICE "  For local GUI dev: cmake -B build -DJUCE_DIR=/path/to/JUCE")
    message(NOTICE "")
endif()
