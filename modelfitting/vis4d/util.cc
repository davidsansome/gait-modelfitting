// util.c
// Richard Seely 2006

#include <stdio.h>
#include <time.h>

#include "util.hh"

int _indent_level = 0;
bool _debug = true;

void timestamp(void) {
    float tme;
    if (_debug)
    {
        tme = ((float) (clock())) / ((float) CLOCKS_PER_SEC);
        printf("%5hgs: ", tme);
    }
}

void timestamp2(char * str) {
    float tme;
    if (_debug)
    {
        tme = ((float) (clock())) / ((float) CLOCKS_PER_SEC);
        printf("%5hgs: %s", tme, str);
        fflush(stdout);
    }
}

void timestamp_entry(char * str)
{
    float tme;
    if (_debug)
    {
        tme = ((float) (clock())) / ((float) CLOCKS_PER_SEC);
        printf("%5hgs: ", tme);
        for (int i=0; i < _indent_level; i++)
            printf("  ");
        _indent_level++;
        printf("%s", str);
        fflush(stdout);
    }
}

void timestamp_inside(char * str)
{
    float tme;
    
    tme = ((float) (clock())) / ((float) CLOCKS_PER_SEC);
    printf("%5hgs: ", tme);
    for (int i=0; i < _indent_level; i++)
        printf("  ");
    printf("%s", str);
    fflush(stdout);
}

void timestamp_exit(char * str)
{
    float tme;
    if (_debug)
    {
        tme = ((float) (clock())) / ((float) CLOCKS_PER_SEC);
        printf("%5hgs: ", tme);
        for (int i=0; i < _indent_level; i++)
            printf("  ");
        _indent_level--;
        printf("%s", str);
        fflush(stdout);
    }
}

void print_inside( char * str )
{
    printf("       ");
    for (int i=0; i < _indent_level; i++)
        printf("  ");
    printf("%s", str);
    fflush(stdout);
}

void print_indent( )
{
    printf("       ");
    for (int i=0; i < _indent_level; i++)
        printf("  ");
}
