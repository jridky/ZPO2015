#include <zpogif.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void* allocator(zpogif_format format, uint16_t width, uint16_t height, ptrdiff_t* color_stride, ptrdiff_t* row_stride, void* priv);
void deallocator(zpogif_format format, uint16_t width, uint16_t height, void* image, void* priv);
int ends_with(const char *str, const char *suffix);

int main(int argc, char** argv)
{
	void* image;
	uint16_t width;
	uint16_t height;
	ptrdiff_t color_stride;
	ptrdiff_t row_stride;
	zpogif_error err;
	zpogif_format format = ZPOGIF_RGB;
	
	FILE* fin = fopen(argv[1], "rb");
	if (!fin) { printf("Could not open input file\n"); return 1; }
	err = zpogif_load(fin, format, &image, &width, &height, &color_stride, &row_stride, allocator, deallocator, NULL);
	if (err != ZPOGIF_NO_ERROR) { printf("GIF loading failed\n"); fclose(fin); return 2; }
	fclose(fin);
	
	printf("GIF loaded: %ux%upx, strides: %td, %td\n", width, height, color_stride, row_stride);
	
	FILE* fout = fopen(argv[2], "wb");
	if (!fout) { printf("Could not open output file\n"); return 3; }
	err = zpogif_save(fout, format, image, width, height, color_stride, row_stride);
	if (err != ZPOGIF_NO_ERROR) { printf("GIF saving failed\n"); fclose(fout); return 4; }
	fclose(fout);
	
	deallocator(format, width, height, image, NULL);
	
	return 0;
}

int ends_with(const char *str, const char *suffix)
{
	size_t lenstr = strlen(str);
	size_t lensuffix = strlen(suffix);
	if (lensuffix >  lenstr) return 0;
	return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

void* allocator(zpogif_format format, uint16_t width, uint16_t height, ptrdiff_t* color_stride, ptrdiff_t* row_stride, void* priv)
{
	printf("Allocating %ux%u image\n", width, height);
	int pixelsize = format == ZPOGIF_GRAYSCALE ? 1 : 4;
	*color_stride = pixelsize;
	*row_stride = width * pixelsize;
	return malloc(width * height * pixelsize);
}

void deallocator(zpogif_format format, uint16_t width, uint16_t height, void* image, void* priv)
{
	printf("Deallocating %ux%u image\n", width, height);
	free(image);
}
