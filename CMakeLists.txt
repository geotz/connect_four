cmake_minimum_required(VERSION 3.3)

#set(MACOSX_BUNDLE_BUNDLE_NAME ConnectFour)

aux_source_directory(. SRC_LIST)

add_definitions(-std=c++14)

project (ConnectFour)

add_executable(connect_four ${SRC_LIST})
#set_target_properties(connect_four PROPERTIES MACOSX_BUNDLE TRUE)

target_link_libraries(connect_four sfml-graphics sfml-audio sfml-window sfml-system)
