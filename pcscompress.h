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

#include "ui_pcscompress.h"

#include "util/zjw_obj.h"

class pcsCompress : public QMainWindow
{
	Q_OBJECT

		public slots:
	void clickedOpenFileAction();

public:
	pcsCompress(QWidget *parent = 0);
	~pcsCompress();

private:
	Ui::pcsCompressClass ui;
};

#endif // PCSCOMPRESS_H
