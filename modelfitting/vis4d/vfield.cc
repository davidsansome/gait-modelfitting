// vfield.cc
// A set of routines to create and manipulate a simple volumetric space
// Richard Seely - University of Southampton, UK 2006

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "util.hh"
#include "vfield.hh"

#define DT_5x5x5_a 7
#define DT_5x5x5_b 10
#define DT_5x5x5_c 12
#define DT_5x5x5_d 16
#define DT_5x5x5_e 17
#define DT_5x5x5_f 21

#define REGIONTABLESIZE 10000

Voxel_Field::Voxel_Field( int x, int y, int z )
{
    timestamp_entry("Voxel_Field(x,y,z): ");
    x_size = x;
    y_size = y;
    z_size = z;
    data = NULL;
    
    origin.x = origin.y = origin.z = 0.0;
    alloc( );
    timestamp_exit("new Voxel_Field created\n");
}

void Voxel_Field::alloc( )
{
    if (data != NULL)
        delete [] data;
    data = new VFLD_DTYPE[x_size * y_size * z_size];
    
    for (int i=0; i < x_size * y_size * z_size; i++)
        data[i] = 0;
}

Voxel_Field::~Voxel_Field( )
{
    timestamp_entry("~Voxel_Field: ");
    if (data != NULL)
        delete [] data;
    data=NULL;
    timestamp_exit("goodbye cruel world\n");
}

Voxel_Field::Voxel_Field( const Point_XYZ_List& lst )
{
    Point_XYZ_Item * cur;
    int ix, iy, iz;
    Point_XYZ min, max;
    
    timestamp_entry("Voxel_Field(Point_XYZ_List):\n");
    
    data = NULL;

    min.x = lst.min.x; max.x = lst.max.x;
    min.y = lst.min.y; max.y = lst.max.y;
    min.z = lst.min.z; max.z = lst.max.z;
    
    x_size = (int) roundf(max.x - min.x + 1.0);
    y_size = (int) roundf(max.y - min.y + 1.0);
    z_size = (int) roundf(max.z - min.z + 1.0);
    
    IF_DEBUG {
        printf("  lst cnt=%d\n", lst.count);
        printf("  lst min = (%f,%f,%f)\n", lst.min.x, lst.min.y, lst.min.z);
        printf("  lst max = (%f,%f,%f)\n", lst.max.x, lst.max.y, lst.max.z);
        printf("  %d %d %d\n", x_size, y_size, z_size);
    }
    
    origin.x = min.x;
    origin.y = min.y;
    origin.z = min.z;
    
    alloc( );
    
    cur = lst.first;
    while (cur != NULL) {
        ix = (int) roundf(cur->p.x - min.x);
        iy = (int) roundf(cur->p.y - min.y);
        iz = (int) roundf(cur->p.z - min.z);
        if ((ix < 0) || (iy < 0) || (iz < 0) || (ix >= x_size) || (iy >= y_size) || (iz >= z_size)) {
            printf("out of bound voxel - ignoring\n");
        } else {
            data[(iz * y_size + iy) * x_size + ix] = 1;
        }
        cur = cur->next;
    }
    timestamp_exit("done\n");
}

Voxel_Field::Voxel_Field( char * filename )
{
    FILE * fle;
    VFLD_DTYPE * rootvox;
    int px, py, pz;
    
    timestamp_entry("Voxel_Field(filename):\n");
    
    if ((fle = fopen(filename, "r")) == NULL) {
        perror(" Voxel_Field(filename):  fopen failed");
        delete(this);
    }
    
    fscanf(fle, "VFLDPT\n%g\t%g\t%g", &origin.x, &origin.y, &origin.z);
    fscanf(fle, "%d\t%d\t%d", &x_size, &y_size, &z_size);
    data = NULL;
    
    alloc( );
    IF_DEBUG printf("sx=%d, sy=%d, sz=%d\n", x_size, y_size, z_size);
    
    for (pz=0; pz < z_size; pz++) {
        for (py=0; py < y_size; py++) {
            rootvox = data;
            rootvox += (pz * y_size + py) * x_size;
            for (px=0; px < x_size; px++) {
                fscanf(fle, "%hd", rootvox++);
            }
        }
    }
    fclose(fle);
    
    timestamp_exit("done\n");
}

Voxel_Field::Voxel_Field( const Voxel_Field& vfld )
{    
    timestamp_entry("Voxel_Field(Voxel_Field):\n");
    
    x_size = vfld.x_size;
    y_size = vfld.y_size;
    z_size = vfld.z_size;
    origin.x = vfld.origin.x;
    origin.y = vfld.origin.y;
    origin.z = vfld.origin.z;
    data = NULL;
    
    alloc( );
    
    memcpy(data, vfld.data, x_size * y_size * z_size * sizeof(VFLD_DTYPE));
    
    timestamp_exit("done\n");
}


Voxel_Field::Voxel_Field( const Voxel_Space& vspc )
{    
    VSPC_DTYPE * rootvox;
    VFLD_DTYPE * destvox;
    int px, py, pz, sx;
    
    timestamp_entry("Voxel_Field(Voxel_Space):\n");
    
    x_size = vspc.x_size;
    y_size = vspc.y_size;
    z_size = vspc.z_size;
    origin.x = vspc.origin.x;
    origin.y = vspc.origin.y;
    origin.z = vspc.origin.z;
    data = NULL;
    
    alloc( );

    for (pz=0; pz < z_size; pz++) {
        for (py=0; py < y_size; py++) {
            rootvox = vspc.data;
            rootvox += (pz * vspc.y_size + py) * vspc.x_span;
            destvox = data;
            destvox += (pz * y_size + py) * x_size;
            sx = 0;
            for (px=0; px < x_size; px++) {
                if ((*rootvox & (VSPC_ONE << sx++)) != 0) *destvox++ = 1; else *destvox++ = 0;
                if (sx >= vspc.data_width)
                {
                    sx -= vspc.data_width;
                    rootvox++;
                }
            }
        }
    }
    timestamp_exit("done\n");
}

void Voxel_Field::save(char * filename)
{
    FILE * fle;
    VFLD_DTYPE * rootvox;
    int px, py, pz;
    
    timestamp_entry("Voxel_Field::save:\n");
    
    if ((fle = fopen(filename, "w")) == NULL) {
        perror("  fopen failed");
        exit(-1);
    }
    
    fprintf(fle, "VFLDPT\n");
    fprintf(fle, "%g\t%g\t%g\n", origin.x, origin.y, origin.z);
    fprintf(fle, "%d\t%d\t%d\n\n", x_size, y_size, z_size);
    
    for (pz=0; pz < z_size; pz++) {
        for (py=0; py < y_size; py++) {
            rootvox = data;
            rootvox += (pz * y_size + py) * x_size;
            for (px=0; px < x_size; px++) {
                fprintf(fle, "%hd ", *rootvox++);
            }
            fprintf(fle, "\n");
        }
        fprintf(fle, "\n");
    }
    fclose(fle);
    
    timestamp_exit("done\n");
}

