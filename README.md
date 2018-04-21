# img2mov

Requires: OpenCV

To compile use

	$ ./autogen.sh && ./configure && make 
	
to use program

	-v print version info
	-i input directory
	-r "match with regular expression""
	-t input file list in text file
	-o output video file mov
	-w frame width
	-h frame height
	-f frames per second
	-s stretch image (if not set will resize to keep aspect ratio)
	-n do not sort list of files
	
example use:

	$ img2mov -i . -o videofile.mov -w 1920 -h 1080 -f 24 
	
	$ img2mov -i ~/Movies -o outputfile.mov -w 1280 -h 720 -f 24 -r "jpg$"

	
