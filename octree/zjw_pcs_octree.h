#pragma once

#include <vector>
#include <set>
#include <iomanip>
#include <math.h>
#include <stdlib.h>
#include <Eigen/Dense>

#include "zjw_macro.h"

#ifdef SGWT_DEBUG
#include "zjw_sgwt_utils.h"
#endif //SGWT_DEBUG

#include "zjw_octree.h"
#include "util\zjw_math.h"
#include "util\zjw_obj.h"
#include "zjw_timer.h"
#include "zjw_dsaupd.h"
#include "zjw_kmeans.h"
#include "signalType.h"

using namespace Eigen;
using namespace std;

#ifdef USE_SPARSE
#include <eigen/Sparse>
typedef Eigen::SparseMatrix<double> SpMat;
typedef Eigen::Triplet<double> T;
#endif // USE_SPARSE

//------------------------------ Class that holds your data.-----------------------

//�ڵ�����������
class Node
{
public:
	//ÿ���ڿռ���һ�ѵ� particle��λ����Ϣ
	vector<Vec3> pointPosList;

	//ÿ���ڿռ���һ�ѵ� particle��λ����Ϣ
	vector<Vec3> colorList;

	//ÿ���ڿռ���һ�ѵ� particle��obj�е����
	vector<int> pointIdxList;

	//����ÿ��Ҷ�ӽڵ�����8�����޵�Ҷ�ӽڵ��λ����Ϣ��ָ��
	vector<vector<Vec3*>> leafNodePos8Areas;
	//����ÿ��Ҷ�ӽڵ�����8�����޵�Ҷ�ӽڵ����ɫ��Ϣ��ָ��
	vector<vector<Vec3*>> leafNodeColor8Areas;

	//�����Ҷ�ӽڵ����ÿ�������е��ź�(true),���û���źţ�false signal (0,0,0)
	vector <bool> pos8Flag;

	//8���źţ�ÿ������һ���� ÿ��node�ϵĶ���λ�õĵ��źš���x,y,z��
	vector<Vec3> pos8AreasSignal;

	//8���źţ�ÿ������һ���� ÿ��node�ϵĶ���λ�õĵ��źš���x,y,z��
	vector<Vec3> color8AreasSignal;
};

//-----------------�ж���������ĸ�Ҷ�ӽڵ���-----------------
class AddPoints : public Octree<Node>::Callback
{
public:
	//���浱ǰ���������node�����
	int nodeIdx;
	//��Ҫ���жϵĵ�
	Vec3 point;
public:
	AddPoints(Vec3 point);
	~AddPoints();

	virtual bool operator()(const Vec3 min, const Vec3 max, Node& nodeData) {
		return true;
	}
	//����false,��ֹ�ݹ����;����true,�����ݹ�����ӽڵ�
	virtual bool operator()(const Vec3 min, const Vec3 max, Octree<Node>::OctreeNode* currNode);
};

//-----------------���������˲�������Ҷ�ӽڵ�ı߽�ֵ���浽list�У�����Ҷ�ӽڵ���-----------------
class CallTraverseGetInfoSetLeaf : public Octree<Node>::Callback
{
public:
	//����Ҷ�ӽڵ��boundary ��������Ⱦ����
	vector<Vec3> minVList;
	vector<Vec3> maxVList;
	//Ҷ�ӽڵ���м�λ�ã�����kmeans
	vector<Vec3> *midVList;

	//����Ҷ�ӽڵ��list,����Ҷ�ӽڵ���
	vector<Octree<Node>::OctreeNode*> nodeList;
	//Ҷ�ӽڵ�ı����
	int leafIncr;

public:
	CallTraverseGetInfoSetLeaf();
	~CallTraverseGetInfoSetLeaf();

	virtual bool operator()(const Vec3 min, const Vec3 max, Node& nodeData) {
		return true;
	}
	//��Ҷ�ӽڵ���
	virtual bool operator()(const Vec3 min, const Vec3 max, Octree<Node>::OctreeNode* currNode);
};

//----------------------�ѵ��Ƶõ�ת��Ϊ����----------------------------------

class CallTGetGraph : public Octree<Node>::Callback
{
public:
	//�õ��ýڵ���Χ�ھӣ�����Ȩ�أ������浽weight�����С�
	int leafIdx;
	Vec3 leafMidPoint;
	//��Ӧ�ô洢��Ԥ�����õ�ֵ����Ӧ����ʵ��cell�Ŀ�ȡ�
	Vec3 octCellSize;
#ifdef USE_EIGEN
	MatrixXd  * dMatPtr;
	MatrixXd  * weightAMatPtr;
#endif // USE_EIGEN

#ifdef USE_SPARSE
	SpMat * spLap;
	std::vector<T> * coeff;
#endif // USE_SPARSE

#ifdef  USE_ARPACK
	Dsaupd * laplacianMat;
#endif //  use_arpack

public:
#ifdef USE_EIGEN
	CallTGetGraph(MatrixXd  * dMatPtr, MatrixXd  * weightAMatPtr)
	{
		this->dMatPtr = dMatPtr;
		this->weightAMatPtr = weightAMatPtr;
	}
#endif // USE_EIGEN

#ifdef USE_SPARSE
	CallTGetGraph(std::vector<T> * coefficent, SpMat * sp = nullptr)
	{
		this->coeff = coefficent;
		this->spLap = sp;
	}
#endif // USE_SPARSE

#ifdef  USE_ARPACK
	CallTGetGraph(Dsaupd *laplacianMat)
	{
		this->laplacianMat = laplacianMat;
	}
#endif //  use_arpack

