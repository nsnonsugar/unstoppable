#pragma once
#include <stdio.h>
#include <png.h>

typedef struct {
	void* pixel_data;
	int height;
	int width;
	png_structp png;
	png_infop info;
	png_infop end;
	FILE* fp;
}PngPixelImage;

PngPixelImage* PngPixelImage_load( const char* file_name );
void PngPixelImage_free( PngPixelImage* pThis );