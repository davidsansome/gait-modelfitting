// image.hh
// Richard Seely 2007

#ifndef _IMAGE_HH
#define _IMAGE_HH
#include "util.hh"

typedef unsigned char IMG_DTYPE;

class ImageL;

class ImageRGB
{
    public:
        IMG_DTYPE * data;
        int x_size;
        int y_size;
       
        ImageRGB( int x, int y );
        ImageRGB( IMG_DTYPE * buf, int x, int y);
        ImageRGB( const ImageL& img, const bool isBayer, const bool isRGGB );
        ImageRGB( char * filename );
        ImageRGB( const ImageRGB& img );
        ~ImageRGB( );
        
        void load_ppm( char * filename );
        void save_ppm( char * filename );
        void load_png( char * filename );
        int save_png( char * filename );
        inline IMG_DTYPE get_red(int x, int y) { return( ( (x < 0) || (y < 0) || ~(x < x_size) || ~ (y < y_size) ? 0 : data[3 * (y * x_size + x)] ) ); }
        inline IMG_DTYPE get_green(int x, int y) { return( ( (x < 0) || (y < 0) || ~(x < x_size) || ~ (y < y_size) ? 0 : data[3 * (y * x_size + x) + 1] ) ); }
        inline IMG_DTYPE get_blue(int x, int y) { return( ( (x < 0) || (y < 0) || ~(x < x_size) || ~ (y < y_size) ? 0 : data[3 * (y * x_size + x) + 2] ) ); }
        inline void set_red(int x, int y, IMG_DTYPE value) { if ( (x>=0) && (y>=0) && (x < x_size) && (y < y_size) )  data[3 * (y * x_size + x)] = value; }
        inline void set_green(int x, int y, IMG_DTYPE value) { if ( (x>=0) && (y>=0) && (x < x_size) && (y < y_size) )  data[3 * (y * x_size + x)+1] = value; }
        inline void set_blue(int x, int y, IMG_DTYPE value) { if ( (x>=0) && (y>=0) && (x < x_size) && (y < y_size) )  data[3 * (y * x_size + x)+2] = value; }
            
        void flip_vertical( );
        void deBayer_BGGR(const ImageL& img);
        void deBayer_RGGB(const ImageL& img);
        void normalise(const int damping, const bool weighted);
            
    private:
        void alloc( );
        void png_abort(const char * s, ...);
};

class ImageL
{
    public:
        IMG_DTYPE * data;
        int x_size;
        int y_size;
       
        ImageL( );
        ImageL( int x, int y );
        ImageL( IMG_DTYPE * buf, int x, int y);
        ImageL( char * filename );
        ImageL( const ImageRGB& img, const bool isBayer );
        ImageL( const ImageL& img );
        ~ImageL( );
        
        void load_ppm( char * filename );
        void save_ppm( char * filename );
        void load_png( char * filename );
        int save_png( char * filename );
    
        inline IMG_DTYPE get_lum(int x, int y) { return( ( (x < 0) || (y < 0) || ~(x < x_size) || ~ (y < y_size) ? 0 : data[y * x_size + x] ) ); }
        inline void set_lum(int x, int y, IMG_DTYPE value) { if ( (x>=0) && (y>=0) && (x < x_size) && (y < y_size) )  data[y * x_size + x] = value; }
            
        void flip_vertical( );
    
    private:
        void alloc( );
        void png_abort(const char * s, ...);
};

#endif
