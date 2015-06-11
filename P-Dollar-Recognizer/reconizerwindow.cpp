#include "reconizerwindow.h"
#include "ui_reconizerwindow.h"
#include <QtAlgorithms>
#include "gestureScene.h"
#include <QDebug>


ReconizerWindow::ReconizerWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::ReconizerWindow)
{
	ui->setupUi(this);
	GestureScene* scene = new GestureScene();
	ui->graphicsView->setScene(scene);
	ui->graphicsView->setSceneRect(0,0,1000,1000);
	qDebug() << "Scene created";
}

ReconizerWindow::~ReconizerWindow()
{
	delete ui;
}

void ReconizerWindow::on_pushButton_clicked()
{

}

void ReconizerWindow::on_detectGestureSelection_clicked()
{
	qDebug() << "Deleting all items";
	if (ui->graphicsView->scene()!= NULL)
	{
		GestureScene* myScene = static_cast<GestureScene*>(ui->graphicsView->scene());
		qDebug() << "Deleted";
		qDeleteAll(myScene->strokes);
	}

}

void ReconizerWindow::on_addgestureSelection_clicked()
{

}
