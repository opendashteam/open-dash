#ifndef __cplusplus
#define GLSL
#endif

#if defined(GLSL) && !defined(VERTEX_SHADER) && !defined(FRAGMENT_SHADER)
    #error "must specify either -DVERTEX_SHADER or -DFRAGMENT_SHADER in command line"
#endif

#define UNIFORM_SLOT_VIEW_PROJECTION 0
#define UNIFORM_SLOT_SPRITE_PROPERTIES 1
#define UNIFORM_SLOT_SPRITE_BATCH_PROPERTIES 1

#ifdef FRAGMENT_SHADER
    #define UNIFORM_BUFFER(_SLOT) layout (std140, set = 3, binding = _SLOT) uniform
#else
    #define UNIFORM_BUFFER(_SLOT) layout (std140, set = 1, binding = _SLOT) uniform
#endif