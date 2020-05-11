# Renders the cgl files in CGoL_out/ that are produced by conway.c into gifs

from PIL import Image
from PIL import ImageDraw
import math
import struct
import time

# First and last frames to include in animation. First frame is 0
start = 0
end = 54

# Prepend to input cgl files
in_prefix = "CGoL_out/"

# filename of the output without the extension
out_name = "Barge"

# The dimensions of the file output
out_width = 512
out_height = 512

# Duration to show each animation frame in milliseconds
duration = 100

# Whether to write the frame number in the upper-left corner
draw_frame_num = False

# Method of coloring live cells based on time. Each entry contains a number being the amount of time the cell has to be alive
# Before this color phases in fully, followed by the RGB color all in a 4-tuple. The colors are linearly interpolated.
live_coloring = [(0, 255, 255, 255)]

# The same as above, but for dead cells
dead_coloring = [(0, 255, 0, 0), (8, 196, 196, 0), (16, 160, 160, 80), (24, 0, 240, 0), (36, 0, 0, 240), (42, 196, 0, 196), (62, 255, 0, 0), (68, 0, 0, 0)]

# The coloring for cells that have always been dead
dflt_coloring = (0, 0, 0) 

start_time = time.time()

img_buf = []
for f in range(start, end+1):
	fin = open(in_prefix + str(f) + ".cgl", "rb")
	width = struct.unpack("<H", fin.read(2))[0]
	height = struct.unpack("<H", fin.read(2))[0]
	rawdata = fin.read(width*height)
	fin.close()
	
	img_buf.append(Image.new("RGB", (width, height)))
	data = []
	for i in rawdata:
		val = i#struct.unpack("B", i)[0]
		is_alive = val & 128
		surv_time = val % 128
		
		if val == 127:
			data.append(dflt_coloring)
		elif val < 127:
			if dead_coloring[0][0] >= surv_time:
				data.append(dead_coloring[0][1:])
				continue
			if dead_coloring[-1][0] <= surv_time:
				data.append(dead_coloring[-1][1:])
				continue
			for c in range(1, len(dead_coloring)):
				C = dead_coloring[c]
				L = dead_coloring[c-1]
				t = (surv_time - L[0]) / (C[0] - L[0])
				if C[0] > surv_time:
					data.append((round(L[1] + t * (C[1]-L[1])), round(L[2] + t * (C[2]-L[2])), round(L[3] + t * (C[3]-L[3]))))
					break
		else:
			if live_coloring[0][0] >= surv_time:
				data.append(live_coloring[0][1:])
				continue
			if live_coloring[-1][0] <= surv_time:
				data.append(live_coloring[-1][1:])
				continue
			for c in range(1, len(live_coloring)):
				C = live_coloring[c]
				L = live_coloring[c-1]
				t = (surv_time - L[0]) / (C[0] - L[0])
				if C[0] > surv_time:
					data.append((round(L[1] + t * (C[1]-L[1])), round(L[2] + t * (C[2]-L[2])), round(L[3] + t * (C[3]-L[3]))))
					break
			
	img_buf[f].putdata(data)
	if draw_frame_num:
		d = ImageDraw.Draw(img_buf[f])
		d.text((0,0), str(f), fill=(255, 255, 255))
	img_buf[f] = img_buf[f].resize((out_width, out_height), Image.NEAREST)

images_colored = time.time() - start_time

img_buf[0].save(out_name + ".gif", save_all=True, append_images=img_buf[1:], duration=duration, loop=0)

images_saved = time.time() - start_time - images_colored

total = images_colored + images_saved

m0 = math.floor(images_colored / 60)
s0 = round(images_colored % 60 * 100) / 100

m1 = math.floor(images_saved / 60)
s1 = round(images_saved % 60 * 100) / 100

m2 = math.floor(total / 60)
s2 = round(total % 60 * 100) / 100

print("Coloring Images: " + str(m0) + "m " + str(s0) + "s, Saving to gif: " + str(m1) + "m " + str(s1) + "s, Total: " + str(m2) + "m " + str(s2) + "s")
				











