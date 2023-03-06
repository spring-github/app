cmake_minimum_required(VERSION 3.0.0)
project(FireApp VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(testApp
            test.cpp
            common/Timer.cpp
            )
target_link_libraries(testApp pthread rt)

add_executable(FireApp 
                main.cpp
                Fire.cpp
                common/Timer.cpp
                )
target_link_libraries(FireApp pthread rt)

# add_executable(FireApp2 main2.cpp Fire.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
