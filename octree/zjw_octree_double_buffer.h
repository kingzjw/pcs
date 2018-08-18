#pragma once
/*!
* \file zjw_octree.h
*
* \author King
* \date 7�� 2018
*  this head file and the implements c++ souce code has set the basis octree build and opration
*  interface. which implements in real application should look at zjw_pcs_octree.h
*/

#include <assert.h>
#include <vector>
#include "util/zjw_macro.h"
#include <util/zjw_math.h>

/*
* ���ã�
*	����꣬�õ�����������λ��
* 
* ���룺
*	bit: ��ʾ��x,y,z�е���һά�Ƚ����ж�
*   p: ��ʾ�ĵ�ǰ��Ҫ�����жϵĵ���֡�еĶ���
*   mid: ��ʾ���ǵ�ǰNode���е�����꣬��Ҫ�����������ֳ�8������
* �����
*	i: ��ʾ��ǰp���㣬Ӧ���ڵ�ǰ��node�ϵ���һ������
*   newMin: ��ʾ�µ����ޱ߽����С���ֵ
*   newMax: ��ʾ�µ����ޱ߽�������ֵ
*/

#define COMPUTE_QUADRANT(i, bit, p, mid, newMin, newMax) \
if (p >= mid)         \
{                     \
    i |= bit;         \
    newMin = mid;     \
}                     \
else                  \
{                     \
    newMax = mid;     \
}

////////////////////////////////////////////////////////////////////////////////////////////////
//double buffer�İ˲����Ľ��
////////////////////////////////////////////////////////////////////////////////////////////////

template <class NodeDataType>
class DoubleBufferOctree;

template <class NodeDataType>
class OctreeDoubelBufferNode
{
private:
	
	//���reference target frame��ָ��
	DoubleBufferOctree<NodeDataType> * oct;

	//��Ҷ�ӽڵ���-1�������Ĵ�0,1,2,3һ�α��.�������Ҫ��ʹ�ã���Ҫ�ָ���ֵ��
	int leafFlag;
	
	//��ǰ�ڵ�ı߽�ֵ����Сֵ�����ֵ
	Vec3 min;
	Vec3 max;


public:
	//Ԫ����ָ��
	OctreeDoubelBufferNode* children[8];
	//����
	NodeDataType * nodeData[2];
	//���
	bool flag[2][8];

public:
	OctreeDoubelBufferNode(DoubleBufferOctree<NodeDataType> * oct);

	/** isTarget: ��ʲônode���ͣ�Ȼ����Ҫ����Ӧ��nodeData ��������	*/
	OctreeDoubelBufferNode(DoubleBufferOctree<NodeDataType> * oct, bool isTarget);
	virtual ~OctreeDoubelBufferNode();

	NodeDataType* getTargetFrameNodeDataPtr();
	NodeDataType* getRefrenceFrameNodeDataPtr();

	//���õ�ǰ�ڵ㣬���ӽڵ㣨true��
	int getTargetFlagIdx();
	int getReferenceFlagIdx();
	void setFlag(bool isTarget, int index);
	//���õ�ǰ�ڵ㣬û���ӽڵ㣨false��
	void resetFlag(bool isTarget, int index);
	//��ʼ��ָ����Flag
	void initFlag(bool isTarget);
	//��ʼ�����е�falg
	void initFlag();

	//��ʼ�� �ض�֡�� nodedata
	NodeDataType * getNodeData(bool isTarget);
	void setNodeData(bool isTarget);
	void resetNodeData(bool isTarget);
	//���nodeData�Ƿ񴴽����� true:��ʾNull
	bool checkNodeDataNull(bool isTarget);

	//�õ����ӵ�ָ��,����ǿ�ָ�룬һ��Ҫ�ر�ע�⣬���ܰѿ�ָ�븳ֵ��һ����ָ��//
	OctreeDoubelBufferNode* getChildren(int index);
	Vec3 getMinPos();
	Vec3 getMaxPos();
	void setMinPos(Vec3 minP);
	void setMaxPos(Vec3 maxP);
	void setLeafFlag(int i);

