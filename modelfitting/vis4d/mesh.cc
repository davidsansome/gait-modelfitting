// mesh.cc
// Converts a vspace to a set of faces for fast rendering
// Richard Seely 2007

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <zlib.h>
#include "mesh.hh"

using namespace std;

unsigned int Point_XYZNC_List::xyz_hash(float x, float y, float z)
{
    int X = (int) x;
    int Y = (int) y;
    int Z = (int) z;
    unsigned int h = X * 1294567 + Y * 4537866 + Z * 9565334;
    return(h % HASHTABLE_SIZE);
}

Point_XYZNC_List::Point_XYZNC_List( )
{
    timestamp_entry("Point_XYZNC_List( ):\n");
    first = NULL;
    last = NULL;
    
    hashtable = new Point_XYZNC_Item*[HASHTABLE_SIZE];
    for (int i=0; i < HASHTABLE_SIZE; i++)
        hashtable[i] = NULL;
    //~ if ( ( lst->hashtable = (Point_XYZNC_Item **) calloc(HASHTABLE_SIZE, sizeof(Point_XYZNC_Item *)) ) == NULL) {
        //~ perror("create_Point_XYZNC_List: calloc failed");
        //~ exit(-1);
    //~ }
    timestamp_exit("done\n");
}

Point_XYZNC_List::~Point_XYZNC_List( )
{
    Point_XYZNC_Item * cur;
    Point_XYZNC_Item * del;
    timestamp_entry("~Point_XYZNC_List( ):\n");
    delete [] hashtable;
    
    cur = first;
    IF_DEBUG timestamp_inside("deleting linked list...\n");
    while ( cur != NULL )
    {
        del = cur;
        cur = cur->next;
        delete del;
    }
    timestamp_exit("goodbye cruel world\n");
}


Point_XYZNC_Item * Point_XYZNC_List::add( float x, float y, float z )
{
    Point_XYZNC_Item * cur = hashtable[xyz_hash(x,y,z)];
    Point_XYZNC_Item * prev = NULL;
    
    while (cur != NULL)
    {
        if ((cur->x == x) && (cur->y == y) && (cur->z == z))
            return(cur);
        prev = cur;
        cur = cur->hash_next;
    }
    
    /* the vertex doesn't exist (yet) */
    cur = new Point_XYZNC_Item;
    cur->next = NULL;
    cur->hash_next = NULL;
    cur->x = x;
    cur->y = y;
    cur->z = z;
    cur->nx = 0.0;
    cur->ny = 0.0;
    cur->nz = 0.0;
    cur->cr = 0.0;
    cur->cg = 0.0;
    cur->cb = 0.0;
    cur->cn = 0.0;
    // add the item into the hash table
    if (prev != NULL)
        prev->hash_next = cur;
    else
        hashtable[xyz_hash(x,y,z)] = cur;
    // add the item to the linked list
    if (last == NULL)
    {
        first = cur;
        last = cur;
        cur->index = 0;
    }
    else
    {
        last->next = cur;
        cur->index = last->index + 1;
        last = cur;
    }
    return(cur);
}

Point_XYZNC_Item * Point_XYZNC_List::find( float x, float y, float z )
{
    Point_XYZNC_Item * cur;
    cur = hashtable[xyz_hash(x,y,z)];
    while (cur != NULL)
    {
        if ((cur->x == x) && (cur->y == y) && (cur->z == z))
            break;
        cur = cur->hash_next;
    }
    return(cur);
}

void Point_XYZNC_List::print( )
{
    Point_XYZNC_Item * cur;
    printf("vertex list:\n");
    printf("============\n");
    cur = first;
    while (cur != NULL)
    {
        printf("%5d: pos=(%+5g, %+5g, %+5g) normals=(%+5g, %+5g, %+5g)\n", cur->index, cur->x, cur->y, cur->z, cur->nx, cur->ny, cur->nz);
        cur = cur->next;
    }
    printf("----end-----\n");
}

Quad_List::Quad_List( )
{
    timestamp_entry("Quad_List( ):\n");
    first = NULL;
    last = NULL;
    points = new Point_XYZNC_List( );
    timestamp_exit("done\n");
}


