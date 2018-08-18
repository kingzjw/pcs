#include "zjw_pcs_double_buffer_octree.h"

void PcsDBufferOctree::setTargetFrameId(int id)
{
	assert(id == 0 || id == 1);
	//һ����0��һ����1
	targetFrameId = id;
	refrenceFrameId = (id + 1) % 2;

	pcsDBufferOct->setTargetFrameId(id);
}

void PcsDBufferOctree::setReferenceFrameId(int id)
{
	assert(id == 0 || id == 1);
	//һ����0��һ����1
	targetFrameId = id;
	refrenceFrameId = (id + 1) % 2;

	pcsDBufferOct->setReferenceFrameId(id);
}

PcsDBufferOctree::PcsDBufferOctree(Vec3 minPos, Vec3 maxPosVec3, Vec3 cellSize)
{
	refre_leaf_count_=0;
	target_leaf_count_=0;
	refFramePointNum = 0;
	targetFramePointNum = 0;

	this->minPos = minPos;
	this->maxPos = maxPosVec3;
	this->cellSize = cellSize;

	targetFrameLoaded = false;
	referenceFrameLoaded = false;

	targetFrameId = 0;
	refrenceFrameId = 1;
	pcsDBufferOct = new DoubleBufferOctree<DBufferNodeData>(minPos, maxPosVec3, cellSize, targetFrameId, refrenceFrameId);
	ctLeaf = new TraverseGetInfoSetLeaf();
}

PcsDBufferOctree::PcsDBufferOctree(Vec3 minPos, Vec3 maxPosVec3, double cellSize)
{
	refre_leaf_count_ = 0;
	target_leaf_count_ = 0;
	
	ctLeaf = new TraverseGetInfoSetLeaf();


	refFramePointNum = 0;
	targetFramePointNum = 0;

	this->minPos = minPos;
	this->maxPos = maxPosVec3;
	this->cellSize.x = cellSize;
	this->cellSize.y = cellSize;
	this->cellSize.z = cellSize;

	targetFrameLoaded = false;
	referenceFrameLoaded = false;

	targetFrameId = 0;
	refrenceFrameId = 1;
	pcsDBufferOct = new DoubleBufferOctree<DBufferNodeData>(minPos, maxPosVec3, cellSize, targetFrameId, refrenceFrameId);
}

PcsDBufferOctree::PcsDBufferOctree()
{
	refre_leaf_count_ = 0;
	target_leaf_count_ = 0;

	ctLeaf = new TraverseGetInfoSetLeaf();
	refFramePointNum = 0;
	targetFramePointNum = 0;

	this->minPos = Vec3(0, 0, 0);
	this->maxPos = Vec3(1, 1, 1);
	this->cellSize.x = 0.5;
	this->cellSize.y = 0.5;
	this->cellSize.z = 0.5;

	targetFrameLoaded = false;
	referenceFrameLoaded = false;

	targetFrameId = 0;
	refrenceFrameId = 1;
	pcsDBufferOct = new DoubleBufferOctree<DBufferNodeData>(minPos, maxPos, cellSize, targetFrameId, refrenceFrameId);
}

PcsDBufferOctree::~PcsDBufferOctree()
{
	if (pcsDBufferOct)
		delete pcsDBufferOct;
	if (ctLeaf)
		delete ctLeaf;
}

void PcsDBufferOctree::buildDBufferOctree(ObjMesh * referenceMesh, ObjMesh * targetMesh)
{
	assert(!targetFrameLoaded && !referenceFrameLoaded);

	buildDBufferOctree(false, referenceMesh);
	buildDBufferOctree(true, targetMesh);
}

void PcsDBufferOctree::buildDBufferOctree(vector<Vertex> & referenceMeshVerList, vector<Vertex> & targetMeshVerList)
{
	assert(!targetFrameLoaded && !referenceFrameLoaded);

	buildDBufferOctree(false, referenceMeshVerList);
	buildDBufferOctree(true, targetMeshVerList);
}

