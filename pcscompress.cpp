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

	ui.openGLWidget->renderState = 2;
	ui.openGLWidget->updateGL();

	//测试数据
	vector<int> f1nIdxList;
	vector<int> f2nIdxList;
	vector<double> maDist;
	ui.openGLWidget->fm.loadContinuousFrames(2, 3, ui.openGLWidget->fm.FileNameForMat::NUM_TAIL, "walk_0_", path);
	ui.openGLWidget->fm.getBestMatchPoint(2, 3, ui.openGLWidget->fm.P, &f1nIdxList, &f2nIdxList, &maDist);
	vector<int> f1SparseIdxList;
	vector<int> f2SparseIdxList;
	ui.openGLWidget->fm.doKmeansGetSparseBestMatch(3, &f1nIdxList, &f2nIdxList, &maDist, &f1SparseIdxList, &f2SparseIdxList);

	ui.openGLWidget->renderState = 2;
	ui.openGLWidget->updateGL();
}

pcsCompress::pcsCompress(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	//打开文件
	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(clickedOpenFileAction()));
}

pcsCompress::~pcsCompress()
{
}