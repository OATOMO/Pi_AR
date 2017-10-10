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
#include <unistd.h>
#include <stdlib.h>
#include "mygl.h"
#include "decode.h"
#include <GL/glu.h>

//0:v4l2  path:video
#define OPEN_VIDEO "/opt/git_Atom/Pi_AR/h264/9.h264"
#define OPEN_OBJ "/opt/git_Atom/Pi_AR/models/kaer/kaer1.obj"
#define OPEN_MTL "/opt/git_Atom/Pi_AR/models/kaer/kaer1.mtl"

#define PI 3.14159265358979323846
#define PI2 6.28318530717958647692

//cMeshModel *m_chanche;

int uStepsNum = 25,vStepNum = 25;
class Point
{
public:
	Point(){};
	Point(double a,double b,double c):x(a),y(b),z(c){};
public:
	double x;
	double y;
	double z;
};

Point getPoint(double u,double v)
{
	double x = sin(PI*v)*cos(PI2*u);
	double y = sin(PI*v)*sin(PI2*u);
	double z = cos(PI*v);
	return Point(x,y,z);
}

void drawWire()
{
    glTranslatef(0,0,1.0);
    glColor3f(0.5,0.5,0);
	double ustep = 1/(double)uStepsNum, vstep = 1/(double)vStepNum;
	double u = 0,v = 0;
	//绘制下端三角形组
	for(int i = 0;i<uStepsNum;i++)
	{
		glBegin(GL_LINE_LOOP);
		Point a = getPoint(0,0);
		glVertex3d(a.x,a.y,a.z);
		Point b = getPoint(u,vstep);
		glVertex3d(b.x,b.y,b.z);
		Point c = getPoint(u+ustep,vstep);
		glVertex3d(c.x,c.y,c.z);
		u += ustep;
		glEnd();
	}
	//绘制中间四边形组
	u = 0, v = vstep;
	for(int i=1;i<vStepNum-1;i++)
	{
		for(int j=0;j<uStepsNum;j++)
		{
			glBegin(GL_LINE_LOOP);
			Point a = getPoint(u,v);
			Point b = getPoint(u+ustep,v);
			Point c = getPoint(u+ustep,v+vstep);
			Point d = getPoint(u,v+vstep);
			glVertex3d(a.x,a.y,a.z);
			glVertex3d(b.x,b.y,b.z);
			glVertex3d(c.x,c.y,c.z);
			glVertex3d(d.x,d.y,d.z);
			u += ustep;
			glEnd();
		}
		v += vstep;
	}
	//绘制下端三角形组
	u = 0;
	for(int i=0;i<uStepsNum;i++)
	{
		glBegin(GL_LINE_LOOP);
		Point a = getPoint(0,1);
		Point b = getPoint(u,1-vstep);
		Point c = getPoint(u+ustep,1-vstep);
		glVertex3d(a.x,a.y,a.z);
		glVertex3d(b.x,b.y,b.z);
		glVertex3d(c.x,c.y,c.z);
		glEnd();
	}
}


GLfloat vertex[][3] = {
	{-1.0,-1.0,2.0},{-1.0,1.0,2.0},{1.0,1.0,2.0},{1.0,-1.0,2.0},
	{-1.0,-1.0,0.0},{-1.0,1.0,0.0},{1.0,1.0,0.0},{1.0,-1.0,0.0},
};

GLfloat colors[][3] = {
	{1.0,0.0,0.0},{0.0,1.0,1.0},{1.0,1.0,0.0},{0.0,1.0,0.0},
	{0.0,0.0,1.0},{1.0,1.0,1.0}
};

void myGL::polygon(int a,int b,int c,int d){
	glBegin(GL_POLYGON);
//	glBegin(GL_LINE_LOOP);
	glVertex3fv(vertex[a]);
	glVertex3fv(vertex[b]);
	glVertex3fv(vertex[c]);
	glVertex3fv(vertex[d]);
	glEnd();
}

