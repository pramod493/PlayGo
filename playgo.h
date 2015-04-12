#pragma once

#include <QObject>
#include <vector>
#include <map>
#include <string>
#include "page.h"

using namespace std;

namespace CDI
{
    class PlayGo : public QObject
    {
        Q_OBJECT
    public:

        map<Page*, string> pages;

    protected:
        Page* currentPage;
    public:
        explicit PlayGo(QObject *parent = 0);

        ~PlayGo();
    protected:
        void Init();

    signals:
        void OnExit();

    private slots:
        void OnPageClose(Page* page);

        void OnPageOpen(Page* page);

    };
}