Quad_List::Quad_List( const Voxel_Space& vspc )
{
    VSPC_DTYPE * rootvox;
    int px, py, pz, ps;
    int ysk, zsk;
    float x,y,z,r,g,b;
    VSPC_DTYPE x1, x2, y1, y2, z1, z2, mask;
    
    timestamp_entry("Quad_List(Voxel_Space):\n");
    first = NULL;
    last = NULL;
    points = new Point_XYZNC_List( );
    
    ysk = vspc.x_span;
    zsk = vspc.x_span * vspc.y_size;
    
    z = (float) vspc.origin.z;
    for (pz=0; pz < vspc.z_size; pz++) {
        y = (float) vspc.origin.y;
        for (py=0; py < vspc.y_size; py++) {
            rootvox = vspc.data;
            rootvox += (pz * vspc.y_size + py) * vspc.x_span;
            x = (float) vspc.origin.x;
            for (px=0; px < ysk; px++) {
                x1 = ~(px > 0 ? (*rootvox << 1) | (rootvox[-1] >> vspc.data_width_m1) : *rootvox << 1) & *rootvox; /* x - 1 */
                x2 = ~(px < vspc.x_span-1 ? (*rootvox >> 1) | (rootvox[1] << vspc.data_width_m1) : *rootvox >> 1) & *rootvox; /* x + 1 */
                y1 = (py > 0 ? ~rootvox[-ysk] & *rootvox : *rootvox); /* y -1 */
                y2 = (py < vspc.y_size-1 ? ~rootvox[ysk] & *rootvox : *rootvox); /* y +1 */
                z1 = (pz > 0 ? ~rootvox[-zsk] & *rootvox : *rootvox); /* z - 1 */
                z2 = (pz < vspc.z_size-1 ? ~rootvox[zsk] & *rootvox++ : *rootvox++); /* z + 1 */                

                for (ps=0; ps < vspc.data_width; ps++)
                {
                    mask = VSPC_ONE << ps;
                    if ((x1 & mask) != 0)
                        add( x-0.5,y+0.5,z+0.5,    x-0.5,y-0.5,z+0.5,    x-0.5,y-0.5,z-0.5,    x-0.5,y+0.5,z-0.5,    -1,0,0 );
                    if ((x2 & mask) != 0)
                        add( x+0.5,y+0.5,z+0.5,    x+0.5,y+0.5,z-0.5,    x+0.5,y-0.5,z-0.5,    x+0.5,y-0.5,z+0.5,    1,0,0 );
                    if ((y1 & mask) != 0)
                        add( x+0.5,y-0.5,z+0.5,    x+0.5,y-0.5,z-0.5,    x-0.5,y-0.5,z-0.5,    x-0.5,y-0.5,z+0.5,    0,-1,0 );
                    if ((y2 & mask) != 0)
                        add( x+0.5,y+0.5,z+0.5,    x-0.5,y+0.5,z+0.5,    x-0.5,y+0.5,z-0.5,    x+0.5,y+0.5,z-0.5,    0,1,0 );
                    if ((z1 & mask) != 0)
                        add( x+0.5,y+0.5,z-0.5,    x-0.5,y+0.5,z-0.5,    x-0.5,y-0.5,z-0.5 ,    x+0.5,y-0.5,z-0.5,    0,0,-1 );
                    if ((z2 & mask) != 0)
                        add( x+0.5,y+0.5,z+0.5,    x+0.5,y-0.5,z+0.5,    x-0.5,y-0.5,z+0.5 ,    x-0.5,y+0.5,z+0.5,    0,0,+1 );
                    x += 1.0;
                }
            }
            y += 1.0;
        }
        z += 1.0;
    }
    timestamp_exit("done\n");
}

