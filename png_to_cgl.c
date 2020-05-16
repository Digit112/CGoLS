#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

// inp_fn: The filename/path to the input image
// out_fn: The filename/path to the file created to store cgl data
// threshold: The threshold representing the boundary between "dead" and "alive" pixels, from 0-255. On 
//             RGB images, The average of the fields is used.
// invert: If 0, interprets all pixels below threshold as dead and all others as alive. Otherwise, interprets all
//		   pixels below threshold as alive, and all pixels above threshold as dead.
int png_to_cgl(char* inp_fn, char* out_fn, int threshold, int invert) {
	unsigned char DEAD, LIVE;
	// If inverted, swap meanings
	if (!invert) {
		DEAD = 0x7f;
		LIVE = 0x80;
	} else {
		DEAD = 0x80;
		LIVE = 0x7f;
	}
	
	int width, height, channels;
	unsigned char* img = stbi_load(inp_fn, &width, &height, &channels, 0);
	if (img == NULL) {
		printf("Error when opening image.\n");
		return 1;
	}
	
	FILE* fouts = fopen(out_fn, "wb");
	fseek(fouts, 0, SEEK_SET);
	fwrite(&width, 2, 1, fouts);
	fwrite(&height, 2, 1, fouts);
	
	// The inefficiency in the following chain's layout is one of pure sacrifice for code speed as
	// The code is likely to never be changed again
	
	// For grayscale images.
	if (channels == 1) {
		unsigned char* img_end = img + width*height;
		for (unsigned char* p = img; p < img_end; p++) {
			if (*p > threshold) {
				fwrite(&LIVE, 1, 1, fouts);
			} else {
				fwrite(&DEAD, 1, 1, fouts);
			}
		}
	// For grayscale + alpha images. The alpha is dropped. (NOT TESTED)
	} else if (channels == 2) {
		unsigned char* img_end = img + width*height*2;
		for (unsigned char* p = img; p < img_end; p+=2) {
			if (*p > threshold) {
				fwrite(&LIVE, 1, 1, fouts);
			} else {
				fwrite(&DEAD, 1, 1, fouts);
			}
		}
	// For RGB images.
	} else if (channels == 3) {
		unsigned char* img_end = img + width*height*3;
		threshold*=3; //  This way the sum of RGB values for each pixel does not have to be divided by 3 each time.
		for (unsigned char* p = img; p < img_end; p+=3) {
			if (*p + *(p+1) + *(p+2) > threshold) {
				fwrite(&LIVE, 1, 1, fouts);
			} else {
				fwrite(&DEAD, 1, 1, fouts);
			}
		}
	// For RGBA images. The alpha channel is dropped.
	} else if (channels == 4) {
		unsigned char* img_end = img + width*height*4;
		threshold*=3;
		for (unsigned char* p = img; p < img_end; p+=4) {
			if (*p + *(p+1) + *(p+2) > threshold) {
				fwrite(&LIVE, 1, 1, fouts);
			} else {
				fwrite(&DEAD, 1, 1, fouts);
			}
		}
	}
	
	fclose(fouts);
	
	stbi_image_free(img);
	
	return 0;
}

int main() {
	return png_to_cgl("0.png", "0.cgl", 128, 0);
}