uint32_t PcsDBufferOctree::getFramePointNum(bool isTarget)
{
	if (isTarget)
	{
		return targetFramePointNum;
	}
	else
	{
		return refFramePointNum;
	}
}

void PcsDBufferOctree::getBoundingBox(double & min_x, double & min_y, double & min_z, double & max_x, double & max_y, double & max_z)
{
	min_x = minPos.x;
	min_y = minPos.y;
	min_z = minPos.z;
	max_x = maxPos.x;
	max_y = maxPos.y;
	max_z = maxPos.z;
}

void PcsDBufferOctree::setBoundingBox(const double &min_x, const double &min_y, const double &min_z,
	const double & max_x, const double & max_y, const double &max_z)
{
	minPos.x = min_x;
	minPos.y = min_y;
	minPos.z = min_z;

	maxPos.x = max_x;
	maxPos.y = max_y;
	maxPos.z = max_z;

	pcsDBufferOct->min = this->minPos;
	pcsDBufferOct->max = this->maxPos;
}

void PcsDBufferOctree::setCellSize(const double cellSize)
{
	this->cellSize.x = cellSize;
	this->cellSize.y = cellSize;
	this->cellSize.z = cellSize;
	pcsDBufferOct->cellSize = this->cellSize;
}

void PcsDBufferOctree::getLeafboundaryAndLeafNode(bool isTarget)
{
	ctLeaf->initParam();
	ctLeaf->setIsTarget(isTarget);

	// Prepare the callback class.
	pcsDBufferOct->traverse(ctLeaf);

	if (isTarget)
	{
		target_leaf_count_ = ctLeaf->nodeList.size();
	}
	else
	{
		refre_leaf_count_ = ctLeaf->nodeList.size();
	}
}

void PcsDBufferOctree::buildDBufferOctree(bool isTarget, ObjMesh * objMesh)
{
	//����Ѿ����ع��ˣ���ô��Ҫ������
	if (isTarget && targetFrameLoaded)
	{
		clearOneFrame(isTarget);
	}
	else if ((!isTarget) && referenceFrameLoaded)
	{
		clearOneFrame(isTarget);
	}

	if (isTarget)
	{
		targetMeshVerList = &objMesh->vertexList;
		targetFramePointNum = objMesh->vertexList.size();
		targetFrameLoaded = true;
	}
	else
	{
		referenceMeshVerList = &objMesh->vertexList;
		refFramePointNum = objMesh->vertexList.size();
		referenceFrameLoaded = true;
	}

	//���ڼ���Frame
	for (int v_it = 0; v_it < objMesh->vertexList.size(); v_it++)
	{
		//����������ȣ������˲���
		DBufferNodeData * node = pcsDBufferOct->getCell(isTarget, objMesh->vertexList[v_it]);

		//�ѵ�ŵ�Ҷ�ӽڵ�����������
		node->pointPosList.push_back(objMesh->vertexList[v_it]);
		node->pointIdxList.push_back(v_it);
	}
}

void PcsDBufferOctree::buildDBufferOctree(bool isTarget, vector<Vertex>& vertexList)
{
	//����Ѿ����ع��ˣ���ô��Ҫ������
	if (isTarget && targetFrameLoaded)
	{
		clearOneFrame(isTarget);
	}
	else if ((!isTarget) && referenceFrameLoaded)
	{
		clearOneFrame(isTarget);
	}

	if (isTarget)
	{
		targetMeshVerList = &vertexList;
		targetFramePointNum = vertexList.size();
		targetFrameLoaded = true;
	}
	else
	{
		referenceMeshVerList = &vertexList;
		refFramePointNum = vertexList.size();
		referenceFrameLoaded = true;
	}

	//���ڼ���Frame
	for (int v_it = 0; v_it < vertexList.size(); v_it++)
	{
		//����������ȣ������˲���
		DBufferNodeData *node = pcsDBufferOct->getCell(isTarget, vertexList[v_it]);

		//�ѵ�ŵ�Ҷ�ӽڵ����������档
		node->pointPosList.push_back(vertexList[v_it]);
		node->pointIdxList.push_back(v_it);
	}
}