Quad_List::Quad_List( const Voxel_Field& vfld )
{
    VFLD_DTYPE * rootvox;
    int px, py, pz;
    int ysk, zsk;
    float x,y,z,r,g,b;
    VFLD_DTYPE x1, x2, y1, y2, z1, z2, mask;
    
    timestamp_entry("Quad_List(Voxel_Field):\n");
    first = NULL;
    last = NULL;
    points = new Point_XYZNC_List( );
    
    ysk = vfld.x_size;
    zsk = vfld.x_size * vfld.y_size;
    
    z = (float) vfld.origin.z;
    for (pz=0; pz < vfld.z_size; pz++) {
        y = (float) vfld.origin.y;
        for (py=0; py < vfld.y_size; py++) {
            rootvox = vfld.data;
            rootvox += (pz * vfld.y_size + py) * vfld.x_size;
            x = (float) vfld.origin.x;
            for (px=0; px < ysk; px++) {
                x1 = (px > 0 ? rootvox[-1] : 0); // x - 1
                x2 = (px < vfld.x_size-1 ? rootvox[1] : 0); /* x + 1 */
                y1 = (py > 0 ? rootvox[-ysk] : 0); /* y -1 */
                y2 = (py < vfld.y_size-1 ? rootvox[ysk] : 0); /* y +1 */
                z1 = (pz > 0 ? rootvox[-zsk] : 0); /* z - 1 */
                z2 = (pz < vfld.z_size-1 ? rootvox[zsk] : 0); /* z + 1 */
                
                switch (rootvox[0] % 17)
                {
                    case 1:
                        r = 1.0; g=0.0; b=0.0;
                        break;
                    case 2:
                        r = 0.0; g=1.0; b=0.0;
                        break;
                    case 3:
                        r = 0.0; g=0.0; b=1.0;
                        break;
                    case 4:
                        r = 1.0; g=1.0; b=0.0;
                        break;
                    case 5:
                        r = 0.0; g=1.0; b=1.0;
                        break;
                    case 6:
                        r = 1.0; g=0.0; b=1.0;
                        break;
                    case 7:
                        r = 1.0; g=0.5; b=0.0;
                        break;
                    case 8:
                        r = 0.5; g=1.0; b=0.0;
                        break;
                    case 9:
                        r = 0.0; g=1.0; b=0.5;
                        break;
                    case 10:
                        r = 0.0; g=0.5; b=1.0;
                        break;
                    case 11:
                        r = 0.5; g=0.0; b=1.0;
                        break;
                    case 12:
                        r = 0.1; g=0.0; b=0.5;
                        break;
                    case 13:
                        r = 1.0; g=0.5; b=0.5;
                        break;
                    case 14:
                        r = 0.5; g=1.0; b=0.5;
                        break;
                    case 15:
                        r = 0.5; g=0.5; b=1.0;
                        break;
                    case 16:
                        r = 0.5; g=0.5; b=0.5;
                        break;
                    case 0:
                        r = 1.0; g=1.0; b=1.0;
                        break;
                }
                if ((x1 == 0)  && (rootvox[0] != 0))
                    addc( x-0.5,y+0.5,z+0.5,    x-0.5,y-0.5,z+0.5,    x-0.5,y-0.5,z-0.5,    x-0.5,y+0.5,z-0.5,    -1,0,0,   r,g,b );
                if ((x2 == 0)  && (rootvox[0] != 0))
                    addc( x+0.5,y+0.5,z+0.5,    x+0.5,y+0.5,z-0.5,    x+0.5,y-0.5,z-0.5,    x+0.5,y-0.5,z+0.5,    1,0,0   ,r,g,b );
                if ((y1 == 0)  && (rootvox[0] != 0))
                    addc( x+0.5,y-0.5,z+0.5,    x+0.5,y-0.5,z-0.5,    x-0.5,y-0.5,z-0.5,    x-0.5,y-0.5,z+0.5,    0,-1,0   ,r,g,b );
                if ((y2 == 0)  && (rootvox[0] != 0))
                    addc( x+0.5,y+0.5,z+0.5,    x-0.5,y+0.5,z+0.5,    x-0.5,y+0.5,z-0.5,    x+0.5,y+0.5,z-0.5,    0,1,0   ,r,g,b );
                if ((z1 == 0)  && (rootvox[0] != 0))
                    addc( x+0.5,y+0.5,z-0.5,    x-0.5,y+0.5,z-0.5,    x-0.5,y-0.5,z-0.5 ,    x+0.5,y-0.5,z-0.5,    0,0,-1   ,r,g,b );
                if ((z2 == 0)  && (rootvox[0] != 0))
                    addc( x+0.5,y+0.5,z+0.5,    x+0.5,y-0.5,z+0.5,    x-0.5,y-0.5,z+0.5 ,    x-0.5,y+0.5,z+0.5,    0,0,+1   ,r,g,b );
                x += 1.0;
                rootvox++;
            }
            y += 1.0;
        }
        z += 1.0;
    }
    timestamp_exit("done\n");
}

