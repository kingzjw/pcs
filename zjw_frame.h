#pragma once
#include "zjw_macro.h"
#include "zjw_pcs_octree.h"
#include "zjw_fileBatch.h"
#include "zjw_timer.h"
#include <sstream>
#include <assert.h>

//点云序列中的帧
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

//序列管理器
class FrameManage
{
public:
	vector<Frame*> frameList;
	FileBatch *fileBatch;
	enum FileNameForMat
	{
		NUM_FRONT,
		NUM_TAIL
	};

private:
	//标记是否调用过batchLoadObj接口
	bool batchLoad;
	//标记是否调用过getAllfilepath接口
	bool getAllFilePath;

public:
	FrameManage();
	~FrameManage();
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
	//保存并处理文件夹下的所有的序列文件.并加载计算出相应的swg系数
	void batchLoadObj(FileNameForMat type = NUM_FRONT, string fileNameFormat="_cleaner", string path = "./testData/football");

	//保存所有格式和路径的，从小到大保存完整路径名字,并得到所有的frameList
	void getAllFilesPath(FileNameForMat type = NUM_FRONT,string fileNameFormat =  "_cleaner", string path = "./testData/football");

	//对指定的连续两帧，读取obj并计算价值sgwt的系数，frameId 从0开始
	bool loadContinuousFrames(int frameId1, int frameId2, FileNameForMat type = NUM_FRONT, string fileNameFormat = "_cleaner", string path = "./testData/football");

	//对训练的数据连续两帧进行匹配
	bool matchNode(int frameId1, int frameId2);

};


