# img2mov

Requires: OpenCV Development Files

Watch this video tutorial on how to compile/use this program

https://youtu.be/K5FHAqI5RL0

This program should be easy to compile, you will just need GCC,OpenCV development files,Autoconf,Automake

In Debian/Ubuntu use:

	$ sudo apt-get install libopencv-dev

To compile use

	$ ./autogen.sh && ./configure && make 

Note: if your CPU has more than one core use -j with number of cores will speed up compilation. Example if your CPU has 8 cores
	
	$ make -j8

Or to compile with CMake use:

	$ mkdir build && cd build

	$ cmake ..

	$ make -j8

	$ sudo make install
	
to use program

	-v print version info
	-i input directory
	-r "search with regular expression"
	-m "match with regular expression"
	-t input file list in text file
	-l output_filename search only do not create video instead output list
	-o output video file mov
	-w frame width
	-h frame height
	-f frames per second
	-s stretch image (if not set will resize to keep aspect ratio)
	-n do not sort list of files
	-q quiet mode
	-I file for images to be extracted
	-L file output prefix for file extraction (test1 or ./folder/testl)
	-j output as JPEG
	-b output as BMP
	-p output as PNG 
	-E extract frame by index
	-H output as HEVC x265

Program now default outputs video as AVC. To use HEVC use -H 
	
example use:

	$ img2mov -i . -o videofile.mp4 -w 1920 -h 1080 -f 24 
	$ img2mov -i ~/Movies -o outputfile.mp4 -w 1280 -h 720 -f 24 -r 'jpg$'
	$ img2mov -i ~/Movies -l file_list.txt -r 'jpg$'
	$ img2mov -i . -l output_text.txt -r 'jpg$'
	$ img2mov -t output_text.txt -f 24 -w 640 -h 360 
	
extract frames: use -I and -L to pass video file and save prefix (location and filename). 

	$ mkdir frames
	$ img2mov -I test.mp4 -L frames/testprefix

optional convert video to jpeg use:

	$ img2mov -I test.mp4 -L frames/testprefix -j 

or to output as bmp:

	$ img2mov -I test.mp4 -L testprog -b

or to extract a single frame example extract frame 5

	$ img2mov -I test.mp4 -L prefix -E 5

