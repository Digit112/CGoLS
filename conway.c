#include <stdio.h>
#include <stdlib.h>

// Stores a game of life grid
struct cgl {
	unsigned short width;
	unsigned short height;
	
	// Byte matrix containing the data. Each entry is either 0 or 255
	unsigned char* data;
};

// Reads cgl files produced by img_to_cgl.py
// Allocates space for this and frames-1 additional frames of identical size
// Initializes the first frame and returns a pointer to the array
// Be sure to call cgl_dealloc later
struct cgl* get_cgl(const char* fn, int frames) {
	struct cgl* out = (struct cgl*) malloc(sizeof(struct cgl) * frames);
	unsigned short width, height;
	unsigned int size;
	
	FILE* fs = fopen(fn, "rb");
	if (fs == NULL) {
		printf("Oof\n");
	}
	fseek(fs, 0, SEEK_SET);
	fread(&width, 2, 1, fs);
	fread(&height, 2, 1, fs);
	size = width*height;
	
	printf("%d, %d, %d\n", width, height, size);
	for (int i = 0; i < frames; i++) {
		out[i].width = width;
		out[i].height = height;
		out[i].data = (unsigned char*) malloc(size);
	}
	
	fread(out[0].data, 1, size, fs);
	fclose(fs);
	
	return out;
}

void cgl_dealloc(struct cgl* full_ptr, int num) {
	for (int i = 0; i < num; i++) {
		free(full_ptr[i].data);
	}
	free(full_ptr);
}

// Perform a Game of Life Simulation and store results in the passed array.
// Expects the first element in the cgl array to contain the starting frame. This frame will be unchanged. Although all cgl objects will store width and height, only the zeroth element's dimensions are considered.
// VERY important: This function expects all cgl objects in the array to contain valid pointers to adequate space (width*height bytes), although only the first cgl object must contain initialized data.
// Returns the passed cgl pointer.
struct cgl* CGoL(struct cgl* data, int frames) {
	int neighbors;
	int cursor;
	int tcursor;
	
	int width = data[0].width;
	int height = data[0].height;
	
	unsigned char prev_val;
	
	// For each frame
	for (int f = 1; f < frames; f++) {
		
		// For each pixel
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				cursor = x + width*y;
				neighbors = 0;
				prev_val = data[f-1].data[cursor];
				
				int ax, ay;
				// For each neighbor
				for (int ty = y-1; ty < y+2; ty++) {
					ay = ty;
					if (ty == -1) {
						ay = height-1;
					} else if (ty == height) {
						ay = 0;
					}
					for (int tx = x-1; tx < x+2; tx++) {
						ax = tx;
						if (ax == -1) {
							ax = width-1;
						} else if (tx == width) {
							ax = 0;
						} else if (tx == x && ty == y) {
							continue;
						}
						tcursor = ax + width*ay;
						
						if (data[f-1].data[tcursor] > 0x7f) {
							neighbors++;
						}
					}
				}
				
				if (prev_val > 0x7f) {
					if (neighbors <= 1 || neighbors >= 4) {
						data[f].data[cursor] = 0;
					} else {
						if (prev_val < 0xff) {
							data[f].data[cursor] = prev_val+1;
						} else {
							data[f].data[cursor] = prev_val;
						}
					}
				} else {
					if (neighbors == 3) {
						data[f].data[cursor] = 0x80;
					} else {
						if (prev_val < 0x7e) {
							data[f].data[cursor] = prev_val+1;
						} else {
							data[f].data[cursor] = prev_val;
						}
					}
				}
			}
		}
	}
	
	return data;
}

int main() {
	// number of frames to output including initial frame
	int frames = 55;
	
	// Read data from file and allocate space for simulation results
	struct cgl* data = get_cgl("0.cgl", frames);
	
	// Perform simulation
	data = CGoL(data, frames);
	printf("Simulation Complete.\n");
	
	// Save output to cgl files
	FILE* fs;
	char buf[12];
	for (int f = 0; f < frames; f++) {
		snprintf(buf, 36, "CGoL_out/%d.cgl", f);
		fs = fopen(buf, "wb");
		fseek(fs, 0, SEEK_SET);
		fwrite(&data[0].width, 2, 1, fs);
		fwrite(&data[0].height, 2, 1, fs);
		fwrite(data[f].data, 1, data[0].width*data[0].height, fs);
		fclose(fs);
	}
	
	cgl_dealloc(data, frames);
	return 0;
}
