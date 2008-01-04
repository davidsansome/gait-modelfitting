// points.cc
// A set of routines for creating, manipulating and converting a volumetric point list
// Richard Seely - University of Southampton, UK 2006

#include <stdlib.h>
#include <stdio.h>

#include "util.hh"
#include "points.hh"

Point_XYZ_List::Point_XYZ_List( )
{
    first = NULL;
    last = NULL;
    min.x = min.y = min.z = 0.0;
    max.x = max.y = max.z = 0.0;
    count = 0;
}

Point_XYZ_List::Point_XYZ_List( const Point_XYZ_List& lst )
{
    Point_XYZ_Item * cur;
    Point_XYZ_Item * nit;
    
    timestamp_entry("Point_XYZ_List( Point_XYZ_List ):\n");
    
    first = NULL;
    last = NULL;
    min.x = lst.min.x; min.y = lst.min.y; min.z = lst.min.z;
    max.x = lst.max.x; max.y = lst.max.y; max.z = lst.max.z;
    count = lst.count;
    
    cur = lst.first;
    while (cur != NULL)
    {
        nit = new Point_XYZ_Item;
        nit->p.x = cur->p.x;
        nit->p.y = cur->p.y;
        nit->p.z = cur->p.z;
        if (last == NULL)
        {
            last = nit;
            first = nit;
        }
        else
        {
            last->next = nit;
            last = nit;
        }
        cur = cur->next;
    }
    if (last != NULL)
        last->next = NULL;
    timestamp_exit("done\n");
}

Point_XYZ_List::~Point_XYZ_List( )
{
    Point_XYZ_Item * ptr;
    
    timestamp_entry("~Point_XYZ_List: ");
    while (first != NULL)
    {
        ptr = first;
        first = ptr->next;
        free(ptr);
    }
    timestamp_exit("goodbye cruel world\n");
}

void Point_XYZ_List::Add(float x, float y, float z)
{
    Point_XYZ_Item * npnt = new Point_XYZ_Item;
    npnt->p.x = x;
    npnt->p.y = y;
    npnt->p.z = z;
    npnt->next = NULL;
    if (last != NULL)
    {
        last->next = npnt;
        last = npnt;
        if (x < min.x) min.x = x;
        if (y < min.y) min.y = y;
        if (z < min.z) min.z = z;
        if (x > max.x) max.x = x;
        if (y > max.y) max.y = y;
        if (z > max.z) max.z = z;
        count++;
    }
    else
    {
        first = npnt;
        last = npnt;
        min.x = max.x = x;
        min.y = max.y = y;
        min.z = max.z = z;
        count = 1;
    }
}


Point_XYZ_List::Point_XYZ_List(char * filename)
{
    FILE * vfile;
    float X, Y, Z;
    
    timestamp_entry("Point_XYZ_List():  reading voxel data\n");
    IF_DEBUG { print_indent(); printf("sizeof(Point_XYZ_Item)=%d\n", sizeof(Point_XYZ_Item)); }
    first = NULL;
    last = NULL;
    min.x = min.y = min.z = 0.0;
    max.x = max.y = max.z = 0.0;
    count = 0;
    
    if ((vfile = fopen(filename, "r")) == NULL)
    {
        perror("Point_XYZ_List(): fopen failed");
        delete(this);
        return;
    }
    while (fscanf(vfile, "%f %f %f", &X, &Y, &Z) == 3)
    {
        Add(X,Y,Z);
    }
    fclose(vfile);
    timestamp_exit("done\n");
}

void Point_XYZ_List::Recentre( )
{
    Point_XYZ_Item * cur;
    float avg_x = 0.0;
    float avg_y = 0.0;
    float avg_z = 0.0;
    float cnt = 0.0;
    
    timestamp_entry("Point_XYZ_List::recentre:\n");
    
    if (first == NULL) {
        timestamp_exit("empty point list\n");
        return;
    }
    cur = first;
    
    IF_DEBUG timestamp_inside("finding average values\n");
    
    while (cur != NULL) {
        avg_x += cur->p.x;
        avg_y += cur->p.y;
        avg_z += cur->p.z;
        cur = cur->next;
        cnt += 1.0;
    }
    avg_x /= cnt;
    avg_y /= cnt;
    avg_z /= cnt;
    
    IF_DEBUG {
        print_indent(); printf("avg X=%5f, Y=%5f, Z=%5f\n",avg_x,avg_y,avg_z); 
        timestamp_inside("moving centre of mass to origin\n");
    }
    
    cur = first;
    while (cur != NULL) {
        cur->p.x -= avg_x;
        cur->p.y -= avg_y;
        cur->p.z -= avg_z;
        cur = cur->next;
    }
    min.x -= avg_x; max.x -= avg_x;
    min.y -= avg_y; max.y -= avg_y;
    min.z -= avg_z; max.z -= avg_z;
    timestamp_exit("done\n");
}

void Point_XYZ_List::Translate(float x,float y,float z)
{
    Point_XYZ_Item * cur;
    
    timestamp_entry("Point_XYZ_List::translate:\n");
    
    if (first == NULL) {
        printf("empty point list\n");
        return;
    }

    cur = first;
    while (cur != NULL) {
        cur->p.x += x;
        cur->p.y += y;
        cur->p.z += z;
        cur = cur->next;
    }
    min.x += x; max.x += x;
    min.y += y; max.y += y;
    min.z += z; max.z += z;
    timestamp_exit("  done\n");
}

void Point_XYZ_List::Scale(float x,float y,float z)
{
    Point_XYZ_Item * cur;
    
    timestamp_entry("Point_XYZ_List::scale:\n");
    
    if (first == NULL) {
        printf("empty point list\n");
        return;
    }

    cur = first;
    while (cur != NULL) {
        cur->p.x *= x;
        cur->p.y *= y;
        cur->p.z *= z;
        cur = cur->next;
    }
    min.x *= x; max.x *= x;
    min.y *= y; max.y *= y;
    min.z *= z; max.z *= z;
    timestamp_exit("done\n");
}
