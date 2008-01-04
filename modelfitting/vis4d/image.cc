// image.cc
// Richard Seely 2007

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "util.hh"
#include "image.hh"

void read_pnm_header( FILE * f, char * magic, int * width, int * height )
{
    char buf[20];
    char chr;
    int pos;
    
    // read and verify magic code
    if (fread(&chr, sizeof(char), 1, f) != 1) { perror("read_pnm_header: fread 1 error"); throw(1); }
    if (chr != magic[0]) { perror("read_pnm_header: magic incorrect"); throw(1); }
    if (fread(&chr, sizeof(char), 1, f) != 1) { perror("read_pnm_header: fread 2 error"); throw(1); }
    if (chr != magic[1]) { perror("read_pnm_header: magic incorrect"); throw(1); }
    
    // eat white space
    if (fread(&chr, sizeof(char), 1, f) != 1) { perror("read_pnm_header: fread 3 error"); throw(1); }
    while ((chr == ' ') || (chr == '\t') || (chr == '\n'))
    {
        if (fread(&chr, sizeof(char), 1, f) != 1) { perror("read_pnm_header: fread 4 error"); throw(1); }
        if (chr == '#')
        {
            // chew up the comment
            while (chr != '\n')
                if (fread(&chr, sizeof(char), 1, f) != 1) { perror("read_pnm_header: fread 5 error"); throw(1); }
        }
    }
    
    // read in the width token
    pos = 0;
    while ((chr != ' ') && (chr != '\t') && (chr != '\n'))
    {
        buf[pos++] = chr;
        if (fread(&chr, sizeof(char), 1, f) != 1) { perror("read_pnm_header: fread 6 error"); throw(1); }
        if (pos >= 19) { perror("read_pnm_header: malformed header 1"); throw(1); }
    }
    buf[pos] = '\0';
    *width = atoi(buf);
    
    // eat white space
    while ((chr == ' ') || (chr == '\t') || (chr == '\n'))
    {
        if (fread(&chr, sizeof(char), 1, f) != 1) { perror("read_pnm_header: fread 7 error"); throw(1); }
        if (chr == '#')
        {
            // chew up the comment
            while (chr != '\n')
                if (fread(&chr, sizeof(char), 1, f) != 1) { perror("read_pnm_header: fread 8 error"); throw(1); }
        }
    }
    
    // read in the width token
    pos = 0;
    while ((chr != ' ') && (chr != '\t') && (chr != '\n'))
    {
        buf[pos++] = chr;
        if (fread(&chr, sizeof(char), 1, f) != 1) { perror("read_pnm_header: fread 9 error"); throw(1); }
        if (pos >= 19) { perror("read_pnm_header: malformed header 2"); throw(1); }
    }
    buf[pos] = '\0';
    *height = atoi(buf);
    
    // eat white space
    while ((chr == ' ') || (chr == '\t') || (chr == '\n'))
    {
        if (fread(&chr, sizeof(char), 1, f) != 1) { perror("read_pnm_header: fread 7 error"); throw(1); }
        if (chr == '#')
        {
            // chew up the comment
            while (chr != '\n')
                if (fread(&chr, sizeof(char), 1, f) != 1) { perror("read_pnm_header: fread 8 error"); throw(1); }
        }
    }
    
    // read in the maxval token
    pos = 0;
    while ((chr != ' ') && (chr != '\t') && (chr != '\n'))
    {
        buf[pos++] = chr;
        if (fread(&chr, sizeof(char), 1, f) != 1) { perror("read_pnm_header: fread 9 error"); throw(1); }
        if (pos >= 19) { perror("read_pnm_header: malformed header 2"); throw(1); }
    }
    buf[pos] = '\0';
    int maxval = atoi(buf);
    if (maxval != 255) { fprintf(stderr, "read_pnm_header: expected a maxval of 255\n"); throw(1); }
}

