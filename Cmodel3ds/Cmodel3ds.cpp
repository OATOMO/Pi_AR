#include "Cmodel3ds.hpp"


CModel3DS::CModel3DS()
{


}


CModel3DS::~CModel3DS()
{
    // Clean up our allocated memory
    delete vertices;
    delete normals;
    delete texCoords;

    // We no longer need lib3ds
    lib3ds_file_free(m_model);
    m_model = NULL;

}

// Load 3DS model
CModel3DS::CModel3DS(std:: string obj)
{
        char tmp[64];
        sprintf(tmp, "%s.3ds", obj.c_str());
        m_TotalFaces = 0;

        m_model = lib3ds_file_load(tmp);
        // If loading the model failed, we throw an exception
        if(!m_model)
        {
                throw printf("Unable to load: %s\n", tmp);
        }else{
             DBG("Load ok: %s \n", tmp);
        }
}



// Count the total number of faces this model has
void CModel3DS::GetFaces()
{
        assert(m_model != NULL);

        m_TotalFaces = 0;
        Lib3dsMesh * mesh;
        // Loop through every mesh
        for(mesh = m_model->meshes;mesh != NULL;mesh = mesh->next)
        {
                // Add the number of faces this mesh has to the total faces
                m_TotalFaces += mesh->faces;

        }

        
}



// Copy vertices and normals to the memory of the GPU
void CModel3DS::CreateVBO()
{
        assert(m_model != NULL);

        // Calculate the number of faces we have in total
        GetFaces();

        // Allocate memory for our vertices and normals
        vertices    = new Lib3dsVector[m_TotalFaces * 3];
        normals     = new Lib3dsVector[m_TotalFaces * 3];        
        texCoords   = new Lib3dsTexel[m_TotalFaces * 3];

        Lib3dsMesh * mesh;
        Lib3dsFace * face;
        unsigned int FinishedFaces = 0;

        // Loop through all the meshes
        for(mesh = m_model->meshes;mesh != NULL;mesh = mesh->next)
        {
                lib3ds_mesh_calculate_normals(mesh, &normals[FinishedFaces*3]);
                // Loop through every face
                for(unsigned int cur_face = 0; cur_face < mesh->faces;cur_face++)
                {
                        face = &mesh->faceL[cur_face];

                        for(unsigned int i = 0;i < 3;i++)
                        {
                                memcpy(&vertices[FinishedFaces*3 + i], mesh->pointL[face->points[ i ]].pos, sizeof(Lib3dsVector));                                
                                memcpy(&texCoords[FinishedFaces*3 + i], mesh->texelL[face->points[ i ]], sizeof(Lib3dsTexel)); 
                        }
                        FinishedFaces++;
                }
        }
}



// Copy vertices and normals to the memory of the GPU
void CModel3DS::Print2file(char *obj)
{
    unsigned int i = 0;
    char tmpout[128];
    sprintf(tmpout, "%s.h", obj);


    DBG("outfile: %s\n", tmpout);


    FILE *pFile=NULL;
    pFile = fopen(tmpout, "w");
    if(!pFile) return;


    fflush(pFile);
    fprintf(pFile, "unsigned int %sNumVerts = %d; \n\n", obj, m_TotalFaces*3);



    fprintf(pFile, "float %sVerts [] = { \n", obj);
    for(i = 0; i < 3*m_TotalFaces ; i++){
//        DBG("vertices[%d] = %lf \n", i, vertices[i][0]);
//        DBG("vertices[%d] = %lf \n", i, vertices[i][1]);
//        DBG("vertices[%d] = %lf \n", i, vertices[i][2]);
        fprintf(pFile, "%f, %f, %f, \n", vertices[i][0], vertices[i][1], vertices[i][2]);
    }   
    fprintf(pFile, "};\n\n");




    fprintf(pFile, "float %sTexCoords [] = { \n", obj);
    for(i = 0; i < 3*m_TotalFaces ; i++){
//        DBG("texCoords[%d] = %lf \n", i, texCoords[i]);
//        DBG("texCoords[%d] = %lf \n", i, texCoords[i][1]);
        fprintf(pFile, "%f, %f, %f, \n", texCoords[i][0], texCoords[i][1]);
    }
    fprintf(pFile, "};\n\n");




    fprintf(pFile, "float %sNormals [] = { \n", obj);
    for(i = 0; i < 3*m_TotalFaces ; i++){
//        DBG("normals[%d] = %lf \n", i, normals[i]);
//        DBG("normals[%d] = %lf \n", i, normals[i][1]);
//        DBG("normals[%d] = %lf \n", i, normals[i][2]);
        fprintf(pFile, "%f, %f, %f, \n", normals[i][0], normals[i][1], normals[i][2]);
    }
    fprintf(pFile, "};\n\n");

    fflush(pFile);
    fclose (pFile);
    return;

}


