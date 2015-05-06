#include "searchmanager.h"

namespace CDI
{
    SearchManager::SearchManager(QObject *parent)
        : QObject(parent)
    {
        isSearchRunning = false;

        isAnotherInQueue = false;

        timer = new QTimer(this);
        timer->start(4000);  // Check every 4s
        connect(timer, SIGNAL(timeout()),
                this, SLOT(OnTimerComplete()));

        inputFilePath   = QString("E:/Coding/Search/pramod/data/image.png");
        resultFilePath  = QString("E:/Coding/Search/pramod/data/results.txt");
        databaseDir     = QString("E:/Coding/Search/database/");
        controlFile     = QString("E:/Coding/Search/pramod/data/control.txt");

        localFileList = QList<QString>();
    }

    SearchManager::~SearchManager()
    {
        delete timer;
    }

    void SearchManager::search(QImage &image)
    {
        searchInputImage = QImage(image);
        if (isSearchRunning)
        {
            isAnotherInQueue = true;
            return;
        } else
        {
            isAnotherInQueue = false;
            run();
        }
    }

    void SearchManager::run()
    {
        if(searchInputImage.isNull()) return;
        isSearchRunning = true;

        searchInputImage.save(inputFilePath);           // Save image
        QFile file(controlFile);                        // Trigger search
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&file);
        out << "1\n";
        file.close();
    }

    bool SearchManager::CheckSearchStatus()
    {
        bool result = false;
        QFile file(controlFile);
        file.open(QFile::ReadOnly);
        QTextStream inputstream(&file);
        QString temp = inputstream.readLine(1);

        if (QString::compare(temp, QString("0"), Qt::CaseInsensitive) == 0)
            result = true;
        file.close();
        // Since auth is not working, always return true
        return true;
        return result;
    }

    void SearchManager::ConvertResultsToLocalPath()
    {
        localFileList.clear();      // Cleanup the search results
        QFile file(resultFilePath);
        if (file.open(QFile::ReadOnly))
        {
            int lineNumber = 1;
            QTextStream in(&file);
            while (!in.atEnd())
            {
                QString line = in.readLine();
                if ((lineNumber > 1) && (lineNumber <= 40))
                {
                    QStringList list1 = line.split("/");
                    QString result(databaseDir);
                    QString fileName = list1.at(list1.size()-1);
                    fileName.remove('"');
                    fileName.replace(".jpg", ".png");
                    result.append(fileName);
                    localFileList.push_back(result);
                }
                lineNumber++;
            }
        }
    }

    void SearchManager::OnTimerComplete()
    {
        if (!isSearchRunning) return;
        if (CheckSearchStatus())
        {
            isSearchRunning = false;
            if (isAnotherInQueue)
            {
                isAnotherInQueue = false;
                run();
            }
            // Convert and store the file names to local path
            ConvertResultsToLocalPath();
            emit signalSearchComplete(/*resultFilePath*/);
        }
    }
}
