cmake_minimum_required(VERSION 3.12)
include(CMakePrintHelpers)

project (ConnectFour)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH /opt/local/share/SFML/cmake/Modules)
set(_sfml_components graphics audio window system)
find_package(SFML REQUIRED COMPONENTS ${_sfml_components})
#cmake_print_variables( SFML_LIBRARIES )
foreach( _lib ${_sfml_components} )
    string( TOUPPER ${_lib} _LIB )
    add_library(sfml::${_lib} UNKNOWN IMPORTED)
    set_property(TARGET sfml::${_lib} PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES ${SFML_INCLUDE_DIR})
    set_property(TARGET sfml::${_lib} PROPERTY
        IMPORTED_LOCATION ${SFML_${_LIB}_LIBRARY})
#    cmake_print_variables( SFML_INCLUDE_DIR SFML_${_LIB}_LIBRARY )
endforeach()

#set(MACOSX_BUNDLE_BUNDLE_NAME ConnectFour)

set(SOURCES 
    include/alphabeta.hpp
    include/audio.h 
    include/connect4.h
    include/connect4.hpp
    include/frameratecontroller.h 
    include/game.h
    include/gameview.h 
    include/mcts.hpp
    include/arguments.hpp

    src/audio.cpp 
    src/connect4.cpp
    src/frameratecontroller.cpp 
    src/game.cpp 
    src/gameview.cpp 
    src/main.cpp
)

FILE(CREATE_LINK ${CMAKE_SOURCE_DIR}/res ${CMAKE_BINARY_DIR}/res SYMBOLIC)

add_executable(connect_four ${SOURCES})
#set_target_properties(connect_four PROPERTIES MACOSX_BUNDLE TRUE)

target_link_libraries(connect_four sfml::graphics sfml::audio sfml::window sfml::system)
target_include_directories( connect_four PRIVATE include )
