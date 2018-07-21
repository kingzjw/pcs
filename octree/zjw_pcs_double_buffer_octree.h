#pragma once
#include <vector>
#include <bitset>
#include <queue>

#include "zjw_macro.h"
#include "util\zjw_math.h"
#include "util\zjw_obj.h"
#include "zjw_octree_double_buffer.h"


////////////////////////////////////////////////////////////////////////////////
//节点包含点的数据
////////////////////////////////////////////////////////////////////////////////

struct DBufferNodeData
{
	//每个在空间有一堆的 particle的位置信息
	vector<Vec3> pointPosList;

	//每个在空间有一堆的 particle在obj中的序号
	vector<int> pointIdxList;

}; 

////////////////////////////////////////////////////////////////////////////////
//对target frame 或者是reference frame的相关内容进行删除
////////////////////////////////////////////////////////////////////////////////

class TraverseAllNodeForFree : public DoubleBufferOctree<DBufferNodeData>::Callback
{
public:
	bool isTarget;
public:
	TraverseAllNodeForFree(bool isTarget);
	~TraverseAllNodeForFree();

	/*
	*作用： 清理这个tree上的需要删除的node信息
	*返回参数: 返回false,终止递归遍历;返回true,继续递归遍历子节点
	*/
	virtual bool operator()(const Vec3 min, const Vec3 max, OctreeDoubelBufferNode<DBufferNodeData>* currNode);
};

////////////////////////////////////////////////////////////////////////////////
//遍历整个八叉树,处理叶子节点上的信息
////////////////////////////////////////////////////////////////////////////////

class TraverseGetInfoSetLeaf : public DoubleBufferOctree<DBufferNodeData>::Callback
{
public:
	//保存叶子节点的boundary ，用于渲染所用
	vector<Vec3> minVList;
	vector<Vec3> maxVList;

	//保存叶子节点的list,并给叶子节点编号
	vector<OctreeDoubelBufferNode<DBufferNodeData> *> nodeList;
	//叶子节点的编号器
	int leafIncr;

public:
	TraverseGetInfoSetLeaf();
	~TraverseGetInfoSetLeaf();

	virtual bool operator()(const Vec3 min, const Vec3 max, DBufferNodeData& nodeData) {
		return true;
	}
	//给叶子节点编号
	virtual bool operator()(const Vec3 min, const Vec3 max, OctreeDoubelBufferNode<DBufferNodeData>* currNode);
};

////////////////////////////////////////////////////////////////////////////////
//双buffer 八叉树
////////////////////////////////////////////////////////////////////////////////

class PcsDBufferOctree
{
public:
	Vec3 minPos;
	Vec3 maxPos;
	Vec3 cellSize;

	//当前八叉树中的具体情况
	bool targetFrameLoaded;
	bool referenceFrameLoaded;

	//双Buffer的序号标志 0 or 1
	int targetFrameId;
	int refrenceFrameId;

	DoubleBufferOctree<DBufferNodeData> *pcsDBufferOct;
	
private:
	void setTargetFrameId(int id);
	void setReferenceFrameId(int id);
	
public:
	PcsDBufferOctree(Vec3 minPos,Vec3 maxPosVec3, Vec3 cellSize);
	~PcsDBufferOctree();

	/**在doubleBuffer中导入指定的一帧*/
	void buildDBufferOctree(bool isTarget, ObjMesh * objMesh);
	void buildDBufferOctree(bool isTarget, vector<Vertex> &vertexList);
	
	/**根据给定两帧内容，创建双Buffer的八叉树*/
	void buildDBufferOctree(ObjMesh * referenceMesh, ObjMesh * targetMesh);
	void buildDBufferOctree(vector<Vertex> & referenceMeshVerList, vector<Vertex> & targetMeshVerList);

	//如果当前以及有两帧，再读入下一帧，然后更新八叉树
	void getNextTwoFrame(ObjMesh * newObjMesh);
	void getNextTwoFrame(vector<Vertex> &newVertexList);

	/**删除doubel buffer octree中指定的那一帧数据*/
	void clearOneFrame(bool isTarget);

	/**利用宽度优先遍历，得到byteSteam*/
	bool getByteSteamOfOctree(vector<char> & byteList_out);

	void clearDBufferOctree();

	/////////////////////////////
	//根据ByteList 以及swap的八叉树结构，恢复出相应reference 的八叉树结构
	/////////////////////////////
	
	bool recoveryDBufferOctree(vector<Vertex> &swapFrameVerList, vector<char> & byteList);
	bool getTargetOctreeByclearReference();

};
