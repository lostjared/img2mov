
# img2mov

A simple and efficient tool to convert a series of images into a movie file, with options for extracting frames, specifying output format, and more. 

## Requirements

- GCC
- OpenCV Development Files
- Autoconf
- Automake

## Installation

### For Debian/Ubuntu

To install the required OpenCV development files, run:

```bash
sudo apt-get install libopencv-dev
```

## Compilation

You can compile the program using either `automake` or `CMake`. 

### Using Automake

Run the following commands in the terminal:

```bash
./autogen.sh && ./configure && make
```

**Note**: If your CPU has more than one core, you can use `-j` followed by the number of cores to speed up the compilation. For example, if your CPU has 8 cores:

```bash
make -j8
```

### Using CMake

For compiling with CMake, use:

```bash
mkdir build && cd build
cmake ..
make -j8
sudo make install
```

## Usage

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


