#include "zjw_octree.h"
#include "util\zjw_math.h"
#include "util\zjw_obj.h"
#include "zjwtimer.h"
#include "zjw_dsaupd.h"

//------------- Class that holds your data.--------------------
#include <vector>
#include <iomanip>
#include <math.h>
#include <stdlib.h>
#include <Eigen/Dense>
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
	//ÿ���ڿռ���һ�ѵ� particle
	vector<Vec3> pointList;

	//����ÿ��Ҷ�ӽڵ�����8�����޵�Ҷ�ӽڵ�
	vector<vector<Vec3*>> leafNode8Areas;

	//�����Ҷ�ӽڵ����ÿ�������е��ź�,���û���źţ���ô����-1,-1,-1
	//vector <bool> posFlag;
	vector<Vec3> pos8Areas;
};

//-----------------���������˲�������Ҷ�ӽڵ�ı߽�ֵ���浽list�У�����Ҷ�ӽڵ���-----------------
class CallTraverseGetInfoSetLeaf : public Octree<Node>::Callback
{
public:
	//����Ҷ�ӽڵ��boundary ��������Ⱦ����
	vector<Vec3> minVList;
	vector<Vec3> maxVList;
	//����Ҳ�ӽڵ��list,����Ҷ�ӽڵ���
	vector<Octree<Node>::OctreeNode*> nodeList;

public:
	CallTraverseGetInfoSetLeaf();

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

	Octree<Node> *pcsOct;
	CallTraverseGetInfoSetLeaf * ctLeaf;
	CallTGetGraph * ctGraph;

	//�ڽ�Ȩ�ؾ����Լ�D�����������ֵ�����������Ĵ洢����
	int nodeNum;
#ifdef USE_EIGEN
	MatrixXd  dMat;
	MatrixXd  weightAMat;
	MatrixXd  LaplacianMat;
	MatrixXd  eigenVecMat;
	MatrixXd  eigenValMat;
#endif // use_ei

#ifdef USE_SPARSE
	SpMat * spLaplacian;
	std::vector<T> coefficients;
#endif // USE_SPARSE

#ifdef USE_ARPACK
	Dsaupd laplacianMat;
#endif // use_arpa

public:
	PcsOctree();
	~PcsOctree();
	//���ò��������³�ʼ���˲���
	void setParam(Vec3 min, Vec3 max, Vec3 cellSize);
	//�����˲���
	void buildPcsOctFrmPC(ObjMesh * objeMesh);
	//�õ�Ҷ�ӽڵ��Boundary������Ҷ�ӽڵ����Ϣ
	void getLeafboundary();
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

	//��ÿ��Ҷ�ӽڵ��еĵ㻮�ֵ�8����������
	void setPointTo8Areas();

	//����Ҷ�ӽڵ���ÿ�������ϵ��ź�
	void getLeafSignal();

	//=============== test ====================
	void printMat();
};