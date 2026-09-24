glslc -DVERTEX_SHADER -fshader-stage=vert .\assets\shaders\sprite.vert -o .\assets\shaders\sprite.vert.spv
glslc -DVERTEX_SHADER -fshader-stage=vert .\assets\shaders\spriteBatch.vert -o .\assets\shaders\spriteBatch.vert.spv
glslc -DFRAGMENT_SHADER -fshader-stage=frag .\assets\shaders\sprite.frag -o .\assets\shaders\sprite.frag.spv