void Voxel_Field::border(int r)
{
    VFLD_DTYPE * dst;
    VFLD_DTYPE * rootvox;
    VFLD_DTYPE * dstvox;
    int px, py, pz;
    int sx, sy, sz;
    
    timestamp_entry("Voxel_Field::border( r ):\n");
    
    if (r==0)
    {
        print_indent(); printf("r == 0 - not doing anything\n");
        return;
    }
    
    sx = x_size + (r * 2);
    sy = y_size + (r * 2);
    sz = z_size + (r * 2);
    
    if ((sx < 1) || (sy < 1) || (sz < 1))
    {
        print_indent(); printf("cannot shrink Voxel_Field - one or more resulting dimensions below 1\n");
        return;
    }
    
    dst = new VFLD_DTYPE[sx * sy * sz]; 
    for (int i=0; i < sx * sy * sz; i++)
        dst[i] = 0;    
    
    origin.x -= (float) r;
    origin.y -= (float) r;
    origin.z -= (float) r;
    
    if (r > 0)
    {
        for (pz=0; pz < z_size; pz++)
        {
            for (py=0; py < y_size; py++)
            {
                rootvox = data;
                rootvox += (pz * y_size + py) * x_size;
                dstvox = dst;
                dstvox += ((r+pz) * sy + py + r) * sx + r;
                for (px=0; px < x_size; px++)
                {
                    *dstvox++ = *rootvox++;
                }
            }
        }
    }
    else
    {
        r = -r;
        for (pz=r; pz < z_size-r; pz++)
        {
            for (py=r; py < y_size-r; py++)
            {
                rootvox = data;
                rootvox += (pz * y_size + py) * x_size + r;
                dstvox = dst;
                dstvox += ((pz-r) * sy + py - r) * sx;
                for (px=0; px < sx; px++)
                {
                    *dstvox++ = *rootvox++;
                }
            }
        }
    }
    delete [] data;
    data = dst;
    x_size = sx;
    y_size = sy;
    z_size = sz;
    timestamp_exit("done\n");    
}

void Voxel_Field::crop(int sx, int sy, int sz, int ex, int ey, int ez)
{
    VFLD_DTYPE * dst;
    VFLD_DTYPE * rootvox;
    VFLD_DTYPE * dstvox;
    int px, py, pz;
    
    timestamp_entry("Voxel_Field::crop( r ):\n");
        
    if ((sx < 0) || (sy < 0) || (sz < 0))
    {
        print_indent(); printf("error: negative start dimensions given\n");
        return;
    }
    if ((ex > x_size) || (ey > y_size) || (ez > z_size))
    {
        print_indent(); printf("error: end dimensions exceed current size\n");
        return;
    }
    if ((ex <= sx) || (ey <= sy) || (ez <= sz))
    {
        print_indent(); printf("error: end dimensions < start dimensions\n");
        return;
    }
    
    int nx_size = ex - sx;
    int ny_size = ey - sy;
    int nz_size = ez - sz;
    
    dst = new VFLD_DTYPE[nz_size * ny_size * nx_size]; 
    for (int i=0; i < nz_size * ny_size * nx_size; i++)
        dst[i] = 0;    
    
    origin.x += (float) sx;
    origin.y += (float) sy;
    origin.z += (float) sz;
    
    for (pz=sz; pz < ez; pz++)
    {
        for (py=sy; py < ey; py++)
        {
            rootvox = data;
            rootvox += (pz * y_size + py) * x_size + sx;
            dstvox = dst;
            dstvox += ((pz-sz) * ny_size + py - sy) * nx_size;
            for (px=sx; px < ex; px++)
            {
                *dstvox++ = *rootvox++;
            }
        }
    }

    delete [] data;
    data = dst;
    x_size = nx_size;
    y_size = ny_size;
    z_size = nz_size;
    timestamp_exit("done\n");    
}

void Voxel_Field::uncrop(const Voxel_Field& vfld)
{
    VFLD_DTYPE * dst;
    VFLD_DTYPE * rootvox;
    VFLD_DTYPE * dstvox;
    int px, py, pz;
    
    timestamp_entry("Voxel_Field::uncrop( vfld ):\n");
    
    int offset_x = (int) roundf( origin.x - vfld.origin.x );
    int offset_y = (int) roundf( origin.y - vfld.origin.y );
    int offset_z = (int) roundf( origin.z - vfld.origin.z );
    
    if ( (offset_x < 0) || (offset_y < 0) || (offset_z < 0) )
    {
        print_indent(); printf("error: Voxel_Field fragment outside of main Voxel_Field vfld (<)\n");
        return;
    }
    if ( (offset_x + x_size > vfld.x_size) || (offset_y + y_size > vfld.y_size) || (offset_z + z_size > vfld.z_size) )
    {
        print_indent(); printf("error: Voxel_Field fragment outside of main Voxel_Field vfld (>)\n");
        return;
    }
        
    dst = new VFLD_DTYPE[vfld.z_size * vfld.y_size * vfld.x_size]; 
    for (int i=0; i < vfld.z_size * vfld.y_size * vfld.x_size; i++)
        dst[i] = 0;
        
    for (pz=0; pz < z_size; pz++)
    {
        for (py=0; py < y_size; py++)
        {
            rootvox = data;
            rootvox += (pz * y_size + py) * x_size;
            dstvox = dst;
            dstvox += ((pz+offset_z) * vfld.y_size + py + offset_y) * vfld.x_size + offset_x;
            for (px=0; px < x_size; px++)
            {
                *dstvox++ = *rootvox++;
            }
        }
    }

    delete [] data;
    data = dst;
    x_size = vfld.x_size;
    y_size = vfld.y_size;
    z_size = vfld.z_size;
    
    origin.x = vfld.origin.x;
    origin.y = vfld.origin.y;
    origin.z = vfld.origin.z;
    timestamp_exit("done\n");    
}

void Voxel_Field::uncrop(const Voxel_Space& vspc)
{
    VFLD_DTYPE * dst;
    VFLD_DTYPE * rootvox;
    VFLD_DTYPE * dstvox;
    int px, py, pz;
    
    timestamp_entry("Voxel_Field::uncrop( vspc ):\n");
    
    int offset_x = (int) roundf( origin.x - vspc.origin.x );
    int offset_y = (int) roundf( origin.y - vspc.origin.y );
    int offset_z = (int) roundf( origin.z - vspc.origin.z );
    
    if ( (offset_x < 0) || (offset_y < 0) || (offset_z < 0) )
    {
        print_indent(); printf("error: Voxel_Field fragment outside of main Voxel_Space vspc (<)\n");
        return;
    }
    if ( (offset_x + x_size > vspc.x_size) || (offset_y + y_size > vspc.y_size) || (offset_z + z_size > vspc.z_size) )
    {
        print_indent(); printf("error: Voxel_Field fragment outside of main Voxel_Space vspc (>)\n");
        return;
    }
        
    dst = new VFLD_DTYPE[vspc.z_size * vspc.y_size * vspc.x_size]; 
    for (int i=0; i < vspc.z_size * vspc.y_size * vspc.x_size; i++)
        dst[i] = 0;
        
    for (pz=0; pz < z_size; pz++)
    {
        for (py=0; py < y_size; py++)
        {
            rootvox = data;
            rootvox += (pz * y_size + py) * x_size;
            dstvox = dst;
            dstvox += ((pz+offset_z) * vspc.y_size + py + offset_y) * vspc.x_size + offset_x;
            for (px=0; px < x_size; px++)
            {
                *dstvox++ = *rootvox++;
            }
        }
    }

    delete [] data;
    data = dst;
    x_size = vspc.x_size;
    y_size = vspc.y_size;
    z_size = vspc.z_size;
    
    origin.x = vspc.origin.x;
    origin.y = vspc.origin.y;
    origin.z = vspc.origin.z;
    timestamp_exit("done\n");
}


