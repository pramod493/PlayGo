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

using namespace std;
using namespace CDI;

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
						  rdpInternalEpsilon, rdpExternalEpsilon, true);
}

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
		QLOG_INFO() << "Start triangulation of "<< absoluteInputPath;
		vector<p2t::Triangle*> trias = generatePolygonFromImage(absoluteInputPath, edpExternalEpsilon, rdpInternalEpsilon);

		for (int j=0; j< trias.size(); j++)
		{
			p2t::Triangle* tria = trias[j];
			if (tria) delete tria;
			trias[j] = 0;
		}
		trias.clear();
	}
}
