#pragma once
#include <QObject>
#include "cdiwindow.h"
#include "playgocore.h"

using namespace CDI;
class Polygonize : public QObject
{
	Q_OBJECT
	QGraphicsScene* scene;

	CDIWindow* _window;
public:
	Polygonize(CDIWindow* window) : QObject(window)
	{
		_window = window;
		scene = window->sketchScene;

		connect(window->openPageAction, SIGNAL(triggered()),
				this, SLOT(openFile()));
	}
public slots:
	void openFile();

};
