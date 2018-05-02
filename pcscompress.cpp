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
	std::cout << "you choose the file: " << std::endl << stt << endl;
	//std::cout << "you can choose more rubost load obj code by yourself!!" << endl;
	cout << "==================" << endl;
	
	//TIME_START
	ui.openGLWidget->objMesh.loadObjMesh(stt);
	//TIME_END("load mesh time : ")

	Vec3 min(ui.openGLWidget->objMesh.rangeMin);
	Vec3 max((ui.openGLWidget->objMesh.rangeMax+Epsilon));
	Vec3 cellSize(0.42);
	//设置参数，并构建八叉树
	ui.openGLWidget->pcsOct.setParam(min,max,cellSize);
	ui.openGLWidget->pcsOct.buildPcsOctFrmPC(& ui.openGLWidget->objMesh);
	//得到叶子节点的边界，并保存相关的信息
	ui.openGLWidget->pcsOct.getLeafboundary();

	ui.openGLWidget->pcsOct.getGraphMat();
	ui.openGLWidget->pcsOct.getMatEigenVerValue();

	//得到八叉树上顶点的信号
	ui.openGLWidget->pcsOct.setPointTo8Areas();
	ui.openGLWidget->pcsOct.getLeafSignal();
	ui.openGLWidget->pcsOct.getSgwtCoeffWS();


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
