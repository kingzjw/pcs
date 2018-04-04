#pragma once
#include "zjw_pcs_octree.h"



PcsOctree::PcsOctree()
{
	Vec3 min( 0.0f, 0.0f, 0.0f );
	Vec3 max(1.0f + Epsilon, 1.0f + Epsilon, 1.0f + Epsilon);
	Vec3 cellSize( 0.1, 0.1, 0.1 );

	ct = new CallbackTraverse;
	pcsOct = new Octree<Node>(min, max, cellSize);
}

PcsOctree::~PcsOctree()
{
	delete pcsOct;
	delete ct;
}

void PcsOctree::setParam(Vec3 min, Vec3 max, Vec3 cellSize)
{
	this->min = min;
	this->max = max;
	this->cellSize = cellSize;
	if (!pcsOct)
	{
		pcsOct = new Octree<Node>(min, max, cellSize);
	}
	else
	{
		delete pcsOct;
		pcsOct = new Octree<Node>(min, max, cellSize);
	}
}

void PcsOctree::buildPcsOctFrmPC(ObjMesh * objeMesh)
{
	cout << endl << "Building octree...." << endl;
	// Initialize octree.
	// Minimum coordinates.
	// Add all particles to the octree.
	for (int v_it = 0; v_it < objeMesh->vertexList.size(); v_it++)
	{
		Node& n = pcsOct->getCell(objeMesh->vertexList[v_it]);
		objeMesh->vertexList[v_it];
		n.pointList.push_back(objeMesh->vertexList[v_it]);
	}
	cout << "Building octree done." << endl;
	return;
}

void PcsOctree::getLeafboundary()
{
	cout << "Computing Leavf boundary...." << endl;
	
	// Prepare the callback class.
	pcsOct->traverse(ct);

#ifdef ZJW_DEUG
	cout << "Computing Leavf boundary done!" << endl;
#endif // ZJW_DEUG

}



void PcsOctree::clearOct()
{
	if (pcsOct != nullptr)
		pcsOct->clear();
}

CallbackTraverse::CallbackTraverse()
{
	minVList.clear();
	maxVList.clear();
}

//bool CallbackTraverse::operator()(const float min[3], const float max[3], Node & n)
//{
//	return true;
//}
//
//bool CallbackTraverse::operator()(const float min[3], const float max[3], OctreeNode<Node>* currNode)
//{
//	bool flag = true;
//	//判断是会否是叶子节点
//	if (currNode->children[0] || currNode->children[1] || currNode->children[2] || currNode->children[3]
//		|| currNode->children[4] || currNode->children[5] || currNode->children[6] || currNode->children[7])
//	{
//		flag = true;
//	}
//	else
//	{
//		//是叶子节点，那么保留叶子节点的矩形范围，然后退出
//		flag = false;
//		for (int i = 0; i < currNode->nodeData.pointList.size(); i++)
//		{
//			minVList.push_back(currNode->min);
//			maxVList.push_back(currNode->max);
//		}
//	}
//	return flag;
//}