void ImageRGB::alloc( )
{
    if (data != NULL)
        delete [] data;
    data = new IMG_DTYPE[x_size * y_size * 3];
    
    for (int i=0; i < x_size * y_size * 3; i++)
        data[i] = 0;
}

ImageRGB::ImageRGB( int x, int y )
{
    timestamp_entry("ImageRGB(x,y):\n");
    data = NULL;
    x_size = x;
    y_size = y;
    
    alloc( );
    
    timestamp_exit("new ImageRGB created\n");
}

ImageRGB::ImageRGB( IMG_DTYPE * buf, int x, int y )
{
    timestamp_entry("ImageRGB(x,y):\n");
    data = NULL;
    x_size = x;
    y_size = y;
    
    data = new IMG_DTYPE[x_size * y_size * 3];
    memcpy(data, buf, x_size * y_size * 3 * sizeof(IMG_DTYPE));
    
    timestamp_exit("new ImageRGB created\n");
}

ImageRGB::ImageRGB( const ImageL& img, const bool isBayer, const bool isRGGB )
{
    IMG_DTYPE * src = img.data;
    IMG_DTYPE * dest;
    
    timestamp_entry("ImageRGB(ImageL):\n");
    
    data = NULL;
    x_size = img.x_size;
    y_size = img.y_size;
    
    alloc();
    
    dest = data;
    
    if (isBayer)
    {
        if (isRGGB)
            deBayer_RGGB(img);
        else
            deBayer_BGGR(img);
    }
    else
    {
        for (int pos=0; pos < x_size * y_size; pos++)
        {
            *dest++ = *src;
            *dest++ = *src;
            *dest++ = *src++;
        }
    }
    
    timestamp_exit("new ImageRGB created\n");
}

ImageRGB::ImageRGB( char * filename )
{
    timestamp_entry("ImageRGB(filename):\n");
    data = NULL;
    
    if (strcasestr(filename, ".ppm") != NULL)
        load_ppm(filename);
    else if (strcasestr(filename, ".png") != NULL)
        load_png(filename);
    else
    {
        timestamp_exit("failed to load file - extension not recognised\n");
        //delete this;
        throw(1);
        return;
    }
    
    if (data == NULL)
    {
        timestamp_exit("failed to load file\n");
        //delete this;
        throw(1);
        return;
    }
    
    timestamp_exit("new ImageRGB created from file\n");
}

ImageRGB::ImageRGB( const ImageRGB& img )
{
    timestamp_entry("ImageRGB(ImageRGB):\n");
    x_size = img.x_size;
    y_size = img.y_size;
    data = NULL;
    alloc();
    memcpy( data, img.data, x_size * y_size * 3 * sizeof(IMG_DTYPE) );
    timestamp_entry("new ImageRGB copy created\n");
}

ImageRGB::~ImageRGB( )
{
    timestamp_entry("~ImageRGB( )\n");
    if (data!=NULL)
        delete [] data;
    timestamp_exit("goodbye cruel world\n");
}

void ImageRGB::load_ppm( char * filename )
{
    FILE * f;
    int n, tmp;

    if ((f = fopen(filename, "rb")) == NULL)
    {
        perror("ImageRGB::load_ppm unable to open file");
        throw(1);
        return;
    }
    
    read_pnm_header(f, "P6", &x_size, &y_size);
    
    alloc( );
    n =fread(data, sizeof(unsigned char), 3 * x_size * y_size, f); 
    if ( n != 3 * x_size * y_size) {
        perror("ImageRGB::load_ppm: failed to read image data");
        fprintf(stderr, "read %d bytes, expected %d.\n",  n, 3 * x_size * y_size);
        delete [] data;
        data=NULL;
        x_size = y_size = 0;
        fclose(f);
        return;
    }
    fclose(f);
}

