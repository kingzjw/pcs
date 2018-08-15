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
#include"zjw_pcsRLGR.h"

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
	void clickPredictTargetRadioButton();

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

	//�ı�render mode
	void changeRenderMode();

	//�㷨���Ĳ���
	void trainMatP();
	void getSparseMatch();
	void getMotionVector();
	//test: ����  train P , sparse match , motion vector
	void test();


	//------------------compress----------------------

	//�����õ��ض�frame ��lap mat ����
	void testLapMat();
	
	//���뱣��õ�motion vector 
	bool getMotionVectorFromFile(VectorXd & vt)
	{
		
		int num;
		ifstream in("motionVector.txt");
		if (in.is_open())
		{
			cout << "start load e motion vertor from motionVector.txt ......." << endl;
			//��һ����������ά��
			in >> num;
			vt.resize(num);
			for (int i = 0; i < num; i++)
			{
				in >> vt(i);
			}	
			//cout << Vt << endl;
			cout << "finish load e motion vertor from motionVector.txt!!" << endl;

			in.close();
			return true;
		}

		return false;
	}

	//����rlgr�����motion vector����ѹ��
	void rlgr_mv_compress();

	//byte stream ��frame�ĵ��λ�ý���ѹ��
	void byteStreamTestPointsCompress();


public:
	pcsCompress(QWidget *parent = 0);
	~pcsCompress();

private:
	Ui::pcsCompressClass ui;

	string dirPath;
	string filePath;
};

#endif // PCSCOMPRESS_H