VFLD_DTYPE Voxel_Field::get(int x, int y, int z)
{
    if ((x<0) || (y<0) || (z<0) || (x >= x_size) || (y >= y_size) || (z >= z_size))
        return(false);
    return( data[(z * y_size + y) * x_size + x] );
}

void Voxel_Field::set(int x, int y, int z, VFLD_DTYPE value)
{
    if ((x<0) || (y<0) || (z<0) || (x >= x_size) || (y >= y_size) || (z >= z_size))
        return;
    data[(z * y_size + y) * x_size + x] =  value;
}

void Voxel_Field::scalar_add(VFLD_DTYPE v)
{
    VFLD_DTYPE * rootvox;
    int px, py, pz;
    
    timestamp_entry("Voxel_Field::scalar_add:\n");
    
    for (pz=0; pz < z_size; pz++) {
        for (py=0; py < y_size; py++) {
            rootvox = data;
            rootvox += (pz * y_size + py) * x_size;
            for (px=0; px < x_size; px++) {
                *rootvox++ += v;
            }
        }
    }
    timestamp_exit("done\n");
}

void Voxel_Field::scalar_multiply(VFLD_DTYPE v, VFLD_DTYPE d)
{
    VFLD_DTYPE * rootvox;
    int px, py, pz;
    
    timestamp_entry("Voxel_Field::scalar_multiply:\n");
    
    for (pz=0; pz < z_size; pz++) {
        for (py=0; py < y_size; py++) {
            rootvox = data;
            rootvox += (pz * y_size + py) * x_size;
            for (px=0; px < x_size; px++) {
                *rootvox++ = (*rootvox * v) / d;
            }
        }
    }
    timestamp_exit("done\n");
}

void Voxel_Field::scalar_inverse(VFLD_DTYPE v)
{
    VFLD_DTYPE * rootvox;
    int px, py, pz;
    
    timestamp_entry("Voxel_Field::scalar_inverse:\n");
    
    for (pz=0; pz < z_size; pz++) {
        for (py=0; py < y_size; py++) {
            rootvox = data;
            rootvox += (pz * y_size + py) * x_size;
            for (px=0; px < x_size; px++) {
                *rootvox++ = v / *rootvox;
            }
        }
    }
    timestamp_exit("done\n");
}

void Voxel_Field::threshold(VFLD_DTYPE v)
{
    VFLD_DTYPE * rootvox;
    int px, py, pz;
    
    timestamp_entry("Voxel_Field::threshold:\n");
    
    for (pz=0; pz < z_size; pz++) {
        for (py=0; py < y_size; py++) {
            rootvox = data;
            rootvox += (pz * y_size + py) * x_size;
            for (px=0; px < x_size; px++) {
                *rootvox++ = (*rootvox >= v ? 1 : 0);
            }
        }
    }
    timestamp_exit("done\n");
}

void Voxel_Field::quantise(short int nstates)
{
    VFLD_DTYPE * srcvox;
    VFLD_DTYPE curmax=-32000;
    int px, py, pz;
    VFLD_DTYPE scaleC;

    timestamp_entry("Voxel_Field::quantise( )\n");
    IF_DEBUG timestamp_inside("scan for maximum value\n");
    for (pz=0; pz < z_size; pz++) {
        for (py=0; py < y_size; py++) {
            srcvox = data;
            srcvox += (pz * y_size + py) * x_size;
            for (px=0; px < x_size; px++) {
                if (*srcvox++ > curmax) curmax = srcvox[-1];
            }
        }
    }
    if (curmax <= 0)
    {
        timestamp_inside("warning - maximum <= 0\n");
        return;
    }
    
    IF_DEBUG timestamp_inside("quantising\n");
    scaleC = (curmax + 1) / nstates;
    for (pz=0; pz < z_size; pz++) {
        for (py=0; py < y_size; py++) {
            srcvox = data;
            srcvox += (pz * y_size + py) * x_size;
            for (px=0; px < x_size; px++) {
                if (*srcvox > 0) 
                    *srcvox = *srcvox / scaleC + 1;
                srcvox++;
            }
        }
    }
    timestamp_exit("done\n");
}

void Voxel_Field::add(const Voxel_Field& vfld)
{
    VFLD_DTYPE * srcvox;
    VFLD_DTYPE * rootvox;
    
    timestamp_entry("Voxel_Field::add:\n");

    if ((x_size != vfld.x_size) || (y_size != vfld.y_size) || (z_size != vfld.z_size) || (origin.x != vfld.origin.x) || (origin.y != vfld.origin.y) || (origin.z != vfld.origin.z))
    {
        timestamp_exit("error: dimension/origin mismatch\n");
        return;
    }
    
    rootvox = data;
    srcvox = vfld.data;
    long size = x_size * y_size * z_size;
    for (long pos=0; pos < size; pos++) {
        *rootvox++ += *srcvox++;
    }
    timestamp_exit("done\n");
}

void Voxel_Field::multiply(const Voxel_Field& vfld)
{
    VFLD_DTYPE * srcvox;
    VFLD_DTYPE * rootvox;
    
    timestamp_entry("Voxel_Field::multiply:\n");

    if ((x_size != vfld.x_size) || (y_size != vfld.y_size) || (z_size != vfld.z_size) || (origin.x != vfld.origin.x) || (origin.y != vfld.origin.y) || (origin.z != vfld.origin.z))
    {
        timestamp_exit("error: dimension/origin mismatch\n");
        return;
    }
    
    rootvox = data;
    srcvox = vfld.data;
    long size = x_size * y_size * z_size;
    for (long pos=0; pos < size; pos++) {
        *rootvox++ *= *srcvox++;
    }
    timestamp_exit("done\n");
}

// ////////////////////////////// DISTANCE TRANSFORM ///////////////////////////////////