void PcsDBufferOctree::clearOneFrame(bool isTarget)
{
	if (isTarget)
	{
		targetFrameLoaded = false;
		targetFramePointNum = 0;
		target_leaf_count_=0;
	}
	else
	{
		referenceFrameLoaded = false;
		refFramePointNum = 0;
		refre_leaf_count_=0;
	}

	//�������е� ��㣬���µ��ϣ�ɾ�����õĽڵ㣬������free NodeData(���ﻹҪ��дflag)
	TraverseAllNodeForFree freeTraverse(isTarget);
	pcsDBufferOct->traverseDepthFirst(&freeTraverse);
}

bool PcsDBufferOctree::getByteSteamOfOctreeXOR(vector<char>& byteList_out)
{
	byteList_out.clear();

	assert(targetFrameLoaded && referenceFrameLoaded);
	vector<OctreeDoubelBufferNode<DBufferNodeData>*>  allNodeList_out;
	pcsDBufferOct->traverseBreathFirst(allNodeList_out);

	for (int i = 0; i < allNodeList_out.size(); i++)
	{
		byteList_out.push_back(allNodeList_out[i]->getChildByteXOR());
	}
	return true;
}

bool PcsDBufferOctree::getByteSteamOfOctreeSigleFrame(vector<char>& byteList_out, bool isTarget)
{
	byteList_out.clear();
	vector<OctreeDoubelBufferNode<DBufferNodeData>*>  allNodeList_out;

	if (isTarget)
	{
		assert(targetFrameLoaded);
		pcsDBufferOct->traverseBreathFirstSigleFrame(allNodeList_out,true);
	}
	else
	{
		assert(referenceFrameLoaded);
		pcsDBufferOct->traverseBreathFirstSigleFrame(allNodeList_out, false);
	}

	for (int i = 0; i < allNodeList_out.size(); i++)
	{
		byteList_out.push_back(allNodeList_out[i]->getChildByteSigleFrame(isTarget));
	}

	return true;
}

void PcsDBufferOctree::clearDBufferOctree()
{
	//clear
	if (pcsDBufferOct)
		delete pcsDBufferOct;

	//init

	refFramePointNum = 0;
	targetFramePointNum = 0;
	targetFrameLoaded = false;
	referenceFrameLoaded = false;
	targetFrameId = 0;
	refrenceFrameId = 1;

	pcsDBufferOct = new DoubleBufferOctree<DBufferNodeData>(minPos, maxPos, cellSize, targetFrameId, refrenceFrameId);
}

bool PcsDBufferOctree::recoverySigleFrameInDBufferOctree(vector<char>& byteListForSigleFrame, bool isTarget)
{
	assert(!byteListForSigleFrame.empty());
	//init

	//�Է���һ�����double buffer octree
	clearDBufferOctree();

	referenceFrameLoaded = true;
	//according to the byteList to recovery the target frame
	std::queue<OctreeDoubelBufferNode<DBufferNodeData>*> nodeQue;
	nodeQue.push(pcsDBufferOct->root);
	int c_it = 0;
	while (!nodeQue.empty())
	{
		OctreeDoubelBufferNode<DBufferNodeData>* tempNode = nodeQue.front();
		pcsDBufferOct->recoveryDBufferOctreeNodeForSigleFrame(nodeQue.front(), byteListForSigleFrame[c_it++], false);
		nodeQue.pop();
		for (int n_it = 0; n_it < 8; n_it++)
		{
			//���ӽڵ㲻Ϊ�յģ����뵽������
			if (tempNode->getChildren(n_it))
			{
				nodeQue.push(tempNode->getChildren(n_it));
			}
		}
	}
	return true;
}

