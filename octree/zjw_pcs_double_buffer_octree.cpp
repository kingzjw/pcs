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
	this->minPos = minPos;
	this->maxPos = maxPosVec3;
	this->cellSize = cellSize;

	targetFrameLoaded = false;
	referenceFrameLoaded = false;

	int targetFrameId = 0;
	int refrenceFrameId = 1;
	pcsDBufferOct = new DoubleBufferOctree<DBufferNodeData>(minPos, maxPosVec3, cellSize, targetFrameId, refrenceFrameId);
}

PcsDBufferOctree::~PcsDBufferOctree()
{
	if(pcsDBufferOct)
		delete pcsDBufferOct;
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

void PcsDBufferOctree::buildDBufferOctree(bool isTarget, ObjMesh * objMesh)
{
	//����Ѿ����ع��ˣ���ô��Ҫ������
	if (isTarget && targetFrameLoaded)
	{
		clearOneFrame(isTarget);
	}
	else if((!isTarget)&&referenceFrameLoaded)
	{
		clearOneFrame(isTarget);
	}

	if (isTarget)
	{
		targetFrameLoaded = true;
	}
	else
	{
		referenceFrameLoaded = true;
	}

	//���ڼ���Frame
	for (int v_it = 0; v_it < objMesh->vertexList.size(); v_it++)
	{
		//����������ȣ������˲���
		DBufferNodeData node = pcsDBufferOct->getCell(isTarget, objMesh->vertexList[v_it]);

		//�ѵ�ŵ�Ҷ�ӽڵ����������档
		node.pointPosList.push_back(objMesh->vertexList[v_it]);
		node.pointIdxList.push_back(v_it);
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
		targetFrameLoaded = true;
	}
	else
	{
		referenceFrameLoaded = true;
	}

	//���ڼ���Frame
	for (int v_it = 0; v_it < vertexList.size(); v_it++)
	{
		//����������ȣ������˲���
		DBufferNodeData node = pcsDBufferOct->getCell(isTarget, vertexList[v_it]);

		//�ѵ�ŵ�Ҷ�ӽڵ����������档
		node.pointPosList.push_back(vertexList[v_it]);
		node.pointIdxList.push_back(v_it);
	}
}

void PcsDBufferOctree::clearOneFrame(bool isTarget)
{
	if (isTarget)
		targetFrameLoaded = false;
	else
		referenceFrameLoaded = false;
	
	//�������е� ��㣬���µ��ϣ�ɾ�����õĽڵ㣬������free NodeData(���ﻹҪ��дflag)
	TraverseAllNodeForFree freeTraverse(isTarget);
	pcsDBufferOct->traverseDepthFirst(&freeTraverse);
}

bool PcsDBufferOctree::getByteSteamOfOctree(vector<char>& byteList_out)
{
	byteList_out.clear();

	assert(targetFrameLoaded && referenceFrameLoaded);
	vector<OctreeDoubelBufferNode<DBufferNodeData>*>  allNodeList_out;
	pcsDBufferOct->traverseBreathFirst(allNodeList_out);

	for (int i = 0; i < allNodeList_out.size(); i++)
	{
		byteList_out.push_back(allNodeList_out[i]->getChildByte());
	}
	return true;
}

void PcsDBufferOctree::clearDBufferOctree()
{
	//clear
	if (pcsDBufferOct)
		delete pcsDBufferOct;

	//init
	targetFrameLoaded = false;
	referenceFrameLoaded = false;
	int targetFrameId = 0;
	int refrenceFrameId = 1;
	new DoubleBufferOctree<DBufferNodeData>(minPos, maxPos, cellSize, targetFrameId, refrenceFrameId);
}

bool PcsDBufferOctree::recoveryDBufferOctree(vector<Vertex>& swapFrameVerList, vector<char>& byteList)
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

bool PcsDBufferOctree::getTargetOctreeByclearReference()
{
	assert(targetFrameLoaded);
	
	if(referenceFrameLoaded)
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

TraverseGetInfoSetLeaf::TraverseGetInfoSetLeaf()
{
	minVList.clear();
	maxVList.clear();
	leafIncr = 0;
}

TraverseGetInfoSetLeaf::~TraverseGetInfoSetLeaf()
{
	//nodelist�����ݵ�ָ����Node�ڹ������ﲻ��ɾ��
}

bool TraverseGetInfoSetLeaf::operator()(const Vec3 min, const Vec3 max, OctreeDoubelBufferNode<DBufferNodeData>* currNode)
{
	bool flag = true;
	//�ж��ǻ����Ҷ�ӽڵ�
	if (currNode->getChildren(0) || currNode->getChildren(1) || currNode->getChildren(2) || currNode->getChildren(3)
		|| currNode->getChildren(4) || currNode->getChildren(5) || currNode->getChildren(6) || currNode->getChildren(7))
	{
		flag = true;
	}
	else
	{
		//��Ҷ�ӽڵ㣬��ô����Ҷ�ӽڵ�ľ��η�Χ��Ȼ���˳�
		flag = false;
		minVList.push_back(currNode->getMinPos());
		maxVList.push_back(currNode->getMaxPos());

		//����Ҷ�ӽڵ���м�
		//Vec3 temp = (currNode->min + currNode->max) / 2;
		//midVList->push_back(temp);
		nodeList.push_back(currNode);
		currNode->setLeafFlag(leafIncr);
		leafIncr++;
	}
	return flag;
}
