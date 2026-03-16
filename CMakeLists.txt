add_library(DirektDSP_GUI INTERFACE)

file(GLOB_RECURSE DirektGUI_Sources CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

target_sources(DirektDSP_GUI INTERFACE ${DirektGUI_Sources})
target_include_directories(DirektDSP_GUI INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
