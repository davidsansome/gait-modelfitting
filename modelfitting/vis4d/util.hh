// util.h
// Richard Seely 2006

#ifndef _UTIL_H
#define _UTIL_H

extern bool _debug;

#define IF_DEBUG if (_debug) 

typedef struct _Point_XYZ {
    float x;
    float y;
    float z;
} Point_XYZ;

typedef struct _MATRIX_3x4
{
    float i_00;
    float i_01;
    float i_02;
    float i_03;
    float i_10;
    float i_11;
    float i_12;
    float i_13;
    float i_20;
    float i_21;
    float i_22;
    float i_23;
} MATRIX_3x4;

class Voxel_Space;
class Voxel_Field;

void timestamp(void);
void timestamp2(char * str) ;

void timestamp_entry(char * str);
void timestamp_inside(char * str);
void timestamp_exit(char * str);
void print_inside( char * str );
void print_indent( );
#endif