	//�ж����node�Ƿ���Ҷ�ӽڵ�
	bool judegeLeafNode();

	//XOR����flag�е����ݣ���������byte
	char getChildByteXOR();
	char getChildByteSigleFrame(bool isTarget);

	//������һ֡���������
	void resetFrame(bool isTarget);
};

////////////////////////////////////////////////////////////////////////////////////////////////
//double buffer �� �˲���
////////////////////////////////////////////////////////////////////////////////////////////////

template <class NodeDataType>
class DoubleBufferOctree
{
public:
	friend class OctreeDoubelBufferNode<NodeDataType>;

	//min�Ķ����Max�Ķ���ȷ�� �ڵ�Ŀռ䷶Χ
	Vec3 min;
	Vec3 max;
	// cellSize is the minimum subdivided cell size.
	Vec3 cellSize;
	//���ڵ�
	OctreeDoubelBufferNode<NodeDataType>* root;

	//˫Buffer����ű�־
	int targetFrameId;
	int refrenceFrameId;

public:

	DoubleBufferOctree(Vec3 min, Vec3 max, Vec3 cellSize,int targetFrameId = 1 , int refrenceFrameId = 0) ;
	virtual ~DoubleBufferOctree();

	void setTargetFrameId(int id);
	void setReferenceFrameId(int id);
	
	//����ָ�����׽ڵ�Ļ��ǽڵ㣬������������ӽڵ��ָ��
	OctreeDoubelBufferNode<NodeDataType>* createChildNode(bool isTarget,OctreeDoubelBufferNode<NodeDataType>* parentNode, int childIdx);
	
	//decode: �����ֽڣ��ָ����node�ĺ�����Ϣ��ǰ����db octree�ǿյģ�û���κ���Ϣ
	bool recoveryDBufferOctreeNodeForSigleFrame(OctreeDoubelBufferNode<NodeDataType>* parentNode, const char byte, bool isTarget =false);

	//decode: �����ֽڣ��ָ����node�ĺ�����Ϣ��ǰ�����Ѿ��е�֡��������˲������ˣ����ڻָ�����һ֡
	bool recoveryDBufferOctreeNode(OctreeDoubelBufferNode<NodeDataType>* parentNode, const char byteXOR);

	
	//////////////////////////////////////////////
	//�������е�Octree����������һ���ࡣ
	//////////////////////////////////////////////

	class Callback
	{
	public:
		// Return value: true = continue; false = abort.
		virtual bool operator()(const Vec3 min, const Vec3 max, NodeDataType& nodeData) { return true; }

		// Return value: true = continue; false = abort.
		virtual bool operator()(const Vec3 min, const Vec3 max, OctreeDoubelBufferNode<NodeDataType>* currNode) { return true; }
	};

	//������ȱ����ķ��������ض���pos���뵽�˲�����Ҷ�ӽڵ��У�ֱ��Ҷ�ӽڵ��sizeС�� cellSize.
	/** ��Ϊ��double buffer��������Ҫ��target ������reference ����ָ��
	* ���ص���Ҷ�ӽڵ��е�Node�����Խ���Ҷ�ӽڵ�Ĵ���
	* */
	NodeDataType* getCell(bool isTarget, const Vec3 ppos, Callback* callback = NULL);

	//������ȵݹ����������.������ĳ�ֲ��������Ȳ��������룬�������������жϵģ�
	void traverse(Callback* callback);
	//������ȱ��������������еĽڵ㶼����������������Ͻ��в�����	
	void traverseDepthFirst(Callback* callback);
	//������ȱ������нڵ�,�õ������б�Ĳ���
	void traverseBreathFirst(std::vector< OctreeDoubelBufferNode<NodeDataType>*> &allNodeList_out);
	void traverseBreathFirstSigleFrame(std::vector< OctreeDoubelBufferNode<NodeDataType>*> &allNodeList_out,bool isTarget_in);

	//ɾ�����������ͷ����нڵ�Ŀռ�
	void clear();
	

protected:

