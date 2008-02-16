// vspace.cc
// A set of routines to create and manipulate a simple binary volumetric space
// Richard Seely - University of Southampton, UK 2006

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <zlib.h>

#include "util.hh"
#include "vspace.hh"

Voxel_Space::Voxel_Space( int x, int y, int z )
{
    timestamp_entry("Voxel_Space(x,y,z): ");
    x_size = x;
    y_size = y;
    z_size = z;
    data = NULL;
    
    origin.x = origin.y = origin.z = 0.0;
    alloc( );
    timestamp_exit("new Voxel_Space created\n");
}

void Voxel_Space::alloc( )
{
    data_width = sizeof(VSPC_DTYPE) * 8;
    data_width_m1 = sizeof(VSPC_DTYPE) * 8 - 1;
    x_span = (x_size / data_width) + (x_size % data_width == 0 ? 0 : 1);
    if (data != NULL)
        delete [] data;
    data = new VSPC_DTYPE[x_span * y_size * z_size];
    
    for (int i=0; i < x_span * y_size * z_size; i++)
        data[i] = 0;
    IF_DEBUG { print_indent(); printf("alloc: Voxel_Space::alloc: data_width=%d\n", data_width); }
}

Voxel_Space::~Voxel_Space( )
{
    timestamp_entry("~Voxel_Space: ");
    if (data != NULL)
        delete [] data;
    timestamp_exit("goodbye cruel world\n");
}

Voxel_Space::Voxel_Space( const Point_XYZ_List& lst )
{
    Point_XYZ_Item * cur;
    int ix, iy, iz, ip, is;
    Point_XYZ min, max;
    
    timestamp_entry("Voxel_Space(Point_XYZ_List):\n");
    
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
            printf("  out of bound voxel - ignoring\n");
        } else {
            ip = ix / data_width;
            is = ix % data_width;
            data[(iz * y_size + iy) * x_span + ip] = data[(iz * y_size + iy) * x_span + ip] | (VSPC_ONE << is);
        }
        cur = cur->next;
    }
    timestamp_exit("done\n");
}

Voxel_Space::Voxel_Space( char * filename )
{   
    timestamp_entry("Voxel_Space(filename):\n");
    
    if (strstr(filename, ".Zspc") != NULL)
    {
        int ps;
        IF_DEBUG timestamp_inside("reading compressed Voxel_Space\n");
        gzFile gfle;
        if ((gfle = gzopen(filename, "rb")) == NULL) {
            perror(" Voxel_Space(filename):  gzopen failed");
            delete(this);
        }
        gzread(gfle, &origin.x, sizeof(float));
        gzread(gfle, &origin.y, sizeof(float));
        gzread(gfle, &origin.z, sizeof(float));
        gzread(gfle, &x_size, sizeof(int));
        gzread(gfle, &y_size, sizeof(int));
        gzread(gfle, &z_size, sizeof(int));
        gzread(gfle, &ps, sizeof(int));
        if (ps != sizeof(VSPC_DTYPE))
        {
            fprintf(stderr, "info: this version of Voxel_Space is compiled for %d bit data, file is %d bit data, int is %d bit\n", sizeof(VSPC_DTYPE) * 8, ps * 8, sizeof(int) * 8);
            data = NULL;
            alloc( );
            // fxs -> how many (ps bytes wide) words each row spans (in the file)
            int fxs = (x_size / (ps * 8)) + (x_size % (ps * 8) == 0 ? 0 : 1);
            // cxs -> how many bytes each row spans
            int cxs = (x_size / 8) + (x_size % 8 == 0 ? 0 : 1);
            unsigned char * tmp_data = new unsigned char[fxs * y_size * z_size * ps];
            gzread(gfle, tmp_data, fxs * y_size * z_size * ps);
            
            unsigned char * dst;
            unsigned char * src;
            for (int pz=0; pz < z_size; pz++)
            {
                for (int py=0; py < y_size; py++)
                {
                    src = tmp_data;
                    src+= (pz * y_size + py) * fxs * ps;
                    dst = (unsigned char *) data;
                    dst += (pz * y_size + py) * x_span * sizeof(VSPC_DTYPE);
                    for (int px=0; px < cxs; px++)
                    {
                        *dst++ = *src++;
                    }
                }
            }
            delete [] tmp_data;
        }
        else
        {
            data = NULL;
            alloc( );
            gzread(gfle, data, x_span * y_size * z_size * sizeof(VSPC_DTYPE));
        }
        gzclose(gfle);
    }
    else
    {
        FILE * fle;
        char * line;
        char * sptr;
        VSPC_DTYPE * rootvox;
        int px, py, pz, ps;
        
        if ((fle = fopen(filename, "r")) == NULL) {
            perror(" Voxel_Space(filename):  fopen failed");
            delete(this);
        }
    
        IF_DEBUG timestamp_inside("reading ASCII Voxel_Space\n");
        fscanf(fle, "%g\t%g\t%g", &origin.x, &origin.y, &origin.z);
        fscanf(fle, "%d\t%d\t%d", &x_size, &y_size, &z_size);
        data = NULL;
        
        if ((line = (char *) malloc((x_size + 1) * sizeof(char))) == NULL) {
            perror("  Voxel_Space(filename): line malloc failed");
            exit(-1);
        }
        
        alloc( );
        
        IF_DEBUG { print_indent(); printf("sx=%d, sy=%d, sz=%d\n", x_size, y_size, z_size); }
        
        for (pz=0; pz < z_size; pz++) {
            for (py=0; py < y_size; py++) {
                rootvox = data;
                rootvox += (pz * y_size + py) * x_span;
                fscanf(fle, "%s", line);
                sptr = line;
                ps = 0;
                for (px=0; px < x_size; px++) {
                    if (*sptr++ == '#')
                        *rootvox = *rootvox | (VSPC_ONE << ps);
                    ps++;
                    if (ps > data_width_m1)
                    {
                        rootvox++;
                        ps = 0;
                    }
                }
            }
        }
        fclose(fle);
        free(line);
    }    
    timestamp_exit("done\n");
}