Quad_List::Quad_List( const Voxel_Field& vfldR, const Voxel_Field& vfldG, const Voxel_Field& vfldB )
{
    VFLD_DTYPE * rootvoxR;
    VFLD_DTYPE * rootvoxG;
    VFLD_DTYPE * rootvoxB;
    int px, py, pz;
    int ysk, zsk;
    float x,y,z;
    VFLD_DTYPE x1, x2, y1, y2, z1, z2, mask;
    
    timestamp_entry("Quad_List(Voxel_Field, Voxel_Field, Voxel_Field):\n");
    first = NULL;
    last = NULL;
    
        if ((vfldR.x_size != vfldG.x_size) | (vfldR.x_size != vfldB.x_size) | (vfldR.y_size != vfldG.y_size) | (vfldR.y_size != vfldB.y_size) |(vfldR.z_size != vfldG.z_size) | (vfldR.z_size != vfldB.z_size))
    {
        timestamp_exit("error: dimension mismatch\n");
        delete this;
        return;
    }
    
    if ((vfldR.origin.x != vfldG.origin.x) | (vfldR.origin.x != vfldB.origin.x) | (vfldR.origin.y != vfldG.origin.y) | (vfldR.origin.y != vfldB.origin.y) |(vfldR.origin.z != vfldG.origin.z) | (vfldR.origin.z != vfldB.origin.z))
    {
        timestamp_exit("error: origin mismatch\n");
        delete this;
        return;
    }
    
    points = new Point_XYZNC_List( );
    
    ysk = vfldR.x_size;
    zsk = vfldR.x_size * vfldR.y_size;
    
    z = (float) vfldR.origin.z;
    for (pz=0; pz < vfldR.z_size; pz++) {
        y = (float) vfldR.origin.y;
        for (py=0; py < vfldR.y_size; py++) {
            rootvoxR = vfldR.data;
            rootvoxR += (pz * vfldR.y_size + py) * vfldR.x_size;
            rootvoxG = vfldG.data;
            rootvoxG += (pz * vfldR.y_size + py) * vfldR.x_size;
            rootvoxB = vfldB.data;
            rootvoxB += (pz * vfldR.y_size + py) * vfldR.x_size;
            x = (float) vfldR.origin.x;
            for (px=0; px < ysk; px++) {
                x1 = (px > 0 ? rootvoxR[-1] + rootvoxG[-1] + rootvoxB[-1] : 0); // x - 1
                x2 = (px < vfldR.x_size-1 ? rootvoxR[1] + rootvoxG[1] + rootvoxB[1] : 0); /* x + 1 */
                y1 = (py > 0 ? rootvoxR[-ysk] + rootvoxG[-ysk] + rootvoxB[-ysk]: 0); /* y -1 */
                y2 = (py < vfldR.y_size-1 ? rootvoxR[ysk] + rootvoxG[ysk] + rootvoxB[ysk] : 0); /* y +1 */
                z1 = (pz > 0 ? rootvoxR[-zsk] + rootvoxG[-zsk] + rootvoxB[-zsk] : 0); /* z - 1 */
                z2 = (pz < vfldR.z_size-1 ? rootvoxR[zsk] + rootvoxG[zsk] + rootvoxB[zsk] : 0); /* z + 1 */
                if ((x1 == 0)  && (rootvoxR[0] + rootvoxG[0] + rootvoxB[0] != 0))
                    addc( x-0.5,y+0.5,z+0.5,    x-0.5,y-0.5,z+0.5,    x-0.5,y-0.5,z-0.5,    x-0.5,y+0.5,z-0.5,    -1,0,0,   ((float) rootvoxR[0]) / 255.0, ((float) rootvoxG[0]) / 255.0, ((float) rootvoxB[0]) / 255.0 );
                if ((x2 == 0)  && (rootvoxR[0] + rootvoxG[0] + rootvoxB[0] != 0))
                    addc( x+0.5,y+0.5,z+0.5,    x+0.5,y+0.5,z-0.5,    x+0.5,y-0.5,z-0.5,    x+0.5,y-0.5,z+0.5,    1,0,0,    ((float) rootvoxR[0]) / 255.0, ((float) rootvoxG[0]) / 255.0, ((float) rootvoxB[0]) / 255.0 );
                if ((y1 == 0)  && (rootvoxR[0] + rootvoxG[0] + rootvoxB[0] != 0))
                    addc( x+0.5,y-0.5,z+0.5,    x+0.5,y-0.5,z-0.5,    x-0.5,y-0.5,z-0.5,    x-0.5,y-0.5,z+0.5,    0,-1,0,    ((float) rootvoxR[0]) / 255.0, ((float) rootvoxG[0]) / 255.0, ((float) rootvoxB[0]) / 255.0 );
                if ((y2 == 0)  && (rootvoxR[0] + rootvoxG[0] + rootvoxB[0] != 0))
                    addc( x+0.5,y+0.5,z+0.5,    x-0.5,y+0.5,z+0.5,    x-0.5,y+0.5,z-0.5,    x+0.5,y+0.5,z-0.5,    0,1,0,    ((float) rootvoxR[0]) / 255.0, ((float) rootvoxG[0]) / 255.0, ((float) rootvoxB[0]) / 255.0 );
                if ((z1 == 0)  && (rootvoxR[0] + rootvoxG[0] + rootvoxB[0] != 0))
                    addc( x+0.5,y+0.5,z-0.5,    x-0.5,y+0.5,z-0.5,    x-0.5,y-0.5,z-0.5 ,    x+0.5,y-0.5,z-0.5,    0,0,-1,    ((float) rootvoxR[0]) / 255.0, ((float) rootvoxG[0]) / 255.0, ((float) rootvoxB[0]) / 255.0 );
                if ((z2 == 0)  && (rootvoxR[0] + rootvoxG[0] + rootvoxB[0] != 0))
                    addc( x+0.5,y+0.5,z+0.5,    x+0.5,y-0.5,z+0.5,    x-0.5,y-0.5,z+0.5 ,    x-0.5,y+0.5,z+0.5,    0,0,+1,    ((float) rootvoxR[0]) / 255.0, ((float) rootvoxG[0]) / 255.0, ((float) rootvoxB[0]) / 255.0 );
                x += 1.0;
                rootvoxR++;
                rootvoxG++;
                rootvoxB++;
            }
            y += 1.0;
        }
        z += 1.0;
    }
    timestamp_exit("done\n");
}

