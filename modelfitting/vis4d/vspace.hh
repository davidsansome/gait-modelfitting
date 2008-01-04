// vspace.hh
// A set of routines to create and manipulate a simple binary volumetric space
// Richard Seely - University of Southampton, UK 2006

#ifndef _VSPACE_HH
#define _VSPACE_HH
#include "util.hh"
#include "points.hh"
#include "vfield.hh"

#define __X86_64__ 1

#ifdef __X86_64__
    typedef unsigned long VSPC_DTYPE;
    #define VSPC_ONE 1l
#else
    typedef unsigned int VSPC_DTYPE;
    #define VSPC_ONE 1
#endif

class Voxel_Space
{
    public:
        VSPC_DTYPE * data;
        int x_size;
        int y_size;
        int z_size;
        int x_span;
        Point_XYZ origin;
        int data_width;
        int data_width_m1;
        
        Voxel_Space( int x, int y, int z );
        Voxel_Space( const Point_XYZ_List& lst );
        Voxel_Space( char * filename );
        Voxel_Space( const Voxel_Space& vspc );
        Voxel_Space( const Voxel_Field& vfld );
        ~Voxel_Space( );
    
        void save(char * filename);
        void saveA(char * filename);
        void saveZ(char * filename);
        void border(int r);
        void invert( );
        void edge_detect( );
        void dilate(int r);
        void erode(int r);
        
        void dilate_fast(int r);
        void dilate_slow(int r);
        void erode_fast(int r);
        void erode_slow(int r);
        
        #ifdef _VSPACE_USE_OPTIMISED_DILATE
        void dilate_r1();
        void dilate_r2();
        void dilate_r3();
        void dilate_r4();
        void dilate_r5();
        void dilate_r6();
        void dilate_r7();
        void dilate_r8();
        void dilate_r9();
        void dilate_r10();
        void dilate_r11();
        void dilate_r12();
        #endif
        
        #ifdef _VSPACE_USE_OPTIMISED_ERODE
        void erode_r1();
        void erode_r2();
        void erode_r3();
        void erode_r4();
        void erode_r5();
        void erode_r6();
        void erode_r7();
        void erode_r8();
        void erode_r9();
        void erode_r10();
        void erode_r11();
        void erode_r12();
        #endif
        
        bool get(int x, int y, int z);
        void set(int x, int y, int z, bool value);
    
    private:
        void alloc( );
};

#endif
