# GUI gallery executable for DirektDSP_GUI. Expects the parent project to have added
# JUCE (juce::juce_gui_extra) and the DirektDSP_GUI interface library.

if(NOT TARGET juce::juce_gui_basics)
    message (STATUS "StandaloneTest: skipped (JUCE gui_basics target not found)")
    return()
endif()

if(NOT TARGET DirektDSP_GUI)
    message (FATAL_ERROR "StandaloneTest: DirektDSP_GUI target missing — configure from DirektDSP_GUI/CMakeLists.txt")
endif()

juce_add_gui_app (StandaloneTest
    VERSION 1.0.0
    COMPANY_NAME DirektDSP
    PRODUCT_NAME "DirektDSP StandaloneTest"
    BUNDLE_ID com.direktdsp.standalonetest
)

target_sources (StandaloneTest PRIVATE
    Main.cpp
    StandaloneTestProcessor.cpp
)

target_include_directories (StandaloneTest PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries (StandaloneTest PRIVATE
    DirektDSP_GUI
    juce::juce_gui_basics
    juce::juce_audio_processors
)

# juce_audio_processors pulls in juce_gui_extra, whose Linux sources need GTK3 + WebKit
# (same dependencies JUCE's top-level CMake probes for).
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package (PkgConfig REQUIRED)
    pkg_check_modules (GTK3 REQUIRED IMPORTED_TARGET gtk+-3.0)
    pkg_check_modules (WEBKIT2GTK REQUIRED IMPORTED_TARGET webkit2gtk-4.1)
    pkg_check_modules (LIBCURL REQUIRED IMPORTED_TARGET libcurl)
    target_link_libraries (StandaloneTest PRIVATE PkgConfig::GTK3 PkgConfig::WEBKIT2GTK PkgConfig::LIBCURL)
endif()

target_compile_features (StandaloneTest PRIVATE cxx_std_17)
