# **Open Dash (beta)**

## Contribution
- Make stuff
- Make sure to run `CMake: Configure` when you add/remove a .cpp source file
- The game's namespace is `opendash` and its engine is `opendash::engine`
- Use `unique_ptr` for owning and raw pointers for non-owning access
- Scene-derived classes should have their `init` override be protected, not public
- When creating nodes/sprites, always call addChild first (see example in [TestScene.cpp](/src/game/TestScene.cpp)):
```cpp
auto* cube = addChild(Sprite::create("cube.png"));
```
- Uhhhhh

<!-- ## TO COMPILE SHADERS
its dis (run in da project root):
```
glslc -fshader-stage=vertex src/game/shaders/sprite.vert.glsl -o src/game/shaders/sprite.vert.spv
glslc -fshader-stage=fragment src/game/shaders/sprite.frag.glsl -o src/game/shaders/sprite.frag.spv
``` -->