void ImageRGB::save_ppm( char * filename )
{
    FILE * f;
    timestamp_entry("ImageRGB::save_ppm( ):\n");
    if ((x_size <= 0) || (y_size <= 0) || (data == NULL))
    {
        timestamp_inside("invalid ImageRGB\n");
        return;
    }
        
    if ((f = fopen(filename, "wb")) == NULL)
    {
        perror("ImageRGB::save_ppm: unable to open file");
        return;
    }
    fprintf(f, "P6 %d %d %d ", x_size, y_size, 255);
    
    if (fwrite(data, sizeof(unsigned char), 3 * x_size * y_size, f) != 3 * x_size * y_size) {
        perror("ImageRGB::save_ppm: failed to save image data");
        return;
    }
    fclose(f);
    timestamp_exit("ImageRGB::save_ppm( ) done.\n");
}

void ImageRGB::flip_vertical( )
{
    IMG_DTYPE swp;
    IMG_DTYPE * top;
    IMG_DTYPE * bot;
    int px, py, ps, ph;
    
    ps = x_size * 3;
    ph = y_size / 2;
    
    // top row
    top = data;
    // bottom row
    bot = data;
    bot += x_size * 3 * (y_size - 1);
    
    for (py=0; py < ph; py++)
    {
        for (px=0; px < ps; px++)
        {
            swp = *top;
            *top++ = *bot;
            *bot++ = swp;
        }
        bot -= ps * 2;
    }
}

