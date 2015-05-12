#pragma once

#include <QObject>
#include <QVector>
#include "page.h"

using namespace std;

namespace CDI
{
    class PlayGo : public QObject
    {
        Q_OBJECT
    public:

        QVector<Page*> pageList;
        //map<Page*, string> pages;

    protected:
        Page* currentPage;
    public:
        explicit PlayGo(QObject *parent = 0);

        ~PlayGo();

        void SetCurrent(Page* page);
    protected:
        void Init();

    signals:
        void OnExit();

    private slots:
        void OnPageClose(Page* page);

        void OnPageOpen(Page* page);

    };
}