void myGL::cube(){
	glColor3fv(colors[0]);
	polygon(0,3,2,1);
	glColor3fv(colors[2]);
	polygon(2,3,7,6);
	glColor3fv(colors[3]);
	polygon(3,0,4,7);
	glColor3fv(colors[1]);
	polygon(1,2,6,5);
	glColor3fv(colors[4]);
	polygon(4,5,6,7);
	glColor3fv(colors[5]);
	polygon(5,4,0,1);

}


myGL::myGL(QWidget * parent):QGLWidget(parent),number(0){
//	cam.open(0);
//    cam.open("/opt/git_Atom/atom_AR/Pi/h264/7.h264");
//	cam.open("/opt/_file/raspi/7.h264");
    cam.open(OPEN_VIDEO);
	if(!cam.isOpened()){
        qDebug() << "open video faild";
        exit(-1);
    }

	initWidget();
	initializeGL();

//	clk.start(1000);
	clk.start(40);
	QObject::connect(&clk,SIGNAL(timeout()),this,SLOT(updateWindow()));

	cameraMatrix.at<float>(0,0) = 1112.75949f;
	cameraMatrix.at<float>(0,1) = 0.0f;
	cameraMatrix.at<float>(0,2) = 284.84473f;
	cameraMatrix.at<float>(1,0) = 0.0f;
	cameraMatrix.at<float>(1,1) = 1112.75949f;
	cameraMatrix.at<float>(1,2) = -100.38948f;
	cameraMatrix.at<float>(2,0) = 0.0f;
	cameraMatrix.at<float>(2,1) = 0.0f;
	cameraMatrix.at<float>(2,2) = 1.0f;

	distCoeffs.at<float>(0,0) = -0.823854f;
	distCoeffs.at<float>(0,1) = 2.014462f;
	distCoeffs.at<float>(0,2) = 0.050665f;
	distCoeffs.at<float>(0,3) = 0.006921f;
//	distCoeffs.at<float>(0,4) = -3.729648f;

//	Xworld.push_back(cv::Point3f(-0.5, -0.5, 0));
//	Xworld.push_back(cv::Point3f(-0.5, 0.5, 0));
//	Xworld.push_back(cv::Point3f(0.5, 0.5, 0));
//	Xworld.push_back(cv::Point3f(0.5, -0.5, 0));
	Xworld.push_back(cv::Point3f(-0.5, 0.5, 0));
	Xworld.push_back(cv::Point3f(-0.5, -0.5, 0));
	Xworld.push_back(cv::Point3f(0.5, -0.5, 0));
	Xworld.push_back(cv::Point3f(0.5, 0.5, 0));
	WINDOW_SIZE = 0.5;

}

void myGL::initializeGL()
{
	qDebug() << "at initializeGL";


//	Load3DS();
//	 m_chanche = new cMeshModel(QString("chanche.3ds"));

	loadGLTextures();		//加载图片文件
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0);
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//light
//	GLfloat light_pos[] = {1.0,1.0,1.0,1.0};
	GLfloat light_pos[] = {1.0f, 1.0f, 1.0f, 0.0f};
	GLfloat ambient[] = {0.1f, 0.1f, 0.1f, 1.0f};  // 环境强度
	GLfloat diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};  // 散射强度
	GLfloat specular[] = {1.0f, 1.0f, 1.0f, 1.0f}; // 镜面强度
	glLightfv(GL_LIGHT0,GL_POSITION,light_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
static int flags = 1;
	if(flags){
//    model1 = new load3DS("/opt/git_Atom/atom_AR/Pi/pi_AR/Cmodel3ds/cube3.3DS");
//    model1 = new load3DS("/opt/git_Atom/atom_AR/Pi/pi_AR/Cmodel3ds/dragon1.3DS");
//	model1 = new load3DS("/opt/git_Atom/atom_AR/Pi/pi_AR/Cmodel3ds/kaer1.3DS");
//    model1 = new load3DS("/opt/git_Atom/atom_AR/Pi/pi_AR/Cmodel3ds/dapigu.3DS");
//    model1 = new load3DS("/opt/git_Atom/atom_AR/Pi/pi_AR/Cmodel3ds/dragon2.3DS");


	modelObj = new ObjLoader();
//	modelObj->loadFromFile("/opt/model/kaer/kaer1.obj","/opt/model/kaer/kaer1.mtl");
    modelObj->loadFromFile(OPEN_OBJ,OPEN_MTL);
//	modelObj->loadFromFile("/opt/model/cube/cubedota.obj","/opt/model/cube/cubedota.mtl");
    flags = 0;
	}
}

