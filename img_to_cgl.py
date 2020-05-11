# Convert image file to a very simple grayscale file
# First two bytes are width, next two are height, and every consecutive byte represents each pixel in order.

# Bit 7 in each byte controls whether the cell is alive or dead. The remaining bits represent the number of frames since the most recent change, but will not be incremented if they max out of course. 0x7f has the special meaning that the cell has always been dead, thus the default values output to file are 0x7f for black and 0x80 for white.

# Conversion from RGB, L, and 1 image types are supported.

# If invert is set, converts black pixels to white and other pixels to black.

# The files are expected to be named a number and be in the png format,
# e.g. "0.png", "1.png" ... "23.png" ...

from PIL import Image
import struct

# Prefix before input files
i_prefix = ""

# Prefix before output files
o_prefix = ""

# First and last frame indicies to convert, inclusive
start = 0
end = 0

invert = True

for i in range(start, end+1):
	fin = i_prefix + str(i) + ".png"
	fout = o_prefix + str(i) + ".cgl"
	
	img = Image.open(fin)
	data = img.getdata()
	
	fout_h = open(fout, "wb")
	
	fout_h.write(img.width.to_bytes(2, 'little'))
	fout_h.write(img.height.to_bytes(2, 'little'))
	for v in data:
		if invert:
			if v == 0 or v == (0, 0, 0):
				fout_h.write(b'\x80')
			else:
				fout_h.write(b'\x7f')
		else:
			if v == 0 or v == (0, 0, 0):
				fout_h.write(b'\x7f')
			else:
				fout_h.write(b'\x80')
	
	fout_h.close()
			
			
			
			
			
			
			
			
			
