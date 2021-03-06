﻿//-----------------------zjw opengl in qt----------------

#include "zjw_opengl.h"
#include <QtOpenGL>
#include <QtGui>
#include <QString>
#include <gl/GLU.H>

#include<iostream>
using namespace std;

ZjwOpenGL::ZjwOpenGL(QWidget * parent) : QGLWidget(parent) {
	lightPos = new GLfloat[4];
	lightPos[0] = 0.0f;
	lightPos[1] = 0.0f;
	lightPos[2] = 300.0f;
	lightPos[3] = 1.0f;
	//end
	rotationX = 0;
	rotationY = 0;
	rotationZ = 10;
	rotationWorldX = 0;
	rotationWorldY = 0;
	rotationWorldZ = 10;
	scale = 1.0;

	//播放设置
	playSpeed = 3;
	palyIntervalTime = 75;
	isPlay = true;

	//============针对应用的扩展，可以删除=======================
	renderState = 0;
	showFrameIdx = 0;
	showFrameIdx2 = 1;
}

ZjwOpenGL::~ZjwOpenGL() {
}

void ZjwOpenGL::initializeGL()
{
	//为了使移动有效果
	setFocusPolicy(Qt::StrongFocus);

	//qglClearColor(Qt::black);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_NORMALIZE);
	//对3D进行光滑
	glEnable(GL_SMOOTH);
	//glEnable(GL_FLAT);
	//设置光照
	glEnable(GL_LIGHTING);

	GLfloat ambientLight[] = { 0.2f,0.2f,0.2f,1.0f };//三种光照的强度
	GLfloat diffuseLight[] = { 0.5f,0.5f,0.5f,1.0f };
	GLfloat  specularLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

	glEnable(GL_LIGHT0);

	//设置反射材料属性
	GLfloat  specref[] = { 1.0f, 1.0f, 1.0f, 1.0f };//镜面反射属性
	glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
	glMateriali(GL_FRONT, GL_SHININESS, 50);  //0-128 设置镜面指数，越大，亮度越大，越集中

											   //设置环境光，散射光的材料属性
	glEnable(GL_COLOR_MATERIAL);  //启用颜色追踪
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}
void ZjwOpenGL::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat x = GLfloat(width) / height;

	//gluPerspective(60, width / height, 0.1, 200);
	////gluLookAt(0, 0, -50,0, 0, 0, 0, 1, 0);
	//glTranslatef(0, 0, -5);

	// 因为我已经把坐标实际的值归一化到-0.5-0.5，所以只需要
	//glOrtho是创建一个正交平行的视景体,范围只需要在-1到1之间就可以了
	GLfloat nRange = 1.0f;
	//下面这段代码，保证了，物体的尺寸不会随着窗体的变化而变化
	if (width <= height)
		glOrtho(-nRange, nRange, -nRange*height / width, nRange*height / width, -nRange, nRange);
	else
		glOrtho(-nRange*width / height, nRange*width / height, -nRange, nRange, -nRange, nRange);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);//设置光照的位置
}
//glDraw()调用下面这个虚函数
void ZjwOpenGL::paintGL()
{
	glClearColor(255, 255, 255, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	render();
}
void ZjwOpenGL::render()
{
	//局部坐标系下，响应鼠标事件等
	glPushMatrix();
	// Save matrix state and do the rotation
	glScalef(scale, scale, scale);

	glRotatef(rotationX, 1.0f, 0.0f, 0.0f);
	glRotatef(rotationY, 0.0f, 1.0f, 0.0f);
	glRotatef(rotationZ, 0.0f, 1.0f, 0.0f);

	if (renderState == 0)
	{
		//不进行渲染
	}
	else if (renderState == 1)
	{
		drawPointCloud(*(fm.frameList[showFrameIdx]->objMesh));
		//drawPointCloud(objMesh);
	}
	else if (renderState == 2)
	{
		drawPointCloudOctree(*(fm.frameList[showFrameIdx]->objMesh), *(fm.frameList[showFrameIdx]->pcsOct));
		//drawPointCloudOctree(objMesh, pcsOct);
	}
	else if (renderState == 3)
	{
		//drawPointCloud(*(fm.frameList[showFrameIdx]->objMesh));
		drawPointCloudOctree(*(fm.frameList[showFrameIdx]->objMesh), *(fm.frameList[showFrameIdx]->pcsOct));

		glPushMatrix();
		double movestep = -0.5;
		glTranslated(-0.5, 0, 0);
		//drawPointCloud(*(fm.frameList[showFrameIdx2]->objMesh));
		drawPointCloudOctree(*(fm.frameList[showFrameIdx2]->objMesh), *(fm.frameList[showFrameIdx2]->pcsOct));
		glPopMatrix();

		//画匹配线
		//drawSparseMatchLine(movestep);

		//所有的线
		drawBestMatchLine(movestep);
	}
	else if (renderState == 5)
	{
		//drawPointCloud(*(fm.frameList[showFrameIdx]->objMesh));
		drawPointCloudOctree(*(fm.frameList[showFrameIdx]->objMesh), *(fm.frameList[showFrameIdx]->pcsOct));

		glPushMatrix();
		double movestep = -0.5;
		glTranslated(-0.5, 0, 0);
		//drawPointCloud(*(fm.frameList[showFrameIdx2]->objMesh));
		drawPointCloudOctree(*(fm.frameList[showFrameIdx2]->objMesh), *(fm.frameList[showFrameIdx2]->pcsOct));
		glPopMatrix();

		//画匹配线
		drawSparseMatchLine(movestep);

		//所有的线
		//drawBestMatchLine(movestep);
	}
	else if (renderState == 4)
	{
		//--------------画训练数据的对应关系------------------------

		drawPointCloudOctree(*(fm.frameList[showFrameIdx]->objMesh), *(fm.frameList[showFrameIdx]->pcsOct));

		glPushMatrix();
		double movestep = -0.5;
		glTranslated(-0.5, 0, 0);
		drawPointCloudOctree(*(fm.frameList[showFrameIdx2]->objMesh), *(fm.frameList[showFrameIdx2]->pcsOct));
		glPopMatrix();

		//画匹配线
		drawTrainMatchLine(movestep);
	}
	else if (renderState == 6)
	{
		//渲染预测出来的targetframe 和真实的targetframe
		drawPointCloudPridictTargetFrame(*(fm.frameList[showFrameIdx]->objMesh));
		drawPointCloud(*(fm.frameList[showFrameIdx2]->objMesh));
	}
	else if (renderState == 7)
	{
		//只渲染预测出来的额target frame
		drawPointCloudPridictTargetFrame(*(fm.frameList[showFrameIdx]->objMesh));
	}
	else if (renderState == 8)
	{
		//只渲染预测出来的额target frame
		drawPointCloudPridictTargetFrame(*(fm.frameList[showFrameIdx]->objMesh));
		drawPointCloud(*(fm.frameList[showFrameIdx2]->objMesh),Vec3(0,0,0));
		drawPointCloud(*(fm.frameList[showFrameIdx]->objMesh));

	}
	else if (renderState == 9)
	{
		//只渲染预测出来的额target frame
		drawPointCloudPridictTargetFrame(*(fm.frameList[showFrameIdx]->objMesh));
		drawPointCloud(*(fm.frameList[showFrameIdx]->objMesh));
	}
	else if (renderState == 10)
	{
		//cout << "red is refrence , black is target" << endl;
		drawPointCloud(*(fm.frameList[showFrameIdx]->objMesh));
		drawPointCloud(*(fm.frameList[showFrameIdx2]->objMesh), Vec3(0, 0, 0));
	}
	else if (renderState == 12)
	{
		drawPositionForFrameCompression(testObjMesh);
	}

	glPopMatrix();
}

void ZjwOpenGL::setShowFrameIdx(int i)
{
	showFrameIdx = i;
	showFrameIdx2 = showFrameIdx + 1;
}

void ZjwOpenGL::drawPointCloud(ObjMesh & objMesh,Vec3 Color)
{
	glColor3f(Color.x, Color.y, Color.z);
	glBegin(GL_POINTS);
	glPointSize(3);
	for (int v_it = 0; v_it < objMesh.mesh.facePoslist.size(); v_it++)
	{
		glVertex3f(objMesh.vertexList[objMesh.mesh.facePoslist[v_it].x].x, objMesh.vertexList[objMesh.mesh.facePoslist[v_it].x].y,
			objMesh.vertexList[objMesh.mesh.facePoslist[v_it].x].z);
		glVertex3f(objMesh.vertexList[objMesh.mesh.facePoslist[v_it].y].x, objMesh.vertexList[objMesh.mesh.facePoslist[v_it].y].y,
			objMesh.vertexList[objMesh.mesh.facePoslist[v_it].y].z);
		glVertex3f(objMesh.vertexList[objMesh.mesh.facePoslist[v_it].z].x, objMesh.vertexList[objMesh.mesh.facePoslist[v_it].z].y,
			objMesh.vertexList[objMesh.mesh.facePoslist[v_it].z].z);
	}
	glEnd();
}

void ZjwOpenGL::drawPointCloudPridictTargetFrame(ObjMesh & objMesh)
{
	//tips:
	//cout << "draw predict target frame" << endl;
	//黑色
	glColor3f(1.0f, 1.0f, 0.0f);
	glBegin(GL_POINTS);
	//glPointSize(3);
	glPointSize(6);
	/*for (int v_it = 0; v_it < objMesh.mesh.facePoslist.size(); v_it++)
	{
		glVertex3f(objMesh.vertexPredictTargetList[objMesh.mesh.facePoslist[v_it].x].x, objMesh.vertexPredictTargetList[objMesh.mesh.facePoslist[v_it].x].y,
			objMesh.vertexPredictTargetList[objMesh.mesh.facePoslist[v_it].x].z);
		glVertex3f(objMesh.vertexPredictTargetList[objMesh.mesh.facePoslist[v_it].y].x, objMesh.vertexPredictTargetList[objMesh.mesh.facePoslist[v_it].y].y,
			objMesh.vertexPredictTargetList[objMesh.mesh.facePoslist[v_it].y].z);
		glVertex3f(objMesh.vertexPredictTargetList[objMesh.mesh.facePoslist[v_it].z].x, objMesh.vertexPredictTargetList[objMesh.mesh.facePoslist[v_it].z].y,
			objMesh.vertexPredictTargetList[objMesh.mesh.facePoslist[v_it].z].z);
	}*/
	for (int i = 0; i < objMesh.vertexPredictTargetList.size(); i++)
	{
		glVertex3f(objMesh.vertexPredictTargetList[i].x, objMesh.vertexPredictTargetList[i].y,objMesh.vertexPredictTargetList[i].z);
	}
	glEnd();
}

void ZjwOpenGL::drawPointCloudOctree(ObjMesh & objMesh, PcsOctree & pcsOct)
{
	drawPointCloud(objMesh);
	//draw octree

	//线框的形式
	glPolygonMode(GL_FRONT, GL_LINE);
	//glDisable(GL_CULL_FACE);
	//glPolygonMode(GL_BACK, GL_LINE);
	//glPolygonMode(GL_BACK, GL_FILL);

	for (int i = 0; i < pcsOct.ctLeaf->minVList.size(); i++)
	{
		glColor3f(0.0f, 0.0f, 1.0f);
		glLineWidth(1);

		////test 
		//if (i == 233)
		//{
		//	glColor3f(0.0f, 1.0f, 0.0f);
		//	glLineWidth(3);
		//}
		//else if ( i == 243 || i == 244 || i == 245 || i == 246 
		//	||( i >=226 && i<=234))
		//{
		//	//黄色
		//	glColor3f(1.0f, 1.0f, 0.0f);
		//	glLineWidth(3);
		//}
		//end test
		drawWireCube(pcsOct.ctLeaf->minVList[i], pcsOct.ctLeaf->maxVList[i]);
	}
#ifdef ZJW_DEBUG
#endif
}

void ZjwOpenGL::drawWireCube(Vec3 min, Vec3 max)
{
	

	glBegin(GL_QUAD_STRIP);
	glVertex3f(min.x, min.y, min.z);
	glVertex3f(min.x, max.y, min.z);

	glVertex3f(max.x, min.y, min.z);
	glVertex3f(max.x, max.y, min.z);

	glVertex3f(max.x, min.y, max.z);
	glVertex3f(max.x, max.y, max.z);

	glVertex3f(min.x, min.y, max.z);
	glVertex3f(min.x, max.y, max.z);

	glVertex3f(min.x, min.y, min.z);
	glVertex3f(min.x, max.y, min.z);
	//结束绘GL_QUAD_STRIP
	glEnd();

	glBegin(GL_QUADS);
	//顶面
	glVertex3f(min.x, max.y, min.z);
	glVertex3f(min.x, max.y, max.z);
	glVertex3f(max.x, max.y, max.z);
	glVertex3f(max.x, max.y, min.z);

	//底面
	glVertex3f(min.x, min.y, min.z);
	glVertex3f(max.x, min.y, min.z);
	glVertex3f(max.x, min.y, max.z);
	glVertex3f(min.x, min.y, max.z);

	glEnd();
}

void ZjwOpenGL::drawSparseMatchLine(double moveStep)
{
	Frame * frame1 = fm.frameList[showFrameIdx];
	Frame * frame2 = fm.frameList[showFrameIdx2];

	//遍历所有稀疏的匹配
	glLineWidth(2.0f);
	glColor3f(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < fm.f1SparseIdxList.size(); i++)
	{
		Vec3 v1 = (*frame1->pcsOct->ctLeaf->midVList)[fm.f1SparseIdxList[i]];
		Vec3 v2 = (*frame2->pcsOct->ctLeaf->midVList)[fm.f2SparseIdxList[i]];
		
		glBegin(GL_LINES);
		glVertex3f(v1.x, v1.y, v1.z);
		glVertex3f(v2.x + moveStep, v2.y, v2.z);
		glEnd();
	}
	glLineWidth(1.0f);

}

void ZjwOpenGL::drawBestMatchLine(double moveStep)
{
#ifdef ZJW_DEBUG
	//cout << "draw Best Match Line!" << endl;
#endif	//zjw_debug

	Frame * frame1 = fm.frameList[showFrameIdx];
	Frame * frame2 = fm.frameList[showFrameIdx2];

	//遍历所有稀疏的匹配
	glLineWidth(1.5f);
	glColor3f(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < fm.f1nIdxList.size(); i++)
	{
		Vec3 v1 = (*frame1->pcsOct->ctLeaf->midVList)[fm.f1nIdxList[i]];
		Vec3 v2 = (*frame2->pcsOct->ctLeaf->midVList)[fm.f2nIdxList[i]];

		glBegin(GL_LINES);
		glVertex3f(v1.x, v1.y, v1.z);
		glVertex3f(v2.x + moveStep, v2.y, v2.z);
		glEnd();
	}
	glLineWidth(1.0f);
}

void ZjwOpenGL::drawTrainMatchLine(double moveStep, Vec3 color)
{
#ifdef ZJW_DEBUG
	//cout << "draw Best Match Line!" << endl;
#endif	//zjw_debug

	Frame * frame1 = fm.frameList[showFrameIdx];
	Frame * frame2 = fm.frameList[showFrameIdx2];

	//遍历所有稀疏的匹配
	glLineWidth(1.5f);
	glColor3f(color.x, color.y, color.z);
	for (int i = 0; i < fm.f1TrainList.size(); i++)
	{
		Vec3 v1 = (*frame1->pcsOct->ctLeaf->midVList)[fm.f1TrainList[i]];
		Vec3 v2 = (*frame2->pcsOct->ctLeaf->midVList)[fm.f2TrainList[i]];

		glBegin(GL_LINES);
		glVertex3f(v1.x, v1.y, v1.z);
		glVertex3f(v2.x + moveStep, v2.y, v2.z);
		glEnd();
	}
	glLineWidth(1.0f);
}

void ZjwOpenGL::drawPositionForFrameCompression(ObjMesh & objMesh)
{
	//黑色
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POINTS);
	//glPointSize(3);
	glPointSize(12);
	
	for (int i = 0; i < objMesh.vertexList.size(); i++)
	{
		glVertex3f(objMesh.vertexList[i].x, objMesh.vertexList[i].y, objMesh.vertexList[i].z);
	}
	glEnd();
}