void myGL::resizeGL(int width, int height)
{
	qDebug() << "at resizeGL";
	if (0 == height) {
		height = 1;
	}

//	intrinsicMatrix2ProjectionMatrix(cameraMatrix, 640, 480, 0.01f, 100.0f);
	intrinsicMatrix2ProjectionMatrix(cameraMatrix, width, height, 0.01f, 100.0f);

	glViewport(0, 0, (GLint)width, (GLint)height);//重置当前的物体显示位置
	glLoadIdentity();
//	glMatrixMode(GL_PROJECTION);//选择矩阵模式
//	glMultMatrixf(projection_matrix);//导入相机内部参数模型
//	glLoadMatrixf(projection_matrix);



//	glEnableClientState(GL_VERTEX_ARRAY);  //启用客户端的某项功能
//	glEnableClientState(GL_NORMAL_ARRAY);
//	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
}
void myGL::initWidget()
{
//	setGeometry(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);//设置窗口位置及大小
//	setWindowTitle(tr("opengl demo"));
}

void myGL::updateWindow(){
	//get camera data
	cam >> srcImage;

//	srcImage.release(); srcImage = cv::imread("/opt/git_Atom/atom_AR/img/img_8_12/arok2.jpg");
//	srcImage.release(); srcImage = cv::imread("/opt/git_Atom/atom_AR/img/img_old/ar5.jpg");
//	qDebug() << srcImage.channels() <<srcImage.cols << srcImage.rows;
//	cv::imshow("1",srcImage);
//	cv::waitKey();

//	cv::cvtColor(srcImage,srcImage,CV_BGR2RGB); //opencv to opengl format

	imageProcess(srcImage);

	//
//	GLint max;
//	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
//	qDebug() <<  "max texture value:" << max;
	//

//	QImage buf, tex;
	//将Mat类型转换成QImage
//	buf = QImage((const unsigned char*)srcImage.data, srcImage.cols, srcImage.rows, srcImage.cols * srcImage.channels(), QImage::Format_RGB888);
//	tex = QGLWidget::convertToGLFormat(buf);
//	glDrawPixels(tex.width(),tex.height(),GL_BGR_EXT,GL_UNSIGNED_BYTE,tex.bits());
	cv::Mat backImage;
	cv::cvtColor(srcImage,backImage,CV_RGB2BGR);
	glEnable(GL_TEXTURE_2D);//启用纹理
	glGenTextures(1, &textureBack);//对应图片的纹理定义
	glBindTexture(GL_TEXTURE_2D, textureBack);//进行纹理绑定
	//纹理创建
    glTexImage2D(GL_TEXTURE_2D, 0, 3, srcImage.cols, srcImage.rows, 0,
        GL_RGB, GL_UNSIGNED_BYTE, backImage.data);
//	glTexImage2D(GL_TEXTURE_2D, 0, 4, tex.width(), tex.height(), 0,
//		GL_RGBA, GL_UNSIGNED_BYTE, tex.bits());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	modelObj->m_mtls[0]->texture = textureBack;
	updateGL();
}

void myGL::imageProcess(cv::Mat srcImage){
	//m_corners_3d = vector<cv::Point3f>(corners_3d, corners_3d + 4);
//	m_camera_matrix = cv::Mat(3, 3, CV_32FC1, camera_matrix).clone();
//	m_dist_coeff = cv::Mat(1, 4, CV_32FC1, dist_coeff).clone();

	Decode decode(srcImage,90,30);
	decode.markerDetect(possible_markers);
	decode.markerRecognize(possible_markers,final_markers);
}



