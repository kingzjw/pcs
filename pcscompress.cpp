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

	std::cout << "you choose the file: " << std::endl << stt << endl;
	std::cout << "you choose the path: " << std::endl << path << endl;

	//训练数据，得到矩阵P
	ui.openGLWidget->fm.trainGetP(0, 1, ui.openGLWidget->fm.FileNameForMat::NUM_TAIL, "walk_0_", path);

	//测试数据，拿到稀疏最佳匹配
	vector<int> f1SparseIdxList;
	vector<int> f2SparseIdxList;
	ui.openGLWidget->fm.getTwoFrameBestSparseMatch(ui.openGLWidget->showFrameIdx, ui.openGLWidget->showFrameIdx2,
		&f1SparseIdxList, &f2SparseIdxList, ui.openGLWidget->fm.FileNameForMat::NUM_TAIL, "walk_0_", path, true);

	//测试数据，拿到motion vector
	VectorXd Vt;
	ui.openGLWidget->fm.computeMotinVector(ui.openGLWidget->showFrameIdx, &f1SparseIdxList, &f2SparseIdxList, Vt);

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
	ui.openGLWidget->fm.cellSize.x = temp;
	ui.openGLWidget->fm.cellSize.y = temp;
	ui.openGLWidget->fm.cellSize.z = temp;

#ifdef ZJW_DEBUG
	cout << "cell size set to " << temp << endl;
#endif // ZJW_DEBUG
}

void pcsCompress::changeClusterNum()
{
	//string to doubel
	istringstream iss(ui.clusterNumLineEdit->text().toStdString());
	double temp;
	iss >> temp;
	ui.openGLWidget->fm.u = temp;

#ifdef ZJW_DEBUG
	cout << "cluster num  set to " << temp << endl;
#endif // ZJW_DEBUG
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

	//改frame id
	connect(ui.refFrameLineEdit, SIGNAL(returnPressed()), this, SLOT(changeRefFrameId()));
	connect(ui.targetFrameLineEdit, SIGNAL(returnPressed()), this, SLOT(changeTargetFrameId()));

	//chagnge oct cell size
	connect(ui.octTreeLeafineEdit, SIGNAL(returnPressed()), this, SLOT(changeOctCellSize()));
	//change kmeans result cluster num
	connect(ui.clusterNumLineEdit, SIGNAL(returnPressed()), this, SLOT(changeClusterNum()));
	//change motion vector u
	connect(ui.u_motionVecLineEdit, SIGNAL(returnPressed()), this, SLOT(changeU()));
}

pcsCompress::~pcsCompress()
{
}