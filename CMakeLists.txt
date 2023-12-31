CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(Umoc)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(DIRECTORIES_TO_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR})

SET(PLATFORM_GLFW TRUE)

# Vulkan
SET(VULKAN_PATH $ENV{VULKAN_SDK})
SET(PROJECT_ROOT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

IF (WIN32)
    add_compile_options(/wd4819)
    SET(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    SET(BUILD_SHARED_LIBS TRUE)
    SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPLATFORM_WINDOW")
    FIND_LIBRARY(
        VULKAN_LIBRARY
        NAMES vulkan-1 vulkan
        PATHS $ENV{VULKAN_SDK}/lib
        REQUIRED)

    IF (VULKAN_LIBRARY)
        INCLUDE_DIRECTORIES($ENV{VULKAN_SDK}/include)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR -DNOMINMAX")
        MESSAGE("Using system vulkan library")
    ELSE()
        message(FATAL_ERROR "Couldn't find Vulkan library. VULKAN_SDK must be set")
    ENDIF()

    add_definitions(-DASSET_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets")

    SET(PLATFORM_GLFW TRUE)

    # Need to check for other platforms
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_ROOT_DIRECTORY}/bin)
    SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_ROOT_DIRECTORY}/bin)
    SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_ROOT_DIRECTORY}/bin)
ELSEIF (APPLE)
    FIND_LIBRARY(VULKAN_LIBRARY NAMES vulkan PATHS ${VULKAN_PATH}/lib)
    FIND_LIBRARY(VULKAN_LAYER NAMES VkLayer_khronos_validation PATHS ${VULKAN_PATH}/lib)

    IF (VULKAN_LIBRARY)
        INCLUDE_DIRECTORIES(${VULKAN_PATH}/include)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -arch arm64 -DVK_USE_PLATFORM_MACOS_MVK -DPLATFORM_MAC")
    ELSE()
        message(FATAL_ERROR "Couldn't find Vulkan library. VULKAN_SDK must be set")
    ENDIF()

    message(${VULKAN_PATH})
    IF (VULKAN_LIBRARY)
        message("Found MoltenVK")
    ELSE()
        meesage("Can't find MoltenVK")
    ENDIF()

    if (VULKAN_LAYER)
        message("Found layers")
    endif()

    SET(PLATFORM_GLFW TRUE)
ENDIF()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOMINMAX -D_USE_MATH_DEFINES")

ADD_SUBDIRECTORY(engine)
INCLUDE_DIRECTORIES(${DIRECTORIES_TO_INCLUDE})