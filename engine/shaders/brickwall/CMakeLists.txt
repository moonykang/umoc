FILE(GLOB SHADERS *.vert *.frag *.comp *.rgen *.rchit *.rint *.rmiss)
SET(GLSL_SHADERS ${GLSL_SHADERS} ${SHADERS} PARENT_SCOPE)