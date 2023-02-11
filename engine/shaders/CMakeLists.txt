FILE(GLOB_RECURSE shader_files *.vert *.frag *.comp *.rgen *.rchit *.rint *.rmiss)

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

FOREACH(shader ${shader_files})
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
			COMMAND ${GLSLANG_VALIDATOR} --target-env vulkan1.2 -V ${full_path} -o ${output_file}
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

FILE(GLOB_RECURSE shader_extra_files ${shader_extra_files} *.glsl)

SOURCE_GROUP(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${shader_files} ${shader_extra_files})
ADD_CUSTOM_TARGET(
	shaders 
	DEPENDS ${compiled_shaders}
	SOURCES ${shader_files} ${shader_extra_files})