	//��traverse��������ʹ��
	void depthFirstTraverseRecursive(Callback* callback, const Vec3& currMin, const Vec3& currMax, OctreeDoubelBufferNode<NodeDataType>* currNode);
	
	//��traverseDepthFirst����ʹ��
	void traverseDepthFirstRecursive(Callback* callback, const Vec3& currMin, const Vec3& currMax, OctreeDoubelBufferNode<NodeDataType>* currNode);

};


/////////////////////////////////////////////////////////////////////////////////////////


template<class NodeDataType>
OctreeDoubelBufferNode<NodeDataType>::OctreeDoubelBufferNode(DoubleBufferOctree<NodeDataType> * oct)
{
	this->oct = oct;

	leafFlag = -1;
	//��ʼ������node data
	nodeData[0] = 0;
	nodeData[1] = 0;

	//��ʼ���˸����ӵĽ��
	for (int i = 0; i < 8; i++)
		children[i] = 0;

	initFlag();

}

template<class NodeDataType>
OctreeDoubelBufferNode<NodeDataType>::OctreeDoubelBufferNode(DoubleBufferOctree<NodeDataType> * oct, bool isTarget)
{
	this->oct = oct;

	leafFlag = -1;
	
	//��ʼ������node data
	nodeData[0] = 0;
	nodeData[1] = 0;

	//��ʼ���˸����ӵĽ��
	for (int i = 0; i < 8; i++)
		children[i] = 0;

	initFlag();
	setNodeData(isTarget);
}

template<class NodeDataType>
inline OctreeDoubelBufferNode<NodeDataType>::~OctreeDoubelBufferNode()
{
	for (int i = 0; i < 8; i++)
	{
		if (children[i])
			delete children[i];
	}

	if (nodeData[0])
		delete nodeData[0];

	if (nodeData[1])
		delete nodeData[1];
}

template<class NodeDataType>
void OctreeDoubelBufferNode<NodeDataType>::initFlag()
{
	//init flag
	for (int i = 0; i < 8; i++)
	{
		flag[0][i] = 0;
		flag[1][i] = 0;
	}
}

template<class NodeDataType>
void OctreeDoubelBufferNode<NodeDataType>::initFlag(bool isTarget)
{
	if (isTarget)
	{
		for (int i = 0; i < 8; i++)
		{
			flag[oct->targetFrameId][i] = 0;
		}
	}
	else
	{
		for (int i = 0; i < 8; i++)
		{
			flag[oct->refrenceFrameId][i] = 0;
		}
	}
}

template<class NodeDataType>
NodeDataType * OctreeDoubelBufferNode<NodeDataType>::getNodeData(bool isTarget)
{
	if (isTarget)
		return nodeData[oct->targetFrameId];
	else
		return nodeData[oct->refrenceFrameId];
}

template<class NodeDataType>
void OctreeDoubelBufferNode<NodeDataType>::setNodeData(bool isTarget)
{
	if (isTarget)
		nodeData[oct->targetFrameId] = new NodeDataType();
	else
		nodeData[oct->refrenceFrameId] = new NodeDataType();
}

template<class NodeDataType>
void OctreeDoubelBufferNode<NodeDataType>::resetNodeData(bool isTarget)
{
	if (isTarget)
	{
		if (nodeData[oct->targetFrameId])
			delete nodeData[oct->targetFrameId];
	}
	else
	{
		if (nodeData[oct->refrenceFrameId])
			delete nodeData[oct->refrenceFrameId];
	}
}

template<class NodeDataType>
bool OctreeDoubelBufferNode<NodeDataType>::checkNodeDataNull(bool isTarget)
{
	if (isTarget)
	{
		if (nodeData[oct->targetFrameId])
			return false;
	}
	else
	{
		//test
		//cout << oct << endl;
		//cout << oct->refrenceFrameId << endl;
		//end test
		if (nodeData[oct->refrenceFrameId])
			return false;
	}
	return true;
}

