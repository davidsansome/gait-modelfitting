// scene.cc
// Richard Seely 2007

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <map>

#include "util.hh"
#include "image.hh"
#include "scene.hh"


View::View( const char * vname, const MATRIX_3x4& pmat)
{
    timestamp_entry("View( ):\n");

    name = new char[strlen(vname)+1];
    strcpy(name, vname);
    
    pImage = NULL;
    lookup = NULL;
    _calc = false;
    _calcP = false;
    x_size = y_size = z_size = 0;
    origin.x = origin.y = origin.z = 0.0;
    Position.x = Position.y = Position.z = 0.0;
    
    P_matrix.i_00 = 0; P_matrix.i_01 = 0; P_matrix.i_02 = 0; P_matrix.i_03 = 0;
    P_matrix.i_10 = 0; P_matrix.i_11 = 0; P_matrix.i_12 = 0; P_matrix.i_13 = 0;
    P_matrix.i_20 = 0; P_matrix.i_21 = 0; P_matrix.i_22 = 0; P_matrix.i_23 = 0;
    load_lut( );
    Set_P(pmat);

    timestamp_exit("done\n");
}

View::~View( )
{
    timestamp_entry("~View( ):\n");
    if (pImage != NULL)
        delete pImage;
    delete name;
    timestamp_exit("goodbye cruel world\n");
}

void View::Print( )
{
    timestamp_entry("View::print( ):\n");
    print_indent( ); printf("name=%s\n", name);
    if (pImage != NULL)
    {
        print_indent( ); printf("image=(%d x %d)\n", pImage->x_size, pImage->y_size);
    }
    else
    {
        print_indent( ); printf("image=NULL\n");
    }
    print_indent( ); printf("P={ %5g  %5g  %5g  %5g\n", P_matrix.i_00, P_matrix.i_01, P_matrix.i_02, P_matrix.i_03);
    print_indent( ); printf("    %5g  %5g  %5g  %5g\n", P_matrix.i_10, P_matrix.i_11, P_matrix.i_12, P_matrix.i_13);
    print_indent( ); printf("    %5g  %5g  %5g  %5g }\n", P_matrix.i_20, P_matrix.i_21, P_matrix.i_22, P_matrix.i_23);
    timestamp_exit("done");
}

void View::Set_Image( const char * filename )
{
    ImageRGB * nimg;
    timestamp_entry("View::Set_Image( )\n");
    if (pImage == NULL)
    {
        pImage = new ImageRGB( (char *) filename );
        _calc = true;
    }
    else
    {
        nimg = new ImageRGB( (char *) filename );
        if ((nimg->x_size != pImage->x_size) || (nimg->y_size != pImage->y_size))
        {
            delete pImage;
            pImage = nimg;
            _calc = true;
        }
        else
        {
            delete pImage;
            pImage = nimg;
        }
    }
    timestamp_exit("done\n");
}

void View::Set_Env( const Voxel_Space& vspc )
{
    timestamp_entry("View::Set_Env( Voxel_Space ):\n");
    //~ if ((x_size != vspc.x_size) || (y_size != vspc.y_size) || (z_size != vspc.z_size) || (origin.x != vspc.origin.x) || (origin.y != vspc.origin.y) || (origin.z != vspc.origin.z))
    if (lookup_big_enough(vspc) == false)
    {
        if (roundf(vspc.origin.x) < roundf(origin.x))
            origin.x = vspc.origin.x;
        if (roundf(vspc.origin.y) < roundf(origin.y))
            origin.y = vspc.origin.y;
        if (roundf(vspc.origin.z) < roundf(origin.z))
            origin.z = vspc.origin.z;
        if (roundf(vspc.origin.x + ((float) vspc.x_size)) > roundf(max.x))
            max.x = vspc.origin.x + ((float) vspc.x_size);
        if (roundf(vspc.origin.y + ((float) vspc.y_size)) > roundf(max.y))
            max.y = vspc.origin.y + ((float) vspc.y_size);
        if (roundf(vspc.origin.z + ((float) vspc.z_size)) > roundf(max.z))
            max.z = vspc.origin.z + ((float) vspc.z_size);
        x_size = (int) roundf(max.x - origin.x);
        y_size = (int) roundf(max.y - origin.y);
        z_size = (int) roundf(max.z - origin.z);
        alloc( );
        _calc = true;
    }
    timestamp_exit("done\n");
}