VFLD_DTYPE Voxel_Field::find_5x5x5_min_fwd(int px, int py, int pz) {
    VFLD_DTYPE curmin;
    VFLD_DTYPE curval=0;
    
    curmin = get(px, py, pz);
    curval = get(px - 1, py, pz) + DT_5x5x5_a; if (curval < curmin) curmin=curval;
    curval = get(px, py - 1, pz) + DT_5x5x5_a; if (curval < curmin) curmin=curval;
    curval = get(px, py, pz - 1) + DT_5x5x5_a; if (curval < curmin) curmin=curval;

    curval = get(px - 1, py - 1, pz) + DT_5x5x5_b; if (curval < curmin) curmin=curval;
    curval = get(px + 1, py - 1, pz) + DT_5x5x5_b; if (curval < curmin) curmin=curval;
    curval = get(px - 1, py, pz - 1) + DT_5x5x5_b; if (curval < curmin) curmin=curval;
    curval = get(px + 1, py, pz - 1) + DT_5x5x5_b; if (curval < curmin) curmin=curval;
    curval = get(px, py - 1, pz - 1) + DT_5x5x5_b; if (curval < curmin) curmin=curval;
    curval = get(px, py - 1, pz - 1) + DT_5x5x5_b; if (curval < curmin) curmin=curval;
    
    curval = get(px - 1, py - 1, pz - 1) + DT_5x5x5_c; if (curval < curmin) curmin=curval;
    curval = get(px + 1, py - 1, pz - 1) + DT_5x5x5_c; if (curval < curmin) curmin=curval;
    curval = get(px + 1, py + 1, pz - 1) + DT_5x5x5_c; if (curval < curmin) curmin=curval;
    curval = get(px - 1, py + 1, pz - 1) + DT_5x5x5_c; if (curval < curmin) curmin=curval;
        
    curval = get(px - 1, py - 2, pz) + DT_5x5x5_d; if (curval < curmin) curmin=curval;
    curval = get(px + 1, py - 2, pz) + DT_5x5x5_d; if (curval < curmin) curmin=curval;
    curval = get(px + 2, py - 1, pz) + DT_5x5x5_d; if (curval < curmin) curmin=curval;
    curval = get(px - 2, py - 1, pz) + DT_5x5x5_d; if (curval < curmin) curmin=curval;
    curval = get(px, py - 2, pz - 1) + DT_5x5x5_d; if (curval < curmin) curmin=curval;
    curval = get(px - 2, py, pz - 1) + DT_5x5x5_d; if (curval < curmin) curmin=curval;
    curval = get(px, py + 2, pz - 1) + DT_5x5x5_d; if (curval < curmin) curmin=curval;
    curval = get(px + 2, py, pz - 1) + DT_5x5x5_d; if (curval < curmin) curmin=curval;
    curval = get(px, py - 1, pz - 2) + DT_5x5x5_d; if (curval < curmin) curmin=curval;
    curval = get(px - 1, py, pz - 2) + DT_5x5x5_d; if (curval < curmin) curmin=curval;
    curval = get(px, py + 1, pz - 2) + DT_5x5x5_d; if (curval < curmin) curmin=curval;
    curval = get(px + 1, py, pz - 2) + DT_5x5x5_d; if (curval < curmin) curmin=curval;

    curval = get(px - 1, py - 2, pz - 1) + DT_5x5x5_e; if (curval < curmin) curmin=curval;
    curval = get(px + 1, py - 2, pz - 1) + DT_5x5x5_e; if (curval < curmin) curmin=curval;
    curval = get(px + 2, py - 1, pz - 1) + DT_5x5x5_e; if (curval < curmin) curmin=curval;
    curval = get(px + 2, py + 1, pz - 1) + DT_5x5x5_e; if (curval < curmin) curmin=curval;
    curval = get(px + 1, py + 2, pz - 1) + DT_5x5x5_e; if (curval < curmin) curmin=curval;
    curval = get(px - 1, py + 2, pz - 1) + DT_5x5x5_e; if (curval < curmin) curmin=curval;
    curval = get(px - 2, py + 1, pz - 1) + DT_5x5x5_e; if (curval < curmin) curmin=curval;
    curval = get(px - 2, py - 1, pz - 1) + DT_5x5x5_e; if (curval < curmin) curmin=curval;
    curval = get(px - 1, py - 1, pz - 2) + DT_5x5x5_e; if (curval < curmin) curmin=curval;
    curval = get(px + 1, py - 1, pz - 2) + DT_5x5x5_e; if (curval < curmin) curmin=curval;
    curval = get(px + 1, py + 1, pz - 2) + DT_5x5x5_e; if (curval < curmin) curmin=curval;
    curval = get(px - 1, py + 1, pz - 2) + DT_5x5x5_e; if (curval < curmin) curmin=curval;
    
    curval = get(px - 2, py - 2, pz - 1) + DT_5x5x5_f; if (curval < curmin) curmin=curval;
    curval = get(px + 2, py - 2, pz - 1) + DT_5x5x5_f; if (curval < curmin) curmin=curval;
    curval = get(px + 2, py + 2, pz - 1) + DT_5x5x5_f; if (curval < curmin) curmin=curval;
    curval = get(px - 2, py + 2, pz - 1) + DT_5x5x5_f; if (curval < curmin) curmin=curval;
    curval = get(px - 1, py - 2, pz - 2) + DT_5x5x5_f; if (curval < curmin) curmin=curval;
    curval = get(px + 1, py - 2, pz - 2) + DT_5x5x5_f; if (curval < curmin) curmin=curval;
    curval = get(px + 2, py - 1, pz - 2) + DT_5x5x5_f; if (curval < curmin) curmin=curval;
    curval = get(px + 2, py + 1, pz - 2) + DT_5x5x5_f; if (curval < curmin) curmin=curval;
    curval = get(px + 1, py + 2, pz - 2) + DT_5x5x5_f; if (curval < curmin) curmin=curval;
    curval = get(px - 1, py + 2, pz - 2) + DT_5x5x5_f; if (curval < curmin) curmin=curval;
    curval = get(px - 2, py + 1, pz - 2) + DT_5x5x5_f; if (curval < curmin) curmin=curval;
    curval = get(px - 2, py - 1, pz - 2) + DT_5x5x5_f; if (curval < curmin) curmin=curval;
    return(curmin);
}

