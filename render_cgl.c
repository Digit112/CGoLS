#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

// Renders a sequence of cgl files which take the form "inp_prefix + frame_#.cgl", for instance "frames/glider_0.cgl" where inp_prefix is "frames/glider_" and frame_# is 0.
// The out_prefix works the same way for output files. For such files, you can use prefix to specify a path and out_type to specify the filetype, 0 for png and 1 for jpg.
// Use the width/height parameters to specify the dimensions of the output. The Game of Life Grid will be stretched to fit.
// Color mode can be set to basic (Black and white for dead and alive cells) or inverted basic with the values 0 and 1 respectively. For value 2, it will accept the values of the color parameters and use them
// to animate the colors. Mode 3 is the same but uses RGB instead of grayscale. Output image files will use the correct number of channels.
int render_cgl(int frames, char* inp_prefix, char* out_prefix, int out_width, int out_height, int color_mode, unsigned char** live_color, int lc_size, unsigned char** dead_color, int dc_size, unsigned char* dflt_color) {
	char fileinp_buffer[128];
	char fileout_buffer[128];
	
	unsigned short width;
	unsigned short height;
	int size; // Values of loaded cgl frame.
	
	unsigned char* rawdata;	// Holds loaded data
	unsigned char* raw_end;	// End of loaded data
	unsigned char* outdata; // Holds RGB
	unsigned char* img;		// Holds image object to be saved
	unsigned char* C;		// temp variable ("Color")
	unsigned char* L;		// temp variable ("Last Color")
	
	unsigned char val;		 // temp variable holds the cached value of a pointed-to unsigned char (just micro-optimization things)
	unsigned char is_alive;	 // whether the currently considered pixel is alive
	unsigned char surv_time; // The number of previous consecutive frames this pixel has had it's current value.
	
	// To hold the final output image with 3 channels.
	img = (unsigned char*) malloc(out_width * out_height * 3);
		
	// For image scaling
	double width_ratio, height_ratio;
	int tx, ty;
	
	double t;
	
	// Simple definitions for modes 0 and 1
	unsigned char LIVE, DEAD;
	LIVE = 255;
	DEAD = 0;
	if (color_mode == 1) {
		LIVE = 0;
		DEAD = 255;
		color_mode = 0;
	}
	
	FILE* fin;
	// Load each frame
	for (int f = 0; f < frames; f++) {
		// Generate filename
		snprintf(fileinp_buffer, 128, "%s%d.cgl", inp_prefix, f);
		
		// Load cgl file
		fin = fopen(fileinp_buffer, "rb");
		fseek(fin, 0, SEEK_SET);
		fread(&width, 2, 1, fin);
		fread(&height, 2, 1, fin);
		size = width * height;
		
		
		// malloc() must be called inside this loop as the grid size may differ between frames. TODO: Try realloc()
		rawdata = (unsigned char*) malloc(size);
		outdata = (unsigned char*) malloc(size*3); // Times 3 for three channels
		raw_end = (unsigned char*) rawdata+size;
		
		// Read remainder of file and close
		fread(rawdata, 1, size, fin);
		fclose(fin);
		
		//printf("%d, %d, %d\n", width, height, size);
		
		// For each pixel, convert to image color. At this point, the input data is 100% in the cgl format.
		// Iterator variables outside of the for loop because they cannot both correctly be declared inside the loop parameters.
		unsigned char* i;
		unsigned char* o;
		int w = 0;
		for (i = rawdata, o = outdata; i < raw_end; i++, o+=3) {
			val = *i;
			is_alive  = val & 128;
			surv_time = val & 127;
			
			//if (w == 32 && f == 0) {printf("\n"); w = 0;}
			w++;
			
			// Simple black and white coloring
			if (color_mode == 0) {
				if (is_alive) {
					o[0] = LIVE;
					o[1] = LIVE;
					o[2] = LIVE;
				} else {
					o[0] = DEAD;
					o[1] = DEAD;
					o[2] = DEAD;
				}
			// Grayscale lifetime coloring with linear interpolation
			} else if (color_mode == 2) {
				printf("TODO: Implement Grayscale coloring. Sorry.\n");
				return 1;
			// RGB lifetime coloring with linear interpolation
			} else if (color_mode == 3) {
				if (val == 127) {
					//if (f == 0) {printf("D");}
					o[0] = dflt_color[0];
					o[1] = dflt_color[1];
					o[2] = dflt_color[2];
				} else if (val < 127) {
					//if (f == 0) {printf("d");}
					// Test for values outside the given range, and use the appropriate entries
					if (dead_color[0][0] >= surv_time) {
						o[0] = dead_color[0][1];
						o[1] = dead_color[0][2];
						o[2] = dead_color[0][3];
					} else if (dead_color[dc_size-1][0] <= surv_time) {
						o[0] = dead_color[dc_size-1][1];
						o[1] = dead_color[dc_size-1][2];
						o[2] = dead_color[dc_size-1][3];
					// If the input value is in range, interpolate its color values from the two entries it lies between.
					} else {
						int c;
						for (c = 1; c < dc_size; c++) {
							C = dead_color[c];
							L = dead_color[c-1];
							if (C[0] > surv_time) {
								t = (surv_time - L[0]) / (double) (C[0] - L[0]);
								o[0] = round(L[1] + t * (C[1]-L[1]));
								o[1] = round(L[2] + t * (C[2]-L[2]));
								o[2] = round(L[3] + t * (C[3]-L[3]));
								break;
							}
						}
					}
				} else {
					// Test for values outside the given range, and use the appropriate entries
					if (live_color[0][0] >= surv_time) {
						o[0] = live_color[0][1];
						o[1] = live_color[0][2];
						o[2] = live_color[0][3];
					} else if (live_color[lc_size-1][0] <= surv_time) {
						o[0] = live_color[lc_size-1][1];
						o[1] = live_color[lc_size-1][2];
						o[2] = live_color[lc_size-1][3];
					// If the input value is in range, interpolate its color values from the two entries it lies between.
					} else {
						for (int c = 1; c < lc_size; c++) {
							C = live_color[c];
							L = live_color[c-1];
							if (C[0] > surv_time) {
								t = (surv_time - L[0]) / (double) (C[0] - L[0]);
								o[0] = round(L[1] + t * (C[1]-L[1]));
								o[1] = round(L[2] + t * (C[2]-L[2]));
								o[2] = round(L[3] + t * (C[3]-L[3]));
								break;
							}
						}
					}
				}
			}
		}
		
		// Calculate scaling ratios for this frame
		width_ratio = width / (double) out_width;
		height_ratio = height / (double) out_height;
		// Scale img in outdata to out_width, out_height and save.
		for (int y = 0; y < out_height; y++) {
			for (int x = 0; x < out_width; x++) {
				tx = floor(x * width_ratio);
				ty = floor(y * height_ratio);
				
				img[(x + out_width*y)*3] = outdata[(tx + width*ty)*3];
				img[(x + out_width*y)*3+1] = outdata[(tx + width*ty)*3+1];
				img[(x + out_width*y)*3+2] = outdata[(tx + width*ty)*3+2];
			}
		}
		
		// Save img
		snprintf(fileout_buffer, 128, "%s%d.png", out_prefix, f);
		stbi_write_png(fileout_buffer, out_width, out_height, 3, img, out_width*3);
		
		free(rawdata);
	}
	
	free(img);
}

