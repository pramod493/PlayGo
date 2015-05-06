#pragma once
#include <QObject>
#include <QString>
#include <QList>
#include <QImage>
#include <QFile>
#include <QTextStream>
#include <QTimer>

namespace CDI
{
    class SearchManager : public QObject
    {
        Q_OBJECT
    public:
        QString inputFilePath;
        QString resultFilePath;
        QString databaseDir;
        QString controlFile;

        QList<QString> localFileList;

    protected:
        QTimer* timer;

        QImage searchInputImage;

        bool isSearchRunning;

        bool isAnotherInQueue;

    public:
        SearchManager(QObject *parent=0);

        ~SearchManager();

        void search(QImage &image);

    protected:
        void run();

        bool CheckSearchStatus();

        void ConvertResultsToLocalPath();

    private slots:
        void OnTimerComplete();


    signals:
        void signalSearchComplete(/*QString inputFilePath*/);
    };
}
