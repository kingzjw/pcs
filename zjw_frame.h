#pragma once
#include "zjw_macro.h"
#include "zjw_pcs_octree.h"
#include "zjw_fileBatch.h"
#include "zjw_timer.h"
#include <sstream>

//���������е�֡
class Frame
{
public:
	int frameId;
	ObjMesh *objMesh;
	PcsOctree *pcsOct;
public:
	//Frame();
	Frame(const int frameId);
	~Frame();
	bool loadObj(string path);
	bool octSgwt();
};

//���й�����
class FrameManage
{
public:
	vector<Frame*> frameList;
	FileBatch *fb;
public:
	FrameManage();
	~FrameManage();

	//�����ļ����µ����е������ļ�
	void batchLoadObj(string fileNameFormat="_cleaner.obj", string path = "./testData/football");
};


