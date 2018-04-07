cmake_minimum_required(VERSION 2.8)
project(FireSim)

#Required Packages
find_package(OpenGL REQUIRED)

#GLFW
add_subdirectory (ext/glfw-3.2.1)
include_directories(
	ext/glfw-3.2.1/include/
	ext/glad/include/
	ext/glm/
	)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
set(OPENGL_LIBRARY
	${OPENGL_LIBRARY}
	-lGL -lGLU -lXrandr -lXext -lX11 -lrt
	${CMAKE_DL_LIBS}
	${GLFW_LIBRARIES}
)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")


#Add executable
add_executable(firesim
	src/main.cpp
	ext/glad/src/glad.c
)

target_link_libraries(firesim
	glfw
)

install (TARGETS firesim 
	DESTINATION ${CMAKE_INSTALL_PREFIX})