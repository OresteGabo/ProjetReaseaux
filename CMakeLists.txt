cmake_minimum_required(VERSION 3.16)
project(osm_viewer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Add this to help find Qt
set(CMAKE_PREFIX_PATH "C:/Qt/6.8.0/msvc2022_64") # Replace with your Qt path

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Network
        REQUIRED)

add_executable(osm_viewer main.cpp
        geojsonviewer.cpp
        geojsonviewer.h
        GeoJSONControls.cpp
        GeoJSONControls.h
)
target_link_libraries(osm_viewer
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Network)

# Add this to automatically copy DLLs (Windows only)
if(WIN32)
    add_custom_command(TARGET osm_viewer POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:Qt6::Core>
            $<TARGET_FILE:Qt6::Gui>
            $<TARGET_FILE:Qt6::Widgets>
            $<TARGET_FILE:Qt6::Network>
            $<TARGET_FILE_DIR:osm_viewer>)
endif()