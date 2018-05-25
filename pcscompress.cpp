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
	ui.openGLWidget->fm.getTwoFrameBestSparseMatch(9, 10, &f1SparseIdxList, &f2SparseIdxList, ui.openGLWidget->fm.FileNameForMat::NUM_TAIL, "walk_0_", path, true);
	
	//测试数据，拿到motion vector
	VectorXd Vt;
	ui.openGLWidget->fm.computeMotinVector(9, &f1SparseIdxList, &f2SparseIdxList, Vt);

	ui.openGLWidget->setShowFrameIdx(9);

	//ui.openGLWidget->renderState = 3;
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
#ifdef ZJW_DEBUG
	//cout << "change Ref Frame Id"<< ui.refFrameLineEdit->text() << endl;
#endif // ZJW_DEBUG
	

}

void pcsCompress::changeTargetFrameId()
{
#ifdef ZJW_DEBUG
	//cout << "change Target Frame Id" << ui.targetFrameLineEdit->text() << endl;
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
	connect(ui.refFrameLineEdit ,SIGNAL(returnPressed()), this, SLOT(changeRefFrameId()));
	connect(ui.targetFrameLineEdit, SIGNAL(returnPressed()), this, SLOT(changeTargetFrameId()));

}

pcsCompress::~pcsCompress()
{
}