VFLD_DTYPE Voxel_Field::find_5x5x5_min_bwd(int px, int py, int pz) {
    VFLD_DTYPE curmin;
    VFLD_DTYPE curval=0;
    curmin = get(px, py, pz);
    curval = get(px + 1, py, pz) + DT_5x5x5_a; if (curval < curmin) curmin=curval;
    curval = get(px, py + 1, pz) + DT_5x5x5_a; if (curval < curmin) curmin=curval;
    curval = get(px, py, pz + 1) + DT_5x5x5_a; if (curval < curmin) curmin=curval;

    curval = get(px - 1, py + 1, pz) + DT_5x5x5_b; if (curval < curmin) curmin=curval;
    curval = get(px + 1, py + 1, pz) + DT_5x5x5_b; if (curval < curmin) curmin=curval;
    curval = get(px - 1, py, pz + 1) + DT_5x5x5_b; if (curval < curmin) curmin=curval;
    curval = get(px + 1, py, pz + 1) + DT_5x5x5_b; if (curval < curmin) curmin=curval;
    curval = get(px, py - 1, pz + 1) + DT_5x5x5_b; if (curval < curmin) curmin=curval;
    curval = get(px, py - 1, pz + 1) + DT_5x5x5_b; if (curval < curmin) curmin=curval;
    
    curval = get(px - 1, py - 1, pz + 1) + DT_5x5x5_c; if (curval < curmin) curmin=curval;
    curval = get(px + 1, py - 1, pz + 1) + DT_5x5x5_c; if (curval < curmin) curmin=curval;
    curval = get(px + 1, py + 1, pz + 1) + DT_5x5x5_c; if (curval < curmin) curmin=curval;
    curval = get(px - 1, py + 1, pz + 1) + DT_5x5x5_c; if (curval < curmin) curmin=curval;
        
    curval = get(px + 2, py + 1, pz) + DT_5x5x5_d; if (curval < curmin) curmin=curval;
    curval = get(px + 1, py + 2, pz) + DT_5x5x5_d; if (curval < curmin) curmin=curval;
    curval = get(px - 1, py + 2, pz) + DT_5x5x5_d; if (curval < curmin) curmin=curval;
    curval = get(px - 2, py + 1, pz) + DT_5x5x5_d; if (curval < curmin) curmin=curval;
    curval = get(px, py - 2, pz + 1) + DT_5x5x5_d; if (curval < curmin) curmin=curval;
    curval = get(px - 2, py, pz + 1) + DT_5x5x5_d; if (curval < curmin) curmin=curval;
    curval = get(px, py + 2, pz + 1) + DT_5x5x5_d; if (curval < curmin) curmin=curval;
    curval = get(px + 2, py, pz + 1) + DT_5x5x5_d; if (curval < curmin) curmin=curval;
    curval = get(px, py - 1, pz + 2) + DT_5x5x5_d; if (curval < curmin) curmin=curval;
    curval = get(px - 1, py, pz + 2) + DT_5x5x5_d; if (curval < curmin) curmin=curval;
    curval = get(px, py + 1, pz + 2) + DT_5x5x5_d; if (curval < curmin) curmin=curval;
    curval = get(px + 1, py, pz + 2) + DT_5x5x5_d; if (curval < curmin) curmin=curval;

    curval = get(px - 1, py - 2, pz + 1) + DT_5x5x5_e; if (curval < curmin) curmin=curval;
    curval = get(px + 1, py - 2, pz + 1) + DT_5x5x5_e; if (curval < curmin) curmin=curval;
    curval = get(px + 2, py - 1, pz + 1) + DT_5x5x5_e; if (curval < curmin) curmin=curval;
    curval = get(px + 2, py + 1, pz + 1) + DT_5x5x5_e; if (curval < curmin) curmin=curval;
    curval = get(px + 1, py + 2, pz + 1) + DT_5x5x5_e; if (curval < curmin) curmin=curval;
    curval = get(px - 1, py + 2, pz + 1) + DT_5x5x5_e; if (curval < curmin) curmin=curval;
    curval = get(px - 2, py + 1, pz + 1) + DT_5x5x5_e; if (curval < curmin) curmin=curval;
    curval = get(px - 2, py - 1, pz + 1) + DT_5x5x5_e; if (curval < curmin) curmin=curval;
    curval = get(px - 1, py - 1, pz + 2) + DT_5x5x5_e; if (curval < curmin) curmin=curval;
    curval = get(px + 1, py - 1, pz + 2) + DT_5x5x5_e; if (curval < curmin) curmin=curval;
    curval = get(px + 1, py + 1, pz + 2) + DT_5x5x5_e; if (curval < curmin) curmin=curval;
    curval = get(px - 1, py + 1, pz + 2) + DT_5x5x5_e; if (curval < curmin) curmin=curval;
    
    curval = get(px - 2, py - 2, pz + 1) + DT_5x5x5_f; if (curval < curmin) curmin=curval;
    curval = get(px + 2, py - 2, pz + 1) + DT_5x5x5_f; if (curval < curmin) curmin=curval;
    curval = get(px + 2, py + 2, pz + 1) + DT_5x5x5_f; if (curval < curmin) curmin=curval;
    curval = get(px - 2, py + 2, pz + 1) + DT_5x5x5_f; if (curval < curmin) curmin=curval;
    curval = get(px - 1, py - 2, pz + 2) + DT_5x5x5_f; if (curval < curmin) curmin=curval;
    curval = get(px + 1, py - 2, pz + 2) + DT_5x5x5_f; if (curval < curmin) curmin=curval;
    curval = get(px + 2, py - 1, pz + 2) + DT_5x5x5_f; if (curval < curmin) curmin=curval;
    curval = get(px + 2, py + 1, pz + 2) + DT_5x5x5_f; if (curval < curmin) curmin=curval;
    curval = get(px + 1, py + 2, pz + 2) + DT_5x5x5_f; if (curval < curmin) curmin=curval;
    curval = get(px - 1, py + 2, pz + 2) + DT_5x5x5_f; if (curval < curmin) curmin=curval;
    curval = get(px - 2, py + 1, pz + 2) + DT_5x5x5_f; if (curval < curmin) curmin=curval;
    curval = get(px - 2, py - 1, pz + 2) + DT_5x5x5_f; if (curval < curmin) curmin=curval;
    return(curmin);
}

void Voxel_Field::dt(bool fromseed) {
    VFLD_DTYPE * srcvox;
    int px, py, pz;
    
    timestamp_entry("Voxel_Field::dt( )\n");
    
    IF_DEBUG timestamp_inside("labelling Voxel_Field...\n");
    for (pz=0; pz < z_size; pz++) {
        for (py=0; py < y_size; py++) {
            srcvox = data;
            srcvox += (pz * y_size + py) * x_size;
            for (px=0; px < x_size; px++) {
                if (fromseed==true)
                {
                    switch (*srcvox)
                    {
                        case 0:
                            *srcvox++ = 32700;
                            break;
                        case 1:
                            *srcvox++ = 10000;
                            break;
                        case 2:
                        default:
                            *srcvox++ = 0;
                    }
                }
                else
                {
                    *srcvox++ = (*srcvox > 0 ? 32766 : 0);
                }
            }
        }
    }
    
    IF_DEBUG timestamp_inside("forward scan\n");
    if (fromseed==true)
    {
        for (pz=0; pz < z_size; pz++) {
            for (py=0; py < y_size; py++) {
                for (px=0; px < x_size; px++) {
                    if (data[(pz * y_size + py) * x_size + px] != 32700)
                        data[(pz * y_size + py) * x_size + px] = find_5x5x5_min_fwd(px, py, pz);
                }
            }
        }
    }
    else
    {
        for (pz=0; pz < z_size; pz++) {
            for (py=0; py < y_size; py++) {
                for (px=0; px < x_size; px++) {
                    if (data[(pz * y_size + py) * x_size + px] > 0)
                        data[(pz * y_size + py) * x_size + px] = find_5x5x5_min_fwd(px, py, pz);
                }
            }
        }
    }
    
    IF_DEBUG timestamp_inside("backward scan\n");
    if (fromseed==true)
    {
        for (pz=z_size-1; pz >= 0; pz--) {
            for (py=y_size-1; py >= 0; py--) {
                for (px=x_size-1; px >= 0; px--) {
                    if (data[(pz * y_size + py) * x_size + px] != 32700)
                        data[(pz * y_size + py) * x_size + px] = find_5x5x5_min_bwd(px, py, pz);
                }
            }
        }
    }
    else
    {
        for (pz=z_size-1; pz >= 0; pz--) {
            for (py=y_size-1; py >= 0; py--) {
                for (px=x_size-1; px >= 0; px--) {
                    if (data[(pz * y_size + py) * x_size + px] > 0)
                        data[(pz * y_size + py) * x_size + px] = find_5x5x5_min_bwd(px, py, pz);
                }
            }
        }
    }
    
    if (fromseed==true)
    {
        IF_DEBUG timestamp_inside("cleanup scan\n");
        for (pz=0; pz < z_size; pz++) {
            for (py=0; py < y_size; py++) {
                srcvox = data;
                srcvox += (pz * y_size + py) * x_size;
                for (px=0; px < x_size; px++) {
                    if (*srcvox >= 10000)
                        *srcvox = 0;
                    else
                        *srcvox += 1;
                    srcvox++;
                }
            }
        }
    }
    timestamp_exit("done\n");
}

