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
	void clickTwoFrameSparseMatchButton();
	void clickPredictTargetRadioButton();


	//frame id change. 改变连续两帧之间的
	void changeRefFrameId();
	void changeTargetFrameId();

	//chagnge oct cell size
	void changeOctCellSize();
	//change kmeans result cluster num
	void changeClusterNum();
	//change motion vector u
	void changeU();

	//改变sgwt中的 scale M阶两个参数
	void changeMDegreeCheby();
	void changeNscales();

	//改变render mode
	void changeRenderMode();

	//算法核心步骤
	void trainMatP();
	void getSparseMatch();
	void getMotionVector();
	void test();
	void testLapMat();

	//读入保存好的motion vector 
	bool getMotionVectorFromFile(VectorXd & vt)
	{
		VectorXd Vt;
		int num;
		ifstream in("motionVector.txt");
		if (in.is_open())
		{
			cout << "start load e motion vertor from motionVector.txt ......." << endl;
			//第一个是向量的维数
			in >> num;
			Vt.resize(num);
			for (int i = 0; i < num; i++)
			{
				cin >> Vt(i);
			}	
			cout << Vt << endl;
			cout << "finish load e motion vertor from motionVector.txt!!" << endl;

			in.close();
		}

		return false;
	}

public:
	pcsCompress(QWidget *parent = 0);
	~pcsCompress();

private:
	Ui::pcsCompressClass ui;

	string dirPath;
	string filePath;
};

#endif // PCSCOMPRESS_H