	~CallTGetGraph();

	void initParam(Vec3 & octreeCellSize);

	virtual bool operator()(const Vec3 min, const Vec3 max, Node& nodeData) {
		return true;
	}
	//�õ����ڽڵ㣬����Ȩ��
	virtual bool operator()(const Vec3 min, const Vec3 max, Octree<Node>::OctreeNode* currNode);
};

//-----------------------------------------------------
class PcsOctree
{
public:
	Vec3 minPos;
	Vec3 maxPos;
	Vec3 cellSize;

	//ObjMesh * objMesh;
	Octree<Node> *pcsOct;
	CallTraverseGetInfoSetLeaf * ctLeaf;
	CallTGetGraph *ctGraph;
	//�԰˲�����Ҷ�ӽڵ����k-means����
	KMeans * kmeans;

	//sgwt �еĲ���
	//Chebyshev��ϵ�������Ƶ�M�׵�����
	int m;
	//sclae�ĸ�����������h
	int Nscales;

	//�ڽ�Ȩ�ؾ����Լ�D�����������ֵ�����������Ĵ洢����
	int nodeNum;

	//8�������е��ź�
	vector<VectorXd> posSignalX;
	vector<VectorXd> posSignalY;
	vector<VectorXd> posSignalZ;

#ifdef USE_EIGEN
	MatrixXd  dMat;
	MatrixXd  weightAMat;
	MatrixXd  LaplacianMat;
	MatrixXd  eigenVecMat;
	MatrixXd  eigenValMat;
#endif // use_ei

#ifdef USE_SPARSE
	SpMat * spLaplacian;
	std::vector<T> * coefficients;
	//sgwt �Ķ���
#ifdef SGWT_DEBUG
	SgwtCheby *fastSgwt;
#endif //SGWT_DEBUG
#endif // USE_SPARSE

#ifdef USE_ARPACK
	Dsaupd laplacianMat;
#endif // use_arpa

public:
	PcsOctree(int _m, int _Nscales);
	~PcsOctree();
	//���ò��������³�ʼ���˲���
	void setParam(Vec3 min, Vec3 max, Vec3 cellSize);
	//�����˲���
	void buildPcsOctFrmPC(ObjMesh * objeMesh);
	//�õ�Ҷ�ӽڵ��Boundary������Ҷ�ӽڵ����Ϣ
	void getLeafboundary();

#ifdef RELATIVE_DIS_SIGNAL
	void changeNodePointsToRelativeDis();
#endif // RELATIVE_DIS_SIGNAL

	//�Ѱ˲���node�еĹ������node���е������ĳ�������ꡣ

	//�õ�graph����������
	//��ʼ���������Ϣ����getGraphMat֮ǰ����
	void initMat();
	//�õ�����
	void getGraphMat();

	//�õ��������������������ֵ
	void getMatEigenVerValue();

	//��հ˲���
	void clearOct();

	//�ж϶���point �ڰ�Χ���е�8�������е���һ��
	int judege8Aeros(Vec3 &min, Vec3 &max, Vec3 &point);
	int judege8Aeros(Vec3& mid, Vec3 &point);

	//��ÿ��Ҷ�ӽڵ��еĵ㻮�ֵ�8���������У�������ÿ��Ҷ�ӽڵ���źţ�������nodeData��
	void setPointTo8Areas();
	//����Ҷ�ӽڵ���ÿ�������ϵ��ź�
	void getLeafSignal();

	//��8�����޵��źţ�x,y,z,r,g,b���ŵ�������:����װ��vector��
	//8��vectorXd�� �ֱ��ʾ��ͬ���޵��źš�ÿ��vector��ʾ���ǣ������ڽڵ�����������еĸ��ͺŵľ����ֵ
	vector<VectorXd> getSignalF(SignalType sType);

	//����getSignalF�õ������źţ�����sgwt������е�ϵ������������sgwt�С�
	bool getAllSignalAndSaveSGWTCoeff();

	//��������ڵ����ض��ź��������������źţ�����ֵӦ����8��ϵ������������Ӧÿ������
	bool getSignalF(SignalType sType, int nodeIdx, VectorXd * sigVQ_out);

	//����,�����źŵ�ֵ�����޵�����(0-7),����node�������гֳ߶��ϵ�ϵ��
	vector<VectorXd> getSgwtCoeffWS(SignalType type, int quadrant);

	//�õ����node��feature vector ά�� 8*6*5
	bool getFeatureVector(int nodeIdx, VectorXd* featureVector);
	//����
	bool getFeatureVector2(int nodeIdx, VectorXd* featureVector);

	//������x,y��z���е��ź�
	void getSgwtCoeffWS();

	//kmeans
	void doKmeans(int clusterNum);
	//�жϵ�ǰ�����ĸ�Ҷ�ӽ����,���ص���Ҷ�ӽڵ�����
	int judegePointToLeafNode(Vec3* point, int& idx);


	//�õ�nodeidx������two hop���õ��ĵ㲻�����Լ�����Ϊ������ͼ�����������two hop����һ����
	void getTwoHopNeighborhood(int nodeIdx, set<int> * nodeList_out, SpMat * spLaplacian);

	//two hop����һ���ģ�����Ľӿ��ǲ�����һ���ġ�
	void getTwoHopNeighborhoodWithOneStep(int nodeIdx, set<int> * nodeList_out, SpMat * spLaplacian);
	
	//����Ľӿ�
	//void
	//=============== test ====================
	void printMat();
};
