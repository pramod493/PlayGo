#ifndef __LOADMODEL_H__
#define __LOADMODEL_H__
#include "QObject"
#include "PlayGoController.h"
#include "polygon2d.h"
#include "qCustomPlot/qcustomplot.h"

namespace CDI
{
	class LoadModel : public QObject
	{
		Q_OBJECT
	private:
		PlayGoController* 	controller;
		Page* 				page;
		QVector<double> vecXaxis;
		QVector<double> vecYaxis;
		QCustomPlot *customPlot;

	public:
		LoadModel(PlayGoController* playgocontroller);

		virtual void createComponents();

		virtual Polygon2D* createPolygon(Component *parent, QPointF pos, QVector<QPointF> points);

		virtual Pixmap* loadImage(QPointF pos, QString imagePath);

		virtual Pixmap* loadImage(QPointF pos, QPixmap image);

	public slots:
		void runTest();
	};
}
#endif //__LOADMODEL_H__
