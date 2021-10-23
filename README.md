# Implicit Function Renderer & Morphing

OpenGL implementation of Implicit Function Rendering, using the ray marching method.

- The algorithm is based on paper *Real-Time Ray Tracing of Implicit Surfaces on the GPU*

- Ray marching is implemented in Fragment Shader, with simple lighting & shadowing.

- FPS > 60fps on RTX 2070s.

- 3D morphing by interpolation between two implicit functions.

- Blob version of some basic mesh

  

# 1. Build

Please use VS2019 to open the sln project and build.



# 2. Results

![ImplicitFunctionRayMarcher/cow.png at main · SuikaSibyl/ImplicitFunctionRayMarcher (github.com)](https://github.com/SuikaSibyl/ImplicitFunctionRayMarcher/blob/main/Figs/cow.png)



![ImplicitFunctionRayMarcher/cow.png at main · SuikaSibyl/ImplicitFunctionRayMarcher (github.com)](https://github.com/SuikaSibyl/ImplicitFunctionRayMarcher/blob/main/Figs/glass_heart.png)



![ImplicitFunctionRayMarcher/cow.png at main · SuikaSibyl/ImplicitFunctionRayMarcher (github.com)](https://github.com/SuikaSibyl/ImplicitFunctionRayMarcher/blob/main/Figs/rabbit.png)



![ImplicitFunctionRayMarcher/cow.png at main · SuikaSibyl/ImplicitFunctionRayMarcher (github.com)](https://github.com/SuikaSibyl/ImplicitFunctionRayMarcher/blob/main/Figs/steiner.png)



Morphing Animation:



[Implicit Function Ray Marching Demo - bilibili](https://www.bilibili.com/video/BV1MT4y1Z7UR?spm_id_from=333.999.0.0)