Quad_List::~Quad_List( )
{
    Quad_Item * cur;
    Quad_Item * del;
    timestamp_entry("~Quad_List( )\n");
    cur = first;
    while (cur != NULL)
    {
        del = cur;
        cur = cur->next;
        delete del;
    }
    delete points;
    timestamp_exit("goodbye cruel world\n");
}

Quad_Item * Quad_List::add( float ax, float ay, float az,
                                            float bx, float by, float bz,
                                            float cx, float cy, float cz,
                                            float dx, float dy, float dz,
                                            float nx, float ny, float nz )
{
    Quad_Item * qit;
    
    //~ if (( qit = (Quad_Item *) malloc(sizeof(Quad_Item))) == NULL) {
        //~ perror("add_Quad_Item: malloc failed");
        //~ exit(-1);
    //~ }
    
    qit = new Quad_Item;
    qit->next = NULL;
    
    if (last == NULL) {
        last = qit;
        first = qit;
        qit->index = 0;
    }
    else
    {
        qit->index = last->index + 1;
        last->next = qit;
        last = qit;
    }
    
    qit->a = points->add(ax, ay, az);
    qit->b = points->add(bx, by, bz);
    qit->c = points->add(cx, cy, cz);
    qit->d = points->add(dx, dy, dz);
    qit->a->nx += nx;   qit->a->ny += ny;   qit->a->nz += nz;
    qit->b->nx += nx;   qit->b->ny += ny;   qit->b->nz += nz;
    qit->c->nx += nx;   qit->c->ny += ny;   qit->c->nz += nz;
    qit->d->nx += nx;   qit->d->ny += ny;   qit->d->nz += nz;
    
    return(qit);
}

