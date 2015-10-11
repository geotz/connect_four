cmake_minimum_required(VERSION 3.3)

#set(MACOSX_BUNDLE_BUNDLE_NAME ConnectFour)

include_directories(./include)

set(SOURCES include/alphabeta.h include/audio.h include/connect4.h include/frameratecontroller.h include/game.h include/gameview.h include/mcts.h
src/audio.cpp src/frameratecontroller.cpp src/game.cpp src/gameview.cpp src/main.cpp)

add_definitions(-std=c++14)

project (ConnectFour)

add_executable(connect_four ${SOURCES})
#set_target_properties(connect_four PROPERTIES MACOSX_BUNDLE TRUE)

target_link_libraries(connect_four sfml-graphics sfml-audio sfml-window sfml-system)