void myGL::paintGL(){
	qDebug() << "paintGL !!!";

	static int HelpTextFlag = 1;
	if(HelpTextFlag){
		Helptexture();
		HelpTextFlag = 0;
	}
//    cv::imshow("2",srcImage);
//	cv::waitKey();

//		glPushMatrix();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		glLoadIdentity();//重置坐标系至屏幕中央 上+下- 左-右+ 里-外+

		GLfloat w,h;
		w=640;h=480;
		const GLfloat bgTextureVertices[] = { 0, 0, w, 0, 0, h, w, h };
		const GLfloat bgTextureCoords[] = { 1, 0, 1, 1, 0, 0, 0, 1 };
		static const GLfloat proj[] = {
			0,-2.0f/640,0,0,
			-2.0f/480,0,0,0,
			0,0,1,0,
			1,1,0,1
		};
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glLoadMatrixf(proj);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();




	#if 1//绑定纹理
		qDebug() << "texture Back number ->" << textureBack;

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureBack);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glVertexPointer(2, GL_FLOAT, 0, bgTextureVertices);
		glTexCoordPointer(2, GL_FLOAT, 0, bgTextureCoords);
		glColor4f(1,1,1,1);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

//		  glDisableClientState(GL_VERTEX_ARRAY);
//		  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//		glDisable(GL_TEXTURE_2D);

	#endif
	//		glPopMatrix();

//		printMatrix(GL_MODELVIEW/*,"123"*/);

//draw AR
	glMatrixMode(GL_PROJECTION);//选择矩阵模式
	glLoadIdentity();
	glLoadMatrixf(projection_matrix);//导入相机内部参数模型

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
//	GLfloat modelview[16];
//	glGetFloatv(GL_MODELVIEW_MATRIX, modelview);


	glPushMatrix();
	glLineWidth(3.f);


	float lineX[] = {0,0,0,1,0,0};
	float lineY[] = {0,0,0,0,1,0};
	float lineZ[] = {0,0,0,0,0,2};
	cv::Mat r, t;
	int i,markers_num = final_markers.size();
	for (i = 0;i < markers_num;i++){
		final_markers[i].estimateTransformToCamera(Xworld, cameraMatrix, distCoeffs, r, t);
		extrinsicMatrix2ModelViewMatrix(r, t);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(model_view_matrix);

		//draw ------------------------------------
		float scale = 0.5;
		glScalef(scale,scale,scale);
		glRotatef(90.0*final_markers[i].nRotations,0,0,1);

        glColor4f(1.0f,0.0f,0.0f,1.0f);
        glVertexPointer(3,GL_FLOAT,0,lineX);
        glDrawArrays(GL_LINES,0,2);

        glColor4f(0.0f,1.0f,0.0f,1.0f);
        glVertexPointer(3,GL_FLOAT,0,lineY);
        glDrawArrays(GL_LINES,0,2);


        glColor4f(0.0f,0.0f,1.0f,1.0f);
        glVertexPointer(3,GL_FLOAT,0,lineZ);
        glDrawArrays(GL_LINES,0,2);

        cube();
        drawWire();
        glTranslatef(0.0,0.0,-0.5);
        model1->draw(GL_TRIANGLES);
        model1->draw();

		glColor4f(0.3f,0.3f,0.3f,1.0f);




#if 1 //kaer
		//obj molde {kaer
		glScalef(0.02,0.02,0.02);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glRotatef(90.0,1.0,0.0,0.0);
		modelObj->renderObj();
		glDisable(GL_DEPTH_TEST);
		//	obj molde }
#endif

#if 0 //cube test


		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glBindTexture(GL_TEXTURE_2D,modelObj->m_mtls[0]->texture );
//		glBindTexture(GL_TEXTURE_2D,textureBack );
		qDebug() << "texture = " <<  modelObj->m_mtls[0]->texture;
		glBegin(GL_QUADS);
			glNormal3f(0.0, 0.0, 1.0);
			glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, 1.0);
			glTexCoord2f(1.0, 0.0); glVertex3f(1.0, -1.0, 1.0);
			glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, 1.0);
			glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, 1.0, 1.0);

			glNormal3f(0.0, 0.0, -1.0);
			glTexCoord2f(1.0, 0.0); glVertex3f(-1.0, -1.0, -1.0);
			glTexCoord2f(1.0, 1.0); glVertex3f(-1.0, 1.0, -1.0);
			glTexCoord2f(0.0, 1.0); glVertex3f(1.0, 1.0, -1.0);
			glTexCoord2f(0.0, 0.0); glVertex3f(1.0, -1.0, -1.0);

			glNormal3f(0.0, 1.0, 0.0);
			glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, 1.0, -1.0);
			glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, 1.0, 1.0);
			glTexCoord2f(1.0, 0.0); glVertex3f(1.0, 1.0, 1.0);
			glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, -1.0);

			glNormal3f(0.0, -1.0, 0.0);
			glTexCoord2f(1.0, 1.0); glVertex3f(-1.0, -1.0, -1.0);
			glTexCoord2f(0.0, 1.0); glVertex3f(1.0, -1.0, -1.0);
			glTexCoord2f(0.0, 0.0); glVertex3f(1.0, -1.0, 1.0);
			glTexCoord2f(1.0, 0.0); glVertex3f(-1.0, -1.0, 1.0);

			glNormal3f(1.0, 0.0, 0.0);
			glTexCoord2f(1.0, 0.0); glVertex3f(1.0, -1.0, -1.0);
			glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, -1.0);
			glTexCoord2f(0.0, 1.0); glVertex3f(1.0, 1.0, 1.0);
			glTexCoord2f(0.0, 0.0); glVertex3f(1.0, -1.0, 1.0);

			glNormal3f(-1.0, 0.0, 0.0);
			glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, -1.0);
			glTexCoord2f(1.0, 0.0); glVertex3f(-1.0, -1.0, 1.0);
			glTexCoord2f(1.0, 1.0); glVertex3f(-1.0, 1.0, 1.0);
			glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, 1.0, -1.0);
		glEnd();//glBegin(GL_QUADS);
		glDisable(GL_DEPTH_TEST);
