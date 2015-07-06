#ifndef BATCHPOLYGONIZE_H
#define BATCHPOLYGONIZE_H

#include <QMainWindow>
#include <QString>

namespace Ui {
	class BatchPolygonize;
}

class BatchPolygonize : public QMainWindow
{
	Q_OBJECT

public:
	explicit BatchPolygonize(QWidget *parent = 0);
	~BatchPolygonize();

private slots:
	void on_cancelButton_clicked();

	void on_runButton_clicked();

private:
	Ui::BatchPolygonize *ui;
};

void convertImageToPolygon(QString inputDir, QString outputDir,
						   bool ignoreSmallPolygons, float minimumPolygonSize,
						   float rdpInternalEpsilon, float edpExternalEpsilon,
						   bool displayErrorMessages);

#endif // BATCHPOLYGONIZE_H