void View::Set_Env( const Voxel_Field& vfld )
{
    timestamp_entry("View::Set_Env( Voxel_Field ):\n");
    if (lookup_big_enough(vfld) == false)
    {
        if (roundf(vfld.origin.x) < roundf(origin.x))
            origin.x = vfld.origin.x;
        if (roundf(vfld.origin.y) < roundf(origin.y))
            origin.y = vfld.origin.y;
        if (roundf(vfld.origin.z) < roundf(origin.z))
            origin.z = vfld.origin.z;
        if (roundf(vfld.origin.x + ((float) vfld.x_size)) > roundf(max.x))
            max.x = vfld.origin.x + ((float) vfld.x_size);
        if (roundf(vfld.origin.y + ((float) vfld.y_size)) > roundf(max.y))
            max.y = vfld.origin.y + ((float) vfld.y_size);
        if (roundf(vfld.origin.z + ((float) vfld.z_size)) > roundf(max.z))
            max.z = vfld.origin.z + ((float) vfld.z_size);
        x_size = (int) roundf(max.x - origin.x);
        y_size = (int) roundf(max.y - origin.y);
        z_size = (int) roundf(max.z - origin.z);
        alloc( );
        _calc = true;
    }
    timestamp_exit("done\n");
}

void View::Set_P( const MATRIX_3x4& pmat )
{
    timestamp_entry("View::Set_P( ):\n");
    newP.i_00 = pmat.i_00; newP.i_01 = pmat.i_01; newP.i_02 = pmat.i_02; newP.i_03 = pmat.i_03;
    newP.i_10 = pmat.i_10; newP.i_11 = pmat.i_11; newP.i_12 = pmat.i_12; newP.i_13 = pmat.i_13;
    newP.i_20 = pmat.i_20; newP.i_21 = pmat.i_21; newP.i_22 = pmat.i_22; newP.i_23 = pmat.i_23;
    
    if ( (newP.i_00 != P_matrix.i_00) || (newP.i_01 != P_matrix.i_01) || (newP.i_02 != P_matrix.i_02) || (newP.i_03 != P_matrix.i_03) ||
          (newP.i_10 != P_matrix.i_10) || (newP.i_11 != P_matrix.i_11) || (newP.i_12 != P_matrix.i_12) || (newP.i_13 != P_matrix.i_13) ||
          (newP.i_20 != P_matrix.i_20) || (newP.i_21 != P_matrix.i_21) || (newP.i_22 != P_matrix.i_22) || (newP.i_23 != P_matrix.i_23) )
        _calcP = true;
    else
        _calcP = false;
    
    timestamp_exit("done\n");
}

void View::Scale_P( float mx, float my, float mz, float mw)
{
    timestamp_entry("View::Scale_P( ):\n");
    newP.i_00 *= mx; newP.i_01 *= my; newP.i_02 *= mz; newP.i_03 *= mw;
    newP.i_10 *= mx; newP.i_11 *= my; newP.i_12 *= mz; newP.i_13 *= mw;
    newP.i_20 *= mx; newP.i_21 *= my; newP.i_22 *= mz; newP.i_23 *= mw;
    
    if ( (newP.i_00 != P_matrix.i_00) || (newP.i_01 != P_matrix.i_01) || (newP.i_02 != P_matrix.i_02) || (newP.i_03 != P_matrix.i_03) ||
          (newP.i_10 != P_matrix.i_10) || (newP.i_11 != P_matrix.i_11) || (newP.i_12 != P_matrix.i_12) || (newP.i_13 != P_matrix.i_13) ||
          (newP.i_20 != P_matrix.i_20) || (newP.i_21 != P_matrix.i_21) || (newP.i_22 != P_matrix.i_22) || (newP.i_23 != P_matrix.i_23) )
        _calcP = true;
    else
        _calcP = false;
    
    timestamp_exit("done\n");
}

