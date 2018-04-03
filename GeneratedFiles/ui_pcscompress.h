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
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include "util/zjw_opengl.h"

QT_BEGIN_NAMESPACE

class Ui_pcsCompressClass
{
public:
    QAction *actionOpen;
    QWidget *centralWidget;
    QTabWidget *tabWidget;
    QWidget *model;
    QRadioButton *model2;
    QLabel *modelList;
    QRadioButton *model1;
    QWidget *others;
    ZjwOpenGL *openGLWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *pathTracingClass)
    {
        if (pathTracingClass->objectName().isEmpty())
            pathTracingClass->setObjectName(QStringLiteral("pathTracingClass"));
        pathTracingClass->resize(811, 554);
        actionOpen = new QAction(pathTracingClass);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        centralWidget = new QWidget(pathTracingClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setGeometry(QRect(0, 0, 131, 481));
        model = new QWidget();
        model->setObjectName(QStringLiteral("model"));
        model2 = new QRadioButton(model);
        model2->setObjectName(QStringLiteral("model2"));
        model2->setEnabled(true);
        model2->setGeometry(QRect(20, 60, 71, 16));
        model2->setMouseTracking(true);
        model2->setChecked(false);
        modelList = new QLabel(model);
        modelList->setObjectName(QStringLiteral("modelList"));
        modelList->setGeometry(QRect(20, 20, 71, 16));
        model1 = new QRadioButton(model);
        model1->setObjectName(QStringLiteral("model1"));
        model1->setEnabled(true);
        model1->setGeometry(QRect(20, 40, 71, 16));
        model1->setChecked(true);
        tabWidget->addTab(model, QString());
        others = new QWidget();
        others->setObjectName(QStringLiteral("others"));
        tabWidget->addTab(others, QString());
        openGLWidget = new ZjwOpenGL(centralWidget);
        openGLWidget->setObjectName(QStringLiteral("openGLWidget"));
        openGLWidget->setGeometry(QRect(130, 0, 681, 491));
        pathTracingClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(pathTracingClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 811, 23));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        pathTracingClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(pathTracingClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        pathTracingClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(pathTracingClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        pathTracingClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionOpen);

        retranslateUi(pathTracingClass);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(pathTracingClass);
    } // setupUi

    void retranslateUi(QMainWindow *pathTracingClass)
    {
        pathTracingClass->setWindowTitle(QApplication::translate("pathTracingClass", "pathTracing", Q_NULLPTR));
        actionOpen->setText(QApplication::translate("pathTracingClass", "Open", Q_NULLPTR));
        model2->setText(QApplication::translate("pathTracingClass", "model 2", Q_NULLPTR));
        modelList->setText(QApplication::translate("pathTracingClass", "Model list", Q_NULLPTR));
        model1->setText(QApplication::translate("pathTracingClass", "model 1", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(model), QApplication::translate("pathTracingClass", "model", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(others), QApplication::translate("pathTracingClass", "others", Q_NULLPTR));
        menuFile->setTitle(QApplication::translate("pathTracingClass", "File", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class pcsCompressClass : public Ui_pcsCompressClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PCSCOMPRESS_H
