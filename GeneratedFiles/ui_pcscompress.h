/********************************************************************************
** Form generated from reading UI file 'pcscompress.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PCSCOMPRESS_H
#define UI_PCSCOMPRESS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include <zjw_opengl.h>

QT_BEGIN_NAMESPACE

class Ui_pcsCompressClass
{
public:
    QAction *actionOpen;
    QAction *actiontrainMatP;
    QAction *actiongetSparseMatch;
    QAction *actiongetMotionVector;
    QAction *actionTest;
    QWidget *centralWidget;
    QTabWidget *tabWidget;
    QWidget *parameter;
    QWidget *renderWidget;
    QLabel *oct_2;
    QRadioButton *pc_radioButtion;
    QRadioButton *oct_radioButton;
    QRadioButton *twoframe_radioButton;
    QLineEdit *refFrameLineEdit;
    QLabel *refFrame;
    QLabel *targerFrame;
    QLineEdit *targetFrameLineEdit;
    QLabel *label;
    QLabel *label_2;
    QWidget *algorithmWidget;
    QLabel *u_motionVec;
    QLineEdit *clusterNumLineEdit;
    QLabel *oct;
    QLabel *cluster;
    QLineEdit *u_motionVecLineEdit;
    QLineEdit *octTreeLeafineEdit;
    QWidget *others;
    ZjwOpenGL *openGLWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuOperation;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *pcsCompressClass)
    {
        if (pcsCompressClass->objectName().isEmpty())
            pcsCompressClass->setObjectName(QStringLiteral("pcsCompressClass"));
        pcsCompressClass->resize(811, 554);
        actionOpen = new QAction(pcsCompressClass);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        actiontrainMatP = new QAction(pcsCompressClass);
        actiontrainMatP->setObjectName(QStringLiteral("actiontrainMatP"));
        actiongetSparseMatch = new QAction(pcsCompressClass);
        actiongetSparseMatch->setObjectName(QStringLiteral("actiongetSparseMatch"));
        actiongetMotionVector = new QAction(pcsCompressClass);
        actiongetMotionVector->setObjectName(QStringLiteral("actiongetMotionVector"));
        actionTest = new QAction(pcsCompressClass);
        actionTest->setObjectName(QStringLiteral("actionTest"));
        centralWidget = new QWidget(pcsCompressClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setGeometry(QRect(0, 0, 131, 481));
        parameter = new QWidget();
        parameter->setObjectName(QStringLiteral("parameter"));
        renderWidget = new QWidget(parameter);
        renderWidget->setObjectName(QStringLiteral("renderWidget"));
        renderWidget->setGeometry(QRect(0, 290, 120, 161));
        oct_2 = new QLabel(renderWidget);
        oct_2->setObjectName(QStringLiteral("oct_2"));
        oct_2->setGeometry(QRect(0, 10, 81, 16));
        pc_radioButtion = new QRadioButton(renderWidget);
        pc_radioButtion->setObjectName(QStringLiteral("pc_radioButtion"));
        pc_radioButtion->setGeometry(QRect(10, 30, 89, 16));
        oct_radioButton = new QRadioButton(renderWidget);
        oct_radioButton->setObjectName(QStringLiteral("oct_radioButton"));
        oct_radioButton->setGeometry(QRect(10, 50, 111, 16));
        oct_radioButton->setChecked(true);
        twoframe_radioButton = new QRadioButton(renderWidget);
        twoframe_radioButton->setObjectName(QStringLiteral("twoframe_radioButton"));
        twoframe_radioButton->setGeometry(QRect(10, 70, 101, 16));
        refFrameLineEdit = new QLineEdit(renderWidget);
        refFrameLineEdit->setObjectName(QStringLiteral("refFrameLineEdit"));
        refFrameLineEdit->setGeometry(QRect(80, 100, 31, 20));
        refFrameLineEdit->setDragEnabled(true);
        refFrame = new QLabel(renderWidget);
        refFrame->setObjectName(QStringLiteral("refFrame"));
        refFrame->setGeometry(QRect(0, 100, 61, 16));
        targerFrame = new QLabel(renderWidget);
        targerFrame->setObjectName(QStringLiteral("targerFrame"));
        targerFrame->setGeometry(QRect(0, 120, 81, 16));
        targetFrameLineEdit = new QLineEdit(renderWidget);
        targetFrameLineEdit->setObjectName(QStringLiteral("targetFrameLineEdit"));
        targetFrameLineEdit->setGeometry(QRect(80, 120, 31, 20));
        targetFrameLineEdit->setDragEnabled(true);
        label = new QLabel(parameter);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(0, 270, 54, 12));
        label_2 = new QLabel(parameter);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(0, 10, 54, 12));
        algorithmWidget = new QWidget(parameter);
        algorithmWidget->setObjectName(QStringLiteral("algorithmWidget"));
        algorithmWidget->setGeometry(QRect(0, 30, 120, 131));
        u_motionVec = new QLabel(algorithmWidget);
        u_motionVec->setObjectName(QStringLiteral("u_motionVec"));
        u_motionVec->setGeometry(QRect(30, 50, 21, 16));
        clusterNumLineEdit = new QLineEdit(algorithmWidget);
        clusterNumLineEdit->setObjectName(QStringLiteral("clusterNumLineEdit"));
        clusterNumLineEdit->setGeometry(QRect(80, 30, 31, 20));
        clusterNumLineEdit->setDragEnabled(true);
        oct = new QLabel(algorithmWidget);
        oct->setObjectName(QStringLiteral("oct"));
        oct->setGeometry(QRect(0, 10, 81, 16));
        cluster = new QLabel(algorithmWidget);
        cluster->setObjectName(QStringLiteral("cluster"));
        cluster->setGeometry(QRect(0, 30, 81, 16));
        u_motionVecLineEdit = new QLineEdit(algorithmWidget);
        u_motionVecLineEdit->setObjectName(QStringLiteral("u_motionVecLineEdit"));
        u_motionVecLineEdit->setGeometry(QRect(80, 50, 31, 20));
        u_motionVecLineEdit->setDragEnabled(true);
        octTreeLeafineEdit = new QLineEdit(algorithmWidget);
        octTreeLeafineEdit->setObjectName(QStringLiteral("octTreeLeafineEdit"));
        octTreeLeafineEdit->setGeometry(QRect(80, 10, 31, 20));
        octTreeLeafineEdit->setDragEnabled(false);
        tabWidget->addTab(parameter, QString());
        others = new QWidget();
        others->setObjectName(QStringLiteral("others"));
        tabWidget->addTab(others, QString());
        openGLWidget = new ZjwOpenGL(centralWidget);
        openGLWidget->setObjectName(QStringLiteral("openGLWidget"));
        openGLWidget->setGeometry(QRect(130, 0, 681, 491));
        pcsCompressClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(pcsCompressClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 811, 23));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuOperation = new QMenu(menuBar);
        menuOperation->setObjectName(QStringLiteral("menuOperation"));
        pcsCompressClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(pcsCompressClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        pcsCompressClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(pcsCompressClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        pcsCompressClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuOperation->menuAction());
        menuFile->addAction(actionOpen);
        menuOperation->addAction(actiontrainMatP);
        menuOperation->addAction(actiongetSparseMatch);
        menuOperation->addAction(actiongetMotionVector);
        menuOperation->addAction(actionTest);

        retranslateUi(pcsCompressClass);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(pcsCompressClass);
    } // setupUi

    void retranslateUi(QMainWindow *pcsCompressClass)
    {
        pcsCompressClass->setWindowTitle(QApplication::translate("pcsCompressClass", "pcsCompress", Q_NULLPTR));
        actionOpen->setText(QApplication::translate("pcsCompressClass", "Open", Q_NULLPTR));
        actiontrainMatP->setText(QApplication::translate("pcsCompressClass", "trainMatP", Q_NULLPTR));
        actiongetSparseMatch->setText(QApplication::translate("pcsCompressClass", "getSparseMatch", Q_NULLPTR));
        actiongetMotionVector->setText(QApplication::translate("pcsCompressClass", "getMotionVector", Q_NULLPTR));
        actionTest->setText(QApplication::translate("pcsCompressClass", "Test", Q_NULLPTR));
        oct_2->setText(QApplication::translate("pcsCompressClass", "render mode", Q_NULLPTR));
        pc_radioButtion->setText(QApplication::translate("pcsCompressClass", "point cloud", Q_NULLPTR));
        oct_radioButton->setText(QApplication::translate("pcsCompressClass", "octTree and pc", Q_NULLPTR));
        twoframe_radioButton->setText(QApplication::translate("pcsCompressClass", "twoFrame cmp", Q_NULLPTR));
        refFrameLineEdit->setInputMask(QString());
        refFrameLineEdit->setText(QApplication::translate("pcsCompressClass", "0", Q_NULLPTR));
        refFrame->setText(QApplication::translate("pcsCompressClass", "ref frame", Q_NULLPTR));
        targerFrame->setText(QApplication::translate("pcsCompressClass", "target frame", Q_NULLPTR));
        targetFrameLineEdit->setInputMask(QString());
        targetFrameLineEdit->setText(QApplication::translate("pcsCompressClass", "1", Q_NULLPTR));
        label->setText(QApplication::translate("pcsCompressClass", "render", Q_NULLPTR));
        label_2->setText(QApplication::translate("pcsCompressClass", "algorithm", Q_NULLPTR));
        u_motionVec->setText(QApplication::translate("pcsCompressClass", " u", Q_NULLPTR));
        clusterNumLineEdit->setInputMask(QString());
        clusterNumLineEdit->setText(QApplication::translate("pcsCompressClass", "5", Q_NULLPTR));
        oct->setText(QApplication::translate("pcsCompressClass", "octTree leaf", Q_NULLPTR));
        cluster->setText(QApplication::translate("pcsCompressClass", "cluster num", Q_NULLPTR));
        u_motionVecLineEdit->setInputMask(QString());
        u_motionVecLineEdit->setText(QApplication::translate("pcsCompressClass", "1", Q_NULLPTR));
        octTreeLeafineEdit->setInputMask(QString());
        octTreeLeafineEdit->setText(QApplication::translate("pcsCompressClass", "0.1", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(parameter), QApplication::translate("pcsCompressClass", "param", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(others), QApplication::translate("pcsCompressClass", "others", Q_NULLPTR));
        menuFile->setTitle(QApplication::translate("pcsCompressClass", "File", Q_NULLPTR));
        menuOperation->setTitle(QApplication::translate("pcsCompressClass", "Operation", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class pcsCompressClass: public Ui_pcsCompressClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PCSCOMPRESS_H
