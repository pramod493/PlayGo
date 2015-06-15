#include "reconizerwindow.h"
#include "ui_reconizerwindow.h"
#include <QtAlgorithms>
#include "gestureScene.h"
#include <QDebug>

#include <QDataStream>
#include <QIODevice>
#include <QFile>
#include <QString>
#include <QUuid>


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
    // Actually this is the clear all button
    GestureScene* myScene = static_cast<GestureScene*>(ui->graphicsView->scene());
    myScene->clearStrokes();
    qDebug() << "Strokes Cleared";
}

void ReconizerWindow::on_detectGestureSelection_clicked()
{
}


void ReconizerWindow::on_addgestureSelection_clicked()
{
}

void ReconizerWindow::on_addButon_clicked()
{
    // It should response only if add gesture mode selected


    // Save the current stroke as template
    // it's just a file containing 32 normalized points
    QString gestureClass =  ui->lineEdit->text();
	QString tmpDir = "C:/Database/gestures/" + QUuid::createUuid().toString()+ ".dat";

    if( gestureClass != "" )
    {
        GestureScene* myScene = static_cast<GestureScene*>(ui->graphicsView->scene());
        myScene->pdRecognizer->savePDRTemplate(tmpDir,gestureClass);
        myScene->clearStrokes();
    }
    else
    {
        qDebug() << "Please Input Gesture Name";
    }

    // After that, we should clear the window?
}

void ReconizerWindow::on_detectGestureSelection_toggled(bool checked)
{
    // Recognize the gesture while finishing each strokes
    // Update search after each new stroke given?
    // Or maybe we should add a recognize button


    // The way we could do this is set the flag
    // and the search could be done in mousereleaseevent

}

void ReconizerWindow::on_DetectButton_clicked()
{
    GestureScene* myScene = static_cast<GestureScene*>(ui->graphicsView->scene());
    myScene->pdRecognizer->gbRecognize();
    myScene->clearStrokes();
}