// ///////////////////////////////// SKELETONISATION //////////////////////////////

VFLD_DTYPE Voxel_Field::mean_neighbours(int px, int py, int pz) {
    VFLD_DTYPE cnt=0;
    VFLD_DTYPE cur=0;
    VFLD_DTYPE sum=0;
    
    cur = get(px - 1, py - 1, pz - 1); if (cur > 0) { cnt++; sum += cur; }
    cur = get(px    , py - 1, pz - 1); if (cur > 0) { cnt++; sum += cur; }
    cur = get(px + 1, py - 1, pz - 1); if (cur > 0) { cnt++; sum += cur; }
    cur = get(px - 1, py    , pz - 1); if (cur > 0) { cnt++; sum += cur; }
    cur = get(px    , py    , pz - 1); if (cur > 0) { cnt++; sum += cur; }
    cur = get(px + 1, py    , pz - 1); if (cur > 0) { cnt++; sum += cur; }
    cur = get(px - 1, py + 1, pz - 1); if (cur > 0) { cnt++; sum += cur; }
    cur = get(px    , py + 1, pz - 1); if (cur > 0) { cnt++; sum += cur; }
    cur = get(px + 1, py + 1, pz - 1); if (cur > 0) { cnt++; sum += cur; }

    cur = get(px - 1, py - 1, pz    ); if (cur > 0) { cnt++; sum += cur; }
    cur = get(px    , py - 1, pz    ); if (cur > 0) { cnt++; sum += cur; }
    cur = get(px + 1, py - 1, pz    ); if (cur > 0) { cnt++; sum += cur; }
    cur = get(px - 1, py    , pz    ); if (cur > 0) { cnt++; sum += cur; }
    cur = get(px + 1, py    , pz    ); if (cur > 0) { cnt++; sum += cur; }
    cur = get(px - 1, py + 1, pz    ); if (cur > 0) { cnt++; sum += cur; }
    cur = get(px    , py + 1, pz    ); if (cur > 0) { cnt++; sum += cur; }
    cur = get(px + 1, py + 1, pz    ); if (cur > 0) { cnt++; sum += cur; }
    
    cur = get(px - 1, py - 1, pz + 1); if (cur > 0) { cnt++; sum += cur; }
    cur = get(px    , py - 1, pz + 1); if (cur > 0) { cnt++; sum += cur; }
    cur = get(px + 1, py - 1, pz + 1); if (cur > 0) { cnt++; sum += cur; }
    cur = get(px - 1, py    , pz + 1); if (cur > 0) { cnt++; sum += cur; }
    cur = get(px    , py    , pz + 1); if (cur > 0) { cnt++; sum += cur; }
    cur = get(px + 1, py    , pz + 1); if (cur > 0) { cnt++; sum += cur; }
    cur = get(px - 1, py + 1, pz + 1); if (cur > 0) { cnt++; sum += cur; }
    cur = get(px    , py + 1, pz + 1); if (cur > 0) { cnt++; sum += cur; }
    cur = get(px + 1, py + 1, pz + 1); if (cur > 0) { cnt++; sum += cur; }

    sum = (cnt > 0 ? sum / cnt : 0);
    
    return(sum);
}

void Voxel_Field::pct(VFLD_DTYPE tp) {
    VFLD_DTYPE * srcvox;
    int px, py, pz;
    
    timestamp_entry("Voxel_Field::pct( ):\n");
    
    VFLD_DTYPE * dst = new VFLD_DTYPE[x_size * y_size * z_size];
    for (int i=0; i < x_size * y_size * z_size; i++)
        dst[i] = 0;
    
    for (pz=0; pz < z_size; pz++) {
        for (py=0; py < y_size; py++) {
            srcvox = data;
            srcvox += (pz * y_size + py) * x_size;
            for (px=0; px < x_size; px++) {
                if (*srcvox > 0)
                {
                    if (mean_neighbours(px,py,pz) < *srcvox - tp)
                        dst[(pz * y_size + py) * x_size + px] = *srcvox;
                }
                srcvox++;
            }
        }
    }
    delete [] data;
    data = dst;
    timestamp_exit("done\n");
}

VFLD_DTYPE Voxel_Field::MAT_maximal(int px, int py, int pz) {
    VFLD_DTYPE cur = get(px, py, pz);
    if (get(px - 1, py - 1, pz - 1) - 11 >= cur) return(0);
    if (get(px    , py - 1, pz - 1) -  9 >= cur) return(0);
    if (get(px + 1, py - 1, pz - 1) - 11 >= cur) return(0);
    if (get(px - 1, py    , pz - 1) -  9 >= cur) return(0);
    if (get(px    , py    , pz - 1) -  6 >= cur) return(0);
    if (get(px + 1, py    , pz - 1) -  9 >= cur) return(0);
    if (get(px - 1, py + 1, pz - 1) - 11 >= cur) return(0);
    if (get(px    , py + 1, pz - 1) -  9 >= cur) return(0);
    if (get(px + 1, py + 1, pz - 1) - 11 >= cur) return(0);
    
    if (get(px - 1, py - 1, pz    ) -  9 >= cur) return(0);
    if (get(px    , py - 1, pz    ) -  6 >= cur) return(0);
    if (get(px + 1, py - 1, pz    ) -  9 >= cur) return(0);
    if (get(px - 1, py    , pz    ) -  6 >= cur) return(0);
    if (get(px + 1, py    , pz    ) -  6 >= cur) return(0);
    if (get(px - 1, py + 1, pz    ) -  9 >= cur) return(0);
    if (get(px    , py + 1, pz    ) -  6 >= cur) return(0);
    if (get(px + 1, py + 1, pz    ) -  9 >= cur) return(0);
    
    if (get(px - 1, py - 1, pz + 1) - 11 >= cur) return(0);
    if (get(px    , py - 1, pz + 1) -  9 >= cur) return(0);
    if (get(px + 1, py - 1, pz + 1) - 11 >= cur) return(0);
    if (get(px - 1, py    , pz + 1) -  9 >= cur) return(0);
    if (get(px    , py    , pz + 1) -  6 >= cur) return(0);
    if (get(px + 1, py    , pz + 1) -  9 >= cur) return(0);
    if (get(px - 1, py + 1, pz + 1) - 11 >= cur) return(0);
    if (get(px    , py + 1, pz + 1) -  9 >= cur) return(0);
    if (get(px + 1, py + 1, pz + 1) - 11 >= cur) return(0);
    
    return(cur);
}

