# Install script for directory: /home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/x86_64-w64-mingw32")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/x86_64-w64-mingw32-objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libassimp5.2.4-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/lib/libassimp.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/anim.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/aabb.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/ai_assert.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/camera.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/color4.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/color4.inl"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/build-mingw/ext/assimp-5.2.5/code/../include/assimp/config.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/ColladaMetaData.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/commonMetaData.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/defs.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/cfileio.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/light.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/material.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/material.inl"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/matrix3x3.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/matrix3x3.inl"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/matrix4x4.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/matrix4x4.inl"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/mesh.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/ObjMaterial.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/pbrmaterial.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/GltfMaterial.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/postprocess.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/quaternion.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/quaternion.inl"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/scene.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/metadata.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/texture.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/types.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/vector2.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/vector2.inl"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/vector3.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/vector3.inl"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/version.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/cimport.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/importerdesc.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/Importer.hpp"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/DefaultLogger.hpp"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/ProgressHandler.hpp"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/IOStream.hpp"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/IOSystem.hpp"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/Logger.hpp"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/LogStream.hpp"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/NullLogger.hpp"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/cexport.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/Exporter.hpp"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/DefaultIOStream.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/DefaultIOSystem.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/ZipArchiveIOSystem.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/SceneCombiner.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/fast_atof.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/qnan.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/BaseImporter.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/Hash.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/MemoryIOWrapper.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/ParsingUtils.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/StreamReader.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/StreamWriter.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/StringComparison.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/StringUtils.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/SGSpatialSort.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/GenericProperty.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/SpatialSort.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/SkeletonMeshBuilder.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/SmallVector.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/SmoothingGroups.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/SmoothingGroups.inl"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/StandardShapes.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/RemoveComments.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/Subdivision.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/Vertex.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/LineSplitter.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/TinyFormatter.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/Profiler.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/LogAux.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/Bitmap.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/XMLTools.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/IOStreamBuffer.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/CreateAnimMesh.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/XmlParser.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/BlobIOSystem.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/MathFunctions.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/Exceptional.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/ByteSwapper.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/Base64.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/Compiler/pushpack1.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/Compiler/poppack1.h"
    "/home/kumaizep/文件/NTHU/Homework/9/GP/FinalProject/indoorScene-dev/ext/assimp-5.2.5/code/../include/assimp/Compiler/pstdint.h"
    )
endif()

