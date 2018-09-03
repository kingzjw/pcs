#pragma once
#include <sstream>
#include <assert.h>
#include <algorithm>
#include<assert.h>
#include <map>
#include <fstream>

#include "zjw_macro.h"
#include "zjw_pcs_octree.h"
#include "zjw_fileBatch.h"
#include "zjw_timer.h"
#include "zjw_minresQLP.h"
#include "zjw_octree_compression_profiles.h"
#include "zjw_octreePointCloudCompression.h"

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

	//point compression 
	OctreePointCloudCompressionZjw * opcCompress;

public:
	FrameManage();
	~FrameManage();

	//改变文件类型(frames的数据源)原数据，必须调用的接口
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

	//利用minresQLP计算最后的motion vector
	void computeMotinVectorMinresQLP(int frameId1, vector<int>* f1SparseIdxList,
		vector<int>* f2SparseIdxList, VectorXd & Vt_out);
	
	//通过motion vectotr和refrece frame 计算出预测的target的frame ver.预测结果保存到objmesh对象中的 vertexPredictTargetList
	void pridicTargetFrameVertex(int frameId1,  VectorXd  Vt); //
	
	

	/////////////////////////////////position comporession//////////////////////////////////

	/*
	*通过bytestream 和pos diff来传递swapframe 和target frame的之间的差异
	* Vt_in: motion vectotr  是frameId1和fraemID2对应的frame得到的mv
	*/
	
	//确定position 压缩文件的名字
	string getPosCompressFileName(int frameId, string filePrefix = "framePosCompress", string fileSuffix = ".pcf");
	
	//废弃了：因为把swap frame也传过去了
	void encoderDiffBetweenSwapTargetFrame(int frameId1, VectorXd  &Vt_in ,int frameId2) = delete;
	//废弃了：因为把swap frame也接受了
	void decoderDiffBetweenSwapTargetFrame(int frameId1, VectorXd  &Vt_in, int frameId2, ObjMesh &frameObj_ref_out) = delete;

	//压缩解压第一帧.(这个接口对单帧进行压缩传输，利用了double buffer octree的一半)
	void encoderDiffByteStreamForFirstFrame(int frameId1);
	void decoderDiffByteStreamForFirstFrame(int frameId1, ObjMesh & frameObj_ref_out);
	//利用swap frame来进行压缩的后续帧
	void encoderDiffBetweenSwapTargetFrame2(int frameId1, VectorXd  &Vt_in, int frameId2);
	//前提frameId1中的Objmesh的VertexList已经恢复了,而且对应的八叉树也已经建立了，叶子节点的信息也已经OK了
	void decoderDiffBetweenSwapTargetFrame2(int frameId1, VectorXd  &Vt_in, int frameId2, ObjMesh &frameObj_ref_out);
	
	//测试: Position 压缩测试接口。（利用bytestream，XOR等）
	void testOctreePCCompress(ObjMesh &frameObj_ref_out);


	/////////////////////////////////color compression//////////////////////////////////

	/*
	* 利用color diff信息进行压缩
	* Vt_in: motion vectotr  是frameId1和fraemID2对应的frame得到的mv
	* frameId1: reference frame index
	* frameId2: target frame id 
	*/
 
	//会在getColorDiff中用到,作用根据pair中的value进行从小到大的排序
	struct CmpByValue {
		bool operator()(const pair<int, double>& lhs, const pair<int, double>& rhs) {
			return lhs.second < rhs.second;
		}
	};
	//确定color信息压缩文件的名字
	string getColorCompressFileName(int frameId, string filePrefix = "frameColorCompress", string fileSuffix = ".ccf");
	
	//根据swap frame以及target frame计算出color diff
	bool getColorDiff(ObjMesh &swapObjMesh_in,int targetFrameId_in, vector<Color> &colorDiffList_out);
	
	//对color diff信息，进行GFT，量化
	bool handleColorDiffBeforeCompress(vector<Color> &colorDiffList_in);

	//color压缩：压缩解压第一帧.(这个接口对单帧进行压缩传输，利用了double buffer octree的一半)
	void encoderColorInfoForFirstFrame(int frameId1);
	void decoderColorInfoForFirstFrame(int frameId1, ObjMesh & frameObj_ref_out);

	//color压缩: 利用swap frame来求出t+1frame中的node最近的node，计算出预测颜色值，和color diff
	void encoderColorDiffInfo(int frameId1, VectorXd  &Vt_in, int frameId2);
	//前提frameId1中的Objmesh的VertexList已经恢复了,而且对应的八叉树也已经建立了，叶子节点的信息也已经OK了
	void decoderColorDiffInfo(int frameId1, VectorXd  &Vt_in, int frameId2, ObjMesh &frameObj_ref_out);

};
