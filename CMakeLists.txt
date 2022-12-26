cmake_minimum_required(VERSION 3.24)

set(PROJECT_NAME FinalProjectIndoor)
project(${PROJECT_NAME})

# set(CMAKE_CXX_COMPILER "g++")
# set(CMAKE_CXX_COMPILER /usr/bin/c++)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "Build type not set - using RelWithDebInfo")
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose build type: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

if (CMAKE_SYSTEM_NAME MATCHES "Linux")  
    message(STATUS "current platform: Linux ")  
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")  
    message(STATUS "current platform: Windows")    
else ()  
    message(STATUS "other platform: ${CMAKE_SYSTEM_NAME}")  
endif ()

if (CMAKE_BUILD_TYPE MATCHES "Release")
    set(CMAKE_BUILD_POSTFIX "${CMAKE_RELEASE_POSTFIX}")
elseif (CMAKE_BUILD_TYPE MATCHES "Debug")
    set(CMAKE_BUILD_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
else ()
    set(CMAKE_BUILD_POSTFIX "")
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

aux_source_directory(src SRC_LIST)
aux_source_directory(ext/glad GLAD_SRC_LIST)
aux_source_directory(ext/imgui IMGUI_SRC_LIST)
set(IMGUI_SRC 
    ext/imgui/backends/imgui_impl_glfw.cpp 
    ext/imgui/backends/imgui_impl_opengl3.cpp
)
add_executable(${PROJECT_NAME} ${SRC_LIST} ${GLAD_SRC_LIST} ${IMGUI_SRC_LIST} ${IMGUI_SRC})

include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/ext")
include_directories("${PROJECT_SOURCE_DIR}/ext/glad")
include_directories("${PROJECT_SOURCE_DIR}/ext/imgui")
include_directories("${PROJECT_SOURCE_DIR}/ext/imgui/backends")

add_custom_target(copy_assets1
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
)
add_dependencies(${PROJECT_NAME} copy_assets1)
add_custom_target(copy_assets2
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)
add_dependencies(${PROJECT_NAME} copy_assets2)


if(NOT CMAKE_SYSTEM_NAME MATCHES Linux)
    set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()
if ( WIN32 )
    if(NOT CMAKE_BUILD_TYPE)
            set(CMAKE_BUILD_TYPE "Release" CACHE STRING
                "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel"
                FORCE)
    endif()
    message("Windows build start")
    set(CMAKE_GENERATOR "Visual Studio 15 2017")
endif()


set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(${PROJECT_SOURCE_DIR}/ext/glfw-3.3.8)
target_link_libraries(${PROJECT_NAME} glfw)

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} OpenGL::GL)


set(ASSIMP_BUILD_ZLIB ON)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL OFF)
set(BUILD_SHARED_LIBS OFF)

add_subdirectory(${PROJECT_SOURCE_DIR}/ext/assimp-5.2.5)
target_link_libraries(${PROJECT_NAME} assimp)
set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=${ASSIMP_LIBRARY_OUTPUT_DIRECTORY}/$<IF:$<CONFIG:Debug>,Debug,Release>")
# find_package(assimp REQUIRED)
# target_link_libraries(${PROJECT_NAME} assimp)


if (MSVC)
    # add_definitions("_CRT_SECURE_NO_WARNINGS")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj /MTd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /bigobj /MT")
elseif(WIN32)
    # link_directories(${PROJECT_NAME}/lib)
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    # set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++ -lwsock32 -lws2_32 ${CMAKE_CXX_STANDARD_LIBRARIES}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static -lssp -Wl,-Bstatic -lstdc++ -Wl,-Bdynamic")
else()
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()
set(CMAKE_CXX_STANDARD 20)
