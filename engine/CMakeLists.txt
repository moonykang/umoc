### Begin of third party libraries
# GLFW
SET(GLFW_PATH external/glfw)
SET(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
SET(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
SET(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
INCLUDE_DIRECTORIES(${GLFW_PATH}/include)
SET(DIRECTORIES_TO_INCLUDE ${DIRECTORIES_TO_INCLUDE} ${CMAKE_CURRENT_SOURCE_DIR}/${GLFW_PATH}/include)
ADD_SUBDIRECTORY(${GLFW_PATH})
SET(THIRD_PARTY_LIBS glfw)

# imgui
SET(IMGUI_PATH external/imgui)
INCLUDE_DIRECTORIES(${IMGUI_PATH})
INCLUDE_DIRECTORIES(${IMGUI_PATH}/backends)
SET(DIRECTORIES_TO_INCLUDE ${DIRECTORIES_TO_INCLUDE} ${CMAKE_CURRENT_SOURCE_DIR}/${IMGUI_PATH})
SET(DIRECTORIES_TO_INCLUDE ${DIRECTORIES_TO_INCLUDE} ${CMAKE_CURRENT_SOURCE_DIR}/${IMGUI_PATH}/backends)
FILE(GLOB IMGUI_SOURCES ${IMGUI_PATH}/*.cpp)
FILE(GLOB IMGUI_SOURCES ${IMGUI_PATH}/*.cpp ${IMGUI_PATH}/backends/imgui_impl_vulkan.cpp)
FILE(GLOB IMGUI_HEADERS ${IMGUI_PATH}/*.h)
ADD_LIBRARY(imgui SHARED ${IMGUI_SOURCES} ${IMGUI_HEADERS})
TARGET_LINK_LIBRARIES(imgui ${VULKAN_LIBRARY})
SET(THIRD_PARTY_LIBS ${THIRD_PARTY_LIBS} imgui)

# xxHash
SET(XXHASH_PATH external/xxHash)
INCLUDE_DIRECTORIES(${XXHASH_PATH})
SET(DIRECTORIES_TO_INCLUDE ${DIRECTORIES_TO_INCLUDE} ${CMAKE_CURRENT_SOURCE_DIR}/${XXHASH_PATH})
SET(XXHASH_BUILD_ENABLE_INLINE_API OFF) #optional
SET(XXHASH_BUILD_XXHSUM OFF) #optional
ADD_SUBDIRECTORY(${XXHASH_PATH}/cmake_unofficial/ ${XXHASH_PATH}/build/ EXCLUDE_FROM_ALL)
SET(THIRD_PARTY_LIBS ${THIRD_PARTY_LIBS} xxHash::xxhash)

# glm
SET(GLM_PATH external/glm)
INCLUDE_DIRECTORIES(${GLM_PATH})
SET(DIRECTORIES_TO_INCLUDE ${DIRECTORIES_TO_INCLUDE} ${CMAKE_CURRENT_SOURCE_DIR}/${GLM_PATH})

# tinygltf
SET(TINYGLTF_PATH external/tinygltf)
INCLUDE_DIRECTORIES(${TINYGLTF_PATH})
SET(DIRECTORIES_TO_INCLUDE ${DIRECTORIES_TO_INCLUDE} ${CMAKE_CURRENT_SOURCE_DIR}/${TINYGLTF_PATH})

# ktx
SET(KTX_PATH external/ktx)
INCLUDE_DIRECTORIES(${KTX_PATH}/include)
INCLUDE_DIRECTORIES(${KTX_PATH}/other_include)
SET(DIRECTORIES_TO_INCLUDE ${DIRECTORIES_TO_INCLUDE} ${CMAKE_CURRENT_SOURCE_DIR}/${KTX_PATH}/include)
SET(DIRECTORIES_TO_INCLUDE ${DIRECTORIES_TO_INCLUDE} ${CMAKE_CURRENT_SOURCE_DIR}/${KTX_PATH}/other_include)
set(KTX_SOURCES
	${KTX_PATH}/lib/texture.c
	${KTX_PATH}/lib/hashlist.c
	${KTX_PATH}/lib/checkheader.c
	${KTX_PATH}/lib/swap.c
	${KTX_PATH}/lib/memstream.c
	${KTX_PATH}/lib/filestream.c
)
set(KTX_INCLUDE
	${KTX_PATH}/include
	${KTX_PATH}/lib
	${KTX_PATH}/other_include
)
add_library(libktx ${KTX_SOURCES})
target_include_directories(libktx PUBLIC ${KTX_INCLUDE})
set_property(TARGET libktx PROPERTY FOLDER "external")
SET(THIRD_PARTY_LIBS ${THIRD_PARTY_LIBS} libktx)

# Google test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG 58d77fa8070e8cec2dc1ed015d66b454c8d78850 # v1.21
)
FetchContent_MakeAvailable(googletest)
### End of third party libraries

# Assets
ADD_SUBDIRECTORY(shaders)
ADD_DEFINITIONS(-DSHADER_PATH="${CMAKE_CURRENT_SOURCE_DIR}/shaders")
ADD_DEFINITIONS(-DTEXTURE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/textures")
ADD_DEFINITIONS(-DMODEL_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/models")
ADD_DEFINITIONS(-DPPM_PATH="${CMAKE_CURRENT_SOURCE_DIR}/ppm")

# Engine
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src)
SET(DIRECTORIES_TO_INCLUDE ${DIRECTORIES_TO_INCLUDE} ${CMAKE_CURRENT_SOURCE_DIR}/src)
ADD_SUBDIRECTORY(src)

SET(DIRECTORIES_TO_INCLUDE ${DIRECTORIES_TO_INCLUDE} PARENT_SCOPE)

# Test
ADD_SUBDIRECTORY(test)