bool PcsDBufferOctree::recoverySecondFrameInDBufferOctreeXOR(vector<char>& byteStreamXOR, bool isTarget)
{
	assert(!byteStreamXOR.empty());
	if (isTarget)
	{
		assert(referenceFrameLoaded && !targetFrameLoaded);
		targetFrameLoaded = true;

	}
	else
	{
		assert(targetFrameLoaded && !referenceFrameLoaded);
		referenceFrameLoaded = true;
	}


	//according to the byteList to recovery the target frame
	std::queue<OctreeDoubelBufferNode<DBufferNodeData>*> nodeQue;
	nodeQue.push(pcsDBufferOct->root);
	int c_it = 0;
	while (!nodeQue.empty())
	{
		OctreeDoubelBufferNode<DBufferNodeData>* tempNode = nodeQue.front();
		pcsDBufferOct->recoveryDBufferOctreeNode(nodeQue.front(), byteStreamXOR[c_it++]);
		nodeQue.pop();
		for (int n_it = 0; n_it < 8; n_it++)
		{
			//���ӽڵ㲻Ϊ�յģ����뵽������
			if (tempNode->getChildren(n_it))
			{
				nodeQue.push(tempNode->getChildren(n_it));
			}
		}
	}
	return false;
}

bool PcsDBufferOctree::recoveryDBufferOctreeForTarget(vector<Vertex>& swapFrameVerList, vector<char>& byteList)
{
	assert(!byteList.empty());
	//init
	clearDBufferOctree();
	targetFrameLoaded = true;

	//build swap frame octree in referenceframe in double buffer octree
	buildDBufferOctree(false, swapFrameVerList);

	//according to the byteList to recovery the target frame
	std::queue<OctreeDoubelBufferNode<DBufferNodeData>*> nodeQue;
	nodeQue.push(pcsDBufferOct->root);
	int c_it = 0;
	while (!nodeQue.empty())
	{
		OctreeDoubelBufferNode<DBufferNodeData>* tempNode = nodeQue.front();
		pcsDBufferOct->recoveryDBufferOctreeNode(nodeQue.front(), byteList[c_it++]);
		nodeQue.pop();
		for (int n_it = 0; n_it < 8; n_it++)
		{
			//���ӽڵ㲻Ϊ�յģ����뵽������
			if (tempNode->getChildren(n_it))
			{
				nodeQue.push(tempNode->getChildren(n_it));
			}
		}
	}
	return true;
}

bool PcsDBufferOctree::getTargetOctreeByClearReference()
{
	assert(targetFrameLoaded);

	if (referenceFrameLoaded)
		clearOneFrame(false);

	return true;
}

void PcsDBufferOctree::getNextTwoFrame(ObjMesh * newObjMesh)
{
	assert(targetFrameLoaded && referenceFrameLoaded);
	//����target frame ��reference frame����Ϊtarget ����Ҫ�����reference���ɵ�referenceû�����ˡ�
	if (targetFrameId == 0)
		setTargetFrameId(1);
	else
		setTargetFrameId(0);

	//ɾ����ʱ��target frame
	clearOneFrame(true);

	//�����µ�target frame
	buildDBufferOctree(true, newObjMesh);
}

void PcsDBufferOctree::getNextTwoFrame(vector<Vertex>& newVertexList)
{
	assert(targetFrameLoaded && referenceFrameLoaded);
	//����target frame ��reference frame����Ϊtarget ����Ҫ�����reference���ɵ�referenceû�����ˡ�
	if (targetFrameId == 0)
		setTargetFrameId(1);
	else
		setTargetFrameId(0);

	//ɾ����ʱ��target frame
	clearOneFrame(true);

	//�����µ�target frame
	buildDBufferOctree(true, newVertexList);

}

TraverseAllNodeForFree::TraverseAllNodeForFree(bool isTarget)
{
	this->isTarget = isTarget;
}

TraverseAllNodeForFree::~TraverseAllNodeForFree()
{

}

