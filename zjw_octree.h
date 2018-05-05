#pragma once

/*!
 * \file zjw_octree.h
 *
 * \author King
 * \date ���� 2018
 *  this head file and the implements c++ souce code has set the basis octree build and opration
 *  interface. which implements in real application should look at zjw_pcs_octree.h
 */

#include <assert.h>
#include "util/zjw_macro.h"
#include <util/zjw_math.h>

#define COMPUTE_SIDE(i, bit, p, mid, newMin, newMax) \
if (p >= mid)         \
{                     \
    i |= bit;         \
    newMin = mid;     \
}                     \
else                  \
{                     \
    newMax = mid;     \
}

 //�����˽ڵ���������Ϊ N �İ˲���
template <class N>
class Octree
{
public:
	//------------define �˲����Ľ�� use in the tree. ---------------
	class OctreeNode
	{
	public:
		//����ڵ�����ݣ��Լ�ָ��˸����ӵĽ��ָ��
		N nodeData;
		//��Ҷ�ӽڵ���-1�������Ĵ�0,1,2,3һ�α��
		int leafFlag;
		//min�Ķ����Max�Ķ���ȷ�� �ڵ�Ŀռ䷶Χ
		Vec3 min;
		Vec3 max;
		OctreeNode* children[8];

	public:
		OctreeNode()
		{
			leafFlag = -1;
			//��ʼ���˸����ӵĽ��
			for (int i = 0; i < 8; i++)
				children[i] = 0;
		}

		virtual ~OctreeNode()
		{
			for (int i = 0; i < 8; i++)
				if (children[i])
					delete children[i];
		}
	};

	//min�Ķ����Max�Ķ���ȷ�� �ڵ�Ŀռ䷶Χ
	Vec3 min;
	Vec3 max;
	//// cellSize is the minimum subdivided cell size.
	Vec3 cellSize;
	OctreeNode* root;

public:
	Octree(Vec3 min, Vec3 max, Vec3 cellSize) : min(min), max(max), cellSize(cellSize), root(0) {}
	virtual ~Octree() { delete root; }

	//�������е�Octree����������һ���ࡣ
	class Callback
	{
	public:
		// Return value: true = continue; false = abort.
		virtual bool operator()(const Vec3 min, const Vec3 max, N& nodeData) { return true; }

		// Return value: true = continue; false = abort.
		virtual bool operator()(const Vec3 min, const Vec3 max, OctreeNode* currNode) { return true; }
	};

	//������ȱ����ķ������ѵ�pos���뵽�˲�����Ҷ�ӽڵ��У�ֱ��Ҷ�ӽڵ��sizeС�� cellSize.
	//N& getCell(const float pos[3], Callback* callback = NULL)
	N& getCell(const Vec3 ppos, Callback* callback = NULL)
	{
		//Point3f ppos(pos);
#ifdef ZJW_DEBUG
//		cout << (ppos >= min) << endl;
	//	cout << (ppos < max) << endl;
#endif
		assert(ppos >= min && ppos < max);
		Vec3 currMin(min);
		Vec3 currMax(max);
		Vec3 delta = max - min;
		if (!root)
			root = new OctreeNode();
		OctreeNode* currNode = root;
		while (delta >= cellSize)
		{
			bool shouldContinue = true;
			if (callback)
				shouldContinue = callback->operator()(currMin, currMax, currNode->nodeData);
			if (!shouldContinue)
				break;
			Vec3 mid = (delta * 0.5f) + currMin;
			Vec3 newMin(currMin);
			Vec3 newMax(currMax);
			int index = 0;
			COMPUTE_SIDE(index, 1, ppos.x, mid.x, newMin.x, newMax.x)
				COMPUTE_SIDE(index, 2, ppos.y, mid.y, newMin.y, newMax.y)
				COMPUTE_SIDE(index, 4, ppos.z, mid.z, newMin.z, newMax.z)
				if (!(currNode->children[index]))
					currNode->children[index] = new OctreeNode();

			currNode = currNode->children[index];
			//ÿ���ڵ㱣���Լ��ķ�Χ
			currNode->min = newMin;
			currNode->max = newMax;

			currMin = newMin;
			currMax = newMax;
			delta = currMax - currMin;
		}
		return currNode->nodeData;
	}

	//�ݹ����������.������ĳ�ֲ�����
	void traverse(Callback* callback)
	{
		assert(callback);
		traverseRecursive(callback, min, max, root);
	}

	//ɾ�����������ͷ����нڵ�Ŀռ�
	void clear()
	{
		delete root;
		root = NULL;
	}

	//�������ڲ���
	class Iterator
	{
	public:
		Iterator getChild(int i)
		{
			return Iterator(currNode->children[i]);
		}
		N* getData()
		{
			if (currNode)
				return &currNode->_nodeData;
			else return NULL;
		}
	protected:
		//��ǰnode��ָ��
		OctreeNode* currNode;

		Iterator(OctreeNode* node) : currNode(node) {}
		friend class Octree;
	};

	Iterator getIterator()
	{
		return Iterator(root);
	}

protected:

	//�ݹ����������
	void traverseRecursive(Callback* callback, const Vec3& currMin, const Vec3& currMax, OctreeNode* currNode)
	{
		//Ϊ�շ���
		if (!currNode)
			return;
		//�ж��Ƿ����������ֹ����
		bool shouldContinue = callback->operator()(currMin, currMax, currNode);
		if (!shouldContinue)
			return;

		//mid �õ�cell�м�ĵ�
		Vec3 delta = currMax - currMin;
		Vec3 mid = (delta * 0.5f) + currMin;
		//�ֳ��Ӱ˸��ڵ�
		traverseRecursive(callback, currMin, mid, currNode->children[0]);
		traverseRecursive(callback, Vec3(mid.x, currMin.y, currMin.z), Vec3(currMax.x, mid.y, mid.z), currNode->children[1]);
		traverseRecursive(callback, Vec3(currMin.x, mid.y, currMin.z), Vec3(mid.x, currMax.y, mid.z), currNode->children[2]);
		traverseRecursive(callback, Vec3(mid.x, mid.y, currMin.z), Vec3(currMax.x, currMax.y, mid.z), currNode->children[3]);
		traverseRecursive(callback, Vec3(currMin.x, currMin.y, mid.z), Vec3(mid.x, mid.y, currMax.z), currNode->children[4]);
		traverseRecursive(callback, Vec3(mid.x, currMin.y, mid.z), Vec3(currMax.x, mid.y, currMax.z), currNode->children[5]);
		traverseRecursive(callback, Vec3(currMin.x, mid.y, mid.z), Vec3(mid.x, currMax.y, currMax.z), currNode->children[6]);
		traverseRecursive(callback, mid, currMax, currNode->children[7]);
	}
};
