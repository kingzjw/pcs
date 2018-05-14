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

	//ui.openGLWidget->fm.batchLoadObj();
	ui.openGLWidget->fm.loadContinuousFrames(0, 1, ui.openGLWidget->fm.FileNameForMat::NUM_TAIL,  "walk_0_", path);
	//ui.openGLWidget->fm.loadContinuousFrames(0, 1, ui.openGLWidget->fm.FileNameForMat::NUM_FRONT, "_cleaner", path);
	ui.openGLWidget->fm.matchNode(0, 1);

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