void View::load_lut( )
{
    FILE * fle;
    char fname[64];
    int w,h;
    timestamp_entry("View::load_lut\n");
    sprintf(fname, "/usr/local/share/vis4d/%s.lutcache", name);
    fle = fopen(fname, "rb");
    if (fle == NULL)
    {
        perror("warning: could not load lookup table");
        return;
    }
    fread(&origin, sizeof(Point_XYZ), 1, fle);
    fread(&x_size, sizeof(int), 1, fle);
    fread(&y_size, sizeof(int), 1, fle);
    fread(&z_size, sizeof(int), 1, fle);
    fread(&P_matrix, sizeof(MATRIX_3x4), 1, fle);
    fread(&Position, sizeof(Point_XYZ), 1, fle);
    fread(&w, sizeof(int), 1, fle);
    fread(&h, sizeof(int), 1, fle);
    if (pImage != NULL)
        delete pImage;
    pImage = new ImageRGB(w,h);
    alloc();
    fread(lookup, sizeof(VLOOKUP_DTYPE), x_size * y_size * z_size, fle);
    fclose(fle);
    timestamp_exit("done\n");
}

void View::save_lut( )
{
    FILE * fle;
    char fname[64];
    int w,h;
    timestamp_entry("View::save_lut\n");
    sprintf(fname, "/usr/local/share/vis4d/%s.lutcache", name);
    fle = fopen(fname, "wb");
    if (fle == NULL)
    {
        perror("warning: could not open lookup table");
        return;
    }
    fwrite(&origin, sizeof(Point_XYZ), 1, fle);
    fwrite(&x_size, sizeof(int), 1, fle);
    fwrite(&y_size, sizeof(int), 1, fle);
    fwrite(&z_size, sizeof(int), 1, fle);
    fwrite(&P_matrix, sizeof(MATRIX_3x4), 1, fle);
    fwrite(&Position, sizeof(Point_XYZ), 1, fle);
    fwrite(&pImage->x_size, sizeof(int), 1, fle);
    fwrite(&pImage->y_size, sizeof(int), 1, fle);
    fwrite(lookup, sizeof(VLOOKUP_DTYPE), x_size * y_size * z_size, fle);
    fclose(fle);
    timestamp_exit("done\n");
}

void View::alloc( )
{
    timestamp_entry("View::alloc( )\n");
    if (lookup != NULL)
    {
        IF_DEBUG timestamp_inside("reallocating lookup table...\n");
        delete [] lookup;
    }
    else
    {
        IF_DEBUG timestamp_inside("allocating lookup table...\n");
    }
    lookup = new VLOOKUP_DTYPE[x_size * y_size * z_size];
    
    max.x = origin.x + ((float) x_size);
    max.y = origin.y + ((float) y_size);
    max.z = origin.z + ((float) z_size);
    
    for (int i=0; i < x_size * y_size * z_size; i++)
        lookup[i] = 0;
    IF_DEBUG {print_indent( ); printf("new lookup, min=(%g,%g,%g), max=(%g,%g,%g)\n", origin.x, origin.y, origin.z, max.x, max.y, max.z);}
    timestamp_exit("done\n");
}

