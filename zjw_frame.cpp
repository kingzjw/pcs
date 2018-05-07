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
	fb = nullptr;
}

FrameManage::~FrameManage()
{
	delete fb;
}

void FrameManage::batchLoadObj(string fileNameFormat, string path)
{
	//--------获取该路径下的所有文件-------------
	if (fb)
		delete fb;
	fb = new FileBatch(path, fileNameFormat);
	fb->fileNum = fb->getFilesNum(fb->filePath);

	//for (int f_it = 0; f_it < fb->fileNum; f_it++)
	for (int f_it = 0; f_it < fb->fileNum; f_it++)
	{
#ifdef ZJW_DEBUG
		cout <<endl<<endl<< "##################################################" << endl;
#endif
		string totalFilePath;
		stringstream ss;
		ss << f_it;
		totalFilePath.assign(path).append("/").append(ss.str()).append(fileNameFormat);
		Frame * frame =new Frame(f_it);
		frameList.push_back(frame);

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
		cout << totalFilePath << endl;
#endif //zjw_debug
	}
}