Voxel_Space::Voxel_Space( const Voxel_Space& vspc )
{    
    timestamp_entry("Voxel_Space(Voxel_Space):\n");
    
    x_size = vspc.x_size;
    y_size = vspc.y_size;
    z_size = vspc.z_size;
    origin.x = vspc.origin.x;
    origin.y = vspc.origin.y;
    origin.z = vspc.origin.z;
    data = NULL;
    
    alloc( );
    
    memcpy(data, vspc.data, x_span * y_size * z_size * sizeof(VSPC_DTYPE));
    
    timestamp_exit("done\n");
}

Voxel_Space::Voxel_Space( const Voxel_Field& vfld )
{
    int px,py,pz,ps;
    VFLD_DTYPE * rootvox;
    VSPC_DTYPE * dstvox;
    timestamp_entry("Voxel_Space(Voxel_Field):\n");
    
    x_size = vfld.x_size;
    y_size = vfld.y_size;
    z_size = vfld.z_size;
    origin.x = vfld.origin.x;
    origin.y = vfld.origin.y;
    origin.z = vfld.origin.z;
    data = NULL;
    
    alloc( );
    
    for (pz=0; pz < z_size; pz++) {
        for (py=0; py < y_size; py++) {
            rootvox = vfld.data;
            rootvox += (pz * vfld.y_size + py) * vfld.x_size;
            dstvox = data;
            dstvox += (pz * y_size + py) * x_span;
            ps = 0;
            for (px=0; px < x_size; px++) {
                if (*rootvox++ > 0)
                    *dstvox = *dstvox | (VSPC_ONE << ps);
                ps++;
                if (ps > data_width_m1)
                {
                    dstvox++;
                    ps = 0;
                }
            }
        }
    }
    timestamp_exit("done\n");
}


void Voxel_Space::save(char * filename)
{
    if (strstr(filename, ".Zspc") != NULL)
        saveZ(filename);
    else
        saveA(filename);
    
    
}

void Voxel_Space::saveA(char * filename)
{
    FILE * fle;
    char * line;
    char * sptr;
    VSPC_DTYPE * rootvox;
    int px, py, pz, sx;
    
    timestamp_entry("Voxel_Space::saveA:\n");
    
    if ((fle = fopen(filename, "w")) == NULL) {
        perror("  fopen failed");
        exit(-1);
    }
    
    if ((line = (char *) malloc((x_size + 1) * sizeof(char))) == NULL) {
        perror("  malloc failed");
        exit(-1);
    }
    
    fprintf(fle, "%g\t%g\t%g\n", origin.x, origin.y, origin.z);
    fprintf(fle, "%d\t%d\t%d\n\n", x_size, y_size, z_size);
    
    for (pz=0; pz < z_size; pz++) {
        for (py=0; py < y_size; py++) {
            rootvox = data;
            rootvox += (pz * y_size + py) * x_span;
            sptr = line;
            sx = 0;
            for (px=0; px < x_size; px++) {
                if ((*rootvox & (VSPC_ONE << sx++)) != 0) *sptr++ = '#'; else *sptr++ = '.';
                if (sx >= data_width)
                {
                    sx -= data_width;
                    rootvox++;
                }
            }
            *sptr = '\0';
            fprintf(fle, "%s\n", line);
        }
        fprintf(fle, "\n");
    }
    fclose(fle);
    free(line);
    
    timestamp_exit("done\n");
}


