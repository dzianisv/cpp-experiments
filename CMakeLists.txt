project(signalfd)

add_executable(mainprocess main.cpp)
add_executable(subprocess subprocess.cpp)


set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(mainprocess PRIVATE Threads::Threads)
