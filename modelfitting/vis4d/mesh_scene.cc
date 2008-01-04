// mesh_scene.cc
// Additional constructors for the Mesh class
// Richard Seely 2007

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "mesh.hh"

Quad_List::Quad_List( const Voxel_Space& vspc, Scene& scn )
{
    VSPC_DTYPE * rootvox;
    int px, py, pz, ps;
    int ysk, zsk;
    float x,y,z,r,g,b;
    float nx, ny, nz;
    VSPC_DTYPE x1, x2, y1, y2, z1, z2, mask;
    
    timestamp_entry("Quad_List(Voxel_Space,Scene):\n");
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
                    nx = 0.0; ny = 0.0; nz = 0.0;
                    if ((x1 & mask) != 0)  nx -= 1.0;
                    if ((x2 & mask) != 0)  nx += 1.0;
                    if ((y1 & mask) != 0)  ny -= 1.0;
                    if ((y2 & mask) != 0)  ny += 1.0;
                    if ((z1 & mask) != 0)  nz -= 1.0;
                    if ((z2 & mask) != 0)  nz += 1.0;
                    
                    if ((x1 & mask) != 0)
                    {
                        scn.Guess_Colour( x, y, z, nx, ny, nz,  &r, &g, &b );
                        addc( x-0.5,y+0.5,z+0.5,    x-0.5,y-0.5,z+0.5,    x-0.5,y-0.5,z-0.5,    x-0.5,y+0.5,z-0.5,    -1,0,0,  r,g,b);
                    }
                    if ((x2 & mask) != 0)
                    {
                        scn.Guess_Colour( x, y, z, nx, ny, nz,  &r, &g, &b );
                        addc( x+0.5,y+0.5,z+0.5,    x+0.5,y+0.5,z-0.5,    x+0.5,y-0.5,z-0.5,    x+0.5,y-0.5,z+0.5,    1,0,0,  r,g,b );
                    }
                    if ((y1 & mask) != 0)
                    {
                        scn.Guess_Colour( x, y, z, nx, ny, nz,  &r, &g, &b );
                        addc( x+0.5,y-0.5,z+0.5,    x+0.5,y-0.5,z-0.5,    x-0.5,y-0.5,z-0.5,    x-0.5,y-0.5,z+0.5,    0,-1,0,  r,g,b );
                    }
                    if ((y2 & mask) != 0)
                    {
                        scn.Guess_Colour( x, y, z, nx, ny, nz,  &r, &g, &b );
                        addc( x+0.5,y+0.5,z+0.5,    x-0.5,y+0.5,z+0.5,    x-0.5,y+0.5,z-0.5,    x+0.5,y+0.5,z-0.5,    0,1,0,  r,g,b );
                    }
                    if ((z1 & mask) != 0)
                    {
                        scn.Guess_Colour( x, y, z, nx, ny, nz,  &r, &g, &b );
                        addc( x+0.5,y+0.5,z-0.5,    x-0.5,y+0.5,z-0.5,    x-0.5,y-0.5,z-0.5 ,    x+0.5,y-0.5,z-0.5,    0,0,-1,  r,g,b );
                    }
                    if ((z2 & mask) != 0)
                    {
                        scn.Guess_Colour( x, y, z, nx, ny, nz,  &r, &g, &b );
                        addc( x+0.5,y+0.5,z+0.5,    x+0.5,y-0.5,z+0.5,    x-0.5,y-0.5,z+0.5 ,    x-0.5,y+0.5,z+0.5,    0,0,1,  r,g,b );
                    }
                    x += 1.0;
                }
            }
            y += 1.0;
        }
        z += 1.0;
    }
    timestamp_exit("done\n");
}
