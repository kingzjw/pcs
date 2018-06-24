#pragma once
#include <sstream>
#include <assert.h>
#include <algorithm>
#include<assert.h>

#include "zjw_macro.h"
#include "zjw_pcs_octree.h"
#include "zjw_fileBatch.h"
#include "zjw_timer.h"

using namespace std;

//���������е�֡
class Frame
{
public:
	int frameId;
	ObjMesh *objMesh;
	PcsOctree *pcsOct;
public:
	//Frame();
	Frame(const int frameId, int _m, int _Nscales);
	~Frame();
	//����point cloud
	bool loadObj(string path);
	//�����˲����������л���������������õ�����sgwtϵ��
	bool octSgwt(Vec3 cellSize);
};

//���й�����
class FrameManage
{
public:
	vector<Frame*> frameList;
	FileBatch *fileBatch;
	enum FileNameForMat
	{
		NUM_FRONT,
		NUM_TAIL
	};
	//oct �����õ��Ĳ�����
	Vec3 cellSize;
	//kmeans�ĸ���
	int clusterNum;
	//�洢ѵ�����ݱ��������ľ���P;
	MatrixXd * P;

	//���� u,���ڼ���motion vector��һ��������
	double u;
	
	//sgwt �еĲ���,Ҫ���ݸ�ÿ��pcs�С�
	//Chebyshev��ϵ�������Ƶ�M�׵�����
	int m;
	//sclae�ĸ�����������h
	int Nscales;

	//ѵ������,��������ƥ���ϵ
	vector<int> f1TrainList;
	vector<int> f2TrainList;

	//�������ݣ��洢���е����ƥ��
	vector<int> f1nIdxList;
	vector<int> f2nIdxList;
	vector<double> maDist;

	//�������ݣ��õ�ϡ�����ƥ��
	vector<int> f1SparseIdxList;
	vector<int> f2SparseIdxList;

private:
	//����Ƿ���ù�batchLoadObj�ӿ�
	bool batchLoad;
	//����Ƿ���ù�getAllfilepath�ӿ�
	bool getAllFilePath;

public:
	FrameManage();
	~FrameManage();

	//�ı�ԭ���ݣ�������õĽӿ�
	void switchSouceData();

	//���沢�����ļ����µ����е������ļ�.�����ؼ������Ӧ��swgϵ��
	void batchLoadObj(FileNameForMat type = NUM_FRONT, string fileNameFormat = "_cleaner",
		string path = "./testData/football");

	//�������и�ʽ��·���ģ���С���󱣴�����·������,���õ����е�frameList
	void getAllFilesPath(FileNameForMat type = NUM_FRONT, string fileNameFormat = "_cleaner",
		string path = "./testData/football");

	//��ָ����������֡����ȡobj�������ֵsgwt��ϵ����frameId ��0��ʼ
	bool loadContinuousFrames(int frameId1, int frameId2, FileNameForMat type = NUM_FRONT,
		string fileNameFormat = "_cleaner", string path = "./testData/football", bool changeData = true);

	//ѵ�����ݣ���ѵ��������������֡����ƥ��,��frameId1�е�ÿ��node����fram2���Ҷ�Ӧ��ƥ���Node idx
	bool matchNode(int frameId1, int frameId2, vector<int>* f1nIdxList, vector<int>* f2nIdxList);

	//ѵ�����ݣ�����matchNode�õ�ƥ���ϵ������õ��������ݵ����������Ȼ�����Э����ľ��󣬼������ P
	bool getMatrixP(int frameId1, int frameId2, vector<int>* f1nIdxList,
		vector<int>* f2nIdxList, MatrixXd * P);

	//�������ݣ��õ�frame2��ÿ��node idx�� frame1�е����ƥ�䡣���ƥ���ϵ��������vector�з��ء�
	bool getBestMatchPoint(int frameId1, int frameId2, MatrixXd * P, vector<int>* f1nIdxList,
		vector<int>* f2nIdxList, vector<double>* maDist);

	//�������ݣ�getBestMatchPoint�����ļ��ٰ棬���ü��ξ�����پܾ�ƥ�䡣�����õ��Ǿ��Ծ��룬���ܲ������󣡣���
	bool getBestMatchPointSpeedUp(int frameId1, int frameId2, MatrixXd * P, 
		vector<int>* f1nIdxList_out,vector<int>* f2nIdxList_out, vector<double>* maDist_out);

	//�������ݣ�����K-mean����Ľ����������ϡ������ƥ��
	bool doKmeansGetSparseBestMatch(int frameId, vector<int>* f1nIdxList, vector<int>* f2nIdxList,
		vector<double>* maDist, vector<int>* f1SparseIdxList_out, vector<int>* f2SparseIdxList_out);

	//ѵ���õ�����P���ܽӿ�
	bool trainGetP(int frameId1, int frameId2, FileNameForMat type = NUM_FRONT,
		string fileNameFormat = "_cleaner", string path = "./testData/football");

	//�õ��������ݵ�ϡ�����ƥ��
	bool getTwoFrameBestSparseMatch(int frameId1, int frameId2, vector<int>* f1SparseIdxList_out,
		vector<int>* f2SparseIdxList_out, FileNameForMat type = NUM_FRONT, string fileNameFormat = "_cleaner",
		string path = "./testData/football", bool changeData = true);

	//-------------------------����motion vector---------------------------

	//��ϡ������ƥ���е�Mn ��n��Ӧ��ϵ���õ���frame1�ϵ�3*3 mat 
	void getMnMat(int frameId1, int MnIdx, int NIdx, MatrixXd & MnMat_out);
	void getMnMatSpeedUP(int frameId1, int MnIdx, int NIdx, MatrixXd & MnMat_out);

	//����Mn Mat, �õ�Q����
	void getQ(int frameId1, vector<int>* f1SparseIdxList ,vector<int>* f2SparseIdxList, MatrixXd & Q_out);
	//����ĵõ�����P
	void getV0(int frameId1, vector<int>* f1SparseIdxList, vector<int>* f2SparseIdxList, VectorXd & V0_out);
	
	//�õ�selection matric ��index�����������±꣺�ֱ��ʾ��ͬ��component
	//index: ȡֵ��Χ��{1,2,3},sMat_out��ʵ��ϡ�����������ԸĽ�
	void selectionMatrix(int frameId1, int index, MatrixXd & sMat_out);

	//��������motion vector
	void computeMotinVector(int frameId1, vector<int>* f1SparseIdxList, 
		vector<int>* f2SparseIdxList,VectorXd & Vt_out);
};
