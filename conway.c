#include <stdio.h>
#include <stdlib.h>

#include "cgols.h"

int main() {
	// number of frames to output including initial frame
	int frames = 64;
	
	struct cgl* data = (struct cgl*) malloc(sizeof(struct cgl)*frames);
	load_cgl(data, "0.cgl");
	for (int f = 1; f < frames; f++) {
		data[f].width  = data[0].width;
		data[f].height = data[0].height;
		cgl_alloc(data[f]);
	}
	
	// Perform simulation
	CGoL(data, frames);
	printf("Simulation Complete. Saving Output.\n");
	
	// Save output to cgl files
	FILE* fs;
	char buf[36];
	
	for (int f = 0; f < frames; f++) {
		snprintf(buf, 36, "CGoL_out/%d.cgl", f);
		fs = fopen(buf, "wb");
		fseek(fs, 0, SEEK_SET);
		fwrite(&data[0].width, 2, 1, fs);
		fwrite(&data[0].height, 2, 1, fs);
		fwrite(data[f].data, 1, data[0].width*data[0].height, fs);
		fclose(fs);
	}
	
	for (int f = 0; f < frames; f++) {
		cgl_dealloc(data[f]);
	}
	free(data);
	return 0;
}
