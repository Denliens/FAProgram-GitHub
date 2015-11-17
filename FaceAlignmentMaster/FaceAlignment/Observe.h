//
//  Observe.h
//
//  Created by Culiner on 11/16/2015.
//  Copyright (c) 2015 Culiner. All rights reserved.
//

#ifndef __myopencv__Observe__
#define __myopencv__Observe__

#include "LBF.h"
using namespace std;
using namespace cv;

class BoundingBox;
namespace Observe
{
	class Statistics
	{
	public:
		Statistics();
		double Total,Avg,Max,Min,MaxChange,MinChange,S;
		int MaxId,MinId,MaxChangeId,MaxChangeStage,MinChangeId,MinChangeStage;
	};

	class Logs
	{
	public:
		Logs(int samples,int stages,int landmarks);
		void AddLogs(int id,int stage,const Mat_<double> &currentShapes,const Mat_<double> &truthShapes);
		void SaveLogs();
	private:
		double **datas;
		int totalSample;
		int totalStage;
		int totalLandmark;
	};

	string intTostring(int o);
	string Mkdir(string path,int count);
	string Mkdir(string path,int count,int stage);
	void TextLog(string path,string content);
	void PtsLog(string path,const Mat_<double> &shapes);
	void BoxLog(string path,const BoundingBox &box);
	void SaveImg(string path,const Mat_<uchar> &img);
	Mat_<uchar> DrawImg(const Mat_<uchar> &img,const Mat_<double> &shapes,Scalar color);
	Mat_<uchar> DrawImg(const Mat_<uchar> &img,const BoundingBox &box,Scalar color);
	Mat_<uchar> DrawImgOnSelf(Mat_<uchar> &img,const Mat_<double> &shapes,Scalar color);
	Mat_<uchar> DrawImgOnSelf(Mat_<uchar> &img,const BoundingBox &box,Scalar color);
}

#endif /* defined(__myopencv__Observe__) */