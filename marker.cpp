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

#include "marker.h"
#include <opencv2/opencv.hpp>

using namespace cv;

Marker::Marker()
{
	/* 
	 * 用来存储marker点的4个位置,和计算camera内参[R|T]
	 * */
	m_id = -1;
	m_corners.resize(4, cv::Point2f(0.0,0.0));
}

Marker::Marker(int _id, cv::Point2f _c0, cv::Point2f _c1, cv::Point2f _c2, cv::Point2f _c3)
{
	m_id = _id;

	m_corners.reserve(4);
	m_corners.push_back(_c0);
	m_corners.push_back(_c1);
	m_corners.push_back(_c2);
	m_corners.push_back(_c3);
}

void Marker::drawToImage(cv::Mat& image, cv::Scalar color, float thickness)
{
	circle(image, m_corners[0], thickness*2, color, thickness);
	circle(image, m_corners[1], thickness, color, thickness);
	line(image, m_corners[0], m_corners[1], color, thickness, CV_AA);
	line(image, m_corners[1], m_corners[2], color, thickness, CV_AA);
	line(image, m_corners[2], m_corners[3], color, thickness, CV_AA);
	line(image, m_corners[3], m_corners[0], color, thickness, CV_AA);

	cv::Point text_point = m_corners[0] + m_corners[2];
	text_point.x /= 2;
	text_point.y /= 2;

	std::stringstream ss;
	ss << m_id;

	putText(image, ss.str(), text_point, cv::FONT_HERSHEY_SIMPLEX, 0.5, color);
}
/* #func		: 估算camera的位置
 * #description	: 用一个固定的3维世界坐标(argument1),和标定获得的camera内参矩阵K(argument2)(3*3),
 *				  还有焦距矩阵(argument3)([1*5]),求出camera外参矩阵[R|T].  ( x = K[R|T]X)
 *
 * */
void Marker::estimateTransformToCamera(std::vector<cv::Point3f> corners_3d, cv::Mat& camera_matrix, cv::Mat& dist_coeff, cv::Mat& rmat, cv::Mat& tvec)
{
	cv::Mat rot_vec;
	/*bool res = */solvePnP(corners_3d, m_corners, camera_matrix, dist_coeff, rot_vec, tvec);
	Rodrigues(rot_vec, rmat);//旋转向量转换为旋转矩阵
}

void Marker::sort(void){
	//c0  -- -- c3
	std::vector<cv::Point2i> tmp;
	tmp.resize(0);
	std::vector<cv::Point2i> up;
	up.resize(0);
	std::vector<cv::Point2i> down;
	down.resize(0);

	/*for (int i = 0;i < (int)m_corners.size();i++){*/
		//printf("*****\tm_corners[%d] = (%f,%f)\n",i,m_corners[i].x,m_corners[i].y);
	/*}*/

	int min1 = 1000;
	int min2 = 1000;
	unsigned int p1 = 0;
	unsigned int p2 = 0;
	unsigned int p3 = 0;
	unsigned int p4 = 0;
	for(unsigned int i = 0;i < m_corners.size();i++){
		if(m_corners[i].y < min1){
			min1 = m_corners[i].y;
			p1 = i;
		}
	}
	for(unsigned int i = 0;i < m_corners.size();i++){
		if(i == p1)
			continue;
		if(m_corners[i].y < min2){
			min2 = m_corners[i].y;
			p2 = i;
		}
	}
	printf("min1 = %d.min2 = %d\n",min1,min2);

	// -----^  y
	if(m_corners[p1].x < m_corners[p2].x){
			printf("what?\n");
		up.push_back(m_corners[p1]);
		up.push_back(m_corners[p2]);
	}
	else if(m_corners[p1].x > m_corners[p2].x){
		up.push_back(m_corners[p2]);
		up.push_back(m_corners[p1]);
	}
	printf("up[0] = (%d,%d)\n",up[0].x,up[0].y);
	printf("up[1] = (%d,%d)\n",up[1].x,up[1].y);
	//---^   y
	// c1 ---- c2
	p3 = 0;
	p4 = 0;
	for(;(p3 == p1) || (p3 == p2);p3++){
	}
	for(;(p4 == p1) || (p4 == p2) || (p4 == p3);p4++){
	}

	if(m_corners[p3].x < m_corners[p4].x){
		down.push_back(m_corners[p3]);
		down.push_back(m_corners[p4]);
	}
	else if(m_corners[p3].x > m_corners[p4].x){
		down.push_back(m_corners[p4]);
		down.push_back(m_corners[p3]);
	}
	//swap(m_corners[0],up[0]);
	m_corners[0] = up[0];
	//swap(m_corners[1],down[0]);
	m_corners[1] = down[0];
	//swap(m_corners[2],down[1]);
	m_corners[2] = down[1];
	//swap(m_corners[0],up[1]);
	m_corners[3] = up[1];
}

void Marker::printPoint(cv::Mat & image,int mode){
	if(0 == mode)
		for(unsigned int i = 0;i < m_corners.size();i++){
		char  buf[15];
		sprintf(buf,"(%d,%d)",(int)m_corners[i].x,(int)m_corners[i].y);
		std::string s = buf;
		cv::putText(image,s,m_corners[i],cv::FONT_HERSHEY_PLAIN,0.5,cv::Scalar(100,100,255));
	}
	else{
		for(unsigned int i = 0;i < m_corners.size();i++){
		printf("point (%d,%d) ",(int)m_corners[i].x,(int)m_corners[i].y);
		}
	}
}

