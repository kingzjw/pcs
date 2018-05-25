#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include "zjw_kmeans.h"

using namespace std;

KMeans::KMeans(int clusterNum, int dimNum)
{
	this->dimNum = dimNum;
	this->clusterNum = clusterNum;

	initMode = InitRandom;
	maxIterNum = 100;
	thresholdValue = 0.001;
	dataSize = 0;
}

KMeans::~KMeans()
{
	//释放空间
	/*for (int i = 0; i < clusterNum; i++)
	{
		delete[] means[i];
	}*/
}

void KMeans::setClusterNum(int i)
{
	clusterNum = i;
}

//N 为特征向量数
void KMeans::cluster(vector<Vec3> *data)
{
	//midVList的数量
	dataSize = data->size();
	assert(dataSize >= clusterNum);

	//分配需要的空间
	vector<int> temp;
	for (int i = 0; i < clusterNum; i++)
	{
		/*Vec3 *v = new Vec3();
		means.push_back(v);*/
		means.push_back(Vec3());
		clusterRes.push_back(temp);
	}

	// Initialize model
	init(data, dataSize);

	// Recursion
	// Sample data
	Vec3 sampleData;

	//标记 Class index
	int clusterIdx = -1;
	//表示当前迭代的次数
	double iterNum = 0;
	//所有采样点距离相应质点的平均距离
	double lastAvgDis = 0;
	double currAvgDis = 0;

	//标记当前的迭代的误差持续小于阈值的次数，达到一定次数也会退出
	int unchanged = 0;
	//标记是否继续迭代
	bool loop = true;
	//记录每个分类下面总共有多少数目
	int* counts = new int[clusterNum];

	// 存储计算得到新的质点，现在分配空间
	vector<Vec3> next_means;
	for (int i = 0; i < clusterNum; i++)
	{
		next_means.push_back(Vec3());
	}
	/*double** next_means = new double*[clusterNum];
	for(int i = 0; i < clusterNum; i++)
	{
		next_means[i] = new double[dimNum];
	}*/

	while (loop)
	{
		//clean buffer for classification
		memset(counts, 0, sizeof(int) * clusterNum);

		next_means.clear();
		for (int i = 0; i < clusterNum; i++)
		{
			next_means.push_back(Vec3());
		}

		lastAvgDis = currAvgDis;
		currAvgDis = 0;

		// Classification 遍历所有的采样点，得到属于他的分类，以及距离该分类中质点的距离
		for (int i = 0; i < dataSize; i++)
		{
			sampleData = (*data)[i];
			/*for(int j = 0; j < dimNum; j++)
				x[j] = data[i*dimNum+j];*/

			currAvgDis += getClusteAndDis((Vec3 *)&sampleData, &clusterIdx);
			//每个分类总共有多少采样点
			counts[clusterIdx]++;
			next_means[clusterIdx] += sampleData;
		}
		//得到所有采样点距离自己相应质点的距离
		currAvgDis /= dataSize;

		// 重新得到新的质点
		for (int i = 0; i < clusterNum; i++)
		{
			if (counts[i] > 0)
			{
				next_means[i] /= counts[i];
				means = next_means;
				//memcpy(means[i], next_means[i], sizeof(double) * dimNum);
			}
		}

		// Terminal conditions
		iterNum++;
		if (fabs(lastAvgDis - currAvgDis) < thresholdValue * lastAvgDis)
		{
			unchanged++;
		}
		if (iterNum >= maxIterNum || unchanged >= 3)
		{
			loop = false;
		}

#ifdef ZJW_DEBUG
		cout << "Iter: " << iterNum << ", Average Cost: " << currCost << endl;
#endif
	}

	// 得到最终的分类结果，保存在clusterRes中
	for (int i = 0; i < dataSize; i++)
	{
		sampleData = (*data)[i];
		getClusteAndDis((Vec3*)&sampleData, &clusterIdx);
		//保存最终的结果
		clusterRes[clusterIdx].push_back(i);
	}
	delete[] counts;
}

void KMeans::init(vector<Vec3> *data, int num)
{
	int size = num;

	//随机产生质点
	if (initMode == InitRandom)
	{
		int inteval = size / clusterNum;

		//sample保存随机产生的数据.默认是0,0,0
		Vec3 sample;

		// Seed the random-number generator with current time
		srand((unsigned)time(NULL));

		for (int i = 0; i < clusterNum; i++)
		{
			int select = inteval * i + (inteval - 1) * rand() / RAND_MAX;
			sample = (*data)[select];

			means[i] = sample;
		}
	}
	else if (initMode == InitUniform)
	{
		//产生的均匀的质点
		//double* sample = new double[dimNum];
		Vec3 sample;
		for (int i = 0; i < clusterNum; i++)
		{
			int select = i * size / clusterNum;
			sample = (*data)[select];
			means[i] = sample;
		}
	}
	else if (initMode == InitManual)
	{
		// Do nothing
	}
}
//拿到于离这个sample采样点，最近的质点的序号，并返回距离
double KMeans::getClusteAndDis(Vec3 * sample, int* clusterIdx)
{
	double dist = -1;
	//拿到于这个sample采样点，最近的质点的序号，并返回距离
	for (int i = 0; i < clusterNum; i++)
	{
		double temp = calcDistance(sample, (Vec3 *)&means[i]);
		if (temp < dist || dist == -1)
		{
			dist = temp;
			*clusterIdx = i;
		}
	}
	return dist;
}

double KMeans::calcDistance(Vec3 * m, Vec3 * n)
{
	double temp = 0;
	temp += (m->x - n->x) * (m->x - n->x);
	temp += (m->y - n->y) * (m->y - n->y);
	temp += (m->z - n->z) * (m->z - n->z);
	return sqrt(temp);
}

ostream& operator<<(ostream& out, KMeans& kmeans)
{
	out << "<KMeans>" << endl;
	out << "<DimNum> " << kmeans.dimNum << " </DimNum>" << endl;
	out << "<ClusterNum> " << kmeans.clusterNum << " </CluterNum>" << endl;

	out << "<Mean>" << endl;
	for (int i = 0; i < kmeans.clusterNum; i++)
	{
		for (int d = 0; d < kmeans.dimNum; d++)
		{
			out << kmeans.means[i][d] << " ";
		}
		out << endl;
	}
	out << "</Mean>" << endl;

	out << "</KMeans>" << endl;
	return out;
}

void KMeans::setMean(int i, Vec3  pos)
{
	means[i] = pos;
}