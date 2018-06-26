#include "pcscompress.h"

void pcsCompress::clickedOpenFileAction()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Input"), ".//testData", "Image file(*.obj)", 0);

	std::string st = filename.toStdString();
	std::string stt = "";
	for (int i = 0; i < st.length(); i++)
	{
		if (st[i] == '/')
			stt += "//";
		else
			stt += st[i];
	}

	string path = stt;
	size_t lastpos = stt.rfind("//");
	if (lastpos != string::npos)
		path.erase(lastpos, path.length());

	dirPath = path;
	filePath = stt;

	std::cout << "you choose the file: " << std::endl << stt << endl;
	std::cout << "you choose the path: " << std::endl << path << endl;
	
	//render
	ui.openGLWidget->updateGL();
}

void pcsCompress::clickPointCloudButton()
{
#ifdef ZJW_DEBUG
	cout << "clickPointCloudButton: render mode change to 1" << endl;
#endif // ZJW_DEBUG

	ui.openGLWidget->renderState = 1;
	ui.openGLWidget->updateGL();
}

void pcsCompress::clickPCOctTreeButton()
{
#ifdef ZJW_DEBUG
	cout << "clickPointCloudButton: render mode change to 2" << endl;
#endif // ZJW_DEBUG
	ui.openGLWidget->renderState = 2;
	ui.openGLWidget->updateGL();
}

void pcsCompress::clickTwoFrameButton()
{
#ifdef ZJW_DEBUG
	cout << "clickPointCloudButton: render mode change to 3" << endl;
#endif // ZJW_DEBUG
	ui.openGLWidget->renderState = 3;
	ui.openGLWidget->updateGL();
}

void pcsCompress::clickTwoFrameSparseMatchButton()
{
#ifdef ZJW_DEBUG
	cout << "clickPointCloudButton: render mode change to 5" << endl;
#endif // ZJW_DEBUG
	ui.openGLWidget->renderState = 5;
	ui.openGLWidget->updateGL();
}

void pcsCompress::clickOthers()
{
	cout << "clicked others (need to add something)!" << endl;
}

void pcsCompress::changeRefFrameId()
{
	//string to doubel
	istringstream iss(ui.refFrameLineEdit->text().toStdString());
	iss >> ui.openGLWidget->showFrameIdx;
	ui.openGLWidget->showFrameIdx2 = ui.openGLWidget->showFrameIdx + 1;

	//set target frame id ( double to string )
	std::ostringstream stream;
	stream << ui.openGLWidget->showFrameIdx2;
	string str = stream.str();
	ui.targetFrameLineEdit->setText(QString::fromStdString(str));

#ifdef ZJW_DEBUG
	cout << "Ref Frame Id " << ui.openGLWidget->showFrameIdx << " target frame id: " << ui.openGLWidget->showFrameIdx2 << endl;
#endif // ZJW_DEBUG
}

void pcsCompress::changeTargetFrameId()
{
	//string to doubel
	istringstream iss(ui.targetFrameLineEdit->text().toStdString());
	iss >> ui.openGLWidget->showFrameIdx2;
	ui.openGLWidget->showFrameIdx = ui.openGLWidget->showFrameIdx2 - 1;

	//set target frame id ( double to string )
	std::ostringstream stream;
	stream << ui.openGLWidget->showFrameIdx;
	string str = stream.str();
	ui.refFrameLineEdit->setText(QString::fromStdString(str));

#ifdef ZJW_DEBUG
	cout << "Ref Frame Id " << ui.openGLWidget->showFrameIdx << " target frame id: " << ui.openGLWidget->showFrameIdx2 << endl;
#endif // ZJW_DEBUG
}

void pcsCompress::changeOctCellSize()
{
	//string to doubel
	istringstream iss(ui.octTreeLeafineEdit->text().toStdString());
	double temp;
	iss >> temp;

	if (temp < 1 && temp>0)
	{
		ui.openGLWidget->fm.cellSize.x = temp;
		ui.openGLWidget->fm.cellSize.y = temp;
		ui.openGLWidget->fm.cellSize.z = temp;
#ifdef ZJW_DEBUG
		cout << "cell size set to " << temp << endl;
#endif // ZJW_DEBUG
	}
	else
	{
		cout << "error input,please input range (0,1) !" << endl;
	}
}

