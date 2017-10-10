#include "load3ds.h"

#define POINT3F_NUM 3

load3DS::load3DS(std::string name){
    //init
    m_faces_total = 0;
    Lib3dsMesh * mesh;			//temp mesh
    Lib3dsFace * face; 			//temp face
    //load
    m_model = lib3ds_file_load(name.c_str());
    if(!m_model){
        throw strcat("Unable to load ",name.c_str());
    }
    //get face total
    getFaces();
    qDebug() << "m_faces_total = " << m_faces_total;


    vertices    = new Lib3dsVector[m_faces_total * 3];
    m_vertex_total = m_faces_total*3;

    qDebug() << "m_vertex_total = " << m_vertex_total;
    vertex = new float[m_vertex_total*POINT3F_NUM];

    for (mesh = m_model->meshes;mesh != NULL;mesh = mesh->next){
        for(unsigned int cur_face = 0;cur_face < mesh->faces;cur_face++){
            face = &mesh->faceL[cur_face];
            for(unsigned int i = 0;i < 3;i++  ){
                memcpy(&vertices[cur_face*3 + i], mesh->pointL[face->points[ i ]].pos, sizeof(Lib3dsVector));
            }
        }
    }

    for(unsigned long x = 0;x < m_vertex_total;x++){
        vertex[x*3+0] = vertices[x][0];
        vertex[x*3+1] = vertices[x][1];
        vertex[x*3+2] = vertices[x][2];
    }

}

load3DS::~load3DS(){

}

int load3DS::getFaces(){
    assert(m_model != NULL);

    Lib3dsMesh * mesh;			//temp mesh
    for(mesh = m_model->meshes;mesh != NULL;mesh = mesh->next)
    {
        // Add the number of faces this mesh has to the total faces
        m_faces_total += mesh->faces;
    }
}


void load3DS::draw(unsigned int mode){
    glColor4f(0.0f,0.0f,1.0f,1.0f);
    glVertexPointer(3,GL_FLOAT,0,vertex);
    glDrawArrays(mode,0,m_vertex_total);
}

