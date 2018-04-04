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

//叶子节点包含点的数据
class Node
{
public:
	vector<Vec3> pointList; //每个在空间有一堆的 particle
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
		//判断是会否是叶子节点
		if (currNode->children[0] || currNode->children[1] || currNode->children[2] || currNode->children[3]
			|| currNode->children[4] || currNode->children[5] || currNode->children[6] || currNode->children[7])
		{
			flag = true;
		}
		else
		{
			//是叶子节点，那么保留叶子节点的矩形范围，然后退出
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
	//设置参数，重新初始化八叉树
	void setParam(Vec3 min, Vec3 max, Vec3 cellSize);
	//建立八叉树
	void buildPcsOctFrmPC(ObjMesh * objeMesh);
	//拿到叶子节点的Boundary
	void getLeafboundary();
	//清空八叉树
	void clearOct();

};