#pragma once
#include "zjw_macro.h"
#include "zjw_pcs_octree.h"
//#include "zjw_fileBatch.h"

//点云序列中的帧
class Frame
{
public:
	int frameId;
	ObjMesh *objMesh;
	PcsOctree *pcsOct;
public:
	Frame();
	Frame(const int frameId);
	~Frame();
	bool loadObj(string path);
	bool octSgwt();
};

//序列管理器
class FrameManage
{
public:
	vector<Frame> frameList;
	int fileNum;
	string filePath;
	string fileName;
public:
	FrameManage(string path, string file);
	FrameManage();

	~FrameManage();
	//处理文件夹下的所有的序列文件
	void batchLoadObj(string fileNameFormat="_cleaner.obj", string path = "./testData/football");
};


