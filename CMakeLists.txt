cmake_minimum_required(VERSION 3.20)

project(MatchingEngine)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3 -fsanitize=undefined")

SET(sources 
	MatchingEngineMain.cpp
	MatchingEngine.cpp
	Order.h
	OrderBook.cpp
	OrderParser.cpp
)

add_executable(${PROJECT_NAME} ${sources})
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-fsanitize=undefined")