template<class NodeDataType>
OctreeDoubelBufferNode<NodeDataType> * OctreeDoubelBufferNode<NodeDataType>::getChildren(int index)
{
	return children[index];
}

template<class NodeDataType>
Vec3 OctreeDoubelBufferNode<NodeDataType>::getMinPos()
{
	return this->min;
}

template<class NodeDataType>
Vec3 OctreeDoubelBufferNode<NodeDataType>::getMaxPos()
{
	return this->max;
}

template<class NodeDataType>
inline void OctreeDoubelBufferNode<NodeDataType>::setMinPos(Vec3 minP)
{
	this->min = minP;
}

template<class NodeDataType>
inline void OctreeDoubelBufferNode<NodeDataType>::setMaxPos(Vec3 maxP)
{
	this->max = maxP;
}

template<class NodeDataType>
void OctreeDoubelBufferNode<NodeDataType>::setLeafFlag(int i)
{
	leafFlag = i;
}

template<class NodeDataType>
bool OctreeDoubelBufferNode<NodeDataType>::judegeLeafNode()
{
	if (this->getChildren(0) || this->getChildren(1) || this->getChildren(2) || this->getChildren(3)
		|| this->getChildren(4) || this->getChildren(5) || this->getChildren(6) || this->getChildren(7))
	{
		return false;
	}
	return true;
}

template<class NodeDataType>
char OctreeDoubelBufferNode<NodeDataType>::getChildByteXOR()
{
	bitset<8> b;

	for (int i = 0; i < 8; i++)
	{
		if (flag[0][i] == flag[1][i])
		{
			b.set(i, 0);
		}
		else
		{
			b.set(i, 1);
		}
	}
	char ch = int(b.to_ulong());
	return ch;
}

template<class NodeDataType>
inline char OctreeDoubelBufferNode<NodeDataType>::getChildByteSigleFrame(bool isTarget)
{
	int  idx = -1;
	if (isTarget)
	{
		idx = getTargetFlagIdx();
	}
	else
	{
		idx = getReferenceFlagIdx();
	}

	bitset<8> b;
	b.reset();
	for (int i = 0; i < 8; i++)
	{
		if (flag[idx][i])
		{
			b.set(i, 1);
		}
	}
	return int(b.to_ulong());
}

template<class NodeDataType>
inline NodeDataType * OctreeDoubelBufferNode<NodeDataType>::getTargetFrameNodeDataPtr()
{
	if (oct->targetFrameId == 0)
		return nodeData[0];
	else if (oct->targetFrameId == 1)
		return nodeData[1];
}

template<class NodeDataType>
inline NodeDataType * OctreeDoubelBufferNode<NodeDataType>::getRefrenceFrameNodeDataPtr()
{
	if (oct->refrenceFrameId == 0)
		return nodeData[0];
	else if (oct->refrenceFrameId == 1)
		return nodeData[1];
}

template<class NodeDataType>
int OctreeDoubelBufferNode<NodeDataType>::getTargetFlagIdx()
{
	return oct->targetFrameId;
}

template<class NodeDataType>
int OctreeDoubelBufferNode<NodeDataType>::getReferenceFlagIdx()
{
	return oct->refrenceFrameId;
}

template<class NodeDataType>
void OctreeDoubelBufferNode<NodeDataType>::setFlag(bool isTarget, int index)
{
	if (isTarget)
		flag[oct->targetFrameId][index] = true;
	else
		flag[oct->refrenceFrameId][index] = true;
}

template<class NodeDataType>
void OctreeDoubelBufferNode<NodeDataType>::resetFlag(bool isTarget, int index)
{
	if (isTarget)
		flag[oct->targetFrameId][index] = false;
	else
		flag[oct->refrenceFrameId][index] = false;
}