Quad_Item * Quad_List::addc( float ax, float ay, float az,
                                            float bx, float by, float bz,
                                            float cx, float cy, float cz,
                                            float dx, float dy, float dz,
                                            float nx, float ny, float nz,
                                            float r, float g, float b )
{
    Quad_Item * qit;
    
    //~ if (( qit = (Quad_Item *) malloc(sizeof(Quad_Item))) == NULL) {
        //~ perror("add_Quad_Item: malloc failed");
        //~ exit(-1);
    //~ }
    
    qit = new Quad_Item;
    qit->next = NULL;
    
    if (last == NULL) {
        last = qit;
        first = qit;
        qit->index = 0;
    }
    else
    {
        qit->index = last->index + 1;
        last->next = qit;
        last = qit;
    }
    
    qit->a = points->add(ax, ay, az);
    qit->b = points->add(bx, by, bz);
    qit->c = points->add(cx, cy, cz);
    qit->d = points->add(dx, dy, dz);
    qit->a->nx += nx;   qit->a->ny += ny;   qit->a->nz += nz;
    qit->b->nx += nx;   qit->b->ny += ny;   qit->b->nz += nz;
    qit->c->nx += nx;   qit->c->ny += ny;   qit->c->nz += nz;
    qit->d->nx += nx;   qit->d->ny += ny;   qit->d->nz += nz;
    qit->a->cr += r; qit->a->cg += g; qit->a->cb += b; qit->a->cn += 1.0;
    qit->b->cr += r; qit->b->cg += g; qit->b->cb += b; qit->b->cn += 1.0;
    qit->c->cr += r; qit->c->cg += g; qit->c->cb += b; qit->c->cn += 1.0;
    qit->d->cr += r; qit->d->cg += g; qit->d->cb += b; qit->d->cn += 1.0;
    
    return(qit);
}

void Quad_List::Add_Cube( float x, float y, float z, float size, float r, float g, float b )
{
    addc( x-size,y+size,z+size,    x-size,y-size,z+size,    x-size,y-size,z-size,    x-size,y+size,z-size,    -1,0,0,   r,g,b );
    addc( x+size,y+size,z+size,    x+size,y+size,z-size,    x+size,y-size,z-size,    x+size,y-size,z+size,    1,0,0   ,r,g,b );        
    addc( x+size,y-size,z+size,    x+size,y-size,z-size,    x-size,y-size,z-size,    x-size,y-size,z+size,    0,-1,0   ,r,g,b );
    addc( x+size,y+size,z+size,    x-size,y+size,z+size,    x-size,y+size,z-size,    x+size,y+size,z-size,    0,1,0   ,r,g,b );
    addc( x+size,y+size,z-size,    x-size,y+size,z-size,    x-size,y-size,z-size ,    x+size,y-size,z-size,    0,0,-1   ,r,g,b );
    addc( x+size,y+size,z+size,    x+size,y-size,z+size,    x-size,y-size,z+size ,    x-size,y+size,z+size,    0,0,+1   ,r,g,b );
}

void Quad_List::print( )
{
    Quad_Item * cur;
    printf("quad list:\n");
    printf("==========\n");
    cur = first;
    while (cur != NULL)
    {
        printf("%5d: a=V(%d), b=V(%d), c=V(%d), d=V(%d)\n", cur->index, cur->a->index, cur->b->index, cur->c->index, cur->d->index);
        cur = cur->next;
    }
    printf("----end---\n\n");
    points->print();
}

Mesh::Mesh( const Quad_List& lst )
{
    timestamp_entry("Mesh(Quad_List):\n");
    use_draw_fast = false;
    pack(lst);
    timestamp_exit("done\n");
}

Mesh::Mesh( const Voxel_Space& vspc)
{
    timestamp_entry("Mesh(Voxel_Space):\n");
    use_draw_fast = false;
    Quad_List lst (vspc);
    pack(lst);
    timestamp_exit("done\n");
}

Mesh::Mesh( const Voxel_Field& vfld)
{
    timestamp_entry("Mesh(Voxel_Field):\n");
    use_draw_fast = false;
    Quad_List lst (vfld);
    pack(lst);
    timestamp_exit("done\n");
}