void pcsCompress::changeClusterNum()
{
	//string to doubel
	istringstream iss(ui.clusterNumLineEdit->text().toStdString());
	double temp;
	iss >> temp;

	if (temp > 1 )
	{
		ui.openGLWidget->fm.clusterNum = temp;
#ifdef ZJW_DEBUG
		cout << "cluster num  set to " << temp << endl;
#endif // ZJW_DEBUG
	}
	else
	{
		cout << "error input,please input range [2,+inf) !" << endl;
	}
}

void pcsCompress::changeU()
{
	//string to doubel
	istringstream iss(ui.u_motionVecLineEdit->text().toStdString());
	double temp;
	iss >> temp;

	ui.openGLWidget->fm.u = temp;
#ifdef ZJW_DEBUG
	cout << "motion vector param u set to " << temp << endl;
#endif // ZJW_DEBUG
}

void pcsCompress::changeMDegreeCheby()
{
	//string to doubel
	istringstream iss(ui.m_ChebyshevLineEdit->text().toStdString());
	double temp;
	iss >> temp;

	if (temp > 0)
	{
		ui.openGLWidget->fm.m = temp;
#ifdef ZJW_DEBUG
		cout << "m_Chebyshev set to " << temp << endl;
#endif // ZJW_DEBUG
	}
	else
	{
		cout << "error input,please input range [1,+inf) !" << endl;
	}
}

void pcsCompress::changeNscales()
{
	//string to doubel
	istringstream iss(ui.num_scalesLineEdit->text().toStdString());
	double temp;
	iss >> temp;

	if (temp > 1)
	{
		ui.openGLWidget->fm.Nscales = temp;
#ifdef ZJW_DEBUG
		cout << "sgw n scales  set to " << temp << endl;
#endif // ZJW_DEBUG
	}
	else
	{
		cout << "error input,please input range [2,+inf) !" << endl;
	}
}

void pcsCompress::trainMatP()
{
#ifdef ZJW_DEBUG
	cout<<endl<<endl << "####################################  traing   ##########################################" << endl;
#endif //zjw_debug
	//训练数据，得到矩阵P
	//ui.openGLWidget->fm.trainGetP(0, 1, ui.openGLWidget->fm.FileNameForMat::NUM_TAIL, 
		//"walkTexture_0_", "E://1.study//pointCloud//code//pcsCompress//pcsCompress//testData//walk");
	assert(!dirPath.empty());
	ui.openGLWidget->fm.trainGetP(0, 1, ui.openGLWidget->fm.FileNameForMat::NUM_TAIL, 
		"walkTexture_0_", dirPath);

	//展现测试的匹配的状态
	ui.openGLWidget->showFrameIdx = 0;
	ui.openGLWidget->showFrameIdx2 = 1;

	ui.openGLWidget->renderState = 4;
	ui.openGLWidget->updateGL();

#ifdef ZJW_DEBUG
	cout << "train path: " << ".//testData//walk" << endl;
	cout << "train frame id :  0 and 1"<< endl;
	cout <<"######################################################################################" << endl;
#endif // ZJW_DEBUG
}

void pcsCompress::getSparseMatch()
{
#ifdef ZJW_DEBUG
	cout << endl << endl << "####################################  getSparseMatch   ##########################################" << endl;
#endif //zjw_debug

	ui.openGLWidget->fm.getTwoFrameBestSparseMatch(ui.openGLWidget->showFrameIdx, ui.openGLWidget->showFrameIdx2,
		&ui.openGLWidget->fm.f1SparseIdxList, &ui.openGLWidget->fm.f2SparseIdxList,
		ui.openGLWidget->fm.FileNameForMat::NUM_TAIL, "walkTexture_0_", dirPath, true);

#ifdef ZJW_DEBUG
	cout << "#######################################################################################" << endl;
#endif //zjw_debug

	ui.openGLWidget->renderState = 2;
	ui.openGLWidget->updateGL();
}

void pcsCompress::getMotionVector()
{
#ifdef ZJW_DEBUG
	cout << endl << endl << "####################################  getMotionVector   ##########################################" << endl;
#endif //zjw_debug

	//测试数据，拿到motion vector
	VectorXd Vt;
	ui.openGLWidget->fm.computeMotinVectorMinresQLP(ui.openGLWidget->showFrameIdx,
		&ui.openGLWidget->fm.f1SparseIdxList, &ui.openGLWidget->fm.f2SparseIdxList, Vt);

#ifdef ZJW_DEBUG
	cout << "#######################################################################################" << endl;
#endif //zjw_debug

}