template<class NodeDataType>
void OctreeDoubelBufferNode<NodeDataType>::resetFrame(bool isTarget)
{
	initFlag(isTarget);
	resetNodeData(isTarget);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<class NodeDataType>
inline DoubleBufferOctree<NodeDataType>::DoubleBufferOctree(Vec3 min, Vec3 max, Vec3 cellSize, int targetFrameId, int refrenceFrameId)
{
	assert(targetFrameId == 0 || targetFrameId == 1);
	assert(refrenceFrameId == 0 || refrenceFrameId == 1);

	this->min = min;
	this->max = max;
	this->cellSize = cellSize;
	this->targetFrameId = targetFrameId;
	this->refrenceFrameId = refrenceFrameId;

	root = new OctreeDoubelBufferNode<NodeDataType>(this);
	//min�Ķ����Max�Ķ���ȷ�� �ڵ�Ŀռ䷶Χ
	root->setMinPos(min);
	root->setMaxPos(max);
}

template<class NodeDataType>
inline DoubleBufferOctree<NodeDataType>::~DoubleBufferOctree()
{
	if (root)
		delete root;
}

template<class NodeDataType>
void DoubleBufferOctree<NodeDataType>::setTargetFrameId(int id)
{
	assert(id == 0 || id == 1);
	//һ����0��һ����1
	targetFrameId = id;
	refrenceFrameId = (id + 1) % 2;
}

template<class NodeDataType>
void DoubleBufferOctree<NodeDataType>::setReferenceFrameId(int id)
{
	assert(id == 0 || id == 1);
	//һ����0��һ����1
	targetFrameId = (id + 1) % 2;
	refrenceFrameId = id;
}

template<class NodeDataType>
OctreeDoubelBufferNode<NodeDataType>* DoubleBufferOctree<NodeDataType>::createChildNode(bool isTarget,
	OctreeDoubelBufferNode<NodeDataType>* parentNode, int childIdx)
{
	assert(parentNode);
	assert(childIdx > -1 && childIdx < 8);
	
	if (!parentNode->getChildren(childIdx))
	{
		//û��������ӽڵ㣬��ô��������ڵ�
		parentNode->children[childIdx] = new OctreeDoubelBufferNode<NodeDataType>(this, isTarget);
		OctreeDoubelBufferNode<NodeDataType> * temp = parentNode->getChildren(childIdx);

		//���ݸ��׽ڵ�ı߽磬�õ����ӽڵ�ı߽�
		Vec3 newMin = parentNode->getMinPos();
		Vec3 newMax = parentNode->getMaxPos();
		Vec3 mid = (parentNode->getMinPos() + parentNode->getMaxPos())/2;
		switch (childIdx)
		{
		case 0:
			newMax = mid;
			break;
		case 1:
			newMin.x = mid.x;
			newMax.y = mid.y;
			newMax.z = mid.z;
			break;
		case 2:
			newMin.y = mid.y;
			newMax.x = mid.x;
			newMax.z = mid.z;
			break;
		case 3:
			newMin.x = mid.x;
			newMin.y = mid.y;
			newMax.z = mid.z;
			break;
		case 4:
			newMin.z = mid.z;
			newMax.x = mid.x;
			newMax.y = mid.y;
			break;
		case 5:
			newMin.x = mid.x;
			newMin.z = mid.z;
			newMax.y = mid.y;
			break;
		case 6:
			newMin.y = mid.y;
			newMin.z = mid.z;
			newMax.x = mid.x;
			break;
		case 7:
			newMin = mid;
			break;
		default:
			cout << "do not support the child idx!" << endl;
			break;
		}

		temp->setMinPos(newMin);
		temp->setMaxPos(newMax);
		//flag
		if (isTarget)
			parentNode->flag[targetFrameId][childIdx] = true;
		else
			parentNode->flag[refrenceFrameId][childIdx] = true;

		return temp;
	}
	else
	{
		//�Ѿ���������ӽڵ㣬��ô����������һ֡�����
		if (isTarget)
		{
			parentNode->flag[targetFrameId][childIdx] = true;
		}
		else
		{
			parentNode->flag[refrenceFrameId][childIdx] = true;
		}
				
		parentNode->setNodeData(isTarget);
		return parentNode->getChildren(childIdx);
	}
}

template<class NodeDataType>
inline bool DoubleBufferOctree<NodeDataType>::recoveryDBufferOctreeNodeForSigleFrame(OctreeDoubelBufferNode<NodeDataType>* parentNode, const char byte, bool isTarget)
{
	assert(parentNode);

	bitset<8> bits = byte;
	//����ǰ�ڵ�ĵ�8������
	for (int b_it=0; b_it < 8; b_it++)
	{
		//��һλ��1����������ڵ�
		if (bits[b_it])
		{
			//��������ڵ�ģ���b-it�����ӽڵ�
			createChildNode(isTarget, parentNode, b_it);
		}
	}
	return true;
}

template<class NodeDataType>
bool DoubleBufferOctree<NodeDataType>::recoveryDBufferOctreeNode(OctreeDoubelBufferNode<NodeDataType>* parentNode, const char byte)
{
	assert(parentNode);

	bitset<8> bits = byte;
	//����ǰ�ڵ�ĵ�8������
	for (int b_it; b_it < 8; b_it++)
	{
		//��һλ��1����ô˵�����ӽڵ����ǲ�һ������Ҫ���д���
		if (bits[b_it])
		{
			if (!parentNode->flag[refrenceFrameId][b_it])
			{
				//swapframe ������ڵ�����û�к��ӵģ�targetFrame���к��Ӳ��ô���
				createChildNode(true, parentNode, b_it);
			}
			else
			{
				//swapframe ������ڵ������к��ӵģ�targetFrame��û�к��ӣ����ô���
			}
		}
		else
		{
			//��һλ��0��˵�����ӽڵ���һ����

			if (parentNode->flag[refrenceFrameId][b_it])
			{
				//swapframe ������ڵ������к��ӵģ�targetFrame���к��ӣ��ô���
				createChildNode(true, parentNode, b_it);
			}
			else
			{
				//swapframe ������ڵ�����û�к��ӵģ����ô���

			}
		}
	}
	return true;
}

template<class NodeDataType>
NodeDataType * DoubleBufferOctree<NodeDataType>::getCell(bool isTarget, const Vec3 ppos, Callback * callback)
{
	assert(ppos >= min && ppos <= max);

	Vec3 currMin(min);
	Vec3 currMax(max);
	Vec3 delta = max - min;

	if (!root)
	{
		//root��һ��ͱ���ʼ����
		root = new OctreeDoubelBufferNode<NodeDataType>(this, isTarget);
	}
	//���root�Ѿ����䣬��ô����ض�frame�ķ������.��Ϊ������frame��Ҫ���й���
	if (root->checkNodeDataNull(isTarget))
		root->setNodeData(isTarget);

	OctreeDoubelBufferNode<NodeDataType> * currNode = root;

	//�ж��Ƿ���Ҫ�������˲���������
	while (delta >= cellSize)
	{
		bool shouldContinue = true;

		if (callback)
			shouldContinue = callback->operator()(currMin, currMax, *(currNode->getNodeData(isTarget)));
		if (!shouldContinue)
			break;

		//�õ���ǰ�ռ�����ĵ�
		Vec3 mid = (delta * 0.5f) + currMin;

		//��ʼ����һ���ӿռ����Сλ�ú����λ�á�
		Vec3 newMin(currMin);
		Vec3 newMax(currMax);
		//index��¼����������λ�á�
		int index = 0;

		//�ж�ppos����㣬�ڵ�ǰ�����node��Χ�У�Ӧ�������ĸ�����,���õ��ӿռ��λ��
		COMPUTE_QUADRANT(index, 1, ppos.x, mid.x, newMin.x, newMax.x);
		COMPUTE_QUADRANT(index, 2, ppos.y, mid.y, newMin.y, newMax.y);
		COMPUTE_QUADRANT(index, 4, ppos.z, mid.z, newMin.z, newMax.z);

		//�������ӽڵ㶼û�з��䣬��ô���з���
		
		if (!(currNode->getChildren(index)))
		{
			currNode->children[index] = new OctreeDoubelBufferNode<NodeDataType>(this, isTarget);
			//����flag
			//currNode->setFlag(isTarget, index);
		}
		//����ӿռ�ڵ��Ѿ����䣬��ô����ض�frame�е��ӽڵ��nodedata�ķ������
		if (currNode->children[index]->checkNodeDataNull(isTarget))
			currNode->children[index]->setNodeData(isTarget);

		currNode->setFlag(isTarget, index);

		//currNode = currNode->getChildren(index);
		currNode = currNode->children[index];
		//ÿ���ڵ㱣���Լ��ķ�Χ
		currNode->setMinPos(newMin);
		currNode->setMaxPos(newMax);

		currMin = newMin;
		currMax = newMax;
		delta = currMax - currMin;
	}
	return currNode->getNodeData(isTarget);
}

template<class NodeDataType>
void DoubleBufferOctree<NodeDataType>::traverse(Callback * callback)
{
	assert(callback);
	depthFirstTraverseRecursive(callback, min, max, root);
}

template<class NodeDataType>
void DoubleBufferOctree<NodeDataType>::traverseDepthFirst(Callback * callback)
{
	assert(callback);
	traverseDepthFirstRecursive(callback, min, max, root);
}

//allNodeList_out ���ص�ֵ�а���Ҷ�ӽڵ㡣
template<class NodeDataType>
void DoubleBufferOctree<NodeDataType>::traverseBreathFirst(std::vector<OctreeDoubelBufferNode<NodeDataType>*> &allNodeList_out)
{

	//������ȱ������õ����нڵ��һ������
	allNodeList_out.push_back(root);
	for (int i = 0; i < allNodeList_out.size(); i++)
	{
		for (int child_it = 0; child_it < 8; child_it++)
		{
			if (allNodeList_out[i]->getChildren(child_it))
			{
				allNodeList_out.push_back(allNodeList_out[i]->getChildren(child_it));
			}
		}
	}
}

template<class NodeDataType>
inline void DoubleBufferOctree<NodeDataType>::traverseBreathFirstSigleFrame(std::vector<OctreeDoubelBufferNode<NodeDataType>*>& allNodeList_out, bool isTarget_in)
{
	//������ȱ������õ����нڵ��һ������
	if (isTarget_in)
	{
		allNodeList_out.push_back(root);
		for (int i = 0; i < allNodeList_out.size(); i++)
		{
			for (int child_it = 0; child_it < 8; child_it++)
			{
				//target �ϵ�Ҷ�ӽڵ���OK��
				if (allNodeList_out[i]->flag[allNodeList_out[i]->getTargetFlagIdx()][child_it])
				{
					allNodeList_out.push_back(allNodeList_out[i]->getChildren(child_it));
				}
			}
		}
	}
	else
	{
		allNodeList_out.push_back(root);
		for (int i = 0; i < allNodeList_out.size(); i++)
		{
			for (int child_it = 0; child_it < 8; child_it++)
			{
				if (allNodeList_out[i]->flag[allNodeList_out[i]->getReferenceFlagIdx()][child_it])
				{
					allNodeList_out.push_back(allNodeList_out[i]->getChildren(child_it));
				}
			}
		}
	}
}

template<class NodeDataType>
void DoubleBufferOctree<NodeDataType>::clear()
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
	depthFirstTraverseRecursive(callback, currMin, mid, currNode->children[0]);
	depthFirstTraverseRecursive(callback, Vec3(mid.x, currMin.y, currMin.z), Vec3(currMax.x, mid.y, mid.z), currNode->children[1]);
	depthFirstTraverseRecursive(callback, Vec3(currMin.x, mid.y, currMin.z), Vec3(mid.x, currMax.y, mid.z), currNode->children[2]);
	depthFirstTraverseRecursive(callback, Vec3(mid.x, mid.y, currMin.z), Vec3(currMax.x, currMax.y, mid.z), currNode->children[3]);
	depthFirstTraverseRecursive(callback, Vec3(currMin.x, currMin.y, mid.z), Vec3(mid.x, mid.y, currMax.z), currNode->children[4]);
	depthFirstTraverseRecursive(callback, Vec3(mid.x, currMin.y, mid.z), Vec3(currMax.x, mid.y, currMax.z), currNode->children[5]);
	depthFirstTraverseRecursive(callback, Vec3(currMin.x, mid.y, mid.z), Vec3(mid.x, currMax.y, currMax.z), currNode->children[6]);
	depthFirstTraverseRecursive(callback, mid, currMax, currNode->children[7]);
}