void Voxel_Space::saveZ(char * filename)
{
    gzFile gfle;
    int ps = sizeof(VSPC_DTYPE);
    timestamp_entry("Voxel_Space::saveZ:\n");
    
    if ((gfle = gzopen(filename, "wb")) == NULL) {
        perror("  gzopen failed");
        exit(-1);
    }
    
    gzwrite(gfle, &origin.x, sizeof(float));
    gzwrite(gfle, &origin.y, sizeof(float));
    gzwrite(gfle, &origin.z, sizeof(float));
    gzwrite(gfle, &x_size, sizeof(int));
    gzwrite(gfle, &y_size, sizeof(int));
    gzwrite(gfle, &z_size, sizeof(int));
    gzwrite(gfle, &ps, sizeof(int));
    gzwrite(gfle, data, x_span * y_size * z_size * sizeof(VSPC_DTYPE));
    gzclose(gfle);
    
    timestamp_exit("done\n");
}


void Voxel_Space::edge_detect( )
{
    VSPC_DTYPE * dst;
    VSPC_DTYPE * rootvox;
    VSPC_DTYPE * dstvox;
    int px, py, pz;
    int zsk;
    VSPC_DTYPE x1, x2, y1, y2, z1, z2;
    
    timestamp_entry("Voxel_Space::edge_detect( ):\n");
    
    dst = new VSPC_DTYPE[x_span * y_size * z_size]; 
    for (int i=0; i < x_span * y_size * z_size; i++)
        dst[i] = 0;

    
    zsk = x_span * y_size;
    
    for (pz=1; pz < z_size-1; pz++) {
        for (py=1; py < y_size-1; py++) {
            rootvox = data;
            rootvox += (pz * y_size + py) * x_span + 1;
            dstvox = dst;
            dstvox += (pz * y_size + py) * x_span + 1;
            for (px=1; px < x_span-1; px++) {
                x1 = (*rootvox << 1) | (rootvox[-1] >> data_width_m1);
                x2 = (*rootvox >> 1) | (rootvox[1] << data_width_m1);
                y1 = rootvox[-x_span];
                y2 = rootvox[x_span];
                z1 = rootvox[-zsk];
                z2 = rootvox[zsk];
                *dstvox++ = *rootvox++ & ~(x1 & x2 & y1 & y2 & z1 & z2);
            }
        }
    }

    for (pz=1; pz < z_size-1; pz++) {
        for (py=1; py < y_size-1; py++) {
            rootvox = data;
            rootvox += (pz * y_size + py) * x_span;
            dstvox = dst;
            dstvox += (pz * y_size + py) * x_span;
            x1 = (*rootvox << 1);
            x2 = (*rootvox >> 1) | (rootvox[1] << data_width_m1);
            y1 = rootvox[-x_span];
            y2 = rootvox[x_span];
            z1 = rootvox[-zsk];
            z2 = rootvox[zsk];
            *dstvox++ = *rootvox++ & ~(x1 & x2 & y1 & y2 & z1 & z2);
            rootvox = data;
            rootvox += (pz * y_size + py + 1) * x_span - 1;
            dstvox = dst;
            dstvox += (pz * y_size + py + 1) * x_span - 1;
            x1 = (*rootvox << 1) | (rootvox[-1] >> data_width_m1);
            x2 = (*rootvox >> 1);
            y1 = rootvox[-x_span];
            y2 = rootvox[x_span];
            z1 = rootvox[-zsk];
            z2 = rootvox[zsk];
            *dstvox++ = *rootvox++ & ~(x1 & x2 & y1 & y2 & z1 & z2);
        }
    }
    
    for (pz=1; pz < z_size-1; pz++) {
        rootvox = data;
        rootvox += (pz * y_size) * x_span + 1;
        dstvox = dst;
        dstvox += (pz * y_size) * x_span + 1;
        for (px=1; px < x_span-1; px++) {
            *dstvox++ = *rootvox++;
        }
        rootvox = data;
        rootvox += (pz * y_size + y_size - 1) * x_span + 1;
        dstvox = dst;
        dstvox += (pz * y_size + y_size - 1) * x_span + 1;
        for (px=1; px < x_span-1; px++) {
            *dstvox++ = *rootvox++;
        }
    }
    for (py=1; py < y_size-1; py++) {
        rootvox = data;
        rootvox += py * x_span + 1;
        dstvox = dst;
        dstvox += py * x_span + 1;
        for (px=1; px < x_span-1; px++) {
            *dstvox++ = *rootvox++;
        }
        rootvox = data;
        rootvox += ((z_size-1) * y_size + py) * x_span + 1;
        dstvox = dst;
        dstvox += ((z_size-1) * y_size + py) * x_span + 1;
        for (px=1; px < x_span-1; px++) {
            *dstvox++ = *rootvox++;
        }
    }
    
    memcpy(data, dst, x_span * y_size * z_size * sizeof(VSPC_DTYPE));
    delete [] dst;
    timestamp_exit("done\n");
}

