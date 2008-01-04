// points.hh
// A set of routines for creating, manipulating and converting a volumetric point list
// Richard Seely - University of Southampton, UK 2006

#ifndef _POINTS_HH
#define _POINTS_HH
#include "util.hh"

typedef struct _Point_XYZ_Item Point_XYZ_Item;
struct _Point_XYZ_Item {
    Point_XYZ p;
    Point_XYZ_Item * next;
};
    
class Point_XYZ_List
{
    public:
        Point_XYZ_List( );
        Point_XYZ_List(char * filename);
        Point_XYZ_List( const Point_XYZ_List& lst );
        ~Point_XYZ_List( );
        void Add(float x, float y, float z);
        void Print( );
        void Recentre( );
        void Translate(float x, float y, float z);
        void Scale(float x, float y, float z);
        
        Point_XYZ min;
        Point_XYZ max;
        int count;
    
        Point_XYZ_Item * first;
        Point_XYZ_Item * last;
};

//VSPACE * points_to_VSPACE(POINTS * pnts, float border);
#endif