void ImageRGB::deBayer_BGGR(const ImageL& img)
{
    IMG_DTYPE * src = img.data;
    IMG_DTYPE * dest = data;
    int hx = x_size / 2;
    int hy = y_size / 2;
    int yskip = x_size * 3;
    int x,y;

    timestamp_entry("ImageRGB.deBayer_BGGR(ImageL):\n");
    
    // R(0,0)
    *dest++ = src[1 + x_size];
    // G(0,0)
    *dest++ = src[x_size];
    // B(0,0)
    *dest++ = src[0];
    src++;
    // R(0,1)
    *dest++ = src[x_size];
    // G(0,1)
    *dest++ = src[0];
    // B(0,1)
    *dest++ = (IMG_DTYPE) ((((int) src[-1]) + ((int) src[1])) >> 1);
    src++;
    for (x=1; x < hx-1; x++)
    {
        // B G B G B G
        // G R G R G R
        // B G-B-G B G
        // G R G R G R
        // B G B G B G
        // G R G R G R
        
        // R(0,0)
        *dest++ = (IMG_DTYPE) ((((int) src[-1 + x_size]) + ((int) src[1 + x_size])) >> 1);
        // G(0,0)
        *dest++ = (IMG_DTYPE) ((((int) src[-1]) + ((int) src[1])) >> 1);
        // B(0,0)
        *dest++ = src[0];
        src++;
        // R(0,1)
        *dest++ = src[x_size];
        // G(0,1)
        *dest++ = src[0];
        // B(0,1)
        *dest++ = (IMG_DTYPE) ((((int) src[-1]) + ((int) src[1])) >> 1);
        src++;
    }
    // R(0,0)
    *dest++ = (IMG_DTYPE) ((((int) src[-1 + x_size]) + ((int) src[1 + x_size])) >> 1);
    // G(0,0)
    *dest++ = (IMG_DTYPE) ((((int) src[-1]) + ((int) src[1])) >> 1);
    // B(0,0)
    *dest++ = src[0];
    src++;
    // R(0,1)
    *dest++ = src[x_size];
    // G(0,1)
    *dest++ = src[0];
    // B(0,1)
    *dest++ = src[-1];
    src++;
    if (x_size % 2)
    {
        src++;
        *dest++ = 0;
        *dest++ = 0;
        *dest++ = 0;
    }
    
    // R(1,0)
    *dest++ = src[1];
    // G(1,0)
    *dest++ = src[0];
    // B(1,0)
    *dest++ = (IMG_DTYPE) ((((int) src[-x_size]) + ((int) src[x_size])) >> 1);
    src++;
    // R(1,1)
    *dest++ = src[0];
    // G(1,1)
    *dest++ = (IMG_DTYPE) ((((int) src[-1]) + ((int) src[1]) + ((int) src[-x_size]) + ((int) src[x_size])) >> 2);
    // B(1,1)
    *dest++ = (IMG_DTYPE) ((((int) src[-1 - x_size]) + ((int) src[1 - x_size]) + ((int) src[-1 + x_size]) + ((int) src[1 + x_size])) >> 2);
    src++;
    for (x=1; x < hx-1; x++)
    {
        // B G B G B G
        // G R G R G R
        // B G-B-G B G
        // G R G R G R
        // B G B G B G
        // G R G R G R
        
        // R(1,0)
        *dest++ = (IMG_DTYPE) ((((int) src[-1]) + ((int) src[1])) >> 1);
        // G(1,0)
        *dest++ = src[0];
        // B(1,0)
        *dest++ = (IMG_DTYPE) ((((int) src[-x_size]) + ((int) src[x_size])) >> 1);
        src++;
        // R(1,1)
        *dest++ = src[0];
        // G(1,1)
        *dest++ = (IMG_DTYPE) ((((int) src[-1]) + ((int) src[1]) + ((int) src[-x_size]) + ((int) src[x_size])) >> 2);
        // B(1,1)
        *dest++ = (IMG_DTYPE) ((((int) src[-1 - x_size]) + ((int) src[1 - x_size]) + ((int) src[-1 + x_size]) + ((int) src[1 + x_size])) >> 2);
        src++;
    }
    // R(1,0)
    *dest++ = (IMG_DTYPE) ((((int) src[-1]) + ((int) src[1])) >> 1);
    // G(1,0)
    *dest++ = src[0];
    // B(1,0)
    *dest++ = (IMG_DTYPE) ((((int) src[-x_size]) + ((int) src[x_size])) >> 1);
    src++;
    // R(1,1)
    *dest++ = src[0];
    // G(1,1)
    *dest++ = (IMG_DTYPE) ((((int) src[-x_size]) + ((int) src[x_size])) >> 1);
    // B(1,1)
    *dest++ = (IMG_DTYPE) ((((int) src[-1 - x_size]) + ((int) src[-1 + x_size])) >> 1);
    src++;
    if (x_size % 2)
    {
        src++;
        *dest++ = 0;
        *dest++ = 0;
        *dest++ = 0;
    }
    
    ///////////////////////////////
    
    for (y=1; y < hy-1; y++)
    {
        // R(0,0)
        *dest++ = (IMG_DTYPE) ((((int) src[1 - x_size]) + ((int) src[1 + x_size])) >> 1);
        // G(0,0)
        *dest++ = (IMG_DTYPE) ((((int) src[-x_size]) + ((int) src[x_size])) >> 1);
        // B(0,0)
        *dest++ = src[0];
        src++;
        // R(0,1)
        *dest++ = (IMG_DTYPE) ((((int) src[-x_size]) + ((int) src[x_size])) >> 1);
        // G(0,1)
        *dest++ = src[0];
        // B(0,1)
        *dest++ = (IMG_DTYPE) ((((int) src[-1]) + ((int) src[1])) >> 1);
        src++;
        for (x=1; x < hx-1; x++)
        {
            // B G B G B G
            // G R G R G R
            // B G-B-G B G
            // G R G R G R
            // B G B G B G
            // G R G R G R
            
            // R(0,0)
            *dest++ = (IMG_DTYPE) ((((int) src[-1 - x_size]) + ((int) src[1 - x_size]) + ((int) src[-1 + x_size]) + ((int) src[1 + x_size])) >> 2);
            // G(0,0)
            *dest++ = (IMG_DTYPE) ((((int) src[-1]) + ((int) src[1]) + ((int) src[-x_size]) + ((int) src[x_size])) >> 2);
            // B(0,0)
            *dest++ = src[0];
            src++;
            // R(0,1)
            *dest++ = (IMG_DTYPE) ((((int) src[-x_size]) + ((int) src[x_size])) >> 1);
            // G(0,1)
            *dest++ = src[0];
            // B(0,1)
            *dest++ = (IMG_DTYPE) ((((int) src[-1]) + ((int) src[1])) >> 1);
            src++;
        }
        // R(0,0)
        *dest++ = (IMG_DTYPE) ((((int) src[-1 - x_size]) + ((int) src[1 - x_size]) + ((int) src[-1 + x_size]) + ((int) src[1 + x_size])) >> 2);
        // G(0,0)
        *dest++ = (IMG_DTYPE) ((((int) src[-1]) + ((int) src[1]) + ((int) src[-x_size]) + ((int) src[x_size])) >> 2);
        // B(0,0)
        *dest++ = src[0];
        src++;
        // R(0,1)
        *dest++ = (IMG_DTYPE) ((((int) src[-x_size]) + ((int) src[x_size])) >> 1);
        // G(0,1)
        *dest++ = src[0];
        // B(0,1)
        *dest++ = src[-1];
        src++;
        if (x_size % 2)
        {
            src++;
            *dest++ = 0;
            *dest++ = 0;
            *dest++ = 0;
        }
        
        // R(1,0)
        *dest++ = src[1];
        // G(1,0)
        *dest++ = src[0];
        // B(1,0)
        *dest++ = (IMG_DTYPE) ((((int) src[-x_size]) + ((int) src[x_size])) >> 1);
        src++;
        // R(1,1)
        *dest++ = src[0];
        // G(1,1)
        *dest++ = (IMG_DTYPE) ((((int) src[-1]) + ((int) src[1]) + ((int) src[-x_size]) + ((int) src[x_size])) >> 2);
        // B(1,1)
        *dest++ = (IMG_DTYPE) ((((int) src[-1 - x_size]) + ((int) src[1 - x_size]) + ((int) src[-1 + x_size]) + ((int) src[1 + x_size])) >> 2);
        src++;
        for (x=1; x < hx-1; x++)
        {
            // B G B G B G
            // G R G R G R
            // B G-B-G B G
            // G R G R G R
            // B G B G B G
            // G R G R G R
            
            // R(1,0)
            *dest++ = (IMG_DTYPE) ((((int) src[-1]) + ((int) src[1])) >> 1);
            // G(1,0)
            *dest++ = src[0];
            // B(1,0)
            *dest++ = (IMG_DTYPE) ((((int) src[-x_size]) + ((int) src[x_size])) >> 1);
            src++;
            // R(1,1)
            *dest++ = src[0];
            // G(1,1)
            *dest++ = (IMG_DTYPE) ((((int) src[-1]) + ((int) src[1]) + ((int) src[-x_size]) + ((int) src[x_size])) >> 2);
            // B(1,1)
            *dest++ = (IMG_DTYPE) ((((int) src[-1 - x_size]) + ((int) src[1 - x_size]) + ((int) src[-1 + x_size]) + ((int) src[1 + x_size])) >> 2);
            src++;
        }
        // R(1,0)
        *dest++ = (IMG_DTYPE) ((((int) src[-1]) + ((int) src[1])) >> 1);
        // G(1,0)
        *dest++ = src[0];
        // B(1,0)
        *dest++ = (IMG_DTYPE) ((((int) src[-x_size]) + ((int) src[x_size])) >> 1);
        src++;
        // R(1,1)
        *dest++ = src[0];
        // G(1,1)
        *dest++ = (IMG_DTYPE) ((((int) src[-x_size]) + ((int) src[x_size])) >> 1);
        // B(1,1)
        *dest++ = (IMG_DTYPE) ((((int) src[-1 - x_size]) + ((int) src[-1 + x_size])) >> 1);
        src++;
        if (x_size % 2)
        {
            src++;
            *dest++ = 0;
            *dest++ = 0;
            *dest++ = 0;
        }
    }
    ///////////////////////////////////////////
    
    // R(0,0)
    *dest++ = (IMG_DTYPE) ((((int) src[1 - x_size]) + ((int) src[1 + x_size])) >> 1);
    // G(0,0)
    *dest++ = (IMG_DTYPE) ((((int) src[-x_size]) + ((int) src[x_size])) >> 1);
    // B(0,0)
    *dest++ = src[0];
    src++;
    // R(0,1)
    *dest++ = (IMG_DTYPE) ((((int) src[-x_size]) + ((int) src[x_size])) >> 1);
    // G(0,1)
    *dest++ = src[0];
    // B(0,1)
    *dest++ = (IMG_DTYPE) ((((int) src[-1]) + ((int) src[1])) >> 1);
    src++;
    for (x=1; x < hx-1; x++)
    {
        // B G B G B G
        // G R G R G R
        // B G-B-G B G
        // G R G R G R
        // B G B G B G
        // G R G R G R
        
        // R(0,0)
        *dest++ = (IMG_DTYPE) ((((int) src[-1 - x_size]) + ((int) src[1 - x_size]) + ((int) src[-1 + x_size]) + ((int) src[1 + x_size])) >> 2);
        // G(0,0)
        *dest++ = (IMG_DTYPE) ((((int) src[-1]) + ((int) src[1]) + ((int) src[-x_size]) + ((int) src[x_size])) >> 2);
        // B(0,0)
        *dest++ = src[0];
        src++;
        // R(0,1)
        *dest++ = (IMG_DTYPE) ((((int) src[-x_size]) + ((int) src[x_size])) >> 1);
        // G(0,1)
        *dest++ = src[0];
        // B(0,1)
        *dest++ = (IMG_DTYPE) ((((int) src[-1]) + ((int) src[1])) >> 1);
        src++;
    }
    // R(0,0)
    *dest++ = (IMG_DTYPE) ((((int) src[-1 - x_size]) + ((int) src[1 - x_size]) + ((int) src[-1 + x_size]) + ((int) src[1 + x_size])) >> 2);
    // G(0,0)
    *dest++ = (IMG_DTYPE) ((((int) src[-1]) + ((int) src[1]) + ((int) src[-x_size]) + ((int) src[x_size])) >> 2);
    // B(0,0)
    *dest++ = src[0];
    src++;
    // R(0,1)
    *dest++ = (IMG_DTYPE) ((((int) src[-x_size]) + ((int) src[x_size])) >> 1);
    // G(0,1)
    *dest++ = src[0];
    // B(0,1)
    *dest++ = src[-1];
    src++;
    if (x_size % 2)
    {
        src++;
        *dest++ = 0;
        *dest++ = 0;
        *dest++ = 0;
    }
    
    
    // R(1,0)
    *dest++ = src[1];
    // G(1,0)
    *dest++ = src[0];
    // B(1,0)
    *dest++ = src[-x_size];
    src++;
    // R(1,1)
    *dest++ = src[0];
    // G(1,1)
    *dest++ = (IMG_DTYPE) ((((int) src[-1]) + ((int) src[1])) >> 1);
    // B(1,1)
    *dest++ = (IMG_DTYPE) ((((int) src[-1 - x_size]) + ((int) src[1 - x_size])) >> 1);
    src++;
    for (x=1; x < hx-1; x++)
    {
        // B G B G B G
        // G R G R G R
        // B G-B-G B G
        // G R G R G R
        // B G B G B G
        // G R G R G R
        
        // R(1,0)
        *dest++ = (IMG_DTYPE) ((((int) src[-1]) + ((int) src[1])) >> 1);
        // G(1,0)
        *dest++ = src[0];
        // B(1,0)
        *dest++ = src[-x_size];
        src++;
        // R(1,1)
        *dest++ = src[0];
        // G(1,1)
        *dest++ = (IMG_DTYPE) ((((int) src[-1]) + ((int) src[1])) >> 1);
        // B(1,1)
        *dest++ = (IMG_DTYPE) ((((int) src[-1 - x_size]) + ((int) src[1 - x_size])) >> 1);
        src++;
    }
    // R(1,0)
    *dest++ = (IMG_DTYPE) ((((int) src[-1]) + ((int) src[1])) >> 1);
    // G(1,0)
    *dest++ = src[0];
    // B(1,0)
    *dest++ = src[-x_size];
    src++;
    // R(1,1)
    *dest++ = src[0];
    // G(1,1)
    *dest++ = src[-x_size];
    // B(1,1)
    *dest = src[-1 - x_size];

    timestamp_exit("ImageRGB:deBayer_BGGR(ImageL) finished\n");
}

