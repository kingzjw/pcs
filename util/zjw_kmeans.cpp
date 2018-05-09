#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include "zjw_kmeans.h"

using namespace std;


KMeans::KMeans(int dimNum, int clusterNum)
{
	this->dimNum = dimNum;
	this->clusterNum = clusterNum;

	//���ʵ����ռ䣬����ʼ��ֵ(0,0,0)
	
	for(int i = 0; i < clusterNum; i++)
	{
		/*Vec3 *v = new Vec3();
		means.push_back(v);*/
		means.push_back(Vec3());
	}

	initMode = InitRandom;
	maxIterNum = 100;
	endError = 0.001;
}

KMeans::~KMeans()
{
	//�ͷſռ�
	/*for (int i = 0; i < clusterNum; i++)
	{
		delete[] means[i];
	}*/
}

//void KMeans::cluster(const char* sampleFileName, const char* labelFileName)
//{
//	// Check the sample file
//	ifstream sampleFile(sampleFileName, ios_base::binary);
//	assert(sampleFile);
//
//	int size = 0;
//	int dim = 0;
//	sampleFile.read((char*)&size, sizeof(int));
//	sampleFile.read((char*)&dim, sizeof(int));
//	assert(size >= clusterNum);
//	assert(dim == dimNum);
//
//	// Initialize model
//	init(sampleFile);
//
//	// Recursion
//	double* x = new double[dimNum];	// Sample data
//	int label = -1;		// Class index
//	double iterNum = 0;
//	double lastCost = 0;
//	double currCost = 0;
//	int unchanged = 0;
//	bool loop = true;
//	int* counts = new int[clusterNum];
//	double** next_means = new double*[clusterNum];	// New model for reestimation
//	for(int i = 0; i < clusterNum; i++)
//	{
//		next_means[i] = new double[dimNum];
//	}
//
//	while(loop)
//	{
//	    //clean buffer for classification
//		memset(counts, 0, sizeof(int) * clusterNum);
//		for(int i = 0; i < clusterNum; i++)
//		{
//			memset(next_means[i], 0, sizeof(double) * dimNum);
//		}
//
//		lastCost = currCost;
//		currCost = 0;
//
//		sampleFile.clear();
//		sampleFile.seekg(sizeof(int) * 2, ios_base::beg);
//
//		// Classification
//		for(int i = 0; i < size; i++)
//		{
//			sampleFile.read((char*)x, sizeof(double) * dimNum);
//			currCost += getClusteAndDis(x, &label);
//
//			counts[label]++;
//			for(int d = 0; d < dimNum; d++)
//			{
//				next_means[label][d] += x[d];
//			}
//		}
//		currCost /= size;
//
//		// Reestimation
//		for(int i = 0; i < clusterNum; i++)
//		{
//			if(counts[i] > 0)
//			{
//				for(int d = 0; d < dimNum; d++)
//				{
//					next_means[i][d] /= counts[i];
//				}
//				memcpy(means[i], next_means[i], sizeof(double) * dimNum);
//			}
//		}
//
//		// Terminal conditions
//		iterNum++;
//		if(fabs(lastCost - currCost) < endError * lastCost)
//		{
//			unchanged++;
//		}
//		if(iterNum >= maxIterNum || unchanged >= 3)
//		{
//			loop = false;
//		}
//		//DEBUG
//		//cout << "Iter: " << iterNum << ", Average Cost: " << currCost << endl;
//	}
//
//	// Output the label file
//	ofstream labelFile(labelFileName, ios_base::binary);
//	assert(labelFile);
//
//	labelFile.write((char*)&size, sizeof(int));
//	sampleFile.clear();
//	sampleFile.seekg(sizeof(int) * 2, ios_base::beg);
//
//	for(int i = 0; i < size; i++)
//	{
//		sampleFile.read((char*)x, sizeof(double) * dimNum);
//		getLabel(x, &label);
//		labelFile.write((char*)&label, sizeof(int));
//	}
//
//	sampleFile.close();
//	labelFile.close();
//
//	delete[] counts;
//	delete[] x;
//	for(int i = 0; i < clusterNum; i++)
//	{
//		delete[] next_means[i];
//	}
//	delete[] next_means;
//}

