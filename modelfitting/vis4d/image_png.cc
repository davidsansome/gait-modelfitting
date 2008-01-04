// image_png.cc
// Richard Seely 2007

#include <stdlib.h>
#include <stdio.h>

#include "util.hh"
#include "image.hh"

/* code modified from below */

/*
 * Guillaume Cottenceau (gc at mandrakesoft.com)
 *
 * Copyright 2002 MandrakeSoft
 *
 * This software may be freely redistributed under the terms of the GNU
 * public license.
 *
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define PNG_DEBUG 3
#include <png.h>

void ImageRGB::png_abort(const char * s, ...)
{
	va_list args;
	va_start(args, s);
	vfprintf(stderr, s, args);
	fprintf(stderr, "\n");
	va_end(args);
	abort();
}

void ImageRGB::load_png( char * filename )
{
    png_structp png_ptr;
    png_infop info_ptr;
	char header[8];	// 8 is the maximum size that can be checked

    int y;
    png_byte color_type;
    png_byte bit_depth;
    int number_of_passes;
    png_bytep * row_pointers;
    
	/* open file and test for it being a png */
	FILE *fp = fopen(filename, "rb");
	if (!fp)
		png_abort("[read_png_file] File %s could not be opened for reading", filename);
	fread(header, 1, 8, fp);
	if (png_sig_cmp((png_byte *) header, 0, 8))
		png_abort("[read_png_file] File %s is not recognized as a PNG file", filename);


	/* initialize stuff */
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
	if (!png_ptr)
		png_abort("[read_png_file] png_create_read_struct failed");

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
		png_abort("[read_png_file] png_create_info_struct failed");

	if (setjmp(png_jmpbuf(png_ptr)))
		png_abort("[read_png_file] Error during init_io");

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	x_size = info_ptr->width;
	y_size = info_ptr->height;
	color_type = info_ptr->color_type;
	bit_depth = info_ptr->bit_depth;

	number_of_passes = png_set_interlace_handling(png_ptr);
    
    if (bit_depth == 16)
        png_set_strip_16(png_ptr);
    if (color_type & PNG_COLOR_MASK_ALPHA)
        png_set_strip_alpha(png_ptr);
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
          png_set_gray_to_rgb(png_ptr);
    
	png_read_update_info(png_ptr, info_ptr);

	/* read file */
	if (setjmp(png_jmpbuf(png_ptr)))
		png_abort("[read_png_file] Error during read_image");
    
    alloc();

	row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * y_size);
	for (y=0; y<y_size; y++)
		row_pointers[y] = (png_byte*) &data[3*x_size*y];

	png_read_image(png_ptr, row_pointers);
    
    png_read_end(png_ptr, NULL);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    free(row_pointers);

    fclose(fp);
}

int ImageRGB::save_png( char * filename )
{
    png_bytep * row_pointers;
    int y;
    
    timestamp_entry("save_png: entry\n");
    
    FILE *fp = fopen(filename, "wb");
    if (!fp)
    {
        perror("save_png: failed to open file");
        return(-1);
    }
    
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
       return (-2);

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
       png_destroy_write_struct(&png_ptr,
         (png_infopp)NULL);
       return (-3);
    }
    
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        timestamp_exit("failed...\n");
        return (-4);
    }
    
    png_init_io(png_ptr, fp);
    png_set_IHDR(png_ptr, info_ptr, x_size, y_size, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    
    png_write_info(png_ptr, info_ptr);
    
    row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * y_size);
	for (y=0; y<y_size; y++)
		row_pointers[y] = (png_byte*) &data[3*x_size*y];
    
    png_write_image(png_ptr, row_pointers);
	
    
    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    
    fclose(fp);
    free(row_pointers);
    timestamp_exit("done\n");
    return(0);
}



/*  ************************ ImageL stuff ****************************** */
void ImageL::png_abort(const char * s, ...)
{
	va_list args;
	va_start(args, s);
	vfprintf(stderr, s, args);
	fprintf(stderr, "\n");
	va_end(args);
	abort();
}

void ImageL::load_png( char * filename )
{
    png_structp png_ptr;
    png_infop info_ptr;
	char header[8];	// 8 is the maximum size that can be checked

    int y;
    png_byte color_type;
    png_byte bit_depth;
    int number_of_passes;
    png_bytep * row_pointers;
    
	/* open file and test for it being a png */
	FILE *fp = fopen(filename, "rb");
	if (!fp)
		png_abort("[read_png_file] File %s could not be opened for reading", filename);
	fread(header, 1, 8, fp);
	if (png_sig_cmp((png_byte *) header, 0, 8))
		png_abort("[read_png_file] File %s is not recognized as a PNG file", filename);


	/* initialize stuff */
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
	if (!png_ptr)
		png_abort("[read_png_file] png_create_read_struct failed");

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
		png_abort("[read_png_file] png_create_info_struct failed");

	if (setjmp(png_jmpbuf(png_ptr)))
		png_abort("[read_png_file] Error during init_io");

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	x_size = info_ptr->width;
	y_size = info_ptr->height;
	color_type = info_ptr->color_type;
	bit_depth = info_ptr->bit_depth;

	number_of_passes = png_set_interlace_handling(png_ptr);
    
    if (bit_depth == 16)
        png_set_strip_16(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY &&
        bit_depth < 8) png_set_gray_1_2_4_to_8(png_ptr);
    if (color_type & PNG_COLOR_MASK_ALPHA)
        png_set_strip_alpha(png_ptr);
    if (color_type == PNG_COLOR_TYPE_RGB ||
        color_type == PNG_COLOR_TYPE_RGB_ALPHA)
          png_set_rgb_to_gray_fixed(png_ptr, 1, -1, -1);
    
	png_read_update_info(png_ptr, info_ptr);

	/* read file */
	if (setjmp(png_jmpbuf(png_ptr)))
		png_abort("[read_png_file] Error during read_image");
    
    alloc();

	row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * y_size);
	for (y=0; y<y_size; y++)
		row_pointers[y] = (png_byte*) &data[x_size*y];

	png_read_image(png_ptr, row_pointers);
    
    png_read_end(png_ptr, NULL);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    free(row_pointers);

    fclose(fp);
}

int ImageL::save_png( char * filename )
{
    png_bytep * row_pointers;
    int y;
    
    timestamp_entry("save_png: entry\n");
    
    FILE *fp = fopen(filename, "wb");
    if (!fp)
    {
        perror("save_png: failed to open file");
        return(-1);
    }
    
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
       return (-2);

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
       png_destroy_write_struct(&png_ptr,
         (png_infopp)NULL);
       return (-3);
    }
    
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        timestamp_exit("failed...\n");
        return (-4);
    }
    
    png_init_io(png_ptr, fp);
    png_set_IHDR(png_ptr, info_ptr, x_size, y_size, 8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    
    png_write_info(png_ptr, info_ptr);
    
    row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * y_size);
	for (y=0; y<y_size; y++)
		row_pointers[y] = (png_byte*) &data[x_size*y];
    
    png_write_image(png_ptr, row_pointers);
	
    
    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    
    fclose(fp);
    free(row_pointers);
    timestamp_exit("done\n");
    return(0);
}
