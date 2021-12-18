# Potato
A project to teach myself the graphics pipeline, rendering and Vulkan. It's called potato because it runs like one. Optimizations are planned after I get the basics.

<br/>
<br/>

<div align="center">
    <img alt="Hello World Triangle" src="https://user-images.githubusercontent.com/26112391/135707888-11ea3431-1bf2-4e5c-a249-7e980230f381.png" width="700"></img>
</div>

## Build
### Requirements / Dependencies
* C++20 capable compiler
* Vulkan 1.2 SDK
* CMake 3.20 or above
* GLM 0.9.9
* GLFW 3.3

Clone the repo, along with submodules. 
To build, run 
```
$ cd potato
$ mkdir build && cd build
$ cmake .. -DCMAKE_BUILD_TYPE=Debug
```

If you get this error, `CMake Error: File icon.bin`, that's due to a missing icon used
for the window icon. Replace it with an `icon.bin` file that GLFW [can read](https://www.glfw.org/docs/3.3/group__window.html#gadd7ccd39fe7a7d1f0904666ae5932dc5).