#endif

		//end draw ---------------------------------
	}
//	glGetFloatv(GL_MODELVIEW_MATRIX, modelview);


glPopMatrix();

//relase
possible_markers.clear();
final_markers.clear();


//cv::waitKey();//test
}

void myGL::loadGLTextures(){

}

void myGL::intrinsicMatrix2ProjectionMatrix(cv::Mat& camera_matrix, float width, float height, float near_plane, float far_plane)
{
	float f_x = camera_matrix.at<float>(0,0);
	float f_y = camera_matrix.at<float>(1,1);

	float c_x = camera_matrix.at<float>(0,2);
	float c_y = camera_matrix.at<float>(1,2);

	projection_matrix[0] = 2*f_x/width;
	projection_matrix[1] = 0.0f;
	projection_matrix[2] = 0.0f;
	projection_matrix[3] = 0.0f;

	projection_matrix[4] = 0.0f;
	projection_matrix[5] = 2*f_y/height;
	projection_matrix[6] = 0.0f;
	projection_matrix[7] = 0.0f;

	projection_matrix[8] = 1.0f - 2*c_x/width;
	projection_matrix[9] = 2*c_y/height - 1.0f;
	projection_matrix[10] = -(far_plane + near_plane)/(far_plane - near_plane);
	projection_matrix[11] = -1.0f;

	projection_matrix[12] = 0.0f;
	projection_matrix[13] = 0.0f;
	projection_matrix[14] = -2.0f*far_plane*near_plane/(far_plane - near_plane);
	projection_matrix[15] = 0.0f;
}

