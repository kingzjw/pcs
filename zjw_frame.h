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

	//point compression 
	OctreePointCloudCompressionZjw * opcCompress;

public:
	FrameManage();
	~FrameManage();

	//�ı��ļ�����(frames������Դ)ԭ���ݣ�������õĽӿ�
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

	//����minresQLP��������motion vector
	void computeMotinVectorMinresQLP(int frameId1, vector<int>* f1SparseIdxList,
		vector<int>* f2SparseIdxList, VectorXd & Vt_out);
	
	//ͨ��motion vectotr��refrece frame �����Ԥ���target��frame ver.Ԥ�������浽objmesh�����е� vertexPredictTargetList
	void pridicTargetFrameVertex(int frameId1,  VectorXd  Vt); //
	
	

	/////////////////////////////////position comporession//////////////////////////////////

	/*
	*ͨ��bytestream ��pos diff������swapframe ��target frame��֮��Ĳ���
	* Vt_in: motion vectotr  ��frameId1��fraemID2��Ӧ��frame�õ���mv
	*/
	
	//ȷ��position ѹ���ļ�������
	string getPosCompressFileName(int frameId, string filePrefix = "framePosCompress", string fileSuffix = ".pcf");
	
	//�����ˣ���Ϊ��swap frameҲ����ȥ��
	void encoderDiffBetweenSwapTargetFrame(int frameId1, VectorXd  &Vt_in ,int frameId2) = delete;
	//�����ˣ���Ϊ��swap frameҲ������
	void decoderDiffBetweenSwapTargetFrame(int frameId1, VectorXd  &Vt_in, int frameId2, ObjMesh &frameObj_ref_out) = delete;

	//ѹ����ѹ��һ֡.(����ӿڶԵ�֡����ѹ�����䣬������double buffer octree��һ��)
	void encoderDiffByteStreamForFirstFrame(int frameId1);
	void decoderDiffByteStreamForFirstFrame(int frameId1, ObjMesh & frameObj_ref_out);
	//����swap frame������ѹ���ĺ���֡
	void encoderDiffBetweenSwapTargetFrame2(int frameId1, VectorXd  &Vt_in, int frameId2);
	//ǰ��frameId1�е�Objmesh��VertexList�Ѿ��ָ���,���Ҷ�Ӧ�İ˲���Ҳ�Ѿ������ˣ�Ҷ�ӽڵ����ϢҲ�Ѿ�OK��
	void decoderDiffBetweenSwapTargetFrame2(int frameId1, VectorXd  &Vt_in, int frameId2, ObjMesh &frameObj_ref_out);
	
	//����: Position ѹ�����Խӿڡ�������bytestream��XOR�ȣ�
	void testOctreePCCompress(ObjMesh &frameObj_ref_out);


	/////////////////////////////////color compression//////////////////////////////////

	/*
	* ����color diff��Ϣ����ѹ��
	* Vt_in: motion vectotr  ��frameId1��fraemID2��Ӧ��frame�õ���mv
	* frameId1: reference frame index
	* frameId2: target frame id 
	*/
 
	//����getColorDiff���õ�,���ø���pair�е�value���д�С���������
	struct CmpByValue {
		bool operator()(const pair<int, double>& lhs, const pair<int, double>& rhs) {
			return lhs.second < rhs.second;
		}
	};
	//ȷ��color��Ϣѹ���ļ�������
	string getColorCompressFileName(int frameId, string filePrefix = "frameColorCompress", string fileSuffix = ".ccf");
	
	//����swap frame�Լ�target frame�����color diff
	bool getColorDiff(ObjMesh &swapObjMesh_in,int targetFrameId_in, vector<Color> &colorDiffList_out);
	
	//��color diff��Ϣ������GFT������
	bool handleColorDiffBeforeCompress(vector<Color> &colorDiffList_in);

	//colorѹ����ѹ����ѹ��һ֡.(����ӿڶԵ�֡����ѹ�����䣬������double buffer octree��һ��)
	void encoderColorInfoForFirstFrame(int frameId1);
	void decoderColorInfoForFirstFrame(int frameId1, ObjMesh & frameObj_ref_out);

	//colorѹ��: ����swap frame�����t+1frame�е�node�����node�������Ԥ����ɫֵ����color diff
	void encoderColorDiffInfo(int frameId1, VectorXd  &Vt_in, int frameId2);
	//ǰ��frameId1�е�Objmesh��VertexList�Ѿ��ָ���,���Ҷ�Ӧ�İ˲���Ҳ�Ѿ������ˣ�Ҷ�ӽڵ����ϢҲ�Ѿ�OK��
	void decoderColorDiffInfo(int frameId1, VectorXd  &Vt_in, int frameId2, ObjMesh &frameObj_ref_out);

};
