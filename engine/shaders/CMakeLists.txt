### GLSL Shaders

SET(HLSL_SHADERS)
FILE(GLOB GLSL_SHADERS *.vert *.frag *.comp *.rgen *.rchit *.rint *.rmiss)

SET(SUBDIRECTORIES
    quad
	brickwall
	normal
	screen
	skybox
)

FOREACH(DIRECTORY ${SUBDIRECTORIES})
    ADD_SUBDIRECTORY(${DIRECTORY})
ENDFOREACH()

FIND_PROGRAM(GLSLANG_VALIDATOR
	NAMES glslangValidator
	HINTS ENV VULKAN_SDK
	PATH_SUFFIXES bin)

FIND_PROGRAM(GLSLC
    glslc
    HINTS ${ANDROID_NDK}/shader-tools/*/ ${CMAKE_ANDROID_NDK}/shader-tools/*/)

SET(COMPILER_PARAMETER "")

IF(NOT GLSLANG_VALIDATOR AND NOT GLSLC)
	MESSAGE(FATAL_ERROR "Unable to find glslangValidator or glslc shader compiler")
ENDIF()

FOREACH(shader ${GLSL_SHADERS})
	GET_FILENAME_COMPONENT(file_name ${shader} NAME)
    GET_FILENAME_COMPONENT(file_last_ext ${shader} LAST_EXT)
	GET_FILENAME_COMPONENT(full_path ${shader} ABSOLUTE)
    GET_FILENAME_COMPONENT(directory ${shader} DIRECTORY)
    STRING(REPLACE "." "" file_ext ${file_last_ext})
	SET(output_file ${directory}/${file_name}.spv)

	SET(compiled_shaders ${compiled_shaders} ${output_file})
	SET(compiled_shaders ${compiled_shaders} PARENT_SCOPE)

	IF(GLSLANG_VALIDATOR)
		ADD_CUSTOM_COMMAND(
			OUTPUT ${output_file}
			WORKING_DIRECTORY ${directory}
			COMMAND ${GLSLANG_VALIDATOR} --target-env vulkan1.0 -V ${full_path} -o ${output_file}
			DEPENDS ${full_path}
			USES_TERMINAL
			VERBATIM
		)
	ELSE()
		ADD_CUSTOM_COMMAND(
			OUTPUT ${output_file}
			WORKING_DIRECTORY ${directory}
			COMMAND ${GLSLC} --target-spv=spv1.4 ${full_path} -o ${output_file}
			DEPENDS ${full_path}
			USES_TERMINAL
			VERBATIM
		)
	ENDIF()
ENDFOREACH()

### HLSL Shaders

FIND_PROGRAM(DXC
	NAMES dxc
	HINTS ENV VULKAN_SDK
	PATH_SUFFIXES bin)

SET(COMPILER_PARAMETER "")

IF(NOT DXC)
	MESSAGE(FATAL_ERROR "Unable to find DXC")
ENDIF()

FOREACH(shader ${HLSL_SHADERS})
	GET_FILENAME_COMPONENT(file_name ${shader} NAME)
    GET_FILENAME_COMPONENT(file_last_ext ${shader} LAST_EXT)
	GET_FILENAME_COMPONENT(full_path ${shader} ABSOLUTE)
    GET_FILENAME_COMPONENT(directory ${shader} DIRECTORY)
    STRING(REPLACE "." "" file_ext ${file_last_ext})
	SET(output_file ${directory}/${file_name}.spv)

	SET(compiled_shaders ${compiled_shaders} ${output_file})
	SET(compiled_shaders ${compiled_shaders} PARENT_SCOPE)

	SET(SHADER_PROFILE)

	IF (${file_last_ext} STREQUAL ".vert")
		SET(SHADER_PROFILE "-T vs_6_1")
	ELSEIF (${file_last_ext} STREQUAL ".frag")
		SET(SHADER_PROFILE "-T ps_6_1")
	ENDIF ()

	ADD_CUSTOM_COMMAND(
		OUTPUT ${output_file}
		WORKING_DIRECTORY ${directory}
		COMMAND ${DXC} -spirv -fspv-target-env=vulkan1.0 -E main ${SHADER_PROFILE} ${full_path} -Fo ${output_file}
		DEPENDS ${full_path}
		USES_TERMINAL
		VERBATIM
	)
ENDFOREACH()

### File tree
FILE(GLOB_RECURSE EXTRA_SHADERS ${EXTRA_SHADERS} *.glsl)

SOURCE_GROUP(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${GLSL_SHADERS} ${HLSL_SHADERS} ${EXTRA_SHADERS})

ADD_CUSTOM_TARGET(
	shaders 
	DEPENDS ${compiled_shaders}
	SOURCES ${GLSL_SHADERS} ${HLSL_SHADERS} ${EXTRA_SHADERS})