void ImageRGB::deBayer_RGGB(const ImageL& img)
{
    IMG_DTYPE * swp1 = data;
    IMG_DTYPE * swp2 = data + 2;
    IMG_DTYPE    swp3;
    int x,y;

    timestamp_entry("ImageRGB.deBayer_RGGB(ImageL):\n");
  
    deBayer_BGGR(img);
    
    for (y=0; y < y_size; y++)
        for (x=0; x < x_size; x++)
        {
            swp3 = *swp1;
            *swp1 = *swp2;
            *swp2 = swp3;
            swp1 += 3;
            swp2 += 3;
        }    
    timestamp_exit("ImageRGB:deBayer_RGGB(ImageL) finished\n");
}

void ImageRGB::normalise(const int damping, const bool weighted)
{
    IMG_DTYPE * src = data;
    int sum;
    
    if (weighted)
    {
        const int damping1 = damping + 28;
        for (int pos=0; pos < x_size * y_size; pos++)
        {
            sum = ( ((int) src[0]) * 77 + ((int) src[1]) * 151 + ((int) src[2]) * 28) >> 8;
            *src++ = (IMG_DTYPE) ( (((int) *src) * damping1) / (sum + damping) );
            *src++ = (IMG_DTYPE) ( (((int) *src) * damping1) / (sum + damping) );
            *src++ = (IMG_DTYPE) ( (((int) *src) * damping1) / (sum + damping) );
        }
    }
    else
    {
        const int damping1 = damping + 255;
        for (int pos=0; pos < x_size * y_size; pos++)
        {
            sum = ((int) src[0]) + ((int) src[1]) + ((int) src[2]);
            *src++ = (IMG_DTYPE) ( (((int) *src) * damping1) / (sum + damping) );
            *src++ = (IMG_DTYPE) ( (((int) *src) * damping1) / (sum + damping) );
            *src++ = (IMG_DTYPE) ( (((int) *src) * damping1) / (sum + damping) );
        }
    }
}