void Voxel_Space::dilate(int r)
{
    #ifdef _VSPACE_USE_OPTIMISED_DILATE
    #warning 'using optimised dilate'
    switch (r) {
        case 0:
            return;
        case 1:
            dilate_r1();
            break;
        case 2:
            dilate_r2();
            break;
        case 3:
            dilate_r3();
            break;
        case 4:
            dilate_r4();
            break;
        case 5:
            dilate_r5();
            break;
        case 6:
            dilate_r6();
            break;
        case 7:
            dilate_r7();
            break;
        case 8:
            dilate_r8();
            break;
        case 9:
            dilate_r9();
            break;
        case 10:
            dilate_r10();
            break;
        case 11:
            dilate_r11();
            break;
        case 12:
            dilate_r12();
            break;
        default:
            IF_DEBUG {
                print_inside("no specific code for radius - using generic dilate\n");
            }
            dilate_fast(r);
        }
    #else
        #warning 'NOT using optimised dilate'
        dilate_fast(r);
    #endif
}

void Voxel_Space::erode(int r)
{
    #ifdef _VSPACE_USE_OPTIMISED_ERODE
    #warning 'using optimised erode'
    switch (r) {
        case 0:
            return;
        case 1:
            erode_r1();
            break;
        case 2:
            erode_r2();
            break;
        case 3:
            erode_r3();
            break;
        case 4:
            erode_r4();
            break;
        case 5:
            erode_r5();
            break;
        case 6:
            erode_r6();
            break;
        case 7:
            erode_r7();
            break;
        case 8:
            erode_r8();
            break;
        case 9:
            erode_r9();
            break;
        case 10:
            erode_r10();
            break;
        case 11:
            erode_r11();
            break;
        case 12:
            erode_r12();
            break;
        default:
            IF_DEBUG {
                print_inside("no specific code for radius - using generic erode\n");
            }
            erode_fast(r);
        }
    #else
        #warning 'NOT using optimised erode'
        erode_fast(r);
    #endif
}