Mesh::Mesh( const Voxel_Field& vfldR, const Voxel_Field& vfldG, const Voxel_Field& vfldB)
{
    timestamp_entry("Mesh(Voxel_Field, Voxel_Field, Voxel_Field):\n");
    use_draw_fast = false;
    Quad_List lst (vfldR, vfldG, vfldB);
    pack(lst);
    timestamp_exit("done\n");
}

Mesh::Mesh( char * filename )
{
    timestamp_entry("Mesh(filename):\n");
    use_draw_fast = false;
    
    if (strstr(filename, ".Zmsh") != NULL)
    {
        gzFile gf = gzopen(filename, "rb");
        if (gf == NULL)
        {
            perror("Mesh::Mesh( filename.Zmsh ): could not open file for reading ");
            return;
        }
        gzread(gf, &n_vertices, sizeof(int));
        gzread(gf, &n_normals, sizeof(int));
        gzread(gf, &n_colours, sizeof(int));
        gzread(gf, &n_indices, sizeof(int));
        
        vertices = new float [3 * n_vertices];
        normals = new float [3 * n_normals];
        if (n_colours > 0)
            colours = new float [3 * n_colours];
        else
            colours = NULL;
        indices = new unsigned int [4 * n_indices];
        
        gzread(gf, vertices, sizeof(float) * 3 * n_vertices);
        gzread(gf, normals, sizeof(float) * 3 * n_normals);
        if (colours != NULL)
        {
            gzread(gf, colours, sizeof(float) * 3 * n_colours);
        }
        gzread(gf, indices, sizeof(unsigned int) * 4 * n_indices);
        gzclose(gf);
    }
    else
    {
        fstream f(filename, ios::binary | ios::in );
        
        if (f.fail()) {
            perror("Mesh(filename): failed to open file");
            exit(-2);
        }

        f.read( (char * ) &n_vertices, sizeof(int) );
        if (f.gcount() != sizeof(int)) {
            perror("Mesh(filename): failed to read 1");
            exit(-3);
        }
        f.read( (char * ) &n_normals, sizeof(int) );
        if (f.gcount() != sizeof(int)) {
            perror("Mesh(filename): failed to read 2");
            exit(-3);
        }
        f.read( (char * ) &n_colours, sizeof(int) );
        if (f.gcount() != sizeof(int)) {
            perror("Mesh(filename): failed to read 3");
            exit(-3);
        }
        f.read( (char * ) &n_indices, sizeof(int) );
        if (f.gcount() != sizeof(int)) {
            perror("Mesh(filename): failed to read 4");
            exit(-3);
        }
        
        vertices = new float [3 * n_vertices];
        normals = new float [3 * n_normals];
        if (n_colours > 0)
            colours = new float [3 * n_colours];
        else
            colours = NULL;
        indices = new unsigned int [4 * n_indices];
        
        f.read( (char * ) vertices, sizeof(float) * 3 * n_vertices );
        if ( f.gcount() != sizeof(float) * 3 * n_vertices ) {
            perror("Mesh(filename): failed to read 5");
            exit(-3);
        }
        f.read( (char * ) normals, sizeof(float) * 3 * n_normals );
        if ( f.gcount() != sizeof(float) * 3 * n_normals ) {
            perror("Mesh(filename): failed to read 6");
            exit(-3);
        }
        if (colours != NULL) {
            f.read( (char * ) colours, sizeof(float) * 3 * n_colours );
            if ( f.gcount() != sizeof(float) * 3 * n_colours ) {
                perror("Mesh(filename): failed to read 7");
                exit(-3);
            }
        }
        f.read( (char * ) indices, sizeof(unsigned int) * 4 * n_indices );
        if ( f.gcount() != sizeof(unsigned int) * 4 * n_indices ) {
            perror("Mesh(filename): failed to read 8");
            exit(-3);
        }
        f.close();
    }
    timestamp_exit("done\n");
}

Mesh::~Mesh( )
{
    delete [] vertices;
    delete [] normals;
    delete [] colours;
    delete [] indices;
    
}

