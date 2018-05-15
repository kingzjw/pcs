#pragma once
#include <sstream>
#include <assert.h>
#include <algorithm>

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
	Frame(const int frameId);
	~Frame();
	bool loadObj(string path);
	bool octSgwt();
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
	//�洢ѵ�����ݱ��������ľ���P;
	MatrixXd * P;

private:
	//����Ƿ���ù�batchLoadObj�ӿ�
	bool batchLoad;
	//����Ƿ���ù�getAllfilepath�ӿ�
	bool getAllFilePath;

public:
	FrameManage();
	~FrameManage();
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
	//���沢�����ļ����µ����е������ļ�.�����ؼ������Ӧ��swgϵ��
	void batchLoadObj(FileNameForMat type = NUM_FRONT, string fileNameFormat="_cleaner", 
		string path = "./testData/football");

	//�������и�ʽ��·���ģ���С���󱣴�����·������,���õ����е�frameList
	void getAllFilesPath(FileNameForMat type = NUM_FRONT,string fileNameFormat =  "_cleaner", 
		string path = "./testData/football");

	//��ָ����������֡����ȡobj�������ֵsgwt��ϵ����frameId ��0��ʼ
	bool loadContinuousFrames(int frameId1, int frameId2, FileNameForMat type = NUM_FRONT, 
		string fileNameFormat = "_cleaner", string path = "./testData/football");

	//��ѵ��������������֡����ƥ��,��frameId1�е�ÿ��node����fram2���Ҷ�Ӧ��ƥ���Node idx
	bool matchNode(int frameId1, int frameId2, vector<int>* f1nIdxList, vector<int>* f2nIdxList);

	//ѵ�����ݣ�����õ��������ݵ����������Ȼ�����Э����ľ��󣬼������ P
	bool getMatrixP(int frameId1, int frameId2, vector<int>* f1nIdxList,
		vector<int>* f2nIdxList, MatrixXd * P);

	//�������ݣ��õ�frame2��ÿ��node idx�� frame1�е����ƥ�䡣���ƥ���ϵ��������vector�з��ء�
	bool getBestMatchPoint(int frameId1, int frameId2, MatrixXd * P, vector<int>* f1nIdxList,
		vector<int>* f2nIdxList, vector<double>* maDist );

	//�������ݣ�����K-mean����Ľ����������ϡ������ƥ��
	bool doKmeansGetSparseBestMatch(int frameId, vector<int>* f1nIdxList,vector<int>* f2nIdxList, 
		vector<double>* maDist, vector<int>* f1SparseIdxList, vector<int>* f2SparseIdxList);
};


