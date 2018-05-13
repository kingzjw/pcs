/*!
 * \file zjw_k-means.h
 *
 * \author King
 * \date 五月 2018
 * 针对三维点的k-means，参考http://www.cnblogs.com/luxiaoxun/archive/2013/05/09/3069594.html
 * 
 */
#pragma once
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <assert.h>
#include <vector>

#include "zjw_math.h"

using namespace std;

/*
sample:

int main()
{
	vector<Vec3*> data;

	data.push_back(new Vec3(0.0, 0.2, 0.4));
	data.push_back(new Vec3(0.3, 0.2, 0.4));
	data.push_back(new Vec3(0.4, 0.2, 0.4));
	data.push_back(new Vec3(0.5, 0.2, 0.4));
	data.push_back(new Vec3(5.0, 5.2, 8.4));

	data.push_back(new Vec3(6.0, 5.2, 7.4));
	data.push_back(new Vec3(4.0, 5.2, 4.4));
	data.push_back(new Vec3(10.3, 10.4, 10.5));
	data.push_back(new Vec3(10.1, 10.6, 10.7));
	data.push_back(new Vec3(11.3, 10.2, 10.9));

	const int size = 10; //Number of samples
	const int dim = 3;   //Dimension of feature
	const int cluster_num = 4; //Cluster number

	KMeans* kmeans = new KMeans(dim, cluster_num);
	int* labels = new int[size];
	kmeans->setInitMode(KMeans::InitUniform);
	kmeans->cluster(data, size, labels);

	for (int i = 0; i < size; ++i)
	{
		printf("%f, %f, %f belongs to %d cluster\n", data[i]->x, data[i]->y, data[i]->z, labels[i]);
	}

	delete[]labels;
	delete kmeans;

	system("pause");
	return 0;
}*/

class KMeans
{
private:
	//每个样本的维度
	int dimNum;
	//分组的个数
	int clusterNum;
	//最终分类的结果，只保存叶子节点的序号
	vector<vector<int>> clusterRes;
	//每个组的质点
	vector<Vec3> means;
	//质点初始化的方式
	int initMode;
	// 最大迭代次数
	int maxIterNum;	
	// 截止误差
	double thresholdValue;

	int dataSize;

public:
	enum InitMode
	{
		InitRandom,
		InitManual,
		InitUniform,
	};

	KMeans(int clusterNum = 4,int dimNum = 3);
	~KMeans();

	void setClusterNum(int i);
	void setInitMode(int i) { initMode = i; }
	void setMaxIterNum(int i) { maxIterNum = i; }
	void setEndError(double f) { thresholdValue = f; }
	//设置第i个质点的坐标.从0开始
	void setMean(int i, Vec3  pos);

	//拿到第i个质点的位置
	Vec3 getMean(int i) { return means[i]; }
	int getInitMode() { return initMode; }
	int getMaxIterNum() { return maxIterNum; }
	double getEndError() { return thresholdValue; }
	//拿到于这个sample采样点，最近的质点的序号，并返回距离
	double getClusteAndDis(Vec3 * x, int* cluIdxList);
	//得到两个点的距离
	double calcDistance(Vec3 * m, Vec3 * n);

	//初始化质点： 主函数  data 数据  N：数据的个数
	void init(vector<Vec3>  * data, int num);
	//k-means 主函数  data 数据  N：数据的个数  Label: N个int的数组
	void cluster(vector<Vec3> *data);

	friend std::ostream& operator<<(std::ostream& out, KMeans& kmeans);
};