/* ************ start of ImageL class ******************** */

void ImageL::alloc( )
{
    if (data != NULL)
        delete [] data;
    data = new IMG_DTYPE[x_size * y_size];
    
    for (int i=0; i < x_size * y_size; i++)
        data[i] = 0;
}

ImageL::ImageL( )
{
    timestamp_entry("ImageL( ):\n");
    data = NULL;
    x_size = -1;
    y_size = -1;
    
    timestamp_exit("new (empty) ImageL created\n");
}

ImageL::ImageL( int x, int y )
{
    timestamp_entry("ImageL(x,y):\n");
    data = NULL;
    x_size = x;
    y_size = y;
    
    alloc( );
    
    timestamp_exit("new ImageL created\n");
}

ImageL::ImageL( IMG_DTYPE * buf, int x, int y )
{
    timestamp_entry("ImageL(x,y):\n");
    data = NULL;
    x_size = x;
    y_size = y;
    
    data = new IMG_DTYPE[x_size * y_size];
    memcpy(data, buf, x_size * y_size * sizeof(IMG_DTYPE));
    
    timestamp_exit("new ImageL created\n");
}

/* assume bayer pattern is:
BG
BR
*/

ImageL::ImageL( const ImageRGB& img, const bool isBayer )
{
    IMG_DTYPE * src = img.data;
    IMG_DTYPE * dest;
    
    timestamp_entry("ImageL(ImageRGB):\n");
    
    data = NULL;
    x_size = img.x_size;
    y_size = img.y_size;
    
    alloc();
    
    dest = data;
    
    if (isBayer)
    {
        int x,y;
        for (y=0; y < y_size; y++)
            for (x=0; x < x_size; x++)
            {
                *dest++ = ((y + x) % 2 ? src[1] : ( y % 2 ? src[0] : src[2] ) );
                src += 3;
            }
    }
    else
    {
        for (int pos=0; pos < x_size * y_size; pos++)
        {
            *dest++ = (IMG_DTYPE) (( ((int) src[0]) * 77 + ((int) src[1]) * 151 + ((int) src[2]) * 28) >> 8);
            src += 3;
        }
    }
    
    timestamp_exit("new ImageL created\n");
}

