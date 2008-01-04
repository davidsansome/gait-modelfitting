// vfield.hh
// A set of routines to create and manipulate a simple volumetric space
// Richard Seely - University of Southampton, UK 2006

#ifndef _VFIELD_HH
#define _VFIELD_HH
#include "util.hh"
#include "points.hh"
#include "vspace.hh"

typedef struct _REGION {
    short int qval;
    int link;
} REGION;

typedef short int VFLD_DTYPE;
class Voxel_Field
{
    public:
        VFLD_DTYPE * data;
        int x_size;
        int y_size;
        int z_size;
        Point_XYZ origin;
        
        Voxel_Field( int x, int y, int z );
        Voxel_Field( const Point_XYZ_List& lst );
        Voxel_Field( char * filename );
        Voxel_Field( const Voxel_Space& vspc );
        Voxel_Field( const Voxel_Field& vfld );
        ~Voxel_Field( );
    
        VFLD_DTYPE get(int x, int y, int z);
        void set(int x, int y, int z, VFLD_DTYPE value);
    
        void save(char * filename);
        
        void border(int r);
        void crop(int sx, int sy, int sz, int ex, int ey, int ez);
        void uncrop(const Voxel_Field& vfld);
        void uncrop(const Voxel_Space& vspc);
        
        void scalar_add(VFLD_DTYPE v);
        void scalar_multiply(VFLD_DTYPE v, VFLD_DTYPE d);
        void scalar_inverse(VFLD_DTYPE v);
        void add(const Voxel_Field& vfld);
        void multiply(const Voxel_Field& vfld);
        void threshold(VFLD_DTYPE v);
        void quantise(short int nstates);
        void fill( );
        void half( );
    
        void dt(bool fromseed);
        void pct(VFLD_DTYPE tp);
        void mat( );
        
        float gM(int pow_Z, int pow_Y, int pow_X);
        float cgM(int pow_Z, int pow_Y, int pow_X);
        
    private:
        void alloc( );
        VFLD_DTYPE find_5x5x5_min_fwd(int px, int py, int pz);
        VFLD_DTYPE find_5x5x5_min_bwd(int px, int py, int pz);
        VFLD_DTYPE mean_neighbours(int px, int py, int pz);
        VFLD_DTYPE MAT_maximal(int px, int py, int pz);
        float gM_000( );
        float gM_001( );
        float gM_010( );
        float gM_100( );
        float cgM_200( float cx, float cy, float cz );
        float cgM_020( float cx, float cy, float cz );
        float cgM_002( float cx, float cy, float cz );
};

#endif
