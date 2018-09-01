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
#include <QtWidgets/QCheckBox>
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
    QAction *actionTrainMatP;
    QAction *actionGetSparseMatch;
    QAction *actionGetMotionVector;
    QAction *actionTest;
    QAction *actionRLGR_MV;
    QAction *actionPointsCompress;
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
    QRadioButton *sparseMatchRadioButton;
    QRadioButton *predictTargetRadioButton;
    QLineEdit *renderModeLineEdit;
    QLabel *refFrame_2;
    QLabel *label;
    QLabel *label_2;
    QWidget *algorithmWidget;
    QLabel *u_motionVec;
    QLineEdit *clusterNumLineEdit;
    QLabel *oct;
    QLabel *cluster;
    QLineEdit *u_motionVecLineEdit;
    QLineEdit *octTreeLeafineEdit;
    QLineEdit *m_ChebyshevLineEdit;
    QLabel *m_Chebyshev;
    QLineEdit *num_scalesLineEdit;
    QLabel *num_scales;
    QWidget *others;
    QWidget *algorithmWidget_2;
    QLabel *oct_3;
    QLineEdit *pointResolutionLineEdit;
    QLineEdit *octreeResolutionLineEdit;
    QLabel *oct_4;
    QLabel *oct_5;
    QLineEdit *iFrameRateLineEdit;
    QCheckBox *showInfoCheckBox;
    ZjwOpenGL *openGLWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuOperation;
    QMenu *menuCompress;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *pcsCompressClass)
    {
        if (pcsCompressClass->objectName().isEmpty())
            pcsCompressClass->setObjectName(QStringLiteral("pcsCompressClass"));
        pcsCompressClass->resize(812, 547);
        actionOpen = new QAction(pcsCompressClass);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        actionTrainMatP = new QAction(pcsCompressClass);
        actionTrainMatP->setObjectName(QStringLiteral("actionTrainMatP"));
        actionGetSparseMatch = new QAction(pcsCompressClass);
        actionGetSparseMatch->setObjectName(QStringLiteral("actionGetSparseMatch"));
        actionGetMotionVector = new QAction(pcsCompressClass);
        actionGetMotionVector->setObjectName(QStringLiteral("actionGetMotionVector"));
        actionTest = new QAction(pcsCompressClass);
        actionTest->setObjectName(QStringLiteral("actionTest"));
        actionRLGR_MV = new QAction(pcsCompressClass);
        actionRLGR_MV->setObjectName(QStringLiteral("actionRLGR_MV"));
        actionPointsCompress = new QAction(pcsCompressClass);
        actionPointsCompress->setObjectName(QStringLiteral("actionPointsCompress"));
        centralWidget = new QWidget(pcsCompressClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setGeometry(QRect(0, 0, 131, 481));
        parameter = new QWidget();
        parameter->setObjectName(QStringLiteral("parameter"));
        renderWidget = new QWidget(parameter);
        renderWidget->setObjectName(QStringLiteral("renderWidget"));
        renderWidget->setGeometry(QRect(0, 230, 120, 221));
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
        refFrameLineEdit->setGeometry(QRect(80, 170, 31, 20));
        refFrameLineEdit->setDragEnabled(true);
        refFrame = new QLabel(renderWidget);
        refFrame->setObjectName(QStringLiteral("refFrame"));
        refFrame->setGeometry(QRect(0, 170, 61, 16));
        targerFrame = new QLabel(renderWidget);
        targerFrame->setObjectName(QStringLiteral("targerFrame"));
        targerFrame->setGeometry(QRect(0, 190, 81, 16));
        targetFrameLineEdit = new QLineEdit(renderWidget);
        targetFrameLineEdit->setObjectName(QStringLiteral("targetFrameLineEdit"));
        targetFrameLineEdit->setGeometry(QRect(80, 190, 31, 20));
        targetFrameLineEdit->setDragEnabled(true);
        sparseMatchRadioButton = new QRadioButton(renderWidget);
        sparseMatchRadioButton->setObjectName(QStringLiteral("sparseMatchRadioButton"));
        sparseMatchRadioButton->setGeometry(QRect(10, 90, 101, 16));
        predictTargetRadioButton = new QRadioButton(renderWidget);
        predictTargetRadioButton->setObjectName(QStringLiteral("predictTargetRadioButton"));
        predictTargetRadioButton->setGeometry(QRect(10, 110, 101, 16));
        renderModeLineEdit = new QLineEdit(renderWidget);
        renderModeLineEdit->setObjectName(QStringLiteral("renderModeLineEdit"));
        renderModeLineEdit->setGeometry(QRect(80, 130, 31, 20));
        renderModeLineEdit->setDragEnabled(true);
        refFrame_2 = new QLabel(renderWidget);
        refFrame_2->setObjectName(QStringLiteral("refFrame_2"));
        refFrame_2->setGeometry(QRect(30, 130, 31, 20));
        label = new QLabel(parameter);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(0, 210, 54, 12));
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
        m_ChebyshevLineEdit = new QLineEdit(algorithmWidget);
        m_ChebyshevLineEdit->setObjectName(QStringLiteral("m_ChebyshevLineEdit"));
        m_ChebyshevLineEdit->setGeometry(QRect(80, 70, 31, 20));
        m_ChebyshevLineEdit->setDragEnabled(true);
        m_Chebyshev = new QLabel(algorithmWidget);
        m_Chebyshev->setObjectName(QStringLiteral("m_Chebyshev"));
        m_Chebyshev->setGeometry(QRect(0, 70, 71, 20));
        num_scalesLineEdit = new QLineEdit(algorithmWidget);
        num_scalesLineEdit->setObjectName(QStringLiteral("num_scalesLineEdit"));
        num_scalesLineEdit->setGeometry(QRect(80, 90, 31, 20));
        num_scalesLineEdit->setDragEnabled(true);
        num_scales = new QLabel(algorithmWidget);
        num_scales->setObjectName(QStringLiteral("num_scales"));
        num_scales->setGeometry(QRect(0, 90, 71, 20));
        tabWidget->addTab(parameter, QString());
        others = new QWidget();
        others->setObjectName(QStringLiteral("others"));
        algorithmWidget_2 = new QWidget(others);
        algorithmWidget_2->setObjectName(QStringLiteral("algorithmWidget_2"));
        algorithmWidget_2->setGeometry(QRect(0, 10, 120, 191));
        oct_3 = new QLabel(algorithmWidget_2);
        oct_3->setObjectName(QStringLiteral("oct_3"));
        oct_3->setGeometry(QRect(10, 10, 101, 20));
        pointResolutionLineEdit = new QLineEdit(algorithmWidget_2);
        pointResolutionLineEdit->setObjectName(QStringLiteral("pointResolutionLineEdit"));
        pointResolutionLineEdit->setGeometry(QRect(10, 30, 91, 20));
        pointResolutionLineEdit->setDragEnabled(false);
        octreeResolutionLineEdit = new QLineEdit(algorithmWidget_2);
        octreeResolutionLineEdit->setObjectName(QStringLiteral("octreeResolutionLineEdit"));
        octreeResolutionLineEdit->setGeometry(QRect(10, 70, 91, 20));
        octreeResolutionLineEdit->setDragEnabled(false);
        oct_4 = new QLabel(algorithmWidget_2);
        oct_4->setObjectName(QStringLiteral("oct_4"));
        oct_4->setGeometry(QRect(10, 50, 101, 20));
        oct_5 = new QLabel(algorithmWidget_2);
        oct_5->setObjectName(QStringLiteral("oct_5"));
        oct_5->setGeometry(QRect(10, 90, 101, 20));
        iFrameRateLineEdit = new QLineEdit(algorithmWidget_2);
        iFrameRateLineEdit->setObjectName(QStringLiteral("iFrameRateLineEdit"));
        iFrameRateLineEdit->setGeometry(QRect(10, 110, 91, 20));
        iFrameRateLineEdit->setDragEnabled(false);
        showInfoCheckBox = new QCheckBox(algorithmWidget_2);
        showInfoCheckBox->setObjectName(QStringLiteral("showInfoCheckBox"));
        showInfoCheckBox->setGeometry(QRect(10, 140, 91, 16));
        tabWidget->addTab(others, QString());
        openGLWidget = new ZjwOpenGL(centralWidget);
        openGLWidget->setObjectName(QStringLiteral("openGLWidget"));
        openGLWidget->setGeometry(QRect(130, 0, 681, 491));
        pcsCompressClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(pcsCompressClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 812, 23));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuOperation = new QMenu(menuBar);
        menuOperation->setObjectName(QStringLiteral("menuOperation"));
        menuCompress = new QMenu(menuBar);
        menuCompress->setObjectName(QStringLiteral("menuCompress"));
        pcsCompressClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(pcsCompressClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        pcsCompressClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(pcsCompressClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        pcsCompressClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuOperation->menuAction());
        menuBar->addAction(menuCompress->menuAction());
        menuFile->addAction(actionOpen);
        menuOperation->addAction(actionTrainMatP);
        menuOperation->addAction(actionGetSparseMatch);
        menuOperation->addAction(actionGetMotionVector);
        menuOperation->addAction(actionTest);
        menuCompress->addAction(actionRLGR_MV);
        menuCompress->addAction(actionPointsCompress);

        retranslateUi(pcsCompressClass);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(pcsCompressClass);
    } // setupUi

    void retranslateUi(QMainWindow *pcsCompressClass)
    {
        pcsCompressClass->setWindowTitle(QApplication::translate("pcsCompressClass", "pcsCompress", Q_NULLPTR));
        actionOpen->setText(QApplication::translate("pcsCompressClass", "Open", Q_NULLPTR));
        actionTrainMatP->setText(QApplication::translate("pcsCompressClass", "TrainMatP", Q_NULLPTR));
        actionGetSparseMatch->setText(QApplication::translate("pcsCompressClass", "GetSparseMatch", Q_NULLPTR));
        actionGetMotionVector->setText(QApplication::translate("pcsCompressClass", "GetMotionVector", Q_NULLPTR));
        actionTest->setText(QApplication::translate("pcsCompressClass", "Test", Q_NULLPTR));
        actionRLGR_MV->setText(QApplication::translate("pcsCompressClass", "RLGR_MV", Q_NULLPTR));
        actionPointsCompress->setText(QApplication::translate("pcsCompressClass", "PointsCompress", Q_NULLPTR));
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
        sparseMatchRadioButton->setText(QApplication::translate("pcsCompressClass", "sparse match", Q_NULLPTR));
        predictTargetRadioButton->setText(QApplication::translate("pcsCompressClass", "predict target F", Q_NULLPTR));
        renderModeLineEdit->setInputMask(QString());
        renderModeLineEdit->setText(QApplication::translate("pcsCompressClass", "0", Q_NULLPTR));
        refFrame_2->setText(QApplication::translate("pcsCompressClass", "mode", Q_NULLPTR));
        label->setText(QApplication::translate("pcsCompressClass", "render", Q_NULLPTR));
        label_2->setText(QApplication::translate("pcsCompressClass", "algorithm", Q_NULLPTR));
        u_motionVec->setText(QApplication::translate("pcsCompressClass", " u", Q_NULLPTR));
        clusterNumLineEdit->setInputMask(QString());
        clusterNumLineEdit->setText(QApplication::translate("pcsCompressClass", "10", Q_NULLPTR));
        oct->setText(QApplication::translate("pcsCompressClass", "octTree leaf", Q_NULLPTR));
        cluster->setText(QApplication::translate("pcsCompressClass", "cluster num", Q_NULLPTR));
        u_motionVecLineEdit->setInputMask(QString());
        u_motionVecLineEdit->setText(QApplication::translate("pcsCompressClass", "0.01", Q_NULLPTR));
        octTreeLeafineEdit->setInputMask(QString());
        octTreeLeafineEdit->setText(QApplication::translate("pcsCompressClass", "0.05", Q_NULLPTR));
        m_ChebyshevLineEdit->setInputMask(QString());
        m_ChebyshevLineEdit->setText(QApplication::translate("pcsCompressClass", "50", Q_NULLPTR));
        m_Chebyshev->setText(QApplication::translate("pcsCompressClass", "Chebyshev M", Q_NULLPTR));
        num_scalesLineEdit->setInputMask(QString());
        num_scalesLineEdit->setText(QApplication::translate("pcsCompressClass", "4", Q_NULLPTR));
        num_scales->setText(QApplication::translate("pcsCompressClass", "num scales", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(parameter), QApplication::translate("pcsCompressClass", "param", Q_NULLPTR));
        oct_3->setText(QApplication::translate("pcsCompressClass", "point resulation", Q_NULLPTR));
        pointResolutionLineEdit->setInputMask(QString());
        pointResolutionLineEdit->setText(QApplication::translate("pcsCompressClass", "0.001", Q_NULLPTR));
        octreeResolutionLineEdit->setInputMask(QString());
        octreeResolutionLineEdit->setText(QApplication::translate("pcsCompressClass", "0.01", Q_NULLPTR));
        oct_4->setText(QApplication::translate("pcsCompressClass", "octree resulation", Q_NULLPTR));
        oct_5->setText(QApplication::translate("pcsCompressClass", "iframe rate", Q_NULLPTR));
        iFrameRateLineEdit->setInputMask(QString());
        iFrameRateLineEdit->setText(QApplication::translate("pcsCompressClass", "30", Q_NULLPTR));
        showInfoCheckBox->setText(QApplication::translate("pcsCompressClass", "show info", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(others), QApplication::translate("pcsCompressClass", "others", Q_NULLPTR));
        menuFile->setTitle(QApplication::translate("pcsCompressClass", "File", Q_NULLPTR));
        menuOperation->setTitle(QApplication::translate("pcsCompressClass", "Operation", Q_NULLPTR));
        menuCompress->setTitle(QApplication::translate("pcsCompressClass", "Compress", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class pcsCompressClass: public Ui_pcsCompressClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PCSCOMPRESS_H
