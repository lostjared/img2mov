![img2mov](https://github.com/user-attachments/assets/0a1a07df-96a9-4235-95a2-8d14abe50772)

Requires OpenCV 4 or 5 for reading the images
and FFmpeg to write the video
Just a quick interface.

## Build

From the repository root, build the CLI and GUI together:

```bash
cmake -S . -B build -DBUILD_GUI=ON
cmake --build build -j
```

Alternatively, build only the C++17 GUI:

```bash
cmake -S img2mov-gui -B build-gui
cmake --build build-gui -j
```

CMake automatically selects Qt 5 or Qt 6 and supports OpenCV 4 or 5.
