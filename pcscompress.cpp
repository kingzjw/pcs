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
	//���ò������������˲���
	ui.openGLWidget->pcsOct.setParam(min,max,cellSize);
	ui.openGLWidget->pcsOct.buildPcsOctFrmPC(& ui.openGLWidget->objMesh);
	//�õ�Ҷ�ӽڵ�ı߽磬��������ص���Ϣ
	ui.openGLWidget->pcsOct.getLeafboundary();

	ui.openGLWidget->pcsOct.getGraphMat();
	ui.openGLWidget->pcsOct.getMatEigenVerValue();

	//�õ��˲����϶�����ź�
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

	//���ļ�
	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(clickedOpenFileAction()));
}

pcsCompress::~pcsCompress()
{

}