ImageL::ImageL( char * filename )
{
    timestamp_entry("ImageL(filename):\n");
    data = NULL;
    
    if (strcasestr(filename, ".pgm") != NULL)
        load_ppm(filename);
    else if (strcasestr(filename, ".png") != NULL)
        load_png(filename);
    else
    {
        timestamp_exit("failed to load file - extension not recognised\n");
        //delete this;
        throw(1);
        return;
    }
    
    if (data == NULL)
    {
        timestamp_exit("failed to load file\n");
        //delete this;
        throw(1);
        return;
    }
    
    timestamp_exit("new ImageL created from file\n");
}

ImageL::ImageL( const ImageL& img )
{
    timestamp_entry("ImageL(ImageL):\n");
    x_size = img.x_size;
    y_size = img.y_size;
    data = NULL;
    alloc();
    memcpy( data, img.data, x_size * y_size * sizeof(IMG_DTYPE) );
    timestamp_entry("new ImageL copy created\n");
}

ImageL::~ImageL( )
{
    timestamp_entry("~ImageL( )\n");
    if (data!=NULL)
        delete [] data;
    data = NULL;
    timestamp_exit("goodbye cruel world\n");
}

void ImageL::load_ppm( char * filename )
{
    FILE * f;
    int n;

    if ((f = fopen(filename, "rb")) == NULL)
    {
        perror("ImageL::load_ppm unable to open file");
        throw(1);
        return;
    }
    
    read_pnm_header(f, "P5", &x_size, &y_size);
    
    alloc( );
    n =fread(data, sizeof(unsigned char), x_size * y_size, f); 
    if ( n != x_size * y_size) {
        perror("ImageL::load_ppm: failed to read image data");
        fprintf(stderr, "read %d bytes, expected %d.\n",  n, x_size * y_size);
        delete [] data;
        data=NULL;
        x_size = y_size = 0;
        fclose(f);
        return;
    }
    fclose(f);
}

