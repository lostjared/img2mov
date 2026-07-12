
# img2mov

A C++ image-sequence-to-video tool with a command-line interface and an optional Qt GUI. It can also export image lists and extract frames from existing videos.

## Requirements

- A C++ compiler and CMake 3.16 or newer
- OpenCV 4 or 5 development files
- FFmpeg executable and development files (`libavcodec`, `libavformat`, and `libavutil`)
- `pkg-config`
- Qt 5 or Qt 6 Widgets development files (GUI only)
- Autoconf and Automake (optional alternative CLI build)

## Installation

### For Debian/Ubuntu

Install the common build dependencies with:

```bash
sudo apt-get install build-essential cmake pkg-config libopencv-dev \
  libavcodec-dev libavformat-dev libavutil-dev ffmpeg qt6-base-dev
```

## Build

### CLI and GUI with CMake

Configure and build both applications from the repository root:

```bash
cmake -S . -B build -DBUILD_GUI=ON
cmake --build build -j8
```

The resulting executables are:

```text
build/img2mov
build/img2mov-gui/img2mov-gui
```

Run the GUI with `./build/img2mov-gui/img2mov-gui`. To build only the CLI, omit `-DBUILD_GUI=ON`. To install the configured targets, run `cmake --install build`; use an appropriate install prefix or elevated permissions when required.

### Standalone GUI

The GUI is a C++17 target and can be built independently:

```bash
cmake -S img2mov-gui -B build-gui
cmake --build build-gui -j8
./build-gui/img2mov-gui
```

CMake automatically detects Qt 5 or 6 and OpenCV 4 or 5.

### CLI with Autotools

Run the following commands in the terminal:

```bash
./autogen.sh && ./configure && make
```

This path builds the command-line application only.

## GUI Usage

Start by adding image files. Drag rows to arrange them when **Sort alphabetically** is disabled. The sequence controls provide:

- **Sort alphabetically:** applies case-insensitive path ordering before encoding.
- **Shuffle before encoding:** randomizes the final sequence after optional sorting.
- **Fit:** preserves each image's aspect ratio and adds black padding where needed.
- **Stretch:** resizes each image to fill the selected output dimensions.
- **Frames per second:** treats every input image as one video frame.
- **Frames per image:** repeats each image for the selected number of frames at 30 fps.

The resolution box includes common presets and accepts custom values such as `2560x1440`. The codec list is populated at startup from `ffmpeg -encoders`; `libx264` is selected by default when available. Hardware encoders such as `h264_nvenc` and `hevc_nvenc` may appear when compiled into FFmpeg but still require compatible hardware and drivers.

### Rate Control

- **Quality — CRF/CQ:** uses `-crf` with `libx264`, `libx264rgb`, or `libx265`; `h264_nvenc` and `hevc_nvenc` use NVENC VBR with `-cq` and no target bitrate. Lower values increase quality and file size.
- **CBR:** sets the target, minimum, and maximum bitrate to the requested kbps value. NVENC codecs additionally use `-rc cbr`.
- **VBR:** sets a target bitrate with a maximum and buffer size of twice that value. NVENC codecs additionally use `-rc vbr`.

Not every FFmpeg encoder supports MP4, `yuv420p`, or every rate-control mode. Encoding errors from incompatible choices are shown in the GUI log.

## Command-Line Usage

To use `img2mov`, you have various options:

- `-v` Print version info
- `-i` Input directory
- `-r` Search with regular expression
- `-m` Match with regular expression
- `-t` Input file list in text file
- `-l` Output filename search only; do not create video (instead, output list)
- `-o` Output video file (.mov)
- `-w` Frame width
- `-h` Frame height
- `-f` Frames per second
- `-s` Stretch image (if not set, will resize to keep aspect ratio)
- `-n` Do not sort list of files
- `-q` Quiet mode
- `-I` File for images to be extracted
- `-L` File output prefix for file extraction (e.g., `test1` or `./folder/test1`)
- `-j` Output as JPEG
- `-b` Output as BMP
- `-p` Output as PNG
- `-E` Extract frame by index
- `-H` Output as HEVC x265
- `-4` Pipe to ffmpeg as x264
- `-5` Pipe to ffmpeg as x265
- `-7` Path to ffmpeg
- `-c` CRF Value for ffmpeg pipe

The program defaults to outputting video as AVC. To use HEVC, use `-H` unless you pipe to ffmpeg.

### Examples

```bash
img2mov -i . -o videofile.mp4 -w 1920 -h 1080 -f 24
img2mov -i ~/Movies -o outputfile.mp4 -w 1280 -h 720 -f 24 -r 'jpg$'
img2mov -i ~/Movies -l file_list.txt -r 'jpg$'
img2mov -i . -l output_text.txt -r 'jpg$'
img2mov -t input_list.txt -f 24 -w 640 -h 360 -o output.mp4
img2mov -t input_list.txt -f 24 -w 640 -h 360 -4 -o output.mp4
img2mov -t input_list.txt -f 24 -w 640 -h 360 -5 -o output.mp4 -c 26
```

#### Extracting Frames

To extract frames, use `-I` and `-L` to pass the video file and save prefix (location and filename). Example:

```bash
mkdir frames
img2mov -I test.mp4 -L frames/testprefix
```

Optional: Convert video to jpeg:

```bash
img2mov -I test.mp4 -L frames/testprefix -j
```

Or to output as BMP:

```bash
img2mov -I test.mp4 -L testprog -b
```

To extract a single frame (example: extract frame 5):

```bash
img2mov -I test.mp4 -L prefix -E 5
```