// int render_cgl(int frames, char* inp_prefix, char* out_prefix, int out_width, int out_height, int duration, int color_mode, unsigned char** live_color, int lc_size, unsigned char** dead_color, int dc_size, unsigned char* dflt_color) {
int main() {
	unsigned char*  dflt_color = (unsigned char*) malloc(3);
	
	unsigned char** dead_color = (unsigned char**) malloc(4);
	for (int i = 0; i < 4; i++) {
		dead_color[i] = (unsigned char*) malloc(4);
	}
	
	unsigned char*  live_color = (unsigned char*) malloc(4);
	
	dflt_color[0] = 0; dflt_color[1] = 0; dflt_color[2] = 0;
	
	dead_color[0][0] = 0;  dead_color[0][1] = 255; dead_color[0][2] = 80;  dead_color[0][3] = 120;
	dead_color[1][0] = 8;  dead_color[1][1] = 20;  dead_color[1][2] = 180; dead_color[1][3] = 200;
	dead_color[2][0] = 48; dead_color[2][1] = 4;   dead_color[2][2] = 4;   dead_color[2][3] = 4;
	
	live_color[0] = 0; live_color[1] = 128; live_color[2] = 128; live_color[3] = 255;
	
	render_cgl(64, "CGoL_out/", "CGoL_render/", 128, 128, 0, &live_color, 1, dead_color, 3, dflt_color);
	
	for (int i = 0; i < 4; i++) {
		free(dead_color[i]);
	}
	free(dead_color);
}