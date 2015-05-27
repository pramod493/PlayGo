#pragma once
#include <QObject>
#include <QString>
#include <QList>
#include <QImage>
#include <QFile>
#include <QTextStream>

// Juxtapoze
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include "wbbice.h"
#include "wbsearchengine.h"

namespace CDI
{
    class SearchManager : public QObject
    {
        Q_OBJECT
    public:
        QString inputFilePath;
        QString resultFilePath;
        QString databaseDir;
        QList<QString> localFileList;

        wbSearchEngine* searchEngine;

    public:
        SearchManager(QObject *parent=0);

        ~SearchManager();

        bool search(QImage &image, int numResults=10);

        void ConvertResultsToLocalPath(int numResults);

    };
}
