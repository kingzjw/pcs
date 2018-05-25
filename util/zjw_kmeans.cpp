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
	//�ͷſռ�
	/*for (int i = 0; i < clusterNum; i++)
	{
		delete[] means[i];
	}*/
}

void KMeans::setClusterNum(int i)
{
	clusterNum = i;
}

//N Ϊ����������
void KMeans::cluster(vector<Vec3> *data)
{
	//midVList������
	dataSize = data->size();
	assert(dataSize >= clusterNum);

	//������Ҫ�Ŀռ�
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

	//��� Class index
	int clusterIdx = -1;
	//��ʾ��ǰ�����Ĵ���
	double iterNum = 0;
	//���в����������Ӧ�ʵ��ƽ������
	double lastAvgDis = 0;
	double currAvgDis = 0;

	//��ǵ�ǰ�ĵ�����������С����ֵ�Ĵ������ﵽһ������Ҳ���˳�
	int unchanged = 0;
	//����Ƿ��������
	bool loop = true;
	//��¼ÿ�����������ܹ��ж�����Ŀ
	int* counts = new int[clusterNum];

	// �洢����õ��µ��ʵ㣬���ڷ���ռ�
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

		// Classification �������еĲ����㣬�õ��������ķ��࣬�Լ�����÷������ʵ�ľ���
		for (int i = 0; i < dataSize; i++)
		{
			sampleData = (*data)[i];
			/*for(int j = 0; j < dimNum; j++)
				x[j] = data[i*dimNum+j];*/

			currAvgDis += getClusteAndDis((Vec3 *)&sampleData, &clusterIdx);
			//ÿ�������ܹ��ж��ٲ�����
			counts[clusterIdx]++;
			next_means[clusterIdx] += sampleData;
		}
		//�õ����в���������Լ���Ӧ�ʵ�ľ���
		currAvgDis /= dataSize;

		// ���µõ��µ��ʵ�
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

	// �õ����յķ�������������clusterRes��
	for (int i = 0; i < dataSize; i++)
	{
		sampleData = (*data)[i];
		getClusteAndDis((Vec3*)&sampleData, &clusterIdx);
		//�������յĽ��
		clusterRes[clusterIdx].push_back(i);
	}
	delete[] counts;
}

void KMeans::init(vector<Vec3> *data, int num)
{
	int size = num;

	//��������ʵ�
	if (initMode == InitRandom)
	{
		int inteval = size / clusterNum;

		//sample�����������������.Ĭ����0,0,0
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
		//�����ľ��ȵ��ʵ�
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
//�õ��������sample�����㣬������ʵ����ţ������ؾ���
double KMeans::getClusteAndDis(Vec3 * sample, int* clusterIdx)
{
	double dist = -1;
	//�õ������sample�����㣬������ʵ����ţ������ؾ���
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