template<class NodeDataType>
void DoubleBufferOctree<NodeDataType>::depthFirstTraverseRecursive(Callback * callback, const Vec3 & currMin, const Vec3 & currMax, OctreeDoubelBufferNode<NodeDataType> * currNode)
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
	depthFirstTraverseRecursive(callback, currMin, mid, currNode->getChildren(0));
	depthFirstTraverseRecursive(callback, Vec3(mid.x, currMin.y, currMin.z), Vec3(currMax.x, mid.y, mid.z), currNode->getChildren(1));
	depthFirstTraverseRecursive(callback, Vec3(currMin.x, mid.y, currMin.z), Vec3(mid.x, currMax.y, mid.z), currNode->getChildren(2));
	depthFirstTraverseRecursive(callback, Vec3(mid.x, mid.y, currMin.z), Vec3(currMax.x, currMax.y, mid.z), currNode->getChildren(3));
	depthFirstTraverseRecursive(callback, Vec3(currMin.x, currMin.y, mid.z), Vec3(mid.x, mid.y, currMax.z), currNode->getChildren(4));
	depthFirstTraverseRecursive(callback, Vec3(mid.x, currMin.y, mid.z), Vec3(currMax.x, mid.y, currMax.z), currNode->getChildren(5));
	depthFirstTraverseRecursive(callback, Vec3(currMin.x, mid.y, mid.z), Vec3(mid.x, currMax.y, currMax.z), currNode->getChildren(6));
	depthFirstTraverseRecursive(callback, mid, currMax, currNode->getChildren(7));
}

