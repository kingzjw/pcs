#include "zjw_pcs_double_buffer_octree.h"

void PcsDBufferOctree::setTargetFrameId(int id)
{
	assert(id == 0 || id == 1);
	//一个是0，一个是1
	targetFrameId = id;
	refrenceFrameId = (id + 1) % 2;

	pcsDBufferOct->setTargetFrameId(id);
}

void PcsDBufferOctree::setReferenceFrameId(int id)
{
	assert(id == 0 || id == 1);
	//一个是0，一个是1
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
	//如果已经加载过了，那么需要现清理
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

	//现在加载Frame
	for (int v_it = 0; v_it < objMesh->vertexList.size(); v_it++)
	{
		//利用深度优先，创建八叉树
		DBufferNodeData node = pcsDBufferOct->getCell(isTarget, objMesh->vertexList[v_it]);

		//把点放到叶子节点所属的里面。
		node.pointPosList.push_back(objMesh->vertexList[v_it]);
		node.pointIdxList.push_back(v_it);
	}
}

void PcsDBufferOctree::buildDBufferOctree(bool isTarget, vector<Vertex>& vertexList)
{
	//如果已经加载过了，那么需要现清理
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

	//现在加载Frame
	for (int v_it = 0; v_it < vertexList.size(); v_it++)
	{
		//利用深度优先，创建八叉树
		DBufferNodeData node = pcsDBufferOct->getCell(isTarget, vertexList[v_it]);

		//把点放到叶子节点所属的里面。
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
	
	//遍历所有的 结点，从下到上，删除无用的节点，或者是free NodeData(这里还要改写flag)
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
			//孩子节点不为空的，放入到队列中
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
	//交换target frame 和reference frame，因为target 马上要变成了reference，旧的reference没有用了。
	if (targetFrameId == 0)
		setTargetFrameId(1);
	else
		setTargetFrameId(0);

	//删除此时的target frame
	clearOneFrame(true);

	//加载新的target frame
	buildDBufferOctree(true, newObjMesh);
}

void PcsDBufferOctree::getNextTwoFrame(vector<Vertex>& newVertexList)
{
	assert(targetFrameLoaded && referenceFrameLoaded);
	//交换target frame 和reference frame，因为target 马上要变成了reference，旧的reference没有用了。
	if (targetFrameId == 0)
		setTargetFrameId(1);
	else
		setTargetFrameId(0);

	//删除此时的target frame
	clearOneFrame(true);

	//加载新的target frame
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
	//如果是叶子节点，那么终止。叶子节点由上一层来处理
	if (currNode->judegeLeafNode())
		return true;

	//清理这个node上的子节点的无用的数据
	if (isTarget)
	{
		//判断八个子节点
		for (int i = 0; i < 8; i++)
		{
			//这个子Node上 是否有target的数据
			if (currNode->flag[currNode->getTargetFlagIdx()][i])
			{
				//判断这个子节点上有没有reference上的数据
				if (currNode->flag[currNode->getReferenceFlagIdx()][i])
				{
					//有reference上的数据，那么只要重制flag信息，然后释放NodeData指针
					currNode->flag[currNode->getTargetFlagIdx()][i] = false;
					delete currNode->getTargetFrameNodeDataPtr();
				}
				else
				{
					//那么直接释放整个子节点
					delete  currNode->getChildren(i);
				}
			}
		}
	}
	else
	{
		//删除reference   判断八个子节点
		for (int i = 0; i < 8; i++)
		{
			//这个子Node上 是否有target的数据
			if (currNode->flag[currNode->getReferenceFlagIdx()][i])
			{
				//判断这个子节点上有没有reference上的数据
				if (currNode->flag[currNode->getTargetFlagIdx()][i])
				{
					//有reference上的数据，那么只要重制flag信息，然后释放NodeData指针
					currNode->flag[currNode->getReferenceFlagIdx()][i] = false;
					delete currNode->getRefrenceFrameNodeDataPtr();
				}
				else
				{
					//那么直接释放整个子节点
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
	//nodelist中内容的指针是Node在管理，这里不用删除
}

bool TraverseGetInfoSetLeaf::operator()(const Vec3 min, const Vec3 max, OctreeDoubelBufferNode<DBufferNodeData>* currNode)
{
	bool flag = true;
	//判断是会否是叶子节点
	if (currNode->getChildren(0) || currNode->getChildren(1) || currNode->getChildren(2) || currNode->getChildren(3)
		|| currNode->getChildren(4) || currNode->getChildren(5) || currNode->getChildren(6) || currNode->getChildren(7))
	{
		flag = true;
	}
	else
	{
		//是叶子节点，那么保留叶子节点的矩形范围，然后退出
		flag = false;
		minVList.push_back(currNode->getMinPos());
		maxVList.push_back(currNode->getMaxPos());

		//保存叶子节点的中间
		//Vec3 temp = (currNode->min + currNode->max) / 2;
		//midVList->push_back(temp);
		nodeList.push_back(currNode);
		currNode->setLeafFlag(leafIncr);
		leafIncr++;
	}
	return flag;
}
