#include "zjw_octree.h"
#include "util\zjw_math.h"
#include "util\zjw_obj.h"

//------------- Class that holds your data.--------------------
#include <vector>
#include <iomanip>
#include <math.h>
#include <stdlib.h>
using namespace std;

//------------------------------ Class that holds your data.-----------------------

//Ҷ�ӽڵ�����������
class Node
{
public:
	vector<Vec3> pointList; //ÿ���ڿռ���һ�ѵ� particle
};


// -------------Traverse octree.do something. Create callback class.----------------
class CallbackTraverse : public Octree<Node>::Callback
{
public:
	vector<Vec3> minVList;
	vector<Vec3> maxVList;
public:
	CallbackTraverse();

	virtual bool operator()(const Vec3 min, const Vec3 max, Node& nodeData) {
		return true;
	}
	virtual bool operator()(const Vec3 min, const Vec3 max, Octree<Node>::OctreeNode* currNode)
	{
		bool flag = true;
		//�ж��ǻ����Ҷ�ӽڵ�
		if (currNode->children[0] || currNode->children[1] || currNode->children[2] || currNode->children[3]
			|| currNode->children[4] || currNode->children[5] || currNode->children[6] || currNode->children[7])
		{
			flag = true;
		}
		else
		{
			//��Ҷ�ӽڵ㣬��ô����Ҷ�ӽڵ�ľ��η�Χ��Ȼ���˳�
			flag = false;
			for (int i = 0; i < currNode->nodeData.pointList.size(); i++)
			{
				minVList.push_back(currNode->min);
				maxVList.push_back(currNode->max);
			}
		}
		return flag;
	}
	
};
//-----------------------------------------------------
class PcsOctree
{
public:
	Vec3 min;
	Vec3 max;
	Vec3 cellSize;

	Octree<Node> *pcsOct;
	CallbackTraverse * ct;
public:
	PcsOctree();
	~PcsOctree();
	//���ò��������³�ʼ���˲���
	void setParam(Vec3 min, Vec3 max, Vec3 cellSize);
	//�����˲���
	void buildPcsOctFrmPC(ObjMesh * objeMesh);
	//�õ�Ҷ�ӽڵ��Boundary
	void getLeafboundary();
	//��հ˲���
	void clearOct();

};