cmake_minimum_required(VERSION 3.22.1)

project("test-runner")

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_POSITION_INDEPENDENT_CODE on)

# install path
set(CMAKE_BUILD_WITH_INSTALL_RPATH on)
set(CMAKE_INSTALL_RPATH "$ORIGIN")

add_subdirectory(Catch2-3.4.0)

# main executable
add_executable(main main.cpp)
target_link_libraries(main Catch2::Catch2)

install(TARGETS 
    main
    DESTINATION .)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_LIST_DIR}/dist)
