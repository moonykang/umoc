FILE(GLOB SHADERS *.vert *.frag *.comp *.rgen *.rchit *.rint *.rmiss)
SET(HLSL_SHADERS ${HLSL_SHADERS} ${SHADERS} PARENT_SCOPE)