void View::calc( )
{
    int px, py, pz;
    float fx, fy, fz;
    float cx, cy, cz;
    float d, Hx, Hy;
    int ix, iy;
    VLOOKUP_DTYPE * pnt = lookup;
    
    timestamp_entry("View::calc( ):\n");
    
    P_matrix.i_00 = newP.i_00; P_matrix.i_01 = newP.i_01; P_matrix.i_02 = newP.i_02; P_matrix.i_03 = newP.i_03;
    P_matrix.i_10 = newP.i_10; P_matrix.i_11 = newP.i_11; P_matrix.i_12 = newP.i_12; P_matrix.i_13 = newP.i_13;
    P_matrix.i_20 = newP.i_20; P_matrix.i_21 = newP.i_21; P_matrix.i_22 = newP.i_22; P_matrix.i_23 = newP.i_23;
    
    // Find the values of Hx and Hy - the approximate optical origin (centre of image)
    if (pImage != NULL)
    {
    Hx = ((float) pImage->x_size) / 2.0;
    Hy = ((float) pImage->y_size) / 2.0;
    }
    else
    {
        Hx = 0.0;
        Hy = 0.0;
    }
    // Find the position of the camera
    // use fx, fy, fz as multiplicands
    // use cx as divisor
    fz = - (P_matrix.i_00*(- P_matrix.i_11*P_matrix.i_23 + P_matrix.i_13*P_matrix.i_21 - Hy*P_matrix.i_21) + P_matrix.i_01*(P_matrix.i_10*P_matrix.i_23 - P_matrix.i_13*P_matrix.i_20 + Hy*P_matrix.i_20) + Hx*(P_matrix.i_10*P_matrix.i_21 - P_matrix.i_11*P_matrix.i_20) + P_matrix.i_03*(P_matrix.i_11*P_matrix.i_20 - P_matrix.i_10*P_matrix.i_21));
    fy = (P_matrix.i_00*(- P_matrix.i_12*P_matrix.i_23 + P_matrix.i_13*P_matrix.i_22 - Hy*P_matrix.i_22) + P_matrix.i_02*(P_matrix.i_10*P_matrix.i_23 - P_matrix.i_13*P_matrix.i_20 + Hy*P_matrix.i_20) + Hx*(P_matrix.i_10*P_matrix.i_22 - P_matrix.i_12*P_matrix.i_20) + P_matrix.i_03*(P_matrix.i_12*P_matrix.i_20 - P_matrix.i_10*P_matrix.i_22));
    fx = - (P_matrix.i_01*(- P_matrix.i_12*P_matrix.i_23 + P_matrix.i_13*P_matrix.i_22 - Hy*P_matrix.i_22) + P_matrix.i_02*(P_matrix.i_11*P_matrix.i_23 - P_matrix.i_13*P_matrix.i_21 + Hy*P_matrix.i_21) + Hx*(P_matrix.i_11*P_matrix.i_22 - P_matrix.i_12*P_matrix.i_21) + P_matrix.i_03*(P_matrix.i_12*P_matrix.i_21 - P_matrix.i_11*P_matrix.i_22));
    d = (P_matrix.i_00*(P_matrix.i_12*P_matrix.i_21 - P_matrix.i_11*P_matrix.i_22) + P_matrix.i_01*(P_matrix.i_10*P_matrix.i_22 - P_matrix.i_12*P_matrix.i_20) + P_matrix.i_02*(P_matrix.i_11*P_matrix.i_20 - P_matrix.i_10*P_matrix.i_21));
    if (d != 0.0)
    {
        Position.x = fx / d;
        Position.y = fy / d;
        Position.z = fz / d;
        IF_DEBUG printf("position = %g, %g, %g\n", Position.x, Position.y, Position.z);
    }
    else
    {
        Position.x = Position.y = Position.z = 0.0;
        timestamp_inside("warning: could not solve camera position\n");
    }

    if (pImage == NULL) { timestamp_exit("Image=NULL\n"); return; }
    if (lookup == NULL) { timestamp_exit("Lookup Table=NULL\n"); return; }

    for (pz=0; pz < z_size; pz++)
    {
        fz = ((float) pz)  + origin.z;
        for (py=0; py < y_size; py++)
        {
            fy = ((float) py)  + origin.y;
            for (px=0; px < x_size; px++)
            {
                fx = ((float) px)  + origin.x;
                cx = P_matrix.i_00 * fx + P_matrix.i_01 * fy + P_matrix.i_02 * fz + P_matrix.i_03;
                cy = P_matrix.i_10 * fx + P_matrix.i_11 * fy + P_matrix.i_12 * fz + P_matrix.i_13;
                cz = P_matrix.i_20 * fx + P_matrix.i_21 * fy + P_matrix.i_22 * fz + P_matrix.i_23;
                ix = (int) roundf(cx / cz);
                iy = (int) roundf(cy / cz);
                *pnt++ = ( ((ix < 0) || (iy < 0) || (ix >= pImage->x_size) || (iy >= pImage->y_size)) ? -1 : 3 * (iy * pImage->x_size + ix) );
            }
        }
    }
    _calc = false;
    _calcP = false;
    save_lut();
    timestamp_exit("done\n");
}

