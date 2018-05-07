# img2mov

Requires: OpenCV Development Files


Watch this video tutorial on how to compile/use this program

https://youtu.be/K5FHAqI5RL0

In Debian/Ubuntu use:

	$ sudo apt-get install libopencv-dev

To compile use

	$ ./autogen.sh && ./configure && make 
	
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
	
example use:

	$ img2mov -i . -o videofile.mov -w 1920 -h 1080 -f 24 
	
	$ img2mov -i ~/Movies -o outputfile.mov -w 1280 -h 720 -f 24 -r 'jpg$'

	$ img2mov -i ~/Movies -l file_list.txt -r 'jpg$'

	$ img2mov -i . -l output_text.txt -r 'jpg$'
	$ img2mov -t output_text.txt -f 24 -w 640 -h 360 

