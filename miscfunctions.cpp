#include <QFile>
#include <QDir>
#include <QPixmap>
#include <QBitmap>
#include <QDebug>
#include <QString>
#include <QStringList>
#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

/**
 * @brief GenerateMaskFromFile generates mask from given file
 * based on alpha channel values
 * @param sourceFile
 * @param destFile
 * @remark No sanity check are done
 */
void GenerateMaskFromFile(QString sourceFile, QString destFile)
{
	QPixmap pixmap = QPixmap();
	pixmap.load(sourceFile);
	QBitmap mask = pixmap.mask();
	mask.save(destFile);
}

/**
 * @brief GenerateMask Generate mask based on alpha channel for all files in directory
 * @param sourceDir: Source file directory
 * @param destDir: Output file directory (NOTE: Will overwrite
 *					existing file in case same as sourceDir)
 * @return false when either of the directories do not exist and output
 * directory cannot be created
 *
 * Does not perform any checks on file type. Also that a black image will be written
 * when alpha channel is missing
 */
bool GenerateMask(QString sourceDir, QString destDir)
{
	QDir sdir(sourceDir);
	if (!sdir.exists()) return false;
	QDir outdir(destDir);
	if (!outdir.exists())
		if (!outdir.mkpath(destDir))
			return false;

	QStringList fileList = sdir.entryList(QDir::Files);

	QStringList::const_iterator iter;
	for (iter = fileList.constBegin(); iter != fileList.constEnd();
		 ++iter)
	{
		QString name = (*iter);
		QString fullpath = sdir.filePath(name);
		// If both sourceDir and outDir are same, then it will overwrite
		QString fullOutPath = outdir.filePath(name);
		GenerateMaskFromFile(fullpath, fullOutPath);
	}
	return true;
}