void Voxel_Field::mat( ) {
    VFLD_DTYPE * srcvox;
    VFLD_DTYPE * dstvox;
    int px, py, pz;
    
    timestamp_entry("Voxel_Field::mat( ):\n");
    
    VFLD_DTYPE * dst = new VFLD_DTYPE[x_size * y_size * z_size];
    for (int i=0; i < x_size * y_size * z_size; i++)
        dst[i] = 0;
    
    for (pz=0; pz < z_size; pz++) {
        for (py=0; py < y_size; py++) {
            srcvox = data;
            srcvox += (pz * y_size + py) * x_size;
            dstvox = dst;
            dstvox += (pz * y_size + py) * x_size;
            for (px=0; px < x_size; px++) {
                if (*srcvox > 0)
                {
                    *dstvox = MAT_maximal(px, py, pz);
                }
                srcvox++; dstvox++;
            }
        }
    }
    delete [] data;
    data = dst;
    timestamp_exit("done\n");
}

void Voxel_Field::fill( )
{
    VFLD_DTYPE * srcvox;
    VFLD_DTYPE * voxBX;
    VFLD_DTYPE * voxBY;
    VFLD_DTYPE * voxBZ;
    int px, py, pz;
    VFLD_DTYPE bx, by, bz, bt;
    VFLD_DTYPE cnt;
    int region_count = 0;
    
    timestamp_entry("Voxel_Field::fill( ):\n"); fflush(stdout);
    
    REGION * region_table = new REGION[REGIONTABLESIZE];
    VFLD_DTYPE * trans_table = new VFLD_DTYPE[REGIONTABLESIZE];
    
    for (cnt=0; cnt < REGIONTABLESIZE; cnt++) {
        trans_table[cnt] = region_table[cnt].link = -1;
    }
    
    IF_DEBUG timestamp_inside("building region table\n");
    for (pz=0; pz < z_size; pz++) {
        for (py=0; py < y_size; py++) {
            srcvox = data;
            srcvox += (pz * y_size + py) * x_size;
            voxBX = srcvox - 1;
            voxBY = srcvox - x_size;
            voxBZ = srcvox - x_size * y_size;
            for (px=0; px < x_size; px++) {
                if (*srcvox == 0)
                {
                    bt = -1;
                    bx = (px > 0 ? *voxBX : 0);
                    by = (py > 0 ? *voxBY : 0);
                    bz = (pz > 0 ? *voxBZ : 0);
                    bx = (bx > 0 ? (region_table[bx-1].qval == *srcvox ? bx - 1 : -1) : -1);
                    by = (by > 0 ? (region_table[by-1].qval == *srcvox ? by - 1 : -1) : -1);
                    bz = (bz > 0 ? (region_table[bz-1].qval == *srcvox ? bz - 1 : -1) : -1);
                    if (bx >= 0) for(; bx != region_table[bx].link; bx = region_table[bx].link);
                    if (by >= 0) for(; by != region_table[by].link; by = region_table[by].link);
                    if (bz >= 0) for(; bz != region_table[bz].link; bz = region_table[bz].link);
                    if (bz >= 0) bt = bz;
                    if (by >= 0) {
                        if (bt == -1) {
                            bt = by;
                        } else {
                            if (bt != by) {
                                region_table[by].link = bt;
                                for (cnt=0; cnt < REGIONTABLESIZE; cnt++)
                                    if (region_table[cnt].link >= 0)
                                        for (; region_table[cnt].link != region_table[region_table[cnt].link].link; region_table[cnt].link = region_table[region_table[cnt].link].link);
                            }
                        }
                    }
                    if (bx >= 0) {
                        if (bt == -1) {
                            bt = bx;
                        } else {
                            if (bt != bx) {
                                region_table[bx].link = bt;
                                for (cnt=0; cnt < REGIONTABLESIZE; cnt++)
                                    if (region_table[cnt].link >= 0)
                                        for (; region_table[cnt].link != region_table[region_table[cnt].link].link; region_table[cnt].link = region_table[region_table[cnt].link].link);
                            }
                        }
                    }
                    if (bt == -1)
                    {
                        bt = region_count++;
                        if (region_count >= REGIONTABLESIZE) { fprintf(stderr, "  region_table not big enough!!\n"); exit(1); }
                        region_table[bt].link = bt;
                        region_table[bt].qval = *srcvox;
                    }
                    *srcvox = bt + 1;
                }
                else
                {
                    *srcvox = 0;
                }
                srcvox++; voxBX++; voxBY++; voxBZ++;
            }
        }
    }
    
    IF_DEBUG timestamp_inside("optimising region table\n");
    region_count = 0;
    for (cnt=0; cnt < REGIONTABLESIZE; cnt++)
        if (region_table[cnt].link >= 0) {
            for(bx = region_table[cnt].link; bx != region_table[bx].link; bx = region_table[bx].link);
            if (trans_table[bx] == -1)
            {
                trans_table[bx] = region_count++;
            }
            trans_table[cnt] = trans_table[bx];
            //printf("  > region_table[%hd].link = %hd, trans_table[%hd] = %hd\n", cnt, region_table[cnt].link, cnt,  trans_table[cnt]);
        }
    
    IF_DEBUG timestamp_inside("relabling vfield\n");
    for (pz=0; pz < z_size; pz++) {
        for (py=0; py < y_size; py++) {
            srcvox = data;
            srcvox += (pz * y_size + py) * x_size;
            for (px=0; px < x_size; px++) {
                if (*srcvox > 0)
                {
                    bx = trans_table[*srcvox - 1];
                    *srcvox = (bx > 0 ? 1 : 0);
                }
                else
                {
                    *srcvox = 1;
                }
                srcvox++;
            }
        }
    }
    delete [] region_table;
    delete [] trans_table;
    timestamp_exit("done\n");
}

float Voxel_Field::gM(int pow_Z, int pow_Y, int pow_X)
{
    float t,x,y,z;
    float M=0.0;
    VFLD_DTYPE * pnt = data;
    int c;
    
    timestamp_entry("Voxel_Field::gM( )\n");
    
    if ((x==0) && (y==0) &&  (z==0))
        return( gM_000( ) );
    if ((x==1) && (y==0) &&  (z==0))
        return( gM_001( ) );
    if ((x==0) && (y==1) &&  (z==0))
        return( gM_010( ) );
    if ((x==0) && (y==0) &&  (z==1))
        return( gM_100( ) );
    
    for (int pz=0; pz < z_size; pz++)
    {
        t = ((float) pz) + origin.z;
        z = 1.0;
        for (c=0; c < pow_Z; c++)
            z *= t;
        for (int py=0; py < y_size; py++)
        {
            t = ((float) py) + origin.y;
            y = 1.0;
            for (c=0; c < pow_Y; c++)
                y *= t;
            for (int px=0; px < x_size; px++)
            {
                t = ((float) px) + origin.x;
                x = 1.0;
                for (c=0; c < pow_X; c++)
                    x *= t;
                M += z * y *  x * ((float) *pnt++);
            }
        }
    }
    timestamp_exit("done\n");
    return(M);
}

