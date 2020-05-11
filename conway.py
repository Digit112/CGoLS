from PIL import Image
from PIL import ImageDraw
import time as t

# Size of grid to simulate in
width = 256
height = 256

# Dimensions of output file
out_width = 256
out_height = 256

# Number of frames to simulate
frames = 0

# Duration to show each frame in output gif
time = 100

# whether to write the frame number to the upper-left
write_frame = False

buf1 = [0] * (width * height)
buf2 = [0] * (width * height)

frame_buf = []
img_buf = []

# R-pentanimo, the glider produced ar 69 was the first ever observed
buf1[128 + width*127] = 255
buf1[129 + width*127] = 255
buf1[127 + width*128] = 255
buf1[128 + width*128] = 255
buf1[128 + width*129] = 255

frm_buf = buf1
to_buf = buf2
buf1_is_frm = True

frame_buf.append([])
for i in range(0, width*height):
	frame_buf[0].append(buf1[i])

img = Image.new("L", (width, height))
img.putdata(buf1)
img = img.resize((out_width, out_height), Image.NEAREST)
img.save("0.png")

if frames == 0:
	exit()

start = t.time()
for f in range(1, frames):
	for y in range(0, height):
		for x in range(0, width):
			neighbors = 0
			for ty in range(y-1, y+2):
				for tx in range(x-1, x+2):
					if tx == -1:
						tx = width-1
					elif tx == width:
						tx = 0
					if ty == -1:
						ty = height-1
					elif ty == height:
						ty = 0
				
					if frm_buf[tx + ty*width] == 255:
						neighbors += 1
				
			if frm_buf[x + y*width] == 255:
				neighbors -= 1
		
			if neighbors <= 1 or neighbors >= 4:
				to_buf[x + y*width] = 0
			elif neighbors == 3:
				to_buf[x + y*width] = 255
			else:
				to_buf[x + y*width] = frm_buf[x + y*width]
	
	frame_buf.append([])
	for i in range(0, width*height):
		frame_buf[f].append(to_buf[i])

	if buf1_is_frm:
		to_buf = buf1
		frm_buf = buf2
		buf1_is_frm = False
	else:
		to_buf = buf2
		frm_buf = buf1
		buf1_is_frm = True

end_gen = t.time()

for f in range(0, frames):
	img_buf.append(Image.new("L", (width, height)))
	img_buf[f].putdata(frame_buf[f])
	if write_frame:
		d = ImageDraw.Draw(img_buf[f])
		d.text((0,0), str(f), fill=255)
	img_buf[f] = img_buf[f].resize((out_width, out_height), Image.NEAREST)

img_buf[0].save("out.gif", save_all=True, append_images=img_buf[1:], duration=time, loop=0)

end = t.time()

print("Gen: " + str(end_gen-start) + "s, Sav: " + str(end-end_gen) + "s, Tot: " + str(end-start) + "s") 














