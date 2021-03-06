﻿#ifndef QT_DYNAMIC_PLY_WIDGET_HPP
#define QT_DYNAMIC_PLY_WIDGET_HPP

#include <QGLWidget>
#include <QKeyEvent>
#include <iostream>
#include "util\zjw_obj.h"
#include "util\zjw_macro.h"
#include "zjw_pcs_octree.h"
#include "zjw_frame.h"

//#include <GL\glut.h>
//#include <gl/GLU.h>
//#include <opencv2/core/core.hpp>
//#include<opencv2/highgui/highgui.hpp>
//using namespace cv;
using namespace std;

class ZjwOpenGL : public  QGLWidget {
	Q_OBJECT

public:
	ZjwOpenGL(QWidget * parent = Q_NULLPTR);
	~ZjwOpenGL();

protected:
	//redifine the virtual function
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();

	//方向键平移
	void keyPressEvent(QKeyEvent *event);
	// redefine in QWidget class
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	//滚轮放大放小
	void wheelEvent(QWheelEvent * event);
	//时间事件
	void timerEvent(QTimerEvent *);

public:
	//use in paintGL function
	void render();

public:
	GLfloat	 *lightPos;
	GLfloat scale;
	//标记当前是stop 或者是paly 的状态
	bool isPlay;
	//3 旋转的速度
	int playSpeed;
	//50ms 旋转的刷新时间
	int palyIntervalTime;
	int timeID;  //即使器的id

private:
	//局部坐标系下的
	GLfloat rotationX;
	GLfloat rotationY;
	GLfloat rotationZ;
	//世界坐标系下的
	GLfloat rotationWorldX;
	GLfloat rotationWorldY;
	GLfloat rotationWorldZ;
	QPoint lastPoint;

public:
	//-----------------------------针对应用进行扩展--------------
	//ObjMesh objMesh;
	//PcsOctree pcsOct;
	FrameManage fm;

	//0: 表示不渲染，1: 表示渲染点云，2: 表示点云对应的八叉树 
	//3:表示渲染的两个连续帧之间的点云(所有匹配线). 5. 画训练数据的对应关系
	//4: sparse match之间的点云
	//6: 渲染预测出来的taget frame 和真实的target frame
	//7: 只渲染预测出来的target frame
	//8: 渲染预测出来的taget frame 和真实的target frame+ referece frame
	//9: 渲染预测出来的taget frame + referece frame
	//10:已经使用的。
	//12:测试position的压缩和传输
	int renderState;
	//reference frame
	int showFrameIdx;
	//target frame 比 showFrameIdx
	int showFrameIdx2;

	void setShowFrameIdx(int i = 0);

	//画点云
	void drawPointCloud(ObjMesh &objMesh, Vec3 color = Vec3(1.0, 0.0, 0.0));
	void drawPointCloudPridictTargetFrame(ObjMesh &objMesh);
	//画点云和 八叉树的包围结构
	void drawPointCloudOctree(ObjMesh &objMesh, PcsOctree &pcsOct);
	//draw 长方体的框
	void drawWireCube(Vec3 min, Vec3 max);
	//画线
	void drawSparseMatchLine(double moveStep);
	//画两帧之间所有对称的线
	void drawBestMatchLine(double moveStep);

	//对训练数据之间的data进行学习
	void drawTrainMatchLine(double moveStep, Vec3 color = Vec3(1.0, 1.0, 0.0));

	//==============test=============
	//测试pos传输之后解压的得到的frame的点云的结果是否是正确的
	//存储传输解压之后的结果
	ObjMesh testObjMesh;
	void drawPositionForFrameCompression(ObjMesh &objMesh);

};

#endif