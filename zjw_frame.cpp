#include "zjw_frame.h"

//Frame::Frame()
//{
//	objMesh = new ObjMesh();
//	pcsOct = new PcsOctree();
//}

Frame::Frame(const int frameId)
{
	objMesh = new ObjMesh();
	pcsOct = new PcsOctree();
}

Frame::~Frame()
{
	if (objMesh)
		delete objMesh;
	if (pcsOct)
		delete pcsOct;
}

bool Frame::loadObj(string path)
{
	//return objMesh.loadObjMesh(path);
	return objMesh->loadObjMesh(path);
}

bool Frame::octSgwt()
{
	Vec3 minPos(objMesh->rangeMin);
	Vec3 maxPos((objMesh->rangeMax + Epsilon));
	/*Vec3 minPos(objMesh.rangeMin);
	Vec3 maxPos((objMesh.rangeMax + Epsilon));*/

	Vec3 cellSize(0.42);
	//设置参数，并构建八叉树
	pcsOct->setParam(minPos, maxPos, cellSize);
	//pcsOct->buildPcsOctFrmPC(objMesh);
	pcsOct->buildPcsOctFrmPC(objMesh);
	//得到叶子节点的边界，并保存相关的信息
	pcsOct->getLeafboundary();

	//test
	//cout << pcsOct << endl;
	//end test
	pcsOct->getGraphMat();
	pcsOct->getMatEigenVerValue();

	//得到八叉树上顶点的信号
	pcsOct->setPointTo8Areas();
	pcsOct->getLeafSignal();
	//pcsOct->getSgwtCoeffWS();
	//拿到信号x在第一个象限0中的值
	pcsOct->getSgwtCoeffWS(SignalType::SignalX, 0);
	return true;
}

FrameManage::FrameManage()
{
	batchLoad = false;
	getAllFielPath = false;

	fileBatch = nullptr;
}

FrameManage::~FrameManage()
{
	delete fileBatch;
}

void FrameManage::getAllFilesPath(string fileNameFormat, string path)
{
	//--------获取该路径下的所有文件-------------
	if (fileBatch)
		delete fileBatch;
	fileBatch = new FileBatch(path, fileNameFormat);
	fileBatch->fileNum = fileBatch->getFilesNum(fileBatch->filePath);
	frameList.clear();
	getAllFielPath = true;

	for (int f_it = 0; f_it < fileBatch->fileNum; f_it++)
	{
		//拿到所有的路径
		string totalFilePath;
		stringstream ss;
		ss << f_it;
		totalFilePath.assign(path).append("/").append(ss.str()).append(fileNameFormat);

		//按照需要从小到大，保存所有的文件路径
		fileBatch->files.push_back(totalFilePath);

		//拿到所有的frameList
		Frame * frame = new Frame(f_it);
		frameList.push_back(frame);
	}
}

void FrameManage::batchLoadObj(string fileNameFormat, string path)
{
	if (fileBatch)
		delete fileBatch;
	fileBatch = new FileBatch(path, fileNameFormat);
	fileBatch->fileNum = fileBatch->getFilesNum(fileBatch->filePath);
	frameList.clear();
	batchLoad = true;

	//--------获取该路径下的所有文件-------------
	for (int f_it = 0; f_it < fileBatch->fileNum; f_it++)
	{
#ifdef ZJW_DEBUG
		cout <<endl<<endl<< "##################################################" << endl;
#endif
		string totalFilePath;
		stringstream ss;
		ss << f_it;
		totalFilePath.assign(path).append("/").append(ss.str()).append(fileNameFormat);

		//按照需要从小到大，保存所有的文件路径
		fileBatch->files.push_back(totalFilePath);

		Frame * frame =new Frame(f_it);
		frameList.push_back(frame);

		//-----------对每个路径，都进行读取，和sgw操作------------
		ZjwTimer timer;
		timer.Start();
		if (!frameList[f_it]->objMesh->loadObjMeshSimply(totalFilePath))
		{
			continue;
		}
		//frameList[f_it-1].objMesh->loadObjMeshSimply(totalFilePath);
		timer.Stop();
		timer.printTimeInMs("load obj time: ");
		
		ZjwTimer timer2;
		timer2.Start();
		frameList[f_it]->octSgwt();
		//frameList[f_it-1].octSgwt();

		timer2.Stop();
		timer2.printTimeInMs("build oct and compute the sgwt coeff time: ");

#ifdef ZJW_DEBUG
		//cout << totalFilePath << endl;
#endif //zjw_debug
	}
}

bool FrameManage::loadContinuousFrames(int frameId1, int frameId2, string fileNameFormat, string path)
{
	assert(frameId1 > -1 && frameId2 >-1);
	
	if (batchLoad)
		return true;
	if (!getAllFielPath)
	{
		getAllFilesPath(fileNameFormat, path);
	}

	assert( frameId1 < fileBatch->fileNum && frameId2 < fileBatch->fileNum);

	//-----------进行读取，和sgw操作------------
	ZjwTimer timer;
	timer.Start();
	if (frameList[frameId1]->objMesh->loadObjMeshSimply(fileBatch->files[frameId1]))
	{
		timer.Stop();
		timer.printTimeInMs("load obj time: ");

		ZjwTimer timer2;
		timer2.Start();
		frameList[frameId1]->octSgwt();
		timer2.Stop();
		timer2.printTimeInMs("build oct and compute the sgwt coeff time: ");
	}

	ZjwTimer timer3;
	timer3.Start();
	if (frameList[frameId2]->objMesh->loadObjMeshSimply(fileBatch->files[frameId2]))
	{
		timer3.Stop();
		timer3.printTimeInMs("load obj time: ");

		ZjwTimer timer4;
		timer4.Start();
		frameList[frameId2]->octSgwt();

		timer4.Stop();
		timer4.printTimeInMs("build oct and compute the sgwt coeff time: ");
	}
}