void Voxel_Space::dilate_fast(int r)
{
    VSPC_DTYPE * dst;
    VSPC_DTYPE * rootvox;
    VSPC_DTYPE * dstvox;
    int px, py, pz;
    int zsk;
    int n_M=0;  // no shift needed
    int n_S = 0; // left - shift up required // right - shift down required
    int r2 = r * r;
    
    timestamp_entry("Voxel_Space::dilate_fast( r ):\n");
    
    dst = new VSPC_DTYPE[x_span * y_size * z_size]; 
    for (int i=0; i < x_span * y_size * z_size; i++)
        dst[i] = 0;
    
    zsk = x_span * y_size;
    
    for (pz=-r; pz <= r; pz++)
    {
        for (py=-r; py <= r; py++)
        {
            if ((pz*pz+py*py) <= r2)
            {
                n_M++;
                for (px=1; px <= r; px++)
                {
                    if ((pz*pz + py*py + px*px) <= r2)
                    {
                        n_S++;
                    }
                }
            }
        }
    }
    
    int * word_offset_L = new int[n_S];
    int * word_offset_M = new int[n_M];
    int * word_offset_R = new int[n_S];
    int * bit_shift_M = new int[n_S];
    int * bit_shift_N = new int[n_S];
    bool * bit_shift_safe = new bool[n_S];
    VSPC_DTYPE acc;
    
    n_M = n_S = 0;
    for (pz=-r; pz <= r; pz++)
    {
        for (py=-r; py <= r; py++)
        {
            if ((pz*pz+py*py) <= r2)
            {
                word_offset_M[n_M] = (pz * y_size + py) * x_span;
                for (px=1; px <= r; px++)
                {
                    if ((pz*pz + py*py + px*px) <= r2)
                    {
                        word_offset_L[n_S] = word_offset_M[n_M] - (px / data_width);
                        word_offset_R[n_S] = word_offset_M[n_M] + (px / data_width);
                        bit_shift_M[n_S] = px % data_width;
                        bit_shift_N[n_S] = data_width - (px % data_width);
                        bit_shift_safe[n_S] = ( r - px - data_width >= 0 );
                        n_S++;
                    }
                }
                n_M++;
            }
        }
    }
    
    int rinwords = r / data_width;
    int rinwords_1 = rinwords + 1;
    int c;
    
    for (pz=r; pz < z_size - r; pz++)
    {
        for (py=r; py < y_size - r; py++)
        {
            rootvox = data;
            rootvox += (pz * y_size + py) * x_span + rinwords;
            dstvox = dst;
            dstvox += (pz * y_size + py) * x_span + rinwords;
            if (~*rootvox == 0)
                *dstvox++ = *rootvox++;
            else
            {
                acc = 0;
                for (c=0; c < n_M; c++)
                    acc = acc | rootvox[word_offset_M[c]];
                for (c=0; c < n_S; c++)
                    acc = acc | (rootvox[word_offset_L[c]] << bit_shift_M[c])
                            | (bit_shift_safe[c] ? rootvox[word_offset_L[c]-1] >> bit_shift_N[c] : 0)
                            | (rootvox[word_offset_R[c]] >> bit_shift_M[c])
                            | (rootvox[word_offset_R[c]+1] << bit_shift_N[c]);
                *dstvox++ = acc;
                rootvox++;
            }
            for (px=rinwords_1; px < x_span - rinwords; px++)
            {
                if (~*rootvox == 0)
                    *dstvox++ = *rootvox++;
                else
                {
                    acc = 0;
                    for (c=0; c < n_M; c++)
                        acc = acc | rootvox[word_offset_M[c]];
                    for (c=0; c < n_S; c++)
                        acc = acc | (rootvox[word_offset_L[c]] << bit_shift_M[c])
                                | (rootvox[word_offset_L[c]-1] >> bit_shift_N[c])
                                | (rootvox[word_offset_R[c]] >> bit_shift_M[c])
                                | (rootvox[word_offset_R[c]+1] << bit_shift_N[c]);
                    *dstvox++ = acc;
                    rootvox++;
                }
            }
            if (~*rootvox == 0)
                *dstvox = *rootvox;
            else
            {
                acc = 0;
                for (c=0; c < n_M; c++)
                    acc = acc | rootvox[word_offset_M[c]];
                for (c=0; c < n_S; c++)
                    acc = acc | (rootvox[word_offset_L[c]] << bit_shift_M[c])
                            | (rootvox[word_offset_L[c]-1] >> bit_shift_N[c])
                            | (rootvox[word_offset_R[c]] >> bit_shift_M[c])
                            | (bit_shift_safe[c] ? rootvox[word_offset_R[c]+1] << bit_shift_N[c] : 0);
                *dstvox = acc;
            }
        }
    }
    
    delete [] word_offset_L;
    delete [] word_offset_M;
    delete [] word_offset_R;
    delete [] bit_shift_M;
    delete [] bit_shift_N;
    delete [] bit_shift_safe;
    memcpy(data, dst, x_span * y_size * z_size * sizeof(VSPC_DTYPE));
    delete [] dst;
    timestamp_exit("done\n");    
}

