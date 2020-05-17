#ifndef CGOLS
#define CGOLS

#include <stdio.h>
#include <stdlib.h>

// Stores a game of life grid.
struct cgl {
	unsigned short width;
	unsigned short height;
	
	// Byte matrix containing the data.
	unsigned char* data;
};

// Gets the width and height of cgl values and stores them in the passed cgl object. load_cgl should be used to load the entire file.
struct cgl* stat_cgl(struct cgl* out, const char* fn) {
	FILE* fs = fopen(fn, "rb");
	if (fs == NULL) {
		printf("Failed to open input file.\n");
		exit(1);
	}
	
	fseek(fs, 0, SEEK_SET);
	fread(&out->width, 2, 1, fs);
	fread(&out->height, 2, 1, fs);
	
	return out;
}

// Allocates space for cgl data and assigns it to the passed cgl object under the VITAL assumption that width and height have been initialized.
inline struct cgl* cgl_alloc(struct cgl* out) {
	out->data = (unsigned char*) malloc(out->width*out->height);
	return out;
}

// Deallocates space for the passed cgl struct
inline void cgl_dealloc(struct cgl* out) {
	free(out->data);
}

// Loads a cgl file into a cgl struct. This function will also automatically load width and height, and allocate space in memory. Remember to use cgl_dealloc() later.
struct cgl* load_cgl(struct cgl* out, const char* fn) {
	FILE* fs = fopen(fn, "rb");
	if (fs == NULL) {
		printf("Failed to open input file.\n");
		exit(1);
	}
	
	fseek(fs, 0, SEEK_SET);
	fread(&out->width, 2, 1, fs);
	fread(&out->height, 2, 1, fs);
	
	cgl_alloc(out);
	
	fread(out[0].data, 1, size, fs);
	fclose(fs);
	
	return out;
}

// Get the value at (x, y) in the cgl grid.
inline unsigned char cgl_get(struct cgl* out, int x, int y) {
	return out->data[x + out->width*y];
}

// Set the value at (x, y) in the cgl grid.
inline void cgl_set(struct cgl* out, int x, int y, unsigned char val) {
	out->data[x + out->width*y] = val;
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

#endif