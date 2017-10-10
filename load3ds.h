#ifndef LOAD3DS_H
#define LOAD3DS_H

#include <lib3ds/file.h>
#include <lib3ds/mesh.h>
#include <QDebug>
#include <string>
#include <QGLWidget>
#include <QMatrix4x4>
#include <assert.h>

class load3DS
{
public:
//    load3DS();
    load3DS(std::string);
    int getFaces();
    void draw(unsigned int mode = GL_LINE_LOOP);
    ~load3DS();
public:
    std::string Name;			//file name
    Lib3dsFile * m_model;		//model file
    Lib3dsVector * vertices;	//all vertices adderss
    float * vertex;				//all vertex
    unsigned long m_faces_total;			//faces total
    unsigned long m_vertex_total;			//vertext total

};

#endif // LOAD3DS_H
