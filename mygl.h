/*			DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
					Version 2, December 2004

 Copyright (C) 2017 Romain Lespinasse <romain.lespinasse@gmail.com>

 Everyone is permitted to copy and distribute verbatim or modified
 copies of this license document, and changing it is allowed as long
 as the name is changed.

			DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

  0. You just DO WHAT THE FUCK YOU WANT TO.
*/

#ifndef MYGL_H
#define MYGL_H

#include <QGLWidget>
#include <QtOpenGL/QtOpenGL>
#include <opencv2/opencv.hpp>
#include "marker.h"
#include <lib3ds/file.h>
#include <lib3ds/mesh.h>
//#include "MeshModel.h"
#include <QMatrix4x4>
#include "load3ds.h"
#include "loadOBJ/objloader.h"
#include "loadOBJ/reference.h"

using namespace std;

#define MIN_LENGTH 35
#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480

class myGL:public QGLWidget
{
	Q_OBJECT
public:
	myGL(QWidget * parent = 0);
	~myGL(){};
	//cam
	cv::Mat srcImage;
	cv::VideoCapture cam,video;
	vector<cv::Point3f> Xworld;
	vector<GLfloat> rotMatrix;//某一个标记的变换矩阵
	GLfloat rotMat[16];//为了对应opengl函数格式定义的数组
	vector<vector<GLfloat> >  RotationMatrix;//将所有的变换矩阵保存同一渲染
	cv::Mat cameraMatrix = cv::Mat(3,3,CV_32FC1,1);
	cv::Mat distCoeffs   = cv::Mat(1,5,CV_32FC1,1);
	float projection_matrix[16];//
	float model_view_matrix[16];//
	std::vector<Marker> possible_markers;
	std::vector<Marker> final_markers;

	QTimer clk;
	float WINDOW_SIZE;
	GLuint textureBack; //background texture
	GLuint textureCube; //cube texture


	cv::Mat warpMat;
	cv::Point2f dstRect[4];
	cv::Point2f srcRect[4];
	char number;
	cv::Mat dobotPos = cv::Mat(3, 1, CV_64FC1, cv::Scalar(0, 0, 0));
	cv::Mat dobotTargetPos;
	cv::Point2f dobotFinalTargetPos;//归一化后位置


	void intrinsicMatrix2ProjectionMatrix(cv::Mat& camera_matrix, float width, float height, float near_plane, float far_plane);
	void extrinsicMatrix2ModelViewMatrix(cv::Mat& rotation, cv::Mat& translation);
	void imageProcess(cv::Mat);
	void printMatrix(GLenum type/*,string &message*/);
	void polygon(int a,int b,int c,int d);
	void cube();
    void Helptexture();
    //void Load3DS(/*std::string Name*/);
    //float loadV[3*12*3];
    load3DS * model1;
	ObjLoader * modelObj;

protected:
	void initializeGL();
	void initWidget();
	void paintGL();
	void resizeGL(int width, int height);
	void loadGLTextures();
	void  mousePressEvent(QMouseEvent *);
private slots:
	void updateWindow();
	void updateParams(int);
};

#endif // MYGL_H
