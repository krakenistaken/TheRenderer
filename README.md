# THErenderer

Simple 3d renderer that works faster than unity. Test outcomes are below. 

Both tested in same PC(used 3080 as GPU)

| (Polygons) | TheRenderer (FPS) | Unity (FPS) | difference |
| :--- | :---: | :---: | :---: |
| **230.4 K** | **2850** | 450 | **~6.3x Faster** |
| **576.0 K** | **3750** | 650 | **~5.7x Faster** |
| **11.52 M** | **1065** | 200 | **~5.3x Faster** |
| **17.28 M** | **750** | 160 | **~4.7x Faster** |
| **23.04 M** | **570** | 120 | **~4.7x Faster** |
| **34.56 M** | **395** | 95  | **~4.1x Faster** |
| **46.08 M** | **300** | 70  | **~4.3x Faster** |
| **69.12 M** | **200** | 50  | **~4.0x Faster** |
| **115.2 M** | **120** | 32  | **~3.7x Faster** |
| **172.8 M** | **80** | 22  | **~3.6x Faster** |
| **230.4 M** | **60** | 15  | **~4.0x Faster** |
| **288.0 M** | **50** | 12  | **~4.1x Faster** |
| **345.6 M** | **42** | 10  | **~4.2x Faster** |
| **460.8 M** | **30** | 8   | **~3.7x Faster** |
| **691.2 M** | **22** | 5   | **~4.4x Faster** |
| **921.6 M** | **16** | 4   | **~4.0x Faster** |
| **1.152 B** | **13** | 2   | **~6.5x Faster** |
| **1.728 B** | **9** | Crash/N/A | - |
| **2.304 B** | **7** | Crash/N/A | - |
| **3.456 B** | **4** | Crash/N/A | - |

<img width="1600" height="800" alt="image" src="https://github.com/user-attachments/assets/755956d2-9d49-4faa-a467-9f986c4dea4c" />
<img width="1000" height="600" alt="image" src="https://github.com/user-attachments/assets/fe865a4d-f430-43f9-b878-81d5c4c32a65" />

A **high-performance, hardware-instanced OpenGL 3.3 rendering library** written in modern C++ (C++20). Designed for simulations, visualizations, and any project that needs to push massive amounts of geometry to the GPU with minimal overhead.

Tested rendering **1.5 million objects** in real-time on an NVIDIA RTX 3080.

---

## Features

- **Hardware Instancing** — Render hundreds of thousands of objects in a single draw call
- **OBJ Model Loading** — Load `.obj` files with automatic vertex deduplication
- **Procedural Mesh Generation** — Built-in torus and icosphere generators
- **Texture Support** — PNG texture loading with automatic mipmapping (via `stb_image`)
- **Free-Look Camera** — WASD + mouse camera with direction-based movement
- **HUD System** — Built-in bitmap font text rendering for FPS, stats, and custom overlays
- **Simple Application Framework** — Subclass `Application`, override 3 methods, and you have a working app

---

## Quick Start

### 1. Clone & Build

```bash
git clone https://github.com/krakenistaken/TheRenderer.git
cd TheRenderer
```

Open `THErenderer.slnx` in Visual Studio 2022+. The solution contains two projects:

| Project | Type | Description |
|---------|------|-------------|
| **THErendererLib** | Static Library (`.lib`) | The engine core — all rendering, scene management, and asset loading |
| **BenchmarkDemo** | Application (`.exe`) | A stress-test demo showcasing 29 scenes from 250 to 1.5M objects |

Build the solution (the demo automatically links against the library).

### 2. Run the Benchmark Demo

After building, run `BenchmarkDemo.exe`. Use the `<` and `>` buttons in the top-right corner to cycle through scenes with increasing object counts.

**Controls:**
- **WASD** — Move camera (relative to look direction)
- **Mouse** — Look around
- **ESC** — Exit

---

## Using THErenderer in Your Own Project

### Step 1: Include the Umbrella Header

```cpp
#include <THErenderer.h>
```

This single header gives you access to the entire engine: `Application`, `Mesh`, `Texture`, `Shader`, `Scene`, `OBJLoader`, `HUD`, and all GLM math types.

### Step 2: Subclass `Application`

```cpp
#include <THErenderer.h>

class MySimulation : public Application {
    Mesh cubeMesh;
    Texture cubeTex;

    void onInit() override {
        // Load your assets
        std::vector<Vertex> verts;
        std::vector<uint32_t> inds;
        OBJLoader::load("assets/cube.obj", verts, inds);
        cubeMesh.upload(verts, inds);
        cubeTex.loadFromFile("assets/cube.png");

        // Place 1000 cubes in a grid
        for (int i = 0; i < 1000; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(
                (i % 10) * 3.0f,
                0.0f,
                (i / 10) * 3.0f
            ));
            scene.addInstance(&cubeMesh, &cubeTex, model);
        }
        scene.prepareInstancing();
    }

    void onUpdate(float dt) override {
        // Your per-frame logic here (physics, AI, etc.)
    }

    void onRender() override {
        // Draw HUD or custom overlays
        hud.beginRender();
        
        char buf[64];
        snprintf(buf, sizeof(buf), "Objects: 1000");
        hud.renderText(buf, 12.0f, 64.0f, 2.0f, glm::vec3(1.0f, 0.8f, 0.2f));

        hud.endRender();
    }
};

int main() {
    MySimulation app;
    return app.run(1280, 720, "My Simulation") ? 0 : -1;
}
```

