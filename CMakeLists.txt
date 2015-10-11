cmake_minimum_required(VERSION 3.3)

add_definitions(-std=c++14)

project (ConnectFour)

add_executable(connect_four main.cpp audio.cpp game.cpp frameratecontroller.cpp gameview.cpp audio.h game.h frameratecontroller.h gameview.h alphabeta.h connect4.h mcts.h)

target_link_libraries(connect_four sfml-graphics sfml-audio sfml-window sfml-system)
