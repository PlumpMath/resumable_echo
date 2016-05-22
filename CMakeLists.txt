cmake_minimum_required(VERSION 2.6)

PROJECT(resumable_echo_svr)

SET(BOOST_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS system REQUIRED)

SET(HEADER_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

SET(HEADERS
  ${HEADER_PATH}/echo_server.hpp
)

SET(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

INCLUDE_DIRECTORIES(
  ${HEADER_PATH}
  ${Boost_INCLUDE_DIRS}
)

LINK_DIRECTORIES(
  ${Boost_LIBRARY_DIRS}
)

add_executable(echo_server src/echo_server.cpp ${HEADERS})
add_executable(echo_client src/echo_client.cpp)
# TARGET_LINK_LIBRARIES(echo_server
#     ${Boost_LIBRARIES}
# )

add_definitions(
  -await 
  -Zi
  -sdl-
)