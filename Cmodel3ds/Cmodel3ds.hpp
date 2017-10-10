#ifndef CMODEL3DS_HPP
#define CMODEL3DS_HPP

#include <iostream>
#include <stdio.h>
#include <string>
#include <cstring>
#include <cassert>

#include <lib3ds/file.h>
#include <lib3ds/mesh.h>

#if (!defined(DBG))
    #define DBG(x...) {printf("\e[033;34m%s<%d> \e[0m",__FUNCTION__,__LINE__); printf(x);}
    #define DBGF(x...) {printf("\e[033;34m%s-%s<%d> \e[0m",__FILE__,__FUNCTION__,__LINE__); printf(x);}
    #define FATAL(x...) {printf("\e[033;31m%s<%d> \e[0m",__FUNCTION__,__LINE__); printf(x);}
#endif


typedef unsigned int GLuint;

class CModel3DS
{
public:
    CModel3DS();
    CModel3DS(std:: string obj);

    virtual void CreateVBO();
    virtual void Print2file(char *obj);
    virtual ~CModel3DS();


protected:
    void GetFaces();
    unsigned int m_TotalFaces;
    Lib3dsFile * m_model;
    GLuint m_VertexVBO, m_NormalVBO;

    Lib3dsVector * vertices;
    Lib3dsVector * normals;
    Lib3dsTexel * texCoords;
};

#endif // CMODEL3DS_HPP
