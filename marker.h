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

#ifndef MARKER_H
#define MARKER_H
#include <vector>
#include <opencv2/opencv.hpp>
#include <iostream>

/*	c0------c3
 *	|		|
 *  |       |
 *  |       |
 *  c1------c2  (c -> corners)
 * */

#define __DEBUG

#ifdef __DEBUG
#define Dbug(format,...) printf(format,##__VA_ARGS__)
#else
#define Dbug(format,...)
#endif


class Marker
{
public:
	int m_id;
	int nRotations;
	std::vector<cv::Point2f> m_corners;

public:
	Marker();
	Marker(int _id,cv::Point2f _c0,cv::Point2f _c1,cv::Point2f _c2,cv::Point2f _c3);
	void sort(void);
	void printPoint(cv::Mat & image,int mode = 0);
	void estimateTransformToCamera(std::vector<cv::Point3f> corners_3d,cv::Mat & camera_matrix,cv::Mat & dist_coeff,cv::Mat & rmat,cv::Mat & tvec);
	void drawToImage(cv::Mat & image,cv::Scalar color,float thickness);
};

#endif // MARKER_H