void ImageL::save_ppm( char * filename )
{
    FILE * f;
    timestamp_entry("ImageL::save_ppm( ):\n");
    if ((x_size <= 0) || (y_size <= 0) || (data == NULL))
    {
        timestamp_inside("invalid ImageL\n");
        return;
    }
        
    if ((f = fopen(filename, "wb")) == NULL)
    {
        perror("ImageL::save_ppm: unable to open file");
        return;
    }
    fprintf(f, "P5 %d %d %d ", x_size, y_size, 255);
    
    if (fwrite(data, sizeof(unsigned char), x_size * y_size, f) != x_size * y_size) {
        perror("ImageL::save_ppm: failed to save image data");
        return;
    }
    fclose(f);
    timestamp_exit("ImageL::save_ppm( ) done.\n");
}

void ImageL::flip_vertical( )
{
    IMG_DTYPE swp;
    IMG_DTYPE * top;
    IMG_DTYPE * bot;
    int px, py, ps, ph;
    
    ph = y_size / 2;
    
    // top row
    top = data;
    // bottom row
    bot = data;
    bot += x_size * (y_size - 1);
    
    for (py=0; py < ph; py++)
    {
        for (px=0; px < x_size; px++)
        {
            swp = *top;
            *top++ = *bot;
            *bot++ = swp;
        }
        bot -= x_size * 2;
    }
}
