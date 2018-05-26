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

	//改变render mode
	void clickPointCloudButton();
	void clickPCOctTreeButton();
	void clickTwoFrameButton();

	//frame id change. 改变连续两帧之间的
	void changeRefFrameId();
	void changeTargetFrameId();

	//chagnge oct cell size
	void changeOctCellSize();
	//change kmeans result cluster num
	void changeClusterNum();
	//change motion vector u
	void changeU();

	//算法核心步骤
	void trainMatP();
	void getSparseMatch();
	void getMotionVector();
	void test();

public:
	pcsCompress(QWidget *parent = 0);
	~pcsCompress();

private:
	Ui::pcsCompressClass ui;

	string dirPath;
	string filePath;
	//测试数据，拿到稀疏最佳匹配
	vector<int> f1SparseIdxList;
	vector<int> f2SparseIdxList;
};

#endif // PCSCOMPRESS_H