bool TraverseAllNodeForFree::operator()(const Vec3 min, const Vec3 max, OctreeDoubelBufferNode<DBufferNodeData>* currNode)
{
	//�����Ҷ�ӽڵ㣬��ô��ֹ��Ҷ�ӽڵ�����һ��������
	if (currNode->judegeLeafNode())
		return true;

	//�������node�ϵ��ӽڵ�����õ�����
	if (isTarget)
	{
		//�жϰ˸��ӽڵ�
		for (int i = 0; i < 8; i++)
		{
			//�����Node�� �Ƿ���target������
			if (currNode->flag[currNode->getTargetFlagIdx()][i])
			{
				//�ж�����ӽڵ�����û��reference�ϵ�����
				if (currNode->flag[currNode->getReferenceFlagIdx()][i])
				{
					//��reference�ϵ����ݣ���ôֻҪ����flag��Ϣ��Ȼ���ͷ�NodeDataָ��
					currNode->flag[currNode->getTargetFlagIdx()][i] = false;
					delete currNode->getTargetFrameNodeDataPtr();
				}
				else
				{
					//��ôֱ���ͷ������ӽڵ�
					delete  currNode->getChildren(i);
				}
			}
		}
	}
	else
	{
		//ɾ��reference   �жϰ˸��ӽڵ�
		for (int i = 0; i < 8; i++)
		{
			//�����Node�� �Ƿ���target������
			if (currNode->flag[currNode->getReferenceFlagIdx()][i])
			{
				//�ж�����ӽڵ�����û��reference�ϵ�����
				if (currNode->flag[currNode->getTargetFlagIdx()][i])
				{
					//��reference�ϵ����ݣ���ôֻҪ����flag��Ϣ��Ȼ���ͷ�NodeDataָ��
					currNode->flag[currNode->getReferenceFlagIdx()][i] = false;
					delete currNode->getRefrenceFrameNodeDataPtr();
				}
				else
				{
					//��ôֱ���ͷ������ӽڵ�
					delete  currNode->getChildren(i);
				}
			}
		}
	}
	return true;
}

TraverseGetInfoSetLeaf::TraverseGetInfoSetLeaf(bool isTarget_in)
{
	minVList.clear();
	maxVList.clear();
	nodeList.clear();
	leafIncr = 0;
	isTarget = isTarget_in;
}

TraverseGetInfoSetLeaf::~TraverseGetInfoSetLeaf()
{
	//nodelist�����ݵ�ָ����Node�ڹ������ﲻ��ɾ��
}

void TraverseGetInfoSetLeaf::setIsTarget(bool flag)
{
	this->isTarget = flag;
}

void TraverseGetInfoSetLeaf::initParam()
{
	minVList.clear();
	maxVList.clear();
	nodeList.clear();
	leafIncr = 0;
}

bool TraverseGetInfoSetLeaf::operator()(const Vec3 min, const Vec3 max, OctreeDoubelBufferNode<DBufferNodeData>* currNode)
{
	bool flag = true;
	//�ж��ǻ����Ҷ�ӽڵ�
	/*if (currNode->getChildren(0) || currNode->getChildren(1) || currNode->getChildren(2) || currNode->getChildren(3)
		|| currNode->getChildren(4) || currNode->getChildren(5) || currNode->getChildren(6) || currNode->getChildren(7))*/

	int idx = -1;
	if (isTarget)
		idx = currNode->getTargetFlagIdx();
	else
		idx = currNode->getReferenceFlagIdx();

	if (currNode->flag[idx][0] || currNode->flag[idx][1] || currNode->flag[idx][2] || currNode->flag[idx][3]
		|| currNode->flag[idx][4] || currNode->flag[idx][5] || currNode->flag[idx][6] || currNode->flag[idx][7])
	{
		flag = true;
	}
	else
	{
		//��Ҷ�ӽڵ㣬��ô����Ҷ�ӽڵ�ľ��η�Χ��Ȼ���˳�
		flag = false;
		minVList.push_back(currNode->getMinPos());
		maxVList.push_back(currNode->getMaxPos());

		//����Ҷ�ӽڵ������
		nodeList.push_back(currNode);
		currNode->setLeafFlag(leafIncr);
		leafIncr++;
	}
	return flag;
}