void pcsCompress::test()
{
	//chagne leaf node
	double temp = 0.05;
	ui.openGLWidget->fm.cellSize.x = temp;
	ui.openGLWidget->fm.cellSize.y = temp;
	ui.openGLWidget->fm.cellSize.z = temp;

	//changeClusterNum 
	temp = 10;
	ui.openGLWidget->fm.clusterNum = temp;

	//change M 
	//paper use 30
	temp = 50;
	ui.openGLWidget->fm.m = temp;

	//chagne u
	temp = 0.5;
	ui.openGLWidget->fm.u = temp;
	
	//训练数据，得到矩阵P
	ui.openGLWidget->fm.trainGetP(0, 1, ui.openGLWidget->fm.FileNameForMat::NUM_TAIL, "walkTexture_0_",
		"E://1.study//pointCloud//code//pcsCompress//pcsCompress//testData//walk");

	//测试数据，拿到稀疏最佳匹配
	ui.openGLWidget->fm.getTwoFrameBestSparseMatch(ui.openGLWidget->showFrameIdx, ui.openGLWidget->showFrameIdx2,
		&ui.openGLWidget->fm.f1SparseIdxList, &ui.openGLWidget->fm.f2SparseIdxList,
		ui.openGLWidget->fm.FileNameForMat::NUM_TAIL, "walkTexture_0_", "E://1.study//pointCloud//code//pcsCompress//pcsCompress//testData//walk", true);

	//测试数据，拿到motion vector
	VectorXd Vt;
	ui.openGLWidget->fm.computeMotinVectorMinresQLP(ui.openGLWidget->showFrameIdx,
		&ui.openGLWidget->fm.f1SparseIdxList, &ui.openGLWidget->fm.f2SparseIdxList, Vt);
}

pcsCompress::pcsCompress(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	//打开文件
	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(clickedOpenFileAction()));
	//改变render mode
	connect(ui.pc_radioButtion, SIGNAL(clicked()), this, SLOT(clickPointCloudButton()));
	connect(ui.oct_radioButton, SIGNAL(clicked()), this, SLOT(clickPCOctTreeButton()));
	
	connect(ui.twoframe_radioButton, SIGNAL(clicked()), this, SLOT(clickTwoFrameButton()));
	connect(ui.sparseMatchRadioButton, SIGNAL(clicked()), this, SLOT(clickTwoFrameSparseMatchButton()));
	connect(ui.otherRadioButton, SIGNAL(clicked()), this, SLOT(clickOthers()));
	
	//改frame id
	connect(ui.refFrameLineEdit, SIGNAL(returnPressed()), this, SLOT(changeRefFrameId()));
	connect(ui.targetFrameLineEdit, SIGNAL(returnPressed()), this, SLOT(changeTargetFrameId()));

	//chagnge oct cell size
	connect(ui.octTreeLeafineEdit, SIGNAL(returnPressed()), this, SLOT(changeOctCellSize()));
	//change kmeans result cluster num
	connect(ui.clusterNumLineEdit, SIGNAL(returnPressed()), this, SLOT(changeClusterNum()));
	//change motion vector u
	connect(ui.u_motionVecLineEdit, SIGNAL(returnPressed()), this, SLOT(changeU()));
	//改变算法中的值
	connect(ui.m_ChebyshevLineEdit, SIGNAL(returnPressed()), this, SLOT(changeMDegreeCheby()));
	connect(ui.num_scalesLineEdit, SIGNAL(returnPressed()), this, SLOT(changeNscales()));

	

	//核心算法步骤
	connect(ui.actionTrainMatP, SIGNAL(triggered()), this, SLOT(trainMatP()));
	connect(ui.actionGetSparseMatch, SIGNAL(triggered()), this, SLOT(getSparseMatch()));
	connect(ui.actionGetMotionVector, SIGNAL(triggered()), this, SLOT(getMotionVector()));
	connect(ui.actionTest, SIGNAL(triggered()), this, SLOT(test()));

}

pcsCompress::~pcsCompress()
{
}