// scene.hh
// Richard Seely 2007

#ifndef _scene_HH
#define _scene_HH
#include "util.hh"
#include "vspace.hh"
#include "vfield.hh"
#include "image.hh"
#include <map>
#include <math.h>

using namespace std;

typedef int VLOOKUP_DTYPE;

struct ltstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};

class View
{
    public:
        View( const char * vname, const MATRIX_3x4& pmat);
        ~View( );
        void Print( );
        void Set_Image( const char * filename );
        void Set_Env( const Voxel_Space& vspc );
        void Set_Env( const Voxel_Field& vfld );
        void Set_P( const MATRIX_3x4& P_matrix );
        void Scale_P( float mx, float my, float mz, float mw);
        inline void Force_Calc( ) { calc( ); }
        void Force_Alloc( const Point_XYZ& lmin, const Point_XYZ& lmax );
        void Default_Alloc( const Point_XYZ& lmin, const Point_XYZ& lmax );
        bool Get_RGB( float px, float py, float pz, VFLD_DTYPE * pR, VFLD_DTYPE * pG, VFLD_DTYPE * pB );
        // unsafe (fast) version of above - does not bound check or check that stuff has been initialised
        inline void _Get_RGB( int px, int py, int pz, VFLD_DTYPE * pR, VFLD_DTYPE * pG, VFLD_DTYPE * pB )
        {
            VLOOKUP_DTYPE look;
            
            look = lookup[ (pz * y_size + py) * x_size + px ];
            if (look == -1)
            {
                *pR = 0;
                *pG = 0;
                *pB = 0;
            }
            else
            {
                *pR = pImage->data[look];
                *pG = pImage->data[look+1];
                *pB = pImage->data[look+2];
            }
        }
        void Fill_RGB( Voxel_Field * fR, Voxel_Field * fG, Voxel_Field * fB );
        inline void Save_Image( const char * filename ) { if (pImage != NULL)   pImage->save_ppm( (char *) filename ); }
        inline float Min_X( ) { return(origin.x); }
        inline float Min_Y( ) { return(origin.y); }
        inline float Min_Z( ) { return(origin.z); }
        inline float Max_X( ) { return(max.x); }
        inline float Max_Y( ) { return(max.y); }
        inline float Max_Z( ) { return(max.z); }
        
        char * name;
        Point_XYZ Position;
        
    private:
        void alloc( );
        void calc( );
        void load_lut( );
        void save_lut( );
        inline bool lookup_big_enough( const Voxel_Field& vfld )
        {
            if ( (roundf(vfld.origin.x) < roundf(origin.x)) || (roundf(vfld.origin.y) < roundf(origin.y)) || (roundf(vfld.origin.z) < roundf(origin.z)) || (roundf(vfld.origin.x + ((float) vfld.x_size)) > roundf(max.x)) || (roundf(vfld.origin.y + ((float) vfld.y_size)) > roundf(max.y)) || (roundf(vfld.origin.z + ((float) vfld.z_size)) > roundf(max.z)) )
                return(false);
            else
                return(true);
        }
        inline bool lookup_big_enough( const Voxel_Space& vspc )
        {
            if ( (roundf(vspc.origin.x) < roundf(origin.x)) || (roundf(vspc.origin.y) < roundf(origin.y)) || (roundf(vspc.origin.z) < roundf(origin.z)) || (roundf(vspc.origin.x + ((float) vspc.x_size)) > roundf(max.x)) || (roundf(vspc.origin.y + ((float) vspc.y_size)) > roundf(max.y)) || (roundf(vspc.origin.z + ((float) vspc.z_size)) > roundf(max.z)) )
                return(false);
            else
                return(true);
        }
    
        bool _calc;
        bool _calcP;
        VLOOKUP_DTYPE * lookup;
        int x_size;
        int y_size;
        int z_size;
        Point_XYZ origin;
        Point_XYZ max;
        ImageRGB * pImage;
        MATRIX_3x4 P_matrix;
        MATRIX_3x4 newP;
};

typedef map<const char *, View *, ltstr> View_Map;

class Scene
{
    public:
        Scene(char * filename );
        ~Scene( );
        void Print( );
        void Set_Env( const Voxel_Space& vspc );
        void Set_Env( const Voxel_Field& vfld );
        void Scale_P( float mx, float my, float mz, float mw);
        void Force_Alloc( const Point_XYZ& lmin, const Point_XYZ& lmax );
        void Default_Alloc( const Point_XYZ& lmin, const Point_XYZ& lmax );
        void Force_Calc( );
        void Guess_Colour( float fx, float fy, float fz, float nx, float ny, float nz, float * red, float * green, float * blue );
    
        View_Map views;
};

#endif