void myGL::extrinsicMatrix2ModelViewMatrix(cv::Mat& rotation, cv::Mat& translation)
{
	//绕X轴旋转180度，从OpenCV坐标系变换为OpenGL坐标系
#if 1
	static double d[] =
	{
		1, 0, 0,
		0, -1, 0,
		0, 0, -1
	};
#endif
#if 0
	static double d[] =
	{
		1, 0, 0,
		0, 1, 0,
		0, 0, 1
	};
#endif
#if 0
	static double d[] =
	{
		1, 0, 0,
		0, -1, 0,
		0, 0, 1
	};
#endif

	cv::Mat_<double> rx(3, 3, d);

	rotation = rx*rotation;
	translation = rx*translation;

	model_view_matrix[0] = rotation.at<double>(0,0);
	model_view_matrix[1] = rotation.at<double>(1,0);
	model_view_matrix[2] = rotation.at<double>(2,0);
	model_view_matrix[3] = 0.0f;

	model_view_matrix[4] = rotation.at<double>(0,1);
	model_view_matrix[5] = rotation.at<double>(1,1);
	model_view_matrix[6] = rotation.at<double>(2,1);
	model_view_matrix[7] = 0.0f;

	model_view_matrix[8] = rotation.at<double>(0,2);
	model_view_matrix[9] = rotation.at<double>(1,2);
	model_view_matrix[10] = rotation.at<double>(2,2);
	model_view_matrix[11] = 0.0f;

	model_view_matrix[12] = translation.at<double>(0, 0);
	model_view_matrix[13] = translation.at<double>(1, 0);
	model_view_matrix[14] = translation.at<double>(2, 0);
	model_view_matrix[15] = 1.0f;
}


void myGL::updateParams(int timerValue)
{
	clk.start(timerValue);
}

void myGL::printMatrix(GLenum type/*, string &message*/){
//	qDebug("%s :!!!\n",);
	double modelview[16];
	glGetDoublev(type, modelview);

	qDebug("matrix = {");
	for(int i = 0;i < 4;i++){
		qDebug("%lf, %lf, %lf, %lf,",modelview[i*4+0],modelview[i*4+1],modelview[i*4+2],modelview[i*4+3]);
	}
	qDebug("}\n");
}

void myGL::mousePressEvent(QMouseEvent *mouseEvent)
{

}

#if 0
void myGL::Load3DS(/*std::string Name*/){
    std::string Name = "/opt/git_Atom/atom_AR/Pi/pi_AR/Cmodel3ds/cube3.3DS";
	Lib3dsFile * m_model;
	Lib3dsVector * vertices;
	int m_TotalFaces = 0;
	m_model = lib3ds_file_load(Name.c_str());

	if(!m_model){
		throw strcat("Unable to load ",Name.c_str());
	}

	assert(m_model != NULL);

	Lib3dsMesh * mesh;
	// Loop through every mesh
	for(mesh = m_model->meshes;mesh != NULL;mesh = mesh->next)
	{
		// Add the number of faces this mesh has to the total faces
		m_TotalFaces += mesh->faces;
	}
	qDebug() << "m_TotalFaces =" << m_TotalFaces;

	vertices    = new Lib3dsVector[m_TotalFaces * 3];
	Lib3dsFace * face;
	unsigned int FinishedFaces = 0;
	for (mesh = m_model->meshes;mesh != NULL;mesh = mesh->next){
		for(unsigned int cur_face = 0;cur_face < mesh->faces;cur_face++){
			face = &mesh->faceL[cur_face];
			for(unsigned int i = 0;i < 3;i++  ){
				memcpy(&vertices[FinishedFaces*3 + i], mesh->pointL[face->points[ i ]].pos, sizeof(Lib3dsVector));
			}
			FinishedFaces++;
		}

	}


    for(int x = 0;x < 3*m_TotalFaces;x++){
		loadV[x*3+0] = vertices[x][0];
		loadV[x*3+1] = vertices[x][1];
		loadV[x*3+2] = vertices[x][2];

	}
//		loadV[j] = vertices[j];


}
#endif

void myGL::Helptexture(){
    qDebug() << modelObj->m_textureName;
    int textCount = modelObj->m_mtls.size();
    for (int i = 0 ;i < textCount;i++){

        GLuint tempText;
        Texture::loadTexture(modelObj->m_textureName[i],&tempText);
        modelObj->m_mtls[i]->texture = tempText;
        qDebug() << "m_mtls[i]->texture" << modelObj->m_mtls[i]->texture;
    }
}
