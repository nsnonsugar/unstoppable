#include <stdbool.h>
#include <stdlib.h>
#include "PngPixelImage.h"
#include "android_fopen.h"

PngPixelImage* PngPixelImage_load( const char* file_name )
{
	FILE* fp;

	fp = android_fopen(file_name, "rb");
	if (!fp) {
		return NULL;
	}

	//シグネチャ読み込み
	unsigned char header[8] = {0};

	fread( header, sizeof(unsigned char), sizeof(header), fp );
	int is_png = png_sig_cmp( header, 0, sizeof( header ) );

	//png画像以外はNG
	if( is_png != 0 ){
		return NULL;
	}


	//png_struct 構造体の初期化
	png_structp png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
	if( png_ptr == NULL ){
		return NULL;
	}

	//png_info 構造体の初期化（画像データの前にあるチャンク用）
	png_infop info_ptr = png_create_info_struct( png_ptr );
	if( info_ptr == NULL ){
		png_destroy_read_struct( &png_ptr, (png_infopp)NULL, (png_infopp)NULL );
		return NULL;
	}

	//png_info 構造体の初期化（画像データの後にあるチャンク用）
	png_infop end_info = png_create_info_struct( png_ptr );
	if(end_info == NULL ){
		png_destroy_read_struct( &png_ptr, &info_ptr, (png_infopp)NULL );
		return NULL;
	}

	//libpng内部エラーハンドリング用
	if (setjmp( png_jmpbuf( png_ptr ) )) {
		png_destroy_read_struct( &png_ptr, &info_ptr, &end_info );
		fclose( fp );
		return NULL;
	}

	//初期化
	png_init_io( png_ptr, fp );

	//シグネチャを読込確認済なのでファイル先頭から読み飛ばしているバイト数を知らせる
	png_set_sig_bytes( png_ptr, sizeof( header ) );

	//画像情報を読み込み
	//画像の幅、高さ、ビット深度、色の表現方法、インターレースの情報を取得する
	int depth, colorType, interlaceType;
	unsigned int width, height;
	png_read_info( png_ptr, info_ptr );
	png_get_IHDR( png_ptr, info_ptr, &width, &height, &depth, &colorType, &interlaceType, NULL, NULL );


	//RGBとRGBAのみに対応
	if (colorType != PNG_COLOR_TYPE_RGB && colorType != PNG_COLOR_TYPE_RGBA) {
		return NULL;
	}

	//インターレースは非対応
	if (interlaceType != PNG_INTERLACE_NONE) {
		return NULL;
	}

	//1行のデータサイズと画像の高さから必要なメモリ量を計算して、メモリ確保
	int rowSize = png_get_rowbytes( png_ptr, info_ptr );
	int imgSize = rowSize * height;
	unsigned char* data = malloc( imgSize );
	unsigned int i;

	//ピクセルの読み込み
	for (i = 0; i < height; i++) {
		png_read_row( png_ptr, &data[i * rowSize], NULL );
	}

	png_read_end( png_ptr, end_info );

	PngPixelImage* ret = (PngPixelImage*)malloc(sizeof(PngPixelImage));
	ret->height = height;
	ret->width = width;
	ret->pixel_data = data;
	ret->png = png_ptr;
	ret->info = info_ptr;
	ret->end = end_info;
	ret->fp = fp;

	return ret;
}

void PngPixelImage_free( PngPixelImage* pThis )
{
	png_destroy_read_struct( &pThis->png, &pThis->info, &pThis->end );
	free( pThis->pixel_data );
	free( pThis );
}
