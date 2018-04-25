#include "pcscompress.h"

#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	QApplication a(argc, argv);
	pcsCompress w;
	w.show();
	return a.exec();
}