float Voxel_Field::gM_000( )
{
    float M=0.0;
    VFLD_DTYPE * pnt = data;
    
    for (int pz=0; pz < z_size; pz++)
    {
        for (int py=0; py < y_size; py++)
        {
            for (int px=0; px < x_size; px++)
            {
                M += (float) *pnt++;
            }
        }
    }
    return(M);
}

float Voxel_Field::gM_100( )
{
    float z;
    float M=0.0;
    VFLD_DTYPE * pnt = data;
    
    for (int pz=0; pz < z_size; pz++)
    {
        z = ((float) pz) + origin.z;
        for (int py=0; py < y_size; py++)
        {
            for (int px=0; px < x_size; px++)
            {
                M += z * ((float) *pnt++);
            }
        }
    }
    return(M);
}

float Voxel_Field::gM_010( )
{
    float y;
    float M=0.0;
    VFLD_DTYPE * pnt = data;
    
    for (int pz=0; pz < z_size; pz++)
    {
        for (int py=0; py < y_size; py++)
        {
            y = ((float) py) + origin.y;
            for (int px=0; px < x_size; px++)
            {
                M += y * ((float) *pnt++);
            }
        }
    }
    return(M);
}

float Voxel_Field::gM_001( )
{
    float M=0.0;
    VFLD_DTYPE * pnt = data;
    
    for (int pz=0; pz < z_size; pz++)
    {
        for (int py=0; py < y_size; py++)
        {
            for (int px=0; px < x_size; px++)
            {
                M +=  (((float) px) + origin.x) * ((float) *pnt++);
            }
        }
    }
    return(M);
}

float Voxel_Field::cgM(int pow_Z, int pow_Y, int pow_X)
{
    float t,x,y,z;
    float cx = 0.0;
    float cy = 0.0;
    float cz = 0.0;
    float M=0.0;
    VFLD_DTYPE * pnt = data;
    int c;
    
    timestamp_entry("Voxel_Field::cgM( )\n");
    
    for (int pz=0; pz < z_size; pz++)
    {
        z = (float) pz;
        for (int py=0; py < y_size; py++)
        {
            y = (float) py;
            for (int px=0; px < x_size; px++)
            {
                cx += ((float) px) * ((float) *pnt);
                cy += y * ((float) *pnt);
                cz += z * ((float) *pnt);
                M += (float) *pnt++;
            }
        }
    }
    cx /= M;
    cy /= M;
    cz /= M;
    IF_DEBUG { print_indent(); printf("com x=%g, y=%g, z=%g\n", cx, cy, cz); }
    
    if ((x==2) && (y==0) &&  (z==0))
        return( cgM_002( cx, cy, cz ) );
    if ((x==0) && (y==2) &&  (z==0))
        return( cgM_020( cx, cy, cz ) );
    if ((x==0) && (y==0) &&  (z==2))
        return( cgM_200( cx, cy, cz ) );
    
    pnt = data;
    M = 0.0;
    for (int pz=0; pz < z_size; pz++)
    {
        t = ((float) pz) - cz;
        z = 1.0;
        for (c=0; c < pow_Z; c++)
            z *= t;
        for (int py=0; py < y_size; py++)
        {
            t = ((float) py) - cy;
            y = 1.0;
            for (c=0; c < pow_Y; c++)
                y *= t;
            for (int px=0; px < x_size; px++)
            {
                t = ((float) px) - cx;
                x = 1.0;
                for (c=0; c < pow_X; c++)
                    x *= t;
                M += z * y *  x * ((float) *pnt++);
            }
        }
    }
    timestamp_exit("done\n");
    return(M);
}

float Voxel_Field::cgM_002( float cx, float cy, float cz )
{
    float x;
    float M=0.0;
    VFLD_DTYPE * pnt = data;
    for (int pz=0; pz < z_size; pz++)
    {
        for (int py=0; py < y_size; py++)
        {
            for (int px=0; px < x_size; px++)
            {
                x = ((float) px) - cx;
                M += x * x * ((float) *pnt++);
            }
        }
    }
    return(M);
}

float Voxel_Field::cgM_020( float cx, float cy, float cz )
{
    float y;
    float M=0.0;
    VFLD_DTYPE * pnt = data;
    for (int pz=0; pz < z_size; pz++)
    {
        for (int py=0; py < y_size; py++)
        {
            y = ((float) py) - cy;
            y *= y;
            for (int px=0; px < x_size; px++)
            {
                M += y * ((float) *pnt++);
            }
        }
    }
    return(M);
}

float Voxel_Field::cgM_200( float cx, float cy, float cz )
{
    float z;
    float M=0.0;
    VFLD_DTYPE * pnt = data;
    for (int pz=0; pz < z_size; pz++)
    {
        z = ((float) pz) - cz;
        z *= z;
        for (int py=0; py < y_size; py++)
        {
            for (int px=0; px < x_size; px++)
            {
                M += z * ((float) *pnt++);
            }
        }
    }
    return(M);
}

void Voxel_Field::half()
{
    VFLD_DTYPE * dst;
    VFLD_DTYPE * rootvox;
    VFLD_DTYPE * dstvox;
    int px, py, pz;
    
    timestamp_entry("Voxel_Field::half():\n");
    
    int new_xs = x_size / 2;
    int new_ys = y_size / 2;
    int new_zs = z_size / 2;
    
    if ((new_xs < 1) || (new_ys < 1) || (new_zs < 1))
    {
        IF_DEBUG print_indent(); printf("warning - can't half Voxel_Field - too small\n");
        return;
    }
    
    dst = new VFLD_DTYPE[new_zs * new_ys * new_xs]; 
    for (int i=0; i < new_zs * new_ys * new_xs; i++)
        dst[i] = 0;
        
    for (pz=0; pz < new_zs; pz++)
    {
        for (py=0; py < new_ys; py++)
        {
            rootvox = data;
            rootvox += ((pz * 2) * y_size + py*2) * x_size;
            dstvox = dst;
            dstvox += (pz * new_ys + py) * new_xs;
            for (px=0; px < new_xs; px++)
            {
                *dstvox++ = (rootvox[0] + rootvox[1] + rootvox[x_size] + rootvox[x_size+1] +
                                rootvox[y_size*x_size] + rootvox[y_size*x_size+1] + rootvox[y_size*x_size + x_size] + rootvox[y_size*x_size + 1 + x_size]) / 8;
                rootvox+=2;
            }
        }
    }

    delete [] data;
    data = dst;
    x_size = new_xs;
    y_size = new_ys;
    z_size = new_zs;
    
    timestamp_exit("done\n");    
}