void Voxel_Space::dilate_slow(int r)
{
    VSPC_DTYPE * dst;
    VSPC_DTYPE * rootvox;
    VSPC_DTYPE * dstvox;
    VSPC_DTYPE acc;
    int px, py, pz;
    int sx, sy, sz;
    int fx, tx, fy, ty, fz, tz;
    int zsk;
    int r2 = r * r;
    
    dst = new VSPC_DTYPE[x_span * y_size * z_size]; 
    
    timestamp_entry("Voxel_Space::dilate_slow( r ):\n");
    
    zsk = x_span * y_size;
    
    for (pz=0; pz < z_size; pz++)
    {
        fz = (pz < r ? -pz : -r );
        tz = (z_size - 1 - pz < r ? z_size - 1 - pz : r );
        for (py=0; py < y_size; py++)
        {
            fy = (py < r ? -py : -r );
            ty = (y_size - 1 - py < r ? y_size - 1 - py : r );
            rootvox = data;
            rootvox += (pz * y_size + py) * x_span;
            dstvox = dst;
            dstvox += (pz * y_size + py) * x_span;
            for (px=0; px < x_span; px++)
            {
                fx = (px * data_width + data_width_m1 < r ? px * data_width + data_width_m1 : r);
                tx = ((x_span - px) * data_width - 1 < r ? (x_span - px) * data_width - 1 : r );
                acc = 0;
                for (sz=fz; sz<=tz; sz++)
                    for (sy=fy; sy<=ty; sy++)
                    {
                        if (sz*sz + sy*sy <= r2)
                        {
                            acc = acc | rootvox[(sz*y_size + sy) * x_span];
                            for (sx=1; sx<=fx; sx++)
                            {
                                if (sz*sz+sy*sy+sx*sx > r2)
                                    break;
                                acc = acc | rootvox[(sz*y_size + sy) * x_span - (sx / data_width)] << (sx % data_width);
                                if (px > 0)
                                    acc = acc | rootvox[(sz*y_size + sy) * x_span - (sx / data_width) - 1] >> (data_width - (sx % data_width));
                            }
                            for (sx=1; sx<=tx; sx++)
                            {
                                if (sz*sz+sy*sy+sx*sx > r2)
                                    break;
                                acc = acc | rootvox[(sz*y_size + sy) * x_span + (sx / data_width)] >> (sx % data_width);
                                if (px < x_span-1)
                                    acc = acc | rootvox[(sz*y_size + sy) * x_span + (sx / data_width) + 1] << (data_width - (sx % data_width)); 
                            }
                        }
                    }
                *dstvox++ = acc;
                rootvox++;
            }
        }
    }
    memcpy(data, dst, x_span * y_size * z_size * sizeof(VSPC_DTYPE));
    delete [] dst;
    timestamp_exit("done\n");    
}

void Voxel_Space::border(int r)
{
    VSPC_DTYPE * dst;
    VSPC_DTYPE * rootvox;
    VSPC_DTYPE * dstvox;
    int px, py, pz;
    int sx, sy, sz;
    int spx;
    
    timestamp_entry("Voxel_Space::border( r ):\n");
    
    if (r==0)
    {
        printf("  r == 0 - not doing anything\n");
        return;
    }
    
    sx = x_size + (r * 2);
    sy = y_size + (r * 2);
    sz = z_size + (r * 2);
    spx = (sx / data_width) + (sx % data_width == 0 ? 0 : 1);
    
    IF_DEBUG printf("sx=%d, sy=%d, sz=%d,  spx=%d\n",sx,sy,sz,spx);
    
    if ((sx < 1) || (sy < 1) || (sz < 1))
    {
        printf("  cannot shrink Voxel_Space - one or more resulting dimensions below 1\n");
        return;
    }
    
    dst = new VSPC_DTYPE[spx * sy * sz]; 
    for (int i=0; i < spx * sy * sz; i++)
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
                rootvox += (pz * y_size + py) * x_span;
                dstvox = dst;
                dstvox += ((r+pz) * sy + py + r) * spx + (r / data_width);
                *dstvox++ = rootvox[0] << (r % data_width);
                rootvox++;
                for (px=1; px < x_span; px++)
                {
                    *dstvox++ = (rootvox[0] << (r % data_width)) | (rootvox[-1] >> (data_width - (r % data_width)));
                    rootvox++;
                }
                // FIXME: check for buggyness!!
                if (px < spx)
                    *dstvox = rootvox[-1] >> (data_width - (r % data_width));
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
                rootvox += (pz * y_size + py) * x_span + (r / data_width);
                dstvox = dst;
                dstvox += ((pz-r) * sy + py - r) * spx;
                // FIXME:: mistake corrected below? spx-1 (or made worse?)
                for (px=0; px < spx-1; px++)
                {
                    *dstvox++ = (rootvox[0] >> (r % data_width)) | (rootvox[1] << (data_width - (r % data_width)));
                    rootvox++;
                }
                if ( spx < x_span - 1)
                {
                    *dstvox = (rootvox[0] >> (r % data_width)) | (rootvox[1] << (data_width - (r % data_width)));
                }
                else
                {
                    *dstvox = (rootvox[0] >> (r % data_width));
                }
            }
        }
    }
    delete [] data;
    data = dst;
    x_span = spx;
    x_size = sx;
    y_size = sy;
    z_size = sz;
    timestamp_exit("done\n");    
}