void Mesh::pack(const Quad_List& lst)
{
    
    Point_XYZNC_Item * vcur;
    Quad_Item * qcur;
    int i;
    
    if (lst.points->last != NULL)
        n_normals = n_vertices = n_colours = lst.points->last->index + 1;
    else
        n_normals = n_vertices = n_colours = 0;
    if (lst.last != NULL)
        n_indices = lst.last->index + 1;
    else
        n_indices = 0;
    
    vertices = new float [n_vertices * 3];
    normals = new float [n_normals * 3];
    colours = new float [n_colours * 3];
    indices = new unsigned int [n_indices * 4];
    
    i=0;
    vcur = lst.points->first;
    while (vcur != NULL)
    {
        vertices[i] = vcur->x;
        normals[i] = vcur->nx;
        colours[i++] = ( vcur->cn > 0.0 ? vcur->cr / vcur->cn : 0.0 ); // red
        vertices[i] = vcur->y;
        normals[i] = vcur->ny;
        colours[i++] = ( vcur->cn > 0.0 ? vcur->cg / vcur->cn : 0.0 ); // green
        vertices[i] = vcur->z;
        normals[i] = vcur->nz;
        colours[i++] = ( vcur->cn > 0.0 ? vcur->cb / vcur->cn : 1.0 ); // blue
        vcur = vcur->next;
    }
    
    i=0;
    qcur = lst.first;
    while (qcur != NULL)
    {
        indices[i++] = qcur->d->index;
        indices[i++] = qcur->c->index;
        indices[i++] = qcur->b->index;
        indices[i++] = qcur->a->index;
        qcur = qcur->next;
    }
}

void Mesh::save(char * filename)
{
    if (strstr(filename, ".Zmsh") != NULL)
        saveZ(filename);
    else
        saveR(filename);
}

void Mesh::saveR(char * filename)
{
    fstream f  (filename, ios::binary | ios::out | ios::trunc );
    
    if (f.fail())
    {
        perror("Mesh::saveR( ): could not open file for writing ");
        return;
    }
    f.write((char *) &n_vertices, sizeof(int));
    if (f.bad())
    {
        perror("Mesh::saveR( ): write 1 failed");
        return;
    }
    f.write((char *) &n_normals, sizeof(int));
    if (f.bad())
    {
        perror("Mesh::saveR( ): write 2 failed");
        return;
    }
    f.write((char *) &n_colours, sizeof(int));
    if (f.bad())
    {
        perror("Mesh::saveR( ): write 3 failed");
        return;
    }
    f.write((char *) &n_indices, sizeof(int));
    if (f.bad())
    {
        perror("Mesh::saveR( ): write 4 failed");
        return;
    }
    f.write((char *) vertices, sizeof(float) * 3 * n_vertices);
    if (f.bad())
    {
        perror("Mesh::saveR( ): write 5 failed");
        return;
    }
    f.write((char *) normals, sizeof(float) * 3 * n_normals);
    if (f.bad())
    {
        perror("Mesh::saveR( ): write 6 failed");
        return;
    }
    if (colours != NULL)
    {
        f.write((char *) colours, sizeof(float) * 3 * n_colours);
        if (f.bad())
        {
            perror("Mesh::saveR( ): write 7 failed");
            return;
        }
    }
    f.write((char *) indices, sizeof(unsigned int) * 4 * n_indices);
    if (f.bad())
    {
        perror("Mesh::saveR( ): write 8 failed");
        return;
    }
    f.close();
}

void Mesh::saveZ(char * filename)
{
    gzFile f = gzopen(filename, "wb");
    
    if (f == NULL)
    {
        perror("Mesh::saveZ( ): could not open file for writing ");
        return;
    }
    gzwrite(f, &n_vertices, sizeof(int));
    gzwrite(f, &n_normals, sizeof(int));
    gzwrite(f, &n_colours, sizeof(int));
    gzwrite(f, &n_indices, sizeof(int));
    gzwrite(f, vertices, sizeof(float) * 3 * n_vertices);
    gzwrite(f, normals, sizeof(float) * 3 * n_normals);
    if (colours != NULL)
    {
        gzwrite(f, colours, sizeof(float) * 3 * n_colours);
    }
    gzwrite(f, indices, sizeof(unsigned int) * 4 * n_indices);
    gzclose(f);
}

