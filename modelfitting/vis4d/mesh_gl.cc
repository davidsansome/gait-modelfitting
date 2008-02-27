// mesh_gl.cc
// Contains the functions for drawing the Mesh (using openGL)
// Richard Seely 2007

#include "glwin.h"
#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glext.h>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "mesh.hh"

void Mesh::draw_slow( ) const
{
    int n,i;
    if (colours != NULL)
    {
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
        glBegin(GL_QUADS);
        for (n=0; n < n_indices*4; n++)
        {
            i = indices[n];
            glNormal3f(normals[i * 3], normals[i * 3+1], normals[i * 3+2]);
            glColor3f(colours[i * 3], colours[i * 3+1], colours[i * 3+2]);
            glVertex3f(vertices[i * 3], vertices[i * 3+1], vertices[i * 3+2]);
        }
        glEnd();
        glDisable(GL_COLOR_MATERIAL);
    }
    else
    {
        glBegin(GL_QUADS);
        for (n=0; n < n_indices; n++)
        {
            glNormal3f(normals[indices[n] * 3], normals[indices[n] * 3+1], normals[indices[n] * 3+2]);
            glVertex3f(vertices[indices[n] * 3], vertices[indices[n] * 3+1], vertices[indices[n] * 3+2]);
        }
        glEnd();
    }
}

void Mesh::draw_fast( ) const
{
    glPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_NORMAL_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
    
    glBindBuffer( GL_ARRAY_BUFFER, _vbo[2] );
    glColorPointer( 3, GL_FLOAT, 0, (GLvoid*)((char*)NULL) );
    glBindBuffer( GL_ARRAY_BUFFER, _vbo[1] );
    glNormalPointer( GL_FLOAT, 0, (GLvoid*)((char*)NULL) );
    glBindBuffer( GL_ARRAY_BUFFER, _vbo[0] );
    glVertexPointer( 3, GL_FLOAT, 0, (GLvoid*)((char*)NULL) );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _vbo[3] );
    glDrawRangeElements( GL_QUADS, 0,  n_indices * 4 - 1, n_indices * 4, GL_UNSIGNED_INT, (GLvoid*)((char*)NULL) );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glPopClientAttrib();
    glDisable(GL_COLOR_MATERIAL);
    /*glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    if (colours != NULL)
        glEnableClientState(GL_COLOR_ARRAY);
    else
        glDisableClientState(GL_COLOR_ARRAY);

    glVertexPointer( 3, GL_FLOAT, 0, vertices );
    glNormalPointer(  GL_FLOAT, 0, normals );
    if(colours != NULL)
        glColorPointer( 3, GL_FLOAT, 0, colours );
    //glDrawElements( GL_QUADS, n_indices * 4, GL_UNSIGNED_INT, indices );
    glDrawRangeElements( GL_QUADS, 0, n_indices * 4 - 1, n_indices * 4, GL_UNSIGNED_INT, indices );
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);*/
}

void Mesh::draw( ) const
{
    if (use_draw_fast)
        draw_fast( );
    else
        draw_slow( );
}

void Mesh::draw_init( bool fast )
{
    timestamp2("Mesh::draw_init()\n");
    use_draw_fast = fast;
    if (fast)
    {    
        glGenBuffers(4, _vbo);
        glBindBuffer( GL_ARRAY_BUFFER, _vbo[0] );
        glBufferData( GL_ARRAY_BUFFER, n_vertices*3*sizeof(GLfloat), vertices, GL_STATIC_DRAW );
        glBindBuffer( GL_ARRAY_BUFFER, _vbo[1] );
        glBufferData( GL_ARRAY_BUFFER, n_normals*3*sizeof(GLfloat), normals, GL_STATIC_DRAW );
        glBindBuffer( GL_ARRAY_BUFFER, _vbo[2] );
        glBufferData( GL_ARRAY_BUFFER, n_colours*3*sizeof(GLfloat), colours, GL_STATIC_DRAW );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _vbo[3] );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, n_indices*4*sizeof(GLuint), indices, GL_STATIC_DRAW );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    }
    timestamp2("  done\n");
}

void Mesh::draw_destroy( )
{
    timestamp2("Mesh::draw_destroy()\n");
    glDeleteBuffers( 4, _vbo );
    timestamp2("  done\n");
}

