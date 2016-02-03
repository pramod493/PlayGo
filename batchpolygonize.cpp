#include <QApplication>
#include <QMessageBox>
#include <vector>
#include <QStringList>
#include <QList>
#include <QPixmap>
#include "poly2tri.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QsLog.h>
#include "batchpolygonize.h"
#include "converttopolygons.h"
#include "ui_batchpolygonize.h"
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsPolygonItem>
#include <QPixmap>

using namespace std;
using namespace CDI;

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

BatchPolygonize::BatchPolygonize(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::BatchPolygonize)
{
	ui->setupUi(this);
}

BatchPolygonize::~BatchPolygonize()
{
	delete ui;
}

void BatchPolygonize::on_cancelButton_clicked()
{
	QApplication::quit();
}

void BatchPolygonize::on_runButton_clicked()
{
	// Run polygonization here
	QString inputDir = ui->DirectoryPathEntry->text();
	QString outputDir = ui->resultFilePathEntry->text();
	bool ignoreSmallPolygons = ui->checkBoxIgnoreSmall->isChecked();
	float minimumPolygonSize = ui->minimumPolygonSize->value();
	float rdpInternalEpsilon = ui->rdpInternal->value();
	float rdpExternalEpsilon = ui->rdpExternal->value();

	convertImageToPolygon(inputDir, outputDir, ignoreSmallPolygons, minimumPolygonSize,
						  rdpInternalEpsilon, rdpExternalEpsilon, false);

	QMessageBox::about(this, "Polygonization complete", "All files saved. Please check output folder for details");

}
// Default settings
// Ignore small = true
// Min polygons size = 5	// Keep it larger to ignore smaller holes
// rdp (holes) = 3
// rdp (boundary) = 3
//

void convertImageToPolygon(QString inputDir, QString outputDir,
						   bool ignoreSmallPolygons, float minimumPolygonSize,
						   float rdpInternalEpsilon, float edpExternalEpsilon,
						   bool displayErrorMessages)
{
	Q_UNUSED(displayErrorMessages)

	QLOG_INFO() << "Batch convert start with following arguments";
	QLOG_INFO() << "Input Directory" << inputDir;
	QLOG_INFO() << "Output Directory" << outputDir;
	QLOG_INFO() << "Ignore smalls" << ignoreSmallPolygons;
	QLOG_INFO() << "Minimum polygon size" << minimumPolygonSize;
	QLOG_INFO() << "RDP Epsilon(holes)" << rdpInternalEpsilon;
	QLOG_INFO() << "RDP Epsilon(exterior)" << edpExternalEpsilon;

	QGraphicsView* view = 0;
	QGraphicsScene* scene = 0;
	if (displayErrorMessages)
	{
		view = new QGraphicsView;
		scene = new QGraphicsScene(view);
		view->setScene(scene);
		view->showMaximized();
//		view->show();
	}

	// Check directory existence
	QDir inDir(inputDir); if (inDir.exists() == false) return;
	QDir outDir(outputDir); if (outDir.exists() == false) return;
	QLOG_INFO() << "Directory existence verified. Starting batch conversion";
	// Assuming no nested hierarchy
	QStringList inputFilenameList = inDir.entryList(QDir::NoDotAndDotDot | QDir::System
													| QDir::Hidden  | QDir::AllDirs
													| QDir::Files, QDir::DirsFirst);
	for(int i=0; i < inputFilenameList.size(); i++)
	{
		QString absoluteInputPath = inDir.absoluteFilePath(inputFilenameList[i]);
		QFile f(absoluteInputPath);
		if(f.exists() == false)
		{
			QLOG_ERROR() << "File" << absoluteInputPath << "not found";
			continue;
		}
		if (displayErrorMessages && scene)
		{
			qDeleteAll(scene->items());
			QPixmap pixmap = QPixmap();
			pixmap.load(absoluteInputPath);
			QGraphicsPixmapItem* item = scene->addPixmap(pixmap);
			item->setOpacity(0.50);
		}
		QLOG_INFO() << "Start triangulation of "<< absoluteInputPath;
		vector<p2t::Triangle*> trias = generatePolygonFromImage(absoluteInputPath,
																edpExternalEpsilon, rdpInternalEpsilon,
																minimumPolygonSize, ignoreSmallPolygons);

		if (displayErrorMessages && scene)
		{
			for (int m=0; m< trias.size(); m++)
			{
				QPolygonF polygon = QPolygonF();
				p2t::Point *pt = 0;
				p2t::Triangle* tria = trias[m];

				pt = tria->GetPoint(0);	polygon.push_back(Point2D(pt->x,pt->y));
				pt = tria->GetPoint(1);	polygon.push_back(Point2D(pt->x,pt->y));
				pt = tria->GetPoint(2);	polygon.push_back(Point2D(pt->x,pt->y));
				//		pt = tria->GetPoint(0);	polygon.push_back(Point2D(pt->x,pt->y));

				QGraphicsPolygonItem* item = new QGraphicsPolygonItem(polygon);
				scene->addItem(item);
			}
			scene->update();
		}

		{
			QLOG_INFO() << "Saving data";
			PhysicsShape *physicsShape = new PhysicsShape(trias);
			QString absoluteOutpath = outDir.absoluteFilePath(inputFilenameList[i] + ".meta");
			QFile file(absoluteOutpath);
			if (file.open(QIODevice::WriteOnly))
			{
				QDataStream stream(&file);
				QString smallname = inputFilenameList[i];	// Ignore the size info since we can get that during run time
				stream << smallname;
				physicsShape->serialize(stream);
				file.close();
				QLOG_INFO() << "Writing complete";
			}
			delete physicsShape;
		}

		QLOG_INFO() << "Cleaning up memory";
		for (int j=0; j< trias.size(); j++)
		{
			p2t::Triangle* tria = trias[j];
			if (tria) delete tria;
			trias[j] = 0;
		}
		trias.clear();

		if (view) view->deleteLater();
	}
}
