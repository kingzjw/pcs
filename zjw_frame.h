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

//点云序列中的帧
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
	//导入point cloud
	bool loadObj(string path);
	//建立八叉树，并进行基本操作，并计算得到所有sgwt系数
	bool octSgwt(Vec3 cellSize);
};

//序列管理器
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
	//oct 里面用到的参数：
	Vec3 cellSize;
	//kmeans的个数
	int clusterNum;
	//存储训练数据保存下来的矩阵P;
	MatrixXd * P;

	//常量 u,用于计算motion vector的一个参数。
	double u;
	
	//sgwt 中的参数,要传递给每个pcs中。
	//Chebyshev的系数，近似的M阶的相数
	int m;
	//sclae的个数，不包括h
	int Nscales;

	//训练数据,保存最后的匹配关系
	vector<int> f1TrainList;
	vector<int> f2TrainList;

	//测试数据，存储所有的最佳匹配
	vector<int> f1nIdxList;
	vector<int> f2nIdxList;
	vector<double> maDist;

	//测试数据，拿到稀疏最佳匹配
	vector<int> f1SparseIdxList;
	vector<int> f2SparseIdxList;

private:
	//标记是否调用过batchLoadObj接口
	bool batchLoad;
	//标记是否调用过getAllfilepath接口
	bool getAllFilePath;

public:
	FrameManage();
	~FrameManage();

	//改变原数据，必须调用的接口
	void switchSouceData();

	//保存并处理文件夹下的所有的序列文件.并加载计算出相应的swg系数
	void batchLoadObj(FileNameForMat type = NUM_FRONT, string fileNameFormat = "_cleaner",
		string path = "./testData/football");

	//保存所有格式和路径的，从小到大保存完整路径名字,并得到所有的frameList
	void getAllFilesPath(FileNameForMat type = NUM_FRONT, string fileNameFormat = "_cleaner",
		string path = "./testData/football");

	//对指定的连续两帧，读取obj并计算价值sgwt的系数，frameId 从0开始
	bool loadContinuousFrames(int frameId1, int frameId2, FileNameForMat type = NUM_FRONT,
		string fileNameFormat = "_cleaner", string path = "./testData/football", bool changeData = true);

	//训练数据：对训练的数据连续两帧进行匹配,对frameId1中的每个node，在fram2中找对应的匹配的Node idx
	bool matchNode(int frameId1, int frameId2, vector<int>* f1nIdxList, vector<int>* f2nIdxList);

	//训练数据：根据matchNode得到匹配关系。计算得到样本数据的误差向量，然后计算协方差的矩阵，及其的逆 P
	bool getMatrixP(int frameId1, int frameId2, vector<int>* f1nIdxList,
		vector<int>* f2nIdxList, MatrixXd * P);

	//测试数据：拿到frame2中每个node idx在 frame1中的最佳匹配。最佳匹配关系，从两个vector中返回。
	bool getBestMatchPoint(int frameId1, int frameId2, MatrixXd * P, vector<int>* f1nIdxList,
		vector<int>* f2nIdxList, vector<double>* maDist);

	//测试数据：getBestMatchPoint函数的加速版，利用几何距离快速拒绝匹配。这里用的是绝对距离，可能产生错误！！！
	bool getBestMatchPointSpeedUp(int frameId1, int frameId2, MatrixXd * P, 
		vector<int>* f1nIdxList_out,vector<int>* f2nIdxList_out, vector<double>* maDist_out);

	//测试数据：根据K-mean保存的结果，来保存稀疏的最佳匹配
	bool doKmeansGetSparseBestMatch(int frameId, vector<int>* f1nIdxList, vector<int>* f2nIdxList,
		vector<double>* maDist, vector<int>* f1SparseIdxList_out, vector<int>* f2SparseIdxList_out);

	//训练得到矩阵P的总接口
	bool trainGetP(int frameId1, int frameId2, FileNameForMat type = NUM_FRONT,
		string fileNameFormat = "_cleaner", string path = "./testData/football");

	//得到测试数据的稀疏最佳匹配
	bool getTwoFrameBestSparseMatch(int frameId1, int frameId2, vector<int>* f1SparseIdxList_out,
		vector<int>* f2SparseIdxList_out, FileNameForMat type = NUM_FRONT, string fileNameFormat = "_cleaner",
		string path = "./testData/football", bool changeData = true);

	//-------------------------计算motion vector---------------------------

	//对稀疏的最佳匹配中的Mn 和n对应关系，得到在frame1上的3*3 mat 
	void getMnMat(int frameId1, int MnIdx, int NIdx, MatrixXd & MnMat_out);
	void getMnMatSpeedUP(int frameId1, int MnIdx, int NIdx, MatrixXd & MnMat_out);

	//利用Mn Mat, 得到Q矩阵
	void getQ(int frameId1, vector<int>* f1SparseIdxList ,vector<int>* f2SparseIdxList, MatrixXd & Q_out);
	//计算的得到矩阵P
	void getV0(int frameId1, vector<int>* f1SparseIdxList, vector<int>* f2SparseIdxList, VectorXd & V0_out);
	
	//得到selection matric ，index是这个矩阵的下标：分别表示不同的component
	//index: 取值范围：{1,2,3},sMat_out其实是稀疏矩阵，这里可以改进
	void selectionMatrix(int frameId1, int index, MatrixXd & sMat_out);

	//计算最后的motion vector
	void computeMotinVector(int frameId1, vector<int>* f1SparseIdxList, 
		vector<int>* f2SparseIdxList,VectorXd & Vt_out);
};
