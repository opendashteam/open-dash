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

## Compilation tips
You usually have to run:
```
cmake -S . -B build
cmake --build build
```

If you're on Mac OS, you can make a `.app` file by
switching the first line with:
```
cmake -S . -B build -DCREATE_MAC_BUNDLE=ON
```