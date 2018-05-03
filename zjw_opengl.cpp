//-----------------------zjw opengl in qt----------------

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
	}
	else if (renderState == 1)
	{
		drawPointCloud(objMesh);
	}
	else if (renderState == 2)
	{
		drawPointCloudOctree(objMesh, pcsOct);
	}

	glPopMatrix();
}

void ZjwOpenGL::drawPointCloud(ObjMesh & objMesh)
{
	glColor3f(1.0f, 0.0f, 0.0f);
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
		drawWireCube(pcsOct.ctLeaf->minVList[i], pcsOct.ctLeaf->maxVList[i]);
	}
#ifdef ZJW_DEDUG
	/*Vec3 min(0, 0, 0);
	Vec3 max(1, 1, 1);
	drawWireCube(min,max);*/
#endif
}

void ZjwOpenGL::drawWireCube(Vec3 min, Vec3 max)
{
	glColor3f(0.0f, 0.0f, 1.0f);

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