template<class NodeDataType>
void DoubleBufferOctree<NodeDataType>::traverseDepthFirstRecursive(Callback * callback, const Vec3 & currMin, const Vec3 & currMax, OctreeDoubelBufferNode<NodeDataType>* currNode)
{
	//Ϊ�շ���
	if (!currNode)
		return;

	//mid �õ�cell�м�ĵ�
	Vec3 delta = currMax - currMin;
	Vec3 mid = (delta * 0.5f) + currMin;
	//�ֳ��Ӱ˸��ڵ�
	depthFirstTraverseRecursive(callback, currMin, mid, currNode->getChildren(0));
	depthFirstTraverseRecursive(callback, Vec3(mid.x, currMin.y, currMin.z), Vec3(currMax.x, mid.y, mid.z), currNode->getChildren(1));
	depthFirstTraverseRecursive(callback, Vec3(currMin.x, mid.y, currMin.z), Vec3(mid.x, currMax.y, mid.z), currNode->getChildren(2));
	depthFirstTraverseRecursive(callback, Vec3(mid.x, mid.y, currMin.z), Vec3(currMax.x, currMax.y, mid.z), currNode->getChildren(3));
	depthFirstTraverseRecursive(callback, Vec3(currMin.x, currMin.y, mid.z), Vec3(mid.x, mid.y, currMax.z), currNode->getChildren(4));
	depthFirstTraverseRecursive(callback, Vec3(mid.x, currMin.y, mid.z), Vec3(currMax.x, mid.y, currMax.z), currNode->getChildren(5));
	depthFirstTraverseRecursive(callback, Vec3(currMin.x, mid.y, mid.z), Vec3(mid.x, currMax.y, currMax.z), currNode->getChildren(6));
	depthFirstTraverseRecursive(callback, mid, currMax, currNode->getChildren(7));

	//ִ����Ӧ�Ĳ��������
	callback->operator()(currMin, currMax, currNode);

}