That's it. **3 methods and a main function** — the engine handles the window, OpenGL context, render loop, camera, and input for you.

### Step 3: Set Up Your Visual Studio Project

1. Create a new **Console Application** project in the same solution
2. Add a **Project Reference** to `THErendererLib`
3. Add `$(SolutionDir)THErendererLib\src` to **Additional Include Directories**
4. Link against `opengl32.lib`
5. Make sure vcpkg is enabled (`glew`, `glfw3`, `glm`, `stb` dependencies)

---

## API Reference

### `Application` (Base Class)

| Method | Description |
|--------|-------------|
| `bool run(int w, int h, const char* title)` | Starts the engine and enters the main loop |
| `virtual void onInit()` | Called once after the OpenGL context is created. Load your assets here |
| `virtual void onUpdate(float dt)` | Called every frame before rendering. Put game/simulation logic here |
| `virtual void onRender()` | Called every frame after the scene is rendered. Draw HUD/overlays here |
| `virtual void onShutdown()` | Called when the application is closing |

**Protected members available in your subclass:**

| Member | Type | Description |
|--------|------|-------------|
| `window` | `Window` | The GLFW window (input, size, etc.) |
| `renderer` | `Renderer` | The GL state manager |
| `timer` | `Timer` | Delta time and FPS tracking |
| `meshShader` | `Shader` | The default instanced mesh shader |
| `scene` | `Scene` | The scene graph with instancing support |
| `hud` | `HUD` | The text overlay system |

**Helper methods:**

| Method | Returns |
|--------|---------|
| `getAspectRatio()` | `float` — Current window aspect ratio |
| `getFPS()` | `float` — Smoothed frames per second |
| `getDeltaTime()` | `float` — Seconds since last frame |
| `getWidth()` / `getHeight()` | `int` — Window dimensions |

### `Scene`

```cpp
scene.clear();                                    // Remove all objects
scene.addInstance(&mesh, &texture, modelMatrix);   // Add an instanced object
scene.prepareInstancing();                         // Upload instance data to GPU (call after adding all objects)
```

### `OBJLoader`

```cpp
OBJLoader::load("path/to/model.obj", vertices, indices);           // Load an OBJ file
OBJLoader::generateTorus("out.obj", segments, rings, R, r);        // Generate a torus OBJ
OBJLoader::generateIcosphere("out.obj", subdivisions);             // Generate an icosphere OBJ
```

### `Mesh`

```cpp
Mesh mesh;
mesh.upload(vertices, indices);   // Upload vertex/index data to GPU
mesh.draw();                      // Draw (used internally by the renderer)
mesh.getTriangleCount();          // Get the number of triangles
```

### `Texture`

```cpp
Texture tex;
tex.loadFromFile("path/to/image.png");   // Load a PNG texture with mipmaps
tex.bind(slot);                          // Bind to a texture slot
```

### `HUD`

```cpp
hud.beginRender();
hud.renderText("Hello!", x, y, scale, glm::vec3(1.0f, 0.5f, 0.0f));       // Render custom text
hud.endRender();
```

---

## Architecture

```
THErendererLib/
├── src/
│   ├── THErenderer.h          ← Umbrella header (include this)
│   ├── Engine/
│   │   ├── Application.h/cpp  ← Base class for your app
│   │   ├── Window.h/cpp       ← GLFW window wrapper
│   │   ├── Renderer.h/cpp     ← OpenGL state manager
│   │   └── Timer.h/cpp        ← Delta time & FPS
│   ├── Graphics/
│   │   ├── Mesh.h/cpp         ← VAO/VBO/EBO + instancing
│   │   ├── Shader.h/cpp       ← GLSL compilation + uniform cache
│   │   ├── Texture.h/cpp      ← Image loading + mipmaps
│   │   └── OBJLoader.h/cpp    ← OBJ parser + mesh generators
│   ├── Scene/
│   │   ├── Scene.h/cpp        ← Instance groups + rendering
│   │   └── Camera.h/cpp       ← Free-look FPS camera
│   └── UI/
│       ├── HUD.h/cpp          ← Bitmap font text rendering
│       └── FontData.h         ← Embedded 8x8 font atlas
```

---

## How Hardware Instancing Works

Traditional rendering calls `glDrawElements()` once per object. With 100,000 objects, that's 100,000 CPU→GPU round trips per frame — an instant bottleneck.

THErenderer groups objects by mesh+texture into `InstanceGroup`s. All transform matrices are packed into a single contiguous buffer and uploaded to VRAM. Then `glDrawElementsInstanced()` renders all instances in **one single draw call**.

This reduces CPU overhead by 99.99% and lets the GPU's thousands of parallel cores do what they do best.

---

## Dependencies

Managed automatically via [vcpkg](https://vcpkg.io/):

| Library | Purpose |
|---------|---------|
| **GLEW** | OpenGL extension loading |
| **GLFW3** | Window creation and input |
| **GLM** | Mathematics (vectors, matrices) |
| **stb_image** | PNG/JPG texture loading |

---

## License

MIT License. Use it for anything.
