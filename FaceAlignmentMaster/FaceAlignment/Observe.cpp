//
//  Observe.cpp
//
//  Created by Culiner on 11/16/2015.
//  Copyright (c) 2015 Culiner. All rights reserved.
//

#include  "Observe.h"
using namespace std;
using namespace cv;

namespace Observe
{
	string intTostring(int o)
	{
		char tempChar[10];
		_itoa_s(o,tempChar,10);
		string tempString=tempChar;
		return tempString;
	}

	string Mkdir(string path,int count)
	{
		string tempString=intTostring(count);
		system(("md "+path+tempString).c_str());
		return tempString;
	}

	string Mkdir(string path, int count, int stage)
	{
		string tempCount=intTostring(count);
		for(int i=0;i<stage;i++)
		{
			string tempStage=intTostring(i);
			system(("md "+path+tempCount+"\\Stage"+tempStage).c_str());
		}
		return tempCount;
	}

	void TextLog(string path,string content)
	{
		ofstream outLog;
		outLog.open(path,ios::app);
		outLog<<content;
		outLog.close();
	}

	void PtsLog(string path,const Mat_<double> &shapes)
	{
		ofstream outLog;
		outLog.open(path,ios::app);
		outLog<<"version: 1\t\nn_points:  68\t\n{\t\n";
		for( int i = 0; i < shapes.rows; ++i){ 
			outLog<<shapes(i,0)<<" "<<shapes(i,1)<<"\t\n";
		 }
		outLog<<"}";
		outLog.close();
	}

	void BoxLog(string path,const BoundingBox &box)
	{
		ofstream outLog;
		outLog.open(path,ios::app);
		outLog<<box.start_x<<" "<<box.start_y<<" "<<box.height<<" "<<box.width<<" "<<box.centroid_x<<" "<<box.centroid_y<<"\t\n";
		outLog.close();
	}

	Mat_<uchar> DrawImg(const Mat_<uchar> &img,const Mat_<double> &shapes,Scalar color)
	{
		Mat_<uchar> tempMat=img.clone();
		return DrawImgOnSelf(tempMat,shapes,color);
	}

	Mat_<uchar> DrawImgOnSelf(Mat_<uchar> &img,const Mat_<double> &shapes,Scalar color)
	{
		for(int k = 0;k < global_params.landmark_num;k++){
			circle(img,Point2d(shapes(k,0),shapes(k,1)),3,color,-1,8,0);
		}
		return img;
	}

	Mat_<uchar> DrawImg(const Mat_<uchar> &img,const BoundingBox &box,Scalar color)
	{
		Mat_<uchar> tempMat=img.clone();
		return DrawImgOnSelf(tempMat,box,color);
	}

	Mat_<uchar> DrawImgOnSelf(Mat_<uchar> &img,const BoundingBox &box,Scalar color)
	{
		rectangle(img,Point2d(box.start_x,box.start_y),Point2d(box.start_x+box.width,box.start_y+box.height),color); 
		return img;
	}

	void SaveImg(string path,const Mat_<uchar> &img)
	{
		imwrite(path,img);
	}

	Logs::Logs(int samples,int stages,int landmarks)
	{
		totalSample=samples;
		totalStage=stages;
		totalLandmark=landmarks;
		datas=new double *[samples];
		for(int i=0;i<samples;i++)
		{
			datas[i]=new double [stages];
			for(int j=0;j<stages;j++)
			{
				datas[i][j]=-1;
			}
		}
	}

	void Logs::AddLogs(int id, int stage, const Mat_<double>& currentShapes, const Mat_<double>& truthShapes)
	{
		datas[id][stage]=CalculateError(truthShapes,currentShapes);
	}

	void Logs::SaveLogs()
	{
		ofstream outLog;
		outLog.open("Observe\\statistics.txt",ios::app);
		outLog<<"Bug Log:\t\n";
		Statistics *statistics=new Statistics [totalStage];
		for (int stage=0;stage<totalStage;stage++)
		{
			for (int id=0;id<totalSample;id++)
			{
				statistics[stage].Total+=datas[id][stage];
				if(datas[id][stage]>statistics[stage].Max)
				{
					statistics[stage].Max=datas[id][stage];
					statistics[stage].MaxId=stage;
				}
				if(datas[id][stage]<statistics[stage].Min)
				{
					statistics[stage].Min=datas[id][stage];
					statistics[stage].MinId=stage;
				}
				if(stage>0)
				{
					double change=datas[id][stage]-datas[id][stage-1];
					if(change>0)
					{
						string idString=intTostring(id);
						string stageString=intTostring(stage);
						outLog<<"no "+idString+" in stage "+stageString<<" from "<<datas[id][stage-1]<<" to "<<datas[id][stage]<<" change "<<change<<"\t\n";
					}
					else
					{
						if(change>statistics[stage].MaxChange)
						{
							statistics[stage].MaxChange=change;
							statistics[stage].MaxChangeId=id;
							statistics[stage].MaxChangeStage=stage;
						}
						if(change<statistics[stage].Min)
						{
							statistics[stage].MinChange=change;
							statistics[stage].MinChangeId=id;
							statistics[stage].MinChangeStage=stage;
						}
					}
				}
			}
			statistics[stage].Avg=statistics[stage].Total/totalSample;
			for (int id=0;id<totalSample;id++)
			{
				statistics[stage].S+=(datas[id][stage]-statistics[stage].Avg)*(datas[id][stage]-statistics[stage].Avg);
			}
		}

		for (int stage=0;stage<totalStage;stage++)
		{
			outLog<<"\t\nStage "<<stage<<"\t\n";
			outLog<<"Avg "<<statistics[stage].Avg<<"\t\n";
			outLog<<"Max "<<statistics[stage].Max<<"(in "<<statistics[stage].MaxId<<")"<<"\t\n";
			outLog<<"Min "<<statistics[stage].Min<<"(in "<<statistics[stage].MinId<<")"<<"\t\n";
			outLog<<"MaxChange "<<statistics[stage].MaxChange<<"(in "<<statistics[stage].MaxChangeId<<" stage "<<statistics[stage].MaxChangeStage<<")"<<"\t\n";
			outLog<<"MinChange "<<statistics[stage].MinChange<<"(in "<<statistics[stage].MinChangeId<<" stage "<<statistics[stage].MinChangeStage<<")"<<"\t\n";
			outLog<<"Variance ·½²î "<<statistics[stage].S<<"\t\n\t\n";
		}
		outLog.close();

		outLog.open("Observe\\detail.txt",ios::app);
		for (int i=0;i<totalSample;i++)
		{
			for (int j=0;j<totalStage;j++)
			{
				outLog<<datas[i][j]<<"\t";
			}
			outLog<<"\t\n";
		}
		outLog.close();
	}

	Statistics::Statistics()
	{
		Total=0;
		Avg=0;
		Max=-65536;
		Min=65536;
		MaxChange=-65536;
		MinChange=65536;
		S=0;
		MaxId=-1;
		MinId=-1;
		MaxChangeId=-1;
		MaxChangeStage=-1;
		MinChangeId=-1;
		MinChangeStage=-1;
	}
}