//N Ϊ����������
void KMeans::cluster(vector<Vec3 * > data, int N, int *cluIdxList)
{
	int size = N;

	assert(size >= clusterNum);

	// Initialize model
	init(data,N);

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

	while(loop)
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
		for(int i = 0; i < size; i++)
		{
			sampleData = *data[i];
			/*for(int j = 0; j < dimNum; j++)
				x[j] = data[i*dimNum+j];*/

			currAvgDis += getClusteAndDis((Vec3 *)&sampleData, &clusterIdx);
			//ÿ�������ܹ��ж��ٲ�����
			counts[clusterIdx]++;
			next_means[clusterIdx] += sampleData;
		}
		//�õ����в���������Լ���Ӧ�ʵ�ľ���
		currAvgDis /= size;

		// ���µõ��µ��ʵ�
		for(int i = 0; i < clusterNum; i++)
		{
			if(counts[i] > 0)
			{
				next_means[i] /= counts[i];
				means = next_means;
				//memcpy(means[i], next_means[i], sizeof(double) * dimNum);
			}
		}

		// Terminal conditions
		iterNum++;
		if(fabs(lastAvgDis - currAvgDis) < endError * lastAvgDis)
		{
			unchanged++;
		}
		if(iterNum >= maxIterNum || unchanged >= 3)
		{
			loop = false;
		}

#ifdef ZJW_DEBUG
		cout << "Iter: " << iterNum << ", Average Cost: " << currCost << endl;
#endif
	}

	// �õ����յķ�������������cluIdxList��
	for(int i = 0; i < size; i++)
	{
		sampleData = *data[i];
		getClusteAndDis((Vec3*)&sampleData,&clusterIdx);
		cluIdxList[i] = clusterIdx;
	}
	delete[] counts;
}

void KMeans::init(vector<Vec3 * > data, int num)
{
	int size = num;

	//��������ʵ�
	if(initMode ==  InitRandom)
	{
		int inteval = size / clusterNum;
		
		//sample�����������������.Ĭ����0,0,0
		Vec3 sample;

		// Seed the random-number generator with current time
		srand((unsigned)time(NULL));

		for(int i = 0; i < clusterNum; i++)
		{
			int select = inteval * i + (inteval - 1) * rand() / RAND_MAX;
			sample.x = data[select]->x;
			sample.y = data[select]->y;
			sample.z = data[select]->z;

			means[i] = sample;
		}
	}
	else if(initMode == InitUniform)
	{
		//�����ľ��ȵ��ʵ�
		//double* sample = new double[dimNum];
		Vec3 sample;
		for(int i = 0; i < clusterNum; i++)
		{
			int select = i * size / clusterNum;
			
			sample.x = data[select]->x;
			sample.y = data[select]->y;
			sample.z = data[select]->z;

			means[i] = sample;
		}
	}
	else if(initMode == InitManual)
	{
		// Do nothing
	}
}

//void KMeans::init(ifstream& sampleFile)
//{
//	int size = 0;
//	sampleFile.seekg(0, ios_base::beg);
//	sampleFile.read((char*)&size, sizeof(int));
//
//	if(initMode ==  InitRandom)
//	{
//		int inteval = size / clusterNum;
//		//sample�����������������.Ĭ����0,0,0
//		Vec3 sample;
//
//		// Seed the random-number generator with current time
//		srand((unsigned)time(NULL));
//
//		for(int i = 0; i < clusterNum; i++)
//		{
//			int select = inteval * i + (inteval - 1) * rand() / RAND_MAX;
//			int offset = sizeof(int) * 2 + select * sizeof(double) * dimNum;
//
//			sampleFile.seekg(offset, ios_base::beg);
//			sampleFile.read((char*)sample, sizeof(double) * dimNum);
//			memcpy(means[i], sample, sizeof(double) * dimNum);
//		}
//
//		delete[] sample;
//	}
//	else if(initMode == InitUniform)
//	{
//		double* sample = new double[dimNum];
//
//		for (int i = 0; i < clusterNum; i++)
//		{
//			int select = i * size / clusterNum;
//			int offset = sizeof(int) * 2 + select * sizeof(double) * dimNum;
//
//			sampleFile.seekg(offset, ios_base::beg);
//			sampleFile.read((char*)sample, sizeof(double) * dimNum);
//			memcpy(means[i], sample, sizeof(double) * dimNum);
//		}
//
//		delete[] sample;
//	}
//	else if(initMode == InitManual)
//	{
//		// Do nothing
//	}
//}


//�õ��������sample�����㣬������ʵ����ţ������ؾ���
double KMeans::getClusteAndDis(Vec3 * sample, int* clusterIdx)
{
	double dist = -1;
	//�õ������sample�����㣬������ʵ����ţ������ؾ���
	for(int i = 0; i < clusterNum; i++)
	{
		//test
		//cout << sample->x << " " << sample->y << " " << sample->z << endl;
		//cout << means[i].x << " " << means[i].y << " " << means[i].z << endl;
		//end test
		double temp = calcDistance(sample, (Vec3 *)&means[i]);
		if(temp < dist || dist == -1)
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
	for(int i = 0; i < kmeans.clusterNum; i++)
	{
		for(int d = 0; d < kmeans.dimNum; d++)
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
