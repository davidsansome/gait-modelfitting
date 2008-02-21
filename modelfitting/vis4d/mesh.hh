// mesh.hh
// A set of routines to draw and create meshes from volumetric objects
// Richard Seely - University of Southampton, UK 2007

#ifndef _MESH_HH
#define _MESH_HH
#include "util.hh"
#include "points.hh"
#include "vspace.hh"
#include "vfield.hh"
#include "scene.hh"

#define HASHTABLE_SIZE 20000

typedef struct _Point_XYZNC_Item Point_XYZNC_Item;
struct _Point_XYZNC_Item {
    Point_XYZNC_Item * next;
    Point_XYZNC_Item * hash_next;
    float x;
    float y;
    float z;
    float nx;
    float ny;
    float nz;
    float cr;
    float cg;
    float cb;
    float cn;
    unsigned int index;
};

typedef struct _Quad_Item Quad_Item;
struct _Quad_Item {
    Quad_Item * next;
    Point_XYZNC_Item * a;
    Point_XYZNC_Item * b;
    Point_XYZNC_Item * c;
    Point_XYZNC_Item * d;
    unsigned int index;
};

class Point_XYZNC_List
{
    public:
        // variables
        Point_XYZNC_Item * first;
        Point_XYZNC_Item * last;
        // constructors/destructors
        Point_XYZNC_List( );
        // TODO:
        //~ Point_XYZNC_List( const Point_XYZNC_List& lst );
        ~Point_XYZNC_List( );
        // functions
        Point_XYZNC_Item * add(float x, float y, float z);
        Point_XYZNC_Item * find(float x, float y, float z);
        void print( );
    private:
        // variables
        Point_XYZNC_Item ** hashtable;
        // functions
        unsigned int xyz_hash(float x, float y, float z);
};

class Quad_List
{
    public:
        // variables
        Quad_Item * first;
        Quad_Item * last;
        Point_XYZNC_List * points;
        // constructors/destructors
        Quad_List( );
        // TODO:
        //~ Quad_List( const Quad_List& lst );
        Quad_List( const Voxel_Space& vspc );
        Quad_List( const Voxel_Space& vspc, Scene& scn );
        Quad_List( const Voxel_Field& vfld );
        Quad_List( const Voxel_Field& vfldR, const Voxel_Field& vfldG, const Voxel_Field& vfldB );
        ~Quad_List( );
        // functions
        Quad_Item * add( float ax, float ay, float az,
                                    float bx, float by, float bz,
                                    float cx, float cy, float cz,
                                    float dx, float dy, float dz,
                                    float nx, float ny, float nz );
        Quad_Item * addc( float ax, float ay, float az,
                                    float bx, float by, float bz,
                                    float cx, float cy, float cz,
                                    float dx, float dy, float dz,
                                    float nx, float ny, float nz,
                                    float r, float g, float b );
        void Add_Cube( float x, float y, float z, float size, float r, float g, float b );
        void print( );
};

class Mesh
{
    public:
        Mesh( const Quad_List& lst );
        Mesh( const Voxel_Space& vspc);
        Mesh( const Voxel_Field& vfld);
        Mesh( const Voxel_Field& vfldR, const Voxel_Field& vfldG, const Voxel_Field& vfldB);
        // TODO:
        //~ Mesh( const Mesh& msh );
        Mesh( char * filename );
        ~Mesh( );
    
        void save( char * filename );
        void saveR(char * filename);
        void saveZ(char * filename);
        void draw_init( bool fast );
        void draw( ) const;
        void draw_destroy( );
        
        float * vertices;
        float * normals;
        float * colours;
        unsigned int * indices;
        int n_vertices;
        int n_normals;
        int n_colours;
        int n_indices;
        unsigned int _vbo[4];
    private:
        void pack(const Quad_List& lst);
        void draw_slow( ) const;
        void draw_fast( ) const;
        bool use_draw_fast;
};

/*
typedef struct _VIEW_ITEM VIEW_ITEM;
struct _VIEW_ITEM {
    unsigned char * image;
    int height;
    int width;

    float P_00;
    float P_01;
    float P_02;
    float P_03;
    float P_10;
    float P_11;
    float P_12;
    float P_13;
    float P_20;
    float P_21;
    float P_22;
    float P_23;

    VIEW_ITEM * next;
};*/

#endif
