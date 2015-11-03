#include <QApplication>
#include "batchpolygonize.h"

int main(int argc, char* argv[])
{
	/****************************************************************
	 * Launch batch polygonizer window
	 * Note that we do not use this info right now
	 * **************************************************************/
	QApplication app (argc, argv);
	BatchPolygonize *batchpolygonize = new BatchPolygonize;
	batchpolygonize->show();
	app.exec();
}
