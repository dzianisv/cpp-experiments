cmake_minimum_required(VERSION 3.16)
project(signalfd)

add_executable(main_system main_system.cpp)
add_executable(main_spawn main_spawn.cpp)
add_executable(subprocess subprocess.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(main_system PRIVATE Threads::Threads)
target_link_libraries(main_spawn PRIVATE Threads::Threads)
set_property(TARGET main_spawn PROPERTY CXX_STANDARD 20)