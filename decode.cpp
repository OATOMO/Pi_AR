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

#include "decode.h"

template<typename T>
inline T min(T a,T b){
	return (a < b ? a : b);
}

cv::Mat rotate(cv::Mat in);

/* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 * structuer func */
Decode::Decode(cv::Mat & src,unsigned int min_size,unsigned int min_side_length){
	this->src = src;
	this->min_size = min_size;
	this->min_side_length = min_side_length;
	m_marker_coords.push_back(cv::Point2f(0,0));
	m_marker_coords.push_back(cv::Point2f(0,MARKER_SIZE-1));
	m_marker_coords.push_back(cv::Point2f(MARKER_SIZE-1,MARKER_SIZE-1));
	m_marker_coords.push_back(cv::Point2f(MARKER_SIZE-1,0));
}//end Marker class





/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 *  func:找到可能是的码放入到possible_marker的向量中
 * 
 * 多边形近似条件:
 *	 1.只有4个顶点
 *	 2.一定是凸多边形
 *	 3.每一个边长度不能过小
 * */
void Decode::markerDetect(std::vector<Marker> & possible_marker){
//	qDebug() << "initing possible_Marker.size = "<< possible_marker.size();//test

	/*"AR码检测"*/
	cv::cvtColor(src,dst,cv::COLOR_BGR2GRAY);
	cv::threshold(dst,dst,100,255,cv::THRESH_BINARY_INV);
//	cv::imwrite("/home/atom/Desktop/proc/th.png",dst);
#if 0
	cv::blur(dst,dst,cv::Size(3,3));//模糊,去除毛刺
//	cv::imwrite("/home/atom/Desktop/proc/blur.png",dst);
#endif
	//形态学开运算
	cv::Mat element = cv::getStructuringElement(
					cv::MORPH_RECT, cv::Size(3, 3));
	cv::morphologyEx(dst,dst,cv::MORPH_OPEN,element);
//	cv::imshow("mor",dst);//test
//	cv::imwrite("/home/atom/Desktop/proc/morph_open.png",dst);

	//轮廓
	std::vector<std::vector<cv::Point> > all_contours;
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> all_hierarchy;
	std::vector<cv::Vec4i> hierarchy;
//	cv::findContours(dst,all_contours,hierarchy,cv::RETR_TREE,cv::CHAIN_APPROX_NONE,cv::Point(0,0));
	cv::findContours(dst, all_contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

//	cv::drawContours(src,all_contours,-1,cv::Scalar(0,0,255),1,8);

//	qDebug("contours count = all_contours.size() -> %ld\n",all_contours.size());
	for (unsigned int i = 0;i < all_contours.size();i++){
		if(all_contours[i].size() > min_size){
//			qDebug("\tcontors size = all_contours[%d] = %ld\n",i,all_contours[i].size());
			contours.push_back(all_contours[i]);
			//hierarchy.push_back(all_hierarchy[i]);
		}
	}

//	qDebug("contours size > %d contours count = contours.size() = %ld \n",min_size,contours.size());

	//4边形逼近
	std::vector<cv::Point> approx_poly;//polygon
	for (unsigned int i  = 0;i < contours.size();i++){    //遍历大小足够的每一个轮廓
		double eps = contours[i].size() * APPROX_POLY_EPS;
		approxPolyDP(contours[i], approx_poly, eps, true); //矩形拟合

		if(approx_poly.size() != 4)
			continue; //只要4边型

//		qDebug("\tapprox_poly.size() = %ld\n",approx_poly.size());

		if(!cv::isContourConvex(approx_poly))
			continue; //只要凸多边形

		//确保两连点之间的距离足够大
		float min_side = FLT_MAX;
		for (int j = 0;j < 4;j++){
			cv::Point side = approx_poly[j] - approx_poly[(j+1)%4];
			min_side = min((float)min_side,(float)side.dot(side));
		}
		if (min_side < min_side_length * min_side_length)  //求出的最小边长不能小于min_side_length
			continue;

		Marker marker = Marker(0,approx_poly[0],approx_poly[1],approx_poly[2],approx_poly[3]);
		cv::Point2f v1 = marker.m_corners[1] - marker.m_corners[0];
		cv::Point2f v2 = marker.m_corners[2] - marker.m_corners[0];
		if(v1.cross(v2) > 0){
			swap(marker.m_corners[1],marker.m_corners[3]);
		}
//		marker.sort();
#if 0
		//判断是否平行
		Parallel_rect Para(marker.m_corners[0],marker.m_corners[1],marker.m_corners[2],marker.m_corners[3]);
		if(!Para.isParallel())
			continue;
#endif
		possible_marker.push_back(marker);
		//cv::line(src,marker.m_corners[0],marker.m_corners[1],cv::Scalar(0,100,255));
		//cv::line(src,marker.m_corners[1],marker.m_corners[2],cv::Scalar(0,100,255));
		//cv::line(src,marker.m_corners[2],marker.m_corners[3],cv::Scalar(0,100,255));
		//cv::line(src,marker.m_corners[3],marker.m_corners[0],cv::Scalar(0,100,255));
		//marker.printPoint(src);
	}//end 遍历
	//cv::imshow("maker",src);
	//cv::waitKey();
}

/*
 * @possible_marker :可能是AR码的点集
 * @final_marker    :确定是AR码的点集
 * */
void Decode::markerRecognize(std::vector<Marker>& possible_markers, std::vector<Marker>& final_markers){
	final_markers.clear();

	cv::Mat rotations[4];  //hamm检测时每次旋转后的矩阵5*5   
	int distances[4];	   //4个方向的hamm距离
	std::pair<int,int> minDist(INT_MAX,0); //hamm的信息,<距离,旋转次数>
	int hammID = 0;			//marker ID;

	//cv::flip(img_gray,img_gray,0);
	cv::Mat img_gray = this->src;
	if(img_gray.channels() != 1){
		cv::cvtColor(img_gray,img_gray,cv::COLOR_BGR2GRAY);
	}


	cv::Mat marker_image;  //存放提取出来的单个MARKER码
	cv::Mat bit_matrix(5,5,CV_8UC1); //用来存放AR码中的信息

//	cv::imshow("img_gray",img_gray);
//	cv::waitKey();

//	qDebug("Recognize Marker count = possible_markers.size() = %d\n ",(int)possible_markers.size());
	for (int i = 0;i < (int)possible_markers.size();i++){
		//透视变换
		//先通过(变换前的坐标点和 变换后的坐标点)来得到一个变换矩阵
		cv::Mat M = cv::getPerspectiveTransform(possible_markers[i].m_corners,m_marker_coords);
		//透视变换 @1:输入图片 @2:输出图片 @3:变换矩阵 @4:输出图片大小
		cv::warpPerspective(img_gray,marker_image,M,cv::Size(MARKER_SIZE,MARKER_SIZE));

		//显示单个Marker码
		//cv::imshow("marker_image",marker_image);
		//cv::waitKey();

		cv::threshold(marker_image,marker_image,125,255,cv::THRESH_BINARY | cv::THRESH_OTSU);

		//Dbug("marker_image (%d,%d)",marker_image.cols,marker_image.rows);

		//A marker must has a whole black border.一个MARKER必须有完整的黑色边界
		for (int y = 0;y < 7;y++){ //因为AR码是7x7
			int inc = ((y == 0 || y == 6) ? 1 : 6);  //因为第一行和第7行有7各黑色块
			int cell_y = y*MARKER_CELL_SIZE;  //黑块的起始y
			for (int x = 0;x < 7;x+=inc){
				int cell_x = x*MARKER_CELL_SIZE; //黑块的起始x
				int none_zero_count = cv::countNonZero(marker_image(cv::Rect(cell_x,cell_y,MARKER_CELL_SIZE,MARKER_CELL_SIZE)));
				if(none_zero_count > MARKER_CELL_SIZE*MARKER_CELL_SIZE/2)
						goto __wrongMarker;
			}
		}//end AR边界确认


		//Decode the Marker 解码,读取1010值
		for (int y = 0;y < 5;y++){
			int cell_y = (y+1)*MARKER_CELL_SIZE;
			for(int x = 0;x < 5;x++){ //AR码数据仅为5x5
				int cell_x = (x+1) * MARKER_CELL_SIZE;
				int none_zero_count = cv::countNonZero(marker_image(cv::Rect(cell_x,cell_y,MARKER_CELL_SIZE,MARKER_CELL_SIZE)));
				if(none_zero_count > MARKER_CELL_SIZE*MARKER_CELL_SIZE/2)
						bit_matrix.at<uchar>(y,x) = 1;
				else
						bit_matrix.at<uchar>(y,x) = 0;
			}
		}

	//检测4个反向的hamm距离,为0才能通过
	minDist.first = INT_MAX;    

    rotations[0] = bit_matrix;  
	distances[0] = hammDistMarker(rotations[0]);
	minDist.first = distances[0];
	minDist.second = 0;
			    
	for (int j=1; j<4; j++){
		rotations[j] = rotate(rotations[j-1]);
		distances[j] = hammDistMarker(rotations[j]);

		if (distances[j] < minDist.first){
			minDist.first  = distances[j];
			minDist.second = j;
		}
	}

	if (minDist.first != 0){
		//没通过的话下一个  (还有谁....)
		goto __wrongMarker;
	}
	possible_markers[i].nRotations = minDist.second;//hamm距离为0的旋转次数推入
	//给通过的MARKER码一个ID
	hammID = 0;
	for(int bitnum = 1;bitnum < 25;bitnum+=2){
		hammID |= rotations[minDist.second].at<uchar>(bitnum/5,bitnum%5) & 0x01;
		hammID  = hammID<< 1;
	}

	possible_markers[i].m_id = hammID;

	qDebug() << "ID = " << hammID;
	qDebug() << "nRotations" << possible_markers[i].nRotations;

	
	//possible_markers.at(i).m_id = i;//给通过的MARKER码一个ID

//		cv::imshow("threshold",marker_image);//test

		//通过之后把点集推入final
	//    std::rotate(final_markers[i].m_corners.begin(), final_markers[i].m_corners.begin() + rotation_idx, final_markers[i].m_corners.end());
		final_markers.push_back(possible_markers[i]);

		//possible_markers.at(i).drawToImage(src,cv::Scalar(0,100,255),2);//显示Marker边框


//		for(int i = 0;i < 25;i++)
//		Dbug("%d",bit_matrix.data[i]);
//		Dbug("\n");
		continue;
	__wrongMarker:
//		printf("pass\n");
		continue;
	}//end for
//	qDebug("Pass completely count = %d\n!!!!",final_markers.size());
//	if(final_markers.size() > 2){//
//		final_markers[0].printPoint(dst,1);
//		final_markers[1].printPoint(dst,1);
//	}

	// Refine marker corners using sub pixel accuracy********************
		if (final_markers.size() > 0)
		{
			std::vector<cv::Point2f> preciseCorners(4 *final_markers.size());

			for (size_t i=0; i<final_markers.size(); i++)
			{
				const Marker& marker = final_markers[i];

				for (int c = 0; c <4; c++)
				{
					preciseCorners[i*4 + c] = marker.m_corners[c];
				}
			}

			cv::TermCriteria termCriteria = cv::TermCriteria(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 30, 0.01);
			cv::cornerSubPix(dst, preciseCorners, cvSize(5,5), cvSize(-1,-1), termCriteria);

			// Copy refined corners position back to markers
			for (size_t i=0; i<final_markers.size(); i++)
			{
				Marker& marker =final_markers[i];

				for (int c=0;c<4;c++)
				{
					marker.m_corners[c] = preciseCorners[i*4 + c];
				}
			}
	}//end sub pixel accuracy*******************************************

//		cv::imshow("maker",src);
//		cv::waitKey();
}

//用来计算hamm距离
int Decode::hammDistMarker(cv::Mat bits){
	int ids[4][5]={
		{1,0,0,0,0},
		{1,0,1,1,1},
		{0,1,0,0,1},
		{0,1,1,1,0}
	};

	int dist=0;

	for (int y=0;y<5;y++){
		int minSum=1e5; //hamming distance to each possible word

		for (int p=0;p<4;p++){
			int sum=0;
			//now, count
			for (int x=0;x<5;x++){
				sum += bits.at<uchar>(y,x) == ids[p][x] ? 0 : 1;
			}

			if (minSum>sum)
			minSum=sum;
		}//end for (int p=0;p<4;p++)

		//do the and
		dist += minSum;
	}//end for (int y=0;y<5;y++)

return dist;
}//end hammDistMarker


//hamm检测时旋转一个方向(顺时针)(5*5)
cv::Mat rotate(cv::Mat in)
{
  cv::Mat out;
  in.copyTo(out);
  for (int i=0;i<in.rows;i++)
  {
	for (int j=0;j<in.cols;j++)
	{
	  out.at<uchar>(i,j)=in.at<uchar>(in.cols-j-1,i);
	}
  }
  return out;
}
