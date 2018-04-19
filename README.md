# img2mov

Requires: OpenCV

To compile use

	$ ./autogen.sh && ./configure && make 
	
to use program

	-i input directory
	-o output video file mov
	-w frame width
    	-h frame height
	-f frames per second
	-s stretch image (if not set will resize to keep aspect ratio)
	
example use:

	$ img2mov -i . -o videofile.mov -w 1920 -h 1080 -f 24 

	
