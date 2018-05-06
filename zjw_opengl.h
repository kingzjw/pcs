#ifndef QT_DYNAMIC_PLY_WIDGET_HPP
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
	int renderState;
	void drawPointCloud(ObjMesh &objMesh);
	void drawPointCloudOctree(ObjMesh &objMesh, PcsOctree &pcsOct);
	void drawWireCube(Vec3 min, Vec3 max);
};

#endif