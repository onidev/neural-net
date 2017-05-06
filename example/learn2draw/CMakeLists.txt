cmake_minimum_required(VERSION 2.8)

project(project1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Wno-unused-parameter")

file(GLOB_RECURSE SOURCE_FILES src/*.hpp src/*.h src/*.inl src/*.cpp src/*.c)

add_executable(project1 ${SOURCE_FILES})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(OpenGL REQUIRED)
#find_package(OpenAL REQUIRED)
#find_package(Freetype REQUIRED)
#find_package(Threads REQUIRED)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

set(FOUND_INCLUDES
        #${FREETYPE_INCLUDE_DIRS}
        ${OPENGL_INCLUDE_DIR}
        #${OPENAL_INCLUDE_DIR}
        ${SDL2_INCLUDE_DIR}
        ${SDLIMAGE_INCLUDE_DIR}
)

set(FOUND_LIBS
        #${FREETYPE_LIBRARIES}
        ${OPENGL_gl_LIBRARY}
        ${OPENGL_glu_LIBRARY}
        #${OPENAL_LIBRARY}
        #${CMAKE_THREAD_LIBS_INIT}
        ${SDL2_LIBRARY}
        ${SDLIMAGE_LIBRARY}
)

#if(WIN32)
#    set(FOUND_LIBS ${FOUND_LIBS} ws2_32 wsock32 winmm gdi32 user32 kernel32 sdl2)
#endif()

include_directories(../../src)

include_directories(AFTER ${FOUND_INCLUDES})
target_link_libraries(project1 ${FOUND_LIBS})