int main(int argc, char **argv)
{
    CModel3DS *my3DModel;

    my3DModel = new CModel3DS(argv[1]);
    my3DModel->CreateVBO();
    if(argc > 2)
        my3DModel->Print2file(argv[2]);
    else
        my3DModel->Print2file(argv[1]);

    delete my3DModel;
    my3DModel = NULL;

    return 1;
}







































#if 0
for each mesh do
   for each face do
      setup face material
      draw face
   end
end
// Copy vertices and normals to the memory of the GPU
void CModel3DS::CreateVBO()
{
        assert(m_model != NULL);
        // Calculate the number of faces we have in total
        GetFaces();
        // Allocate memory for our vertices and normals
        Lib3dsVector * vertices = new Lib3dsVector[m_TotalFaces * 3];
        Lib3dsVector * normals = new Lib3dsVector[m_TotalFaces * 3];
        Lib3dsTexel * texCoords = new Lib3dsTexel[m_TotalFaces * 2];
        Lib3dsMesh * mesh;
        Lib3dsFace * face;
        unsigned int FinishedFaces = 0;
        // Loop through all the meshes
        for(mesh = m_model->meshes;mesh != NULL;mesh = mesh->next)
        {
                lib3ds_mesh_calculate_normals(mesh, &normals[FinishedFaces*3]);
                // Loop through every face
                for(unsigned int cur_face = 0; cur_face < mesh->faces;cur_face++)
                {
                        face = &mesh->faceL[cur_face];
                        for(unsigned int i = 0;i < 3;i++)
                        {
                                memcpy(&vertices[FinishedFaces*3 + i], mesh->pointL[face->points[ i ]].pos, sizeof(Lib3dsVector));
                                memcpy(&texCoords[FinishedFaces*3 + i], mesh->texelL[face->points[ i ]].pos, sizeof(Lib3dsTexel);
                        }
                        FinishedFaces++;
                }
        }
        // Generate a Vertex Buffer Object and store it with our vertices
        glGenBuffers(1, &m_VertexVBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Lib3dsVector) * 3 * m_TotalFaces, vertices, GL_STATIC_DRAW);
        // Generate another Vertex Buffer Object and store the normals in it
        glGenBuffers(1, &m_NormalVBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_NormalVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Lib3dsVector) * 3 * m_TotalFaces, normals, GL_STATIC_DRAW);
        // Clean up our allocated memory
        delete []vertices;
        delete []normals;
        // We no longer need lib3ds
        lib3ds_file_free(m_model);
        m_model = NULL;
}
// Render the model using Vertex Buffer Objects
void CModel3DS:: Draw() const
{
        assert(m_TotalFaces != 0);
        // Enable vertex and normal arrays
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        // Bind the vbo with the normals
        glBindBuffer(GL_ARRAY_BUFFER, m_NormalVBO);
        // The pointer for the normals is NULL which means that OpenGL will use the currently bound vbo
        glNormalPointer(GL_FLOAT, 0, NULL);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexVBO);
        glVertexPointer(3, GL_FLOAT, 0, NULL);
        // Render the triangles
        glDrawArrays(GL_TRIANGLES, 0, m_TotalFaces * 3);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
}
// A render widget for QT
class CRender : public QGLWidget
{
        public:
                CRender(QWidget *parent = 0);
        protected:
                virtual void initializeGL();
                virtual void resizeGL(int width, int height);
                virtual void paintGL();
        private:
                CModel3DS * monkey;
};
// Constructor, initialize our model-object
CRender::CRender(QWidget *parent) : QGLWidget(parent)
{
        try
        {
                monkey = new CModel3DS("monkey.3ds");
        }
        catch(std::
 string error_str)
        {
                std::cerr << "Error: " << error_str << std::endl;
                exit(1);
        }
}
// Initialize some OpenGL settings
void CRender::initializeGL()
{
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glShadeModel(GL_SMOOTH);
        // Enable lighting and set the position of the light
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHTING);
        GLfloat pos[] = { 0.0, 4.0, 4.0 };
        glLightfv(GL_LIGHT0, GL_POSITION, pos);
        // Generate Vertex Buffer Objects
        monkey->CreateVBO();
}
// Reset viewport and projection matrix after the window was resized
void CRender::resizeGL(int width, int height)
{
        // Reset the viewport
        glViewport(0, 0, width, height);
        // Reset the projection and modelview matrix
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        // 10 x 10 x 10 viewing volume
        glOrtho(-5.0, 5.0, -5.0, 5.0, -5.0, 5.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
}
// Do all the OpenGL rendering
void CRender:: paintGL()
{
        glClear(GL_COLOR_BUFFER_BIT);
        // Draw our model
        monkey->Draw();
        // We don't need to swap the buffers, because QT does that automaticly for us
}
int main(int argc, char **argv)
{
        QApplication app(argc, argv);
        CRender * window = new CRender();
        window->show();
        return app.exec();
}
#endif