void ZjwOpenGL::keyPressEvent(QKeyEvent * event)
{
	float speed = 0.3;
	//std::cout << "key press event" << std::endl;
	switch (event->key()) {
	case Qt::Key_Up:
		glTranslated(0, speed, 0);  // 上移
		break;
	case Qt::Key_Down:
		glTranslated(0, -speed, 0);  // 上移
		break;
	case Qt::Key_Left:
		glTranslated(-speed, 0, 0);  // 上移
		break;
	case Qt::Key_Right:
		glTranslated(speed, 0, 0);  // 上移
		break;
	}
	updateGL();
}
void ZjwOpenGL::mousePressEvent(QMouseEvent * event)
{
	lastPoint = event->pos();
}
void ZjwOpenGL::mouseMoveEvent(QMouseEvent * event)
{
	GLfloat dx = GLfloat(event->x() - lastPoint.x()) / width();
	GLfloat dy = GLfloat(event->y() - lastPoint.y()) / height();

	if (event->buttons() & Qt::LeftButton) { //左键
		rotationX += 180 * dy;
		rotationY += 180 * dx;
		updateGL(); //Updates the widget by calling glDraw().
	}
	else if (event->buttons() & Qt::RightButton) { //右键
		rotationX += 180 * dy;
		rotationZ += 180 * dx;
		updateGL();
	}
	lastPoint = event->pos();
}
void ZjwOpenGL::wheelEvent(QWheelEvent * event)
{
	double numDegrees = -event->delta() / 8.0;
	double numSteps = numDegrees / 15.0;
	scale *= pow(1.125, numSteps);
	updateGL();
}
void ZjwOpenGL::timerEvent(QTimerEvent *)
{
	rotationWorldY += playSpeed; //绕着Y轴转
	updateGL(); //Updates the widget by calling glDraw().
}