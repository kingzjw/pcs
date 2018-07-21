#pragma once
#include <vector>
#include <bitset>
#include <queue>

#include "zjw_macro.h"
#include "util\zjw_math.h"
#include "util\zjw_obj.h"
#include "zjw_octree_double_buffer.h"


////////////////////////////////////////////////////////////////////////////////
//�ڵ�����������
////////////////////////////////////////////////////////////////////////////////

struct DBufferNodeData
{
	//ÿ���ڿռ���һ�ѵ� particle��λ����Ϣ
	vector<Vec3> pointPosList;

	//ÿ���ڿռ���һ�ѵ� particle��obj�е����
	vector<int> pointIdxList;

}; 

////////////////////////////////////////////////////////////////////////////////
//��target frame ������reference frame��������ݽ���ɾ��
////////////////////////////////////////////////////////////////////////////////

class TraverseAllNodeForFree : public DoubleBufferOctree<DBufferNodeData>::Callback
{
public:
	bool isTarget;
public:
	TraverseAllNodeForFree(bool isTarget);
	~TraverseAllNodeForFree();

	/*
	*���ã� �������tree�ϵ���Ҫɾ����node��Ϣ
	*���ز���: ����false,��ֹ�ݹ����;����true,�����ݹ�����ӽڵ�
	*/
	virtual bool operator()(const Vec3 min, const Vec3 max, OctreeDoubelBufferNode<DBufferNodeData>* currNode);
};

////////////////////////////////////////////////////////////////////////////////
//���������˲���,����Ҷ�ӽڵ��ϵ���Ϣ
////////////////////////////////////////////////////////////////////////////////

class TraverseGetInfoSetLeaf : public DoubleBufferOctree<DBufferNodeData>::Callback
{
public:
	//����Ҷ�ӽڵ��boundary ��������Ⱦ����
	vector<Vec3> minVList;
	vector<Vec3> maxVList;

	//����Ҷ�ӽڵ��list,����Ҷ�ӽڵ���
	vector<OctreeDoubelBufferNode<DBufferNodeData> *> nodeList;
	//Ҷ�ӽڵ�ı����
	int leafIncr;

public:
	TraverseGetInfoSetLeaf();
	~TraverseGetInfoSetLeaf();

	virtual bool operator()(const Vec3 min, const Vec3 max, DBufferNodeData& nodeData) {
		return true;
	}
	//��Ҷ�ӽڵ���
	virtual bool operator()(const Vec3 min, const Vec3 max, OctreeDoubelBufferNode<DBufferNodeData>* currNode);
};

////////////////////////////////////////////////////////////////////////////////
//˫buffer �˲���
////////////////////////////////////////////////////////////////////////////////

class PcsDBufferOctree
{
public:
	Vec3 minPos;
	Vec3 maxPos;
	Vec3 cellSize;

	//��ǰ�˲����еľ������
	bool targetFrameLoaded;
	bool referenceFrameLoaded;

	//˫Buffer����ű�־ 0 or 1
	int targetFrameId;
	int refrenceFrameId;

	DoubleBufferOctree<DBufferNodeData> *pcsDBufferOct;
	
private:
	void setTargetFrameId(int id);
	void setReferenceFrameId(int id);
	
public:
	PcsDBufferOctree(Vec3 minPos,Vec3 maxPosVec3, Vec3 cellSize);
	~PcsDBufferOctree();

	/**��doubleBuffer�е���ָ����һ֡*/
	void buildDBufferOctree(bool isTarget, ObjMesh * objMesh);
	void buildDBufferOctree(bool isTarget, vector<Vertex> &vertexList);
	
	/**���ݸ�����֡���ݣ�����˫Buffer�İ˲���*/
	void buildDBufferOctree(ObjMesh * referenceMesh, ObjMesh * targetMesh);
	void buildDBufferOctree(vector<Vertex> & referenceMeshVerList, vector<Vertex> & targetMeshVerList);

	//�����ǰ�Լ�����֡���ٶ�����һ֡��Ȼ����°˲���
	void getNextTwoFrame(ObjMesh * newObjMesh);
	void getNextTwoFrame(vector<Vertex> &newVertexList);

	/**ɾ��doubel buffer octree��ָ������һ֡����*/
	void clearOneFrame(bool isTarget);

	/**���ÿ�����ȱ������õ�byteSteam*/
	bool getByteSteamOfOctree(vector<char> & byteList_out);

	void clearDBufferOctree();

	/////////////////////////////
	//����ByteList �Լ�swap�İ˲����ṹ���ָ�����Ӧreference �İ˲����ṹ
	/////////////////////////////
	
	bool recoveryDBufferOctree(vector<Vertex> &swapFrameVerList, vector<char> & byteList);
	bool getTargetOctreeByclearReference();

};
