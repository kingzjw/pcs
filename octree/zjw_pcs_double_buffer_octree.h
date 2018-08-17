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
	DBufferNodeData()
	{
		pointPosList.clear();
		pointIdxList.clear();
	}

	~DBufferNodeData()
	{

	}
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
	//default : true
	bool isTarget;

public:
	TraverseGetInfoSetLeaf(bool isTarget_in = true);
	~TraverseGetInfoSetLeaf();

	void setIsTarget(bool flag);
	void initParam();

	virtual bool operator()(const Vec3 min, const Vec3 max, DBufferNodeData& nodeData) {
		return true;
	}
	//�õ��˲�����Ҷ����Ϣ������Ӧ����Ϣ���浽 ������
	virtual bool operator()(const Vec3 min, const Vec3 max, OctreeDoubelBufferNode<DBufferNodeData>* currNode);
};

////////////////////////////////////////////////////////////////////////////////
//˫buffer �˲���
////////////////////////////////////////////////////////////////////////////////
// define frame identifier



class PcsDBufferOctree
{

public:
	//bounding box and cell size
	Vec3 minPos;
	Vec3 maxPos;
	Vec3 cellSize;

	//target(p-frame) Ҷ�ӽڵ�ĸ���
	int refre_leaf_count_;
	int target_leaf_count_;

	//I frame ��P frame��ָ�룬����ɾ������
	vector<Vertex> * referenceMeshVerList;
	vector<Vertex> * targetMeshVerList;

	//����֡�е�Point������
	int refFramePointNum;
	int targetFramePointNum;

	//��ǰ�˲����еľ������
	bool targetFrameLoaded;
	bool referenceFrameLoaded;

	//˫Buffer����ű�־ 0 or 1
	int targetFrameId;
	int refrenceFrameId;

	DoubleBufferOctree<DBufferNodeData> *pcsDBufferOct;
	TraverseGetInfoSetLeaf* ctLeaf;
	//TraverseGetInfoSetLeaf *ctLeaf;
	
	
private:
	void setTargetFrameId(int id);
	void setReferenceFrameId(int id);
	
public:
	PcsDBufferOctree(Vec3 minPos,Vec3 maxPos, Vec3 cellSize);
	PcsDBufferOctree(Vec3 minPos, Vec3 maxPos, double cellSize);
	PcsDBufferOctree();

	~PcsDBufferOctree();

	/**�õ�frame�еĵ�ĸ���*/
	uint32_t getFramePointNum(bool isTarget = false);
	/**�õ�octree�ı߽���*/
	void getBoundingBox(double &min_x, double &min_y, double &min_z, double & max_x, double & max_y, double &max_z);
	void setBoundingBox(const double &min_x, const double &min_y, const double &min_z, 
		const double & max_x, const double & max_y, const double &max_z);
	void setCellSize(const double cellSize);
	
	///////////////////////////////////////////////////////////////////
	//�õ�Ҷ�ӽڵ��Boundary������Ҷ�ӽڵ����Ϣ,�õ�Ҷ�ӽڵ������
	void getLeafboundaryAndLeafNode(bool isTarget);


	///////////////////////////////////////////////////////////////////
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
	bool getByteSteamOfOctreeXOR(vector<char> & byteList_out);
	bool getByteSteamOfOctreeSigleFrame(vector<char> & byteList_out, bool isTarget);

	void clearDBufferOctree();

	/////////////////////////////
	//����ByteList �Լ�swap�İ˲����ṹ���ָ�����Ӧreference �İ˲����ṹ
	/////////////////////////////
	
	/**����byteStream��ֱ���ع���֡�İ˲����ṹ,
	* ǰ���ǿյİ˲���,�ú�������հ˲���
	*/
	bool recoverySigleFrameInDBufferOctree(vector<char> & byteListForSigleFrame, bool isTarget = false);

	/**����byteStream��ֱ�����Ѿ����õ�һ֡������£�����XOR byteStream���ع��ڶ�֡
	* ǰ�����Ѿ��ع���һ֡�ˣ�ͨ����refe frame�Ѿ���double buffer octree����
	*/
	bool recoverySecondFrameInDBufferOctreeXOR(vector<char> & byteStreamXOR, bool isTarget = true);


	/**����swapFrameVerList��Ϊreference frame �ĵ����Ϣ��byteList��byteStream����Ϣ���ָ���target frame �İ˲����Ľ��
	* ǰ�᣺�յ�double buffer octree
	* ����: ����swapFrameVerList_in���ؽ�����֡�İ˲����ṹ��Ȼ�����ByteList�ָ�������һ���˲���
	* */
	bool recoveryDBufferOctreeForTarget(vector<Vertex> &swapFrameVerList_in, vector<char> & byteList_in);

	bool getTargetOctreeByClearReference();

};
