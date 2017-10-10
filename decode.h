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

#ifndef DECODE_H
#define DECODE_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <QDebug>

#include "marker.h"

#define MIN_SIZE 90
#define MIN_SIDE_LENGTH 30

#define MARKER_CELL_SIZE 10
#define MARKER_SIZE (7*MARKER_CELL_SIZE)

#define APPROX_POLY_EPS (0.08)

class Decode
{
	/*
	 * 在图片中找到可能的marker码,
	 * */
public:
	Decode(cv::Mat & src,unsigned int min_size = MIN_SIZE,unsigned int min_side_length = MIN_SIDE_LENGTH);

	/*从不规则的投影中找到二维码*/
private:
	cv::Mat src;	//原图
	cv::Mat dst;	//处理过的图片
	std::vector<cv::Point2f> m_marker_coords; //透视变换后的点集,用来读出码中的数据
	std::vector<Marker> m_markers;   //暂时没用
	unsigned int min_size;   //最小轮廓
	unsigned int min_side_length;  //最小矩形边长
public:
	void markerDetect(std::vector<Marker> & possible_marker);
	void markerRecognize(std::vector<Marker>& possible_markers,std::vector<Marker>& final_markers);
	int hammDistMarker(cv::Mat bits);
};

#endif // DECODE_H
