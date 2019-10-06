# Intro CMake Build
This CMake setup downloads and sets up NASM, crinkler and OpenGL headers for you. It generates a Visual Studio solution that is ready to go.


## Build Instructions

```
mkdir build && cd build
cmake .. -A Win32 && cmake --build .
```