void Voxel_Space::invert( )
{
    timestamp_entry("Voxel_Space::invert():\n");
    for (int i=0; i < x_span * y_size * z_size; i++)
        data[i] = ~data[i];    
    timestamp_exit("done\n");    
}

void Voxel_Space::erode_fast(int r)
{
    VSPC_DTYPE * dst;
    VSPC_DTYPE * rootvox;
    VSPC_DTYPE * dstvox;
    int px, py, pz;
    int zsk;
    int n_M=0;  // no shift needed
    int n_S = 0; // left - shift up required // right - shift down required
    int r2 = r * r;
    
    timestamp_entry("Voxel_Space::erode( r ):\n");
    
    dst = new VSPC_DTYPE[x_span * y_size * z_size]; 
    for (int i=0; i < x_span * y_size * z_size; i++)
        dst[i] = 0;
    
    zsk = x_span * y_size;
    
    for (pz=-r; pz <= r; pz++)
    {
        for (py=-r; py <= r; py++)
        {
            if ((pz*pz+py*py) <= r2)
            {
                n_M++;
                for (px=1; px <= r; px++)
                {
                    if ((pz*pz + py*py + px*px) <= r2)
                    {
                        n_S++;
                    }
                }
            }
        }
    }
    
    int * word_offset_L = new int[n_S];
    int * word_offset_M = new int[n_M];
    int * word_offset_R = new int[n_S];
    int * bit_shift_M = new int[n_S];
    int * bit_shift_N = new int[n_S];
    bool * bit_shift_safe = new bool[n_S];
    VSPC_DTYPE acc;
    
    n_M = n_S = 0;
    for (pz=-r; pz <= r; pz++)
    {
        for (py=-r; py <= r; py++)
        {
            if ((pz*pz+py*py) <= r2)
            {
                word_offset_M[n_M] = (pz * y_size + py) * x_span;
                for (px=1; px <= r; px++)
                {
                    if ((pz*pz + py*py + px*px) <= r2)
                    {
                        word_offset_L[n_S] = word_offset_M[n_M] - (px / data_width);
                        word_offset_R[n_S] = word_offset_M[n_M] + (px / data_width);
                        bit_shift_M[n_S] = px % data_width;
                        bit_shift_N[n_S] = data_width - (px % data_width);
                        bit_shift_safe[n_S] = ( r - px - data_width >= 0 );
                        n_S++;
                    }
                }
                n_M++;
            }
        }
    }
    
    int rinwords = r / data_width;
    int rinwords_1 = rinwords + 1;
    int c;
    
    for (pz=r; pz < z_size - r; pz++)
    {
        for (py=r; py < y_size - r; py++)
        {
            rootvox = data;
            rootvox += (pz * y_size + py) * x_span + rinwords;
            dstvox = dst;
            dstvox += (pz * y_size + py) * x_span + rinwords;
            acc = -1;
            if (*rootvox == 0)
                *dstvox++ = *rootvox++;
            else
            {
                for (c=0; c < n_M; c++)
                    acc = acc & rootvox[word_offset_M[c]];
                for (c=0; c < n_S; c++)
                    acc = acc & ((rootvox[word_offset_L[c]] << bit_shift_M[c])
                            | (bit_shift_safe[c] ? rootvox[word_offset_L[c]-1] >> bit_shift_N[c] : 0))
                            & ((rootvox[word_offset_R[c]] >> bit_shift_M[c])
                            | (rootvox[word_offset_R[c]+1] << bit_shift_N[c]));
                *dstvox++ = acc;
                rootvox++;
            }
            for (px=rinwords_1; px < x_span - rinwords; px++)
            {
                if (*rootvox == 0)
                    *dstvox++ = *rootvox++;
                else
                {
                    acc = -1;
                    for (c=0; c < n_M; c++)
                        acc = acc & rootvox[word_offset_M[c]];
                    for (c=0; c < n_S; c++)
                        acc = acc & ((rootvox[word_offset_L[c]] << bit_shift_M[c])
                                | (rootvox[word_offset_L[c]-1] >> bit_shift_N[c]))
                                & ((rootvox[word_offset_R[c]] >> bit_shift_M[c])
                                | (rootvox[word_offset_R[c]+1] << bit_shift_N[c]));
                    *dstvox++ = acc;
                    rootvox++;
                }
            }
            if (*rootvox == 0)
                *dstvox = *rootvox;
            else
            {
                acc = -1;
                for (c=0; c < n_M; c++)
                    acc = acc & rootvox[word_offset_M[c]];
                for (c=0; c < n_S; c++)
                    acc = acc & ((rootvox[word_offset_L[c]] << bit_shift_M[c])
                            | (rootvox[word_offset_L[c]-1] >> bit_shift_N[c]))
                            & ((rootvox[word_offset_R[c]] >> bit_shift_M[c])
                            | (bit_shift_safe[c] ? rootvox[word_offset_R[c]+1] << bit_shift_N[c] : 0));
                *dstvox = acc;
            }
        }
    }
    
    delete [] word_offset_L;
    delete [] word_offset_M;
    delete [] word_offset_R;
    delete [] bit_shift_M;
    delete [] bit_shift_N;
    delete [] bit_shift_safe;
    memcpy(data, dst, x_span * y_size * z_size * sizeof(VSPC_DTYPE));
    delete [] dst;
    timestamp_exit("done\n");    
}