bool View::Get_RGB( float fx, float fy, float fz, VFLD_DTYPE * pR, VFLD_DTYPE * pG, VFLD_DTYPE * pB )
{
    VLOOKUP_DTYPE look;
    
    if ((_calc == true) || (_calcP == true))
        calc( );
    
    if ((lookup == NULL) || (pImage == NULL))
    {
        timestamp_inside("View::Get_RGB( ): error - lookup table or image not initialised\n");
        return(false);
    }
    
    int px = (int) roundf(fx - origin.x);
    int py = (int) roundf(fy - origin.y);
    int pz = (int) roundf(fz - origin.z);
    
    if ((px < 0) || (py < 0) || (pz < 0) || (px >= x_size) || (py >= y_size) || (pz >= z_size))
    {
        *pR = 0;
        *pG = 0;
        *pB = 0;
        return(false);
    }
    else
    {
        look = lookup[ (pz * y_size + py) * x_size + px ];
        if (look == -1)
        {
            *pR = 0;
            *pG = 0;
            *pB = 0;
            return(false);
        }
        else
        {
            *pR = pImage->data[look];
            *pG = pImage->data[look+1];
            *pB = pImage->data[look+2];
            // temp debug code - can be removed for speedup
            IF_DEBUG {
                pImage->data[look] = 128;
                pImage->data[look+1] = 128;
                pImage->data[look+2] = 128;
            }
            // end temp
            return(true);
        }
    }
}

void View::Fill_RGB( Voxel_Field * fR, Voxel_Field * fG, Voxel_Field * fB )
{
    timestamp_entry("View::Fill_RGB( ):\n");
    
    if (pImage == NULL)
    {
        timestamp_exit("error: image not initialised.\n");
        return;
    }
    
    if (lookup == NULL)
    {
        timestamp_exit("error: lookup table not initialised.\n");
        return;
    }
    
    if ((_calc == true) || (_calcP == true))
        calc( );
    
    if ((fR == NULL) || (fG == NULL) || (fB == NULL))
    {
        timestamp_exit("error: passed at least one NULL Voxel_Field.\n");
        return;
    }
    
    if (lookup_big_enough(*fR)==false)
    {
        timestamp_exit("error: Voxel_Fields to big for computed lookup table\n");
        return;
    }
    if ((fG->x_size != fR->x_size) || (fG->y_size != fR->y_size) || (fG->z_size != fR->z_size) || (fG->origin.x != fR->origin.x) || (fG->origin.y != fR->origin.y) || (fG->origin.z != fR->origin.z))
    {
        timestamp_exit("error: dimension/origin mismatch between Voxel_Field fR and fG.\n");
        return;
    }
    if ((fB->x_size != fR->x_size) || (fB->y_size != fR->y_size) || (fB->z_size != fR->z_size) || (fB->origin.x != fR->origin.x) || (fB->origin.y != fR->origin.y) || (fB->origin.z != fR->origin.z))
    {
        timestamp_exit("error: dimension/origin mismatch between Voxel_Field fR and fB.\n");
        return;
    }
    
    VLOOKUP_DTYPE * look;
    VFLD_DTYPE * pR = fR->data;
    VFLD_DTYPE * pG = fG->data;
    VFLD_DTYPE * pB = fB->data;
    IMG_DTYPE * img = pImage->data;
    
    int ox = (int) roundf(fR->origin.x - origin.x);
    int oy = (int) roundf(fR->origin.y - origin.y);
    int oz = (int) roundf(fR->origin.z - origin.z);
    
    int px, py, pz;
    
    for (pz=0; pz < fR->z_size; pz++)
    {
        for (py=0; py < fR->y_size; py++)
        {
            look = lookup;
            look += ((pz + oz) * y_size + py + oy) * x_size + ox;
            for (px=0; px < fR->x_size; px++)
            {
                if (*look == -1)
                {
                    *pR++ = 0;
                    *pG++ = 0;
                    *pB++ = 0;
                }
                else
                {
                    *pR++ = (VFLD_DTYPE) img[*look];
                    *pG++ = (VFLD_DTYPE) img[*look+1];
                    *pB++ = (VFLD_DTYPE) img[*look+2];
                }
                look++;
            }
        }
    }
    
    timestamp_exit("done\n");
}

