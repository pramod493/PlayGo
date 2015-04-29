#include "searchmanager.h"

namespace CDI
{
    SearchManager::SearchManager(QObject *parent)
        : QThread(parent)
    {
        quit = false;

        inputFilePath   = QString("E:/Coding/Search/pramod/data/image.png");
        resultFilePath  = QString("E:/Coding/Search/pramod/data/results.txt");
        databaseDir     = QString("E:/Coding/Search/database/");
        controlFile     = QString("E:/Coding/Search/pramod/data/control.txt");
    }

    SearchManager::~SearchManager()
    {
        mutex.lock();
        quit = true;
        condition.wakeOne();
        mutex.unlock();
        wait();
    }

    void SearchManager::search(QPixmap &pixmap)
    {
        QMutexLocker locker(&mutex);

        imageMap = new QPixmap(pixmap);

        if (!isRunning())
        {
            start(LowPriority);
        } else {
            condition.wakeOne();
        }
    }

    void SearchManager::run()
    {
        forever {
            mutex.lock();
            imageMap->save(inputFilePath);
            QFile file(controlFile);
            file.open(QIODevice::WriteOnly | QIODevice::Text);
            QTextStream out(&file);
            out << "1\n";
            file.close();
             bool searchComplete = false;
            mutex.unlock();

            while (!quit)
            {
                QFile file(controlFile);
                file.open(QFile::ReadOnly);
                QTextStream inputstream(&file);
                QString temp = inputstream.readLine(1);
                if (QString::compare(temp, QString("0"), Qt::CaseInsensitive) == 0)
                {
                    emit SearchComplete(this);
                    file.close();
                    return;
                }
                file.close();
            }
        }
    }

}