void Voxel_Space::erode_slow(int r)
{
    VSPC_DTYPE * dst;
    VSPC_DTYPE * rootvox;
    VSPC_DTYPE * dstvox;
    VSPC_DTYPE acc;
    int px, py, pz;
    int sx, sy, sz;
    int fx, tx, fy, ty, fz, tz;
    int zsk;
    int r2 = r * r;
    
    dst = new VSPC_DTYPE[x_span * y_size * z_size]; 
    
    timestamp_entry("Voxel_Space::erode_slow( r ):\n");
    
    zsk = x_span * y_size;
    
    for (pz=0; pz < z_size; pz++)
    {
        fz = (pz < r ? -pz : -r );
        tz = (z_size - 1 - pz < r ? z_size - 1 - pz : r );
        for (py=0; py < y_size; py++)
        {
            fy = (py < r ? -py : -r );
            ty = (y_size - 1 - py < r ? y_size - 1 - py : r );
            rootvox = data;
            rootvox += (pz * y_size + py) * x_span;
            dstvox = dst;
            dstvox += (pz * y_size + py) * x_span;
            for (px=0; px < x_span; px++)
            {
                fx = (px * data_width + data_width_m1 < r ? px * data_width + data_width_m1 : r);
                tx = ((x_span - px) * data_width - 1 < r ? (x_span - px) * data_width - 1 : r );
                acc = -1;
                for (sz=fz; sz<=tz; sz++)
                    for (sy=fy; sy<=ty; sy++)
                    {
                        if (sz*sz + sy*sy <= r2)
                        {
                            acc = acc & rootvox[(sz*y_size + sy) * x_span];
                            for (sx=1; sx<=fx; sx++)
                            {
                                if (sz*sz+sy*sy+sx*sx > r2)
                                    break;
                                acc = acc & (( rootvox[(sz*y_size + sy) * x_span - (sx / data_width)] << (sx % data_width)) | (px > 0 ? rootvox[(sz*y_size + sy) * x_span - (sx / data_width) - 1] >> (data_width - (sx % data_width)) : 0));
                            }
                            for (sx=1; sx<=tx; sx++)
                            {
                                if (sz*sz+sy*sy+sx*sx > r2)
                                    break;
                                acc = acc & ((rootvox[(sz*y_size + sy) * x_span + (sx / data_width)] >> (sx % data_width)) | (px < x_span-1 ? rootvox[(sz*y_size + sy) * x_span + (sx / data_width) + 1] << (data_width - (sx % data_width)) : 0));
                            }
                        }
                    }
                *dstvox++ = acc;
                rootvox++;
            }
        }
    }
    memcpy(data, dst, x_span * y_size * z_size * sizeof(VSPC_DTYPE));
    delete [] dst;
    timestamp_exit("done\n");
}

bool Voxel_Space::get(int x, int y, int z) const
{
    if ((x<0) || (y<0) || (z<0) || (x >= x_size) || (y >= y_size) || (z >= z_size))
        return(false);
    if ((data[(z * y_size + y) * x_span + (x / data_width)] & (VSPC_ONE << (x % data_width))) != 0)
        return(true);
    else
        return(false);
}

void Voxel_Space::set(int x, int y, int z, bool value)
{
    if ((x<0) || (y<0) || (z<0) || (x >= x_size) || (y >= y_size) || (z >= z_size))
        return;
    if (value == true)
        data[(z * y_size + y) * x_span + (x / data_width)] =  data[(z * y_size + y) * x_span + (x / data_width)] | (VSPC_ONE << (x % data_width));
    else
        data[(z * y_size + y) * x_span + (x / data_width)] =  data[(z * y_size + y) * x_span + (x / data_width)] & ~(VSPC_ONE << (x % data_width));
}