void View::Force_Alloc( const Point_XYZ& lmin, const Point_XYZ& lmax )
{
    origin.x = lmin.x; origin.y = lmin.y; origin.z = lmin.z;
    max.x = lmax.x + 1.0; max.y = lmax.y + 1.0; max.z = lmax.z + 1.0;
    x_size = (int) roundf(max.x - origin.x);
    y_size = (int) roundf(max.y - origin.y);
    z_size = (int) roundf(max.z - origin.z);
    alloc( );
    _calc = true;
}

void View::Default_Alloc( const Point_XYZ& lmin, const Point_XYZ& lmax )
{
    if (lookup == NULL)
    {
        origin.x = lmin.x; origin.y = lmin.y; origin.z = lmin.z;
        max.x = lmax.x + 1.0; max.y = lmax.y + 1.0; max.z = lmax.z + 1.0;
        x_size = (int) roundf(max.x - origin.x);
        y_size = (int) roundf(max.y - origin.y);
        z_size = (int) roundf(max.z - origin.z);
        alloc( );
        _calc = true;
    }
}

Scene::Scene( char * filename )
{
    FILE * fle;
    MATRIX_3x4 in_mat;
    char in_name[64];
    View * vw;
    
    timestamp_entry("Scene( filename ):\n");
    
    if ((fle = fopen(filename,"r")) == NULL)
    {
        perror("Scene(filename): failed to open file");
        timestamp_exit("failed\n");
        throw(11);
    }
    while (!feof(fle))
    {
        if (fscanf(fle, "%s %g %g %g %g %g %g %g %g %g %g %g %g", in_name,
                            &in_mat.i_00, &in_mat.i_01, &in_mat.i_02, &in_mat.i_03,
                            &in_mat.i_10, &in_mat.i_11, &in_mat.i_12, &in_mat.i_13,
                            &in_mat.i_20, &in_mat.i_21, &in_mat.i_22, &in_mat.i_23 ) == 13)
        {
            vw = new View((const char *) in_name, in_mat);
            views[vw->name] = vw;
        }
    }
    fclose(fle);
    timestamp_exit("done");
}

Scene::~Scene( )
{
    timestamp_entry("~Scene( ):\n");
    
    View_Map::iterator map_it = views.begin();
    View_Map::iterator map_end = views.end();
    
    for (; map_it != map_end; ++map_it )
    {
        delete(*map_it).second;
    }
    
    timestamp_exit("goodbye cruel world\n");
}

void Scene::Print( )
{
    timestamp_entry("Scene::print( ):\n");
    
    View_Map::iterator map_it = views.begin();
    View_Map::iterator map_end = views.end();
    
    printf("size=%d", views.size() );
    
    for (; map_it != map_end; ++map_it )
    {
        (*map_it).second->Print( );
    }
    
    timestamp_exit("done\n");
}

void Scene::Set_Env( const Voxel_Space& vspc )
{
    timestamp_entry("Scene::Set_Env( Voxel_Space ):\n");
    
    View_Map::iterator map_it = views.begin();
    View_Map::iterator map_end = views.end();
    
    for (; map_it != map_end; ++map_it )
    {
        (*map_it).second->Set_Env( vspc );
    }
    
    timestamp_exit("done\n");
}

