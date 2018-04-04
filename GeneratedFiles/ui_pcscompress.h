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
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
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
    QWidget *centralWidget;
    QTabWidget *tabWidget;
    QWidget *model;
    QWidget *others;
    ZjwOpenGL *openGLWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *pcsCompressClass)
    {
        if (pcsCompressClass->objectName().isEmpty())
            pcsCompressClass->setObjectName(QStringLiteral("pcsCompressClass"));
        pcsCompressClass->resize(811, 554);
        actionOpen = new QAction(pcsCompressClass);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        centralWidget = new QWidget(pcsCompressClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setGeometry(QRect(0, 0, 131, 481));
        model = new QWidget();
        model->setObjectName(QStringLiteral("model"));
        tabWidget->addTab(model, QString());
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
        pcsCompressClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(pcsCompressClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        pcsCompressClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(pcsCompressClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        pcsCompressClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionOpen);

        retranslateUi(pcsCompressClass);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(pcsCompressClass);
    } // setupUi

    void retranslateUi(QMainWindow *pcsCompressClass)
    {
        pcsCompressClass->setWindowTitle(QApplication::translate("pcsCompressClass", "pcsCompress", Q_NULLPTR));
        actionOpen->setText(QApplication::translate("pcsCompressClass", "Open", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(model), QApplication::translate("pcsCompressClass", "model", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(others), QApplication::translate("pcsCompressClass", "others", Q_NULLPTR));
        menuFile->setTitle(QApplication::translate("pcsCompressClass", "File", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class pcsCompressClass: public Ui_pcsCompressClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PCSCOMPRESS_H
