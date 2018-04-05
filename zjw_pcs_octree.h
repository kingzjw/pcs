#include "zjw_octree.h"
#include "util\zjw_math.h"
#include "util\zjw_obj.h"

//------------- Class that holds your data.--------------------
#include <vector>
#include <iomanip>
#include <math.h>
#include <stdlib.h>
#include <Eigen/Dense>

using namespace Eigen;
using namespace std;

//------------------------------ Class that holds your data.-----------------------

//�ڵ�����������
class Node
{
public:
	vector<Vec3> pointList; //ÿ���ڿռ���һ�ѵ� particle
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
	MatrixXd  * dMatPtr;
	MatrixXd  * weightAMatPtr;

public:
	CallTGetGraph(MatrixXd  * dMatPtr,MatrixXd  * weightAMatPtr);
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
	Vec3 min;
	Vec3 max;
	Vec3 cellSize;

	Octree<Node> *pcsOct;
	CallTraverseGetInfoSetLeaf * ctLeaf;
	CallTGetGraph * ctGraph;

	//�ڽ�Ȩ�ؾ����Լ�D�����������ֵ�����������Ĵ洢����
	int nodeNum;
	MatrixXd  dMat;
	MatrixXd  weightAMat;

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

	//��ʼ���������Ϣ
	void initParam();
	//�õ�����
	void getGraphMat();

	void getGraph();
	//��հ˲���
	void clearOct();

	//=============== test ====================
	void printMat();
};