void Scene::Set_Env( const Voxel_Field& vfld )
{
    timestamp_entry("Scene::Set_Env( Voxel_Field ):\n");
    
    View_Map::iterator map_it = views.begin();
    View_Map::iterator map_end = views.end();
    
    for (; map_it != map_end; ++map_it )
    {
        (*map_it).second->Set_Env( vfld );
    }
    
    timestamp_exit("done\n");
}

void Scene::Scale_P( float mx, float my, float mz, float mw )
{
    timestamp_entry("Scene::Scale_P( ):\n");
    
    View_Map::iterator map_it = views.begin();
    View_Map::iterator map_end = views.end();
    
    for (; map_it != map_end; ++map_it )
    {
        (*map_it).second->Scale_P( mx, my, mz, mw );
    }
    
    timestamp_exit("done\n");
}

void Scene::Force_Alloc( const Point_XYZ& lmin, const Point_XYZ& lmax )
{
    timestamp_entry("Scene::Force_Alloc( ):\n");
    
    View_Map::iterator map_it = views.begin();
    View_Map::iterator map_end = views.end();
    
    for (; map_it != map_end; ++map_it )
    {
        (*map_it).second->Force_Alloc(lmin, lmax);
    }
    
    timestamp_exit("done\n");
}

void Scene::Default_Alloc( const Point_XYZ& lmin, const Point_XYZ& lmax )
{
    timestamp_entry("Scene::Default_Alloc( ):\n");
    
    View_Map::iterator map_it = views.begin();
    View_Map::iterator map_end = views.end();
    
    for (; map_it != map_end; ++map_it )
    {
        (*map_it).second->Default_Alloc(lmin, lmax);
    }
    
    timestamp_exit("done\n");
}

void Scene::Force_Calc( )
{
    timestamp_entry("Scene::Force_Calc( ):\n");
    
    View_Map::iterator map_it = views.begin();
    View_Map::iterator map_end = views.end();
    
    for (; map_it != map_end; ++map_it )
    {
        (*map_it).second->Force_Calc( );
    }
    
    timestamp_exit("done\n");
}

void Scene::Guess_Colour( float fx, float fy, float fz, float nx, float ny, float nz, float * red, float * green, float * blue )
{
    VFLD_DTYPE tR, tG, tB;
    float accR = 0.0;
    float accG = 0.0;
    float accB = 0.0;
    float accN = 0.0;
    float tN, mN;
    View * vw;
    
    //~ tN = nx * nx + ny * ny + nz * nz;
    //~ nx /= sqrtf(tN);
    //~ ny /= sqrtf(tN);
    //~ nz /= sqrtf(tN);
    mN = nx * nx + ny * ny + nz * nz;
    
    View_Map::iterator map_it = views.begin();
    View_Map::iterator map_end = views.end();
    
    for (; map_it != map_end; ++map_it )
    {
        vw = (*map_it).second;
        if (vw->Get_RGB( fx, fy, fz, &tR, &tG, &tB ) == true)
        {
            tN = (vw->Position.x - fx) * nx + (vw->Position.y - fy) * ny + (vw->Position.z - fz) * nz;
            tN = (tN > 0.0 ? tN * tN : 0.0 );
            tN /= ((vw->Position.x - fx) * (vw->Position.x - fx) + (vw->Position.y - fy) * (vw->Position.y - fy) + (vw->Position.z - fz) * (vw->Position.z - fz)) * mN;
            tN = tN * tN;
            accR += (((float) tR) / 255.0) * tN;
            accG += (((float) tG) / 255.0) * tN;
            accB += (((float) tB) / 255.0) * tN;
            accN += tN;
        }
    }
    
    if (accN > 0.0)
    {
        *red = accR / accN;
        *green = accG / accN;
        *blue = accB / accN;
    }
    else
    {
        *red = 0.0;
        *green = 0.0;
        *blue = 0.0;
    }
}

