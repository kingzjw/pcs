#ifndef PCSCOMPRESS_H
#define PCSCOMPRESS_H

#include <QtWidgets/QMainWindow>
#include <QButtonGroup>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QLabel>
#include <string>
#include <iostream>
#include <sstream>

#include "ui_pcscompress.h"

#include "util/zjw_obj.h"

class pcsCompress : public QMainWindow
{
	Q_OBJECT

public slots:
	void clickedOpenFileAction();

	//�ı�render mode
	void clickPointCloudButton();
	void clickPCOctTreeButton();
	void clickTwoFrameButton();
	void clickTwoFrameSparseMatchButton();
	void clickOthers();


	//frame id change. �ı�������֮֡���
	void changeRefFrameId();
	void changeTargetFrameId();

	//chagnge oct cell size
	void changeOctCellSize();
	//change kmeans result cluster num
	void changeClusterNum();
	//change motion vector u
	void changeU();

	//�ı�sgwt�е� scale M����������
	void changeMDegreeCheby();
	void changeNscales();

	//�㷨���Ĳ���
	void trainMatP();
	void getSparseMatch();
	void getMotionVector();
	void test();
	void testLapMat();

public:
	pcsCompress(QWidget *parent = 0);
	~pcsCompress();

private:
	Ui::pcsCompressClass ui;

	string dirPath;
	string filePath;
};

#endif // PCSCOMPRESS_H
