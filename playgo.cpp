#include "playgo.h"

namespace CDI
{
    PlayGo::PlayGo(QObject *parent) : QObject(parent)
    {
        currentPage = NULL;
        pageList = QVector<Page*>();
    }

    PlayGo::~PlayGo()
    {
        emit OnExit();
    }

    void PlayGo::SetCurrent(Page *page)
    {
        currentPage = page;
    }

    void PlayGo::OnPageClose(Page* page)
    {
        if (page==currentPage) currentPage = NULL;
    }

    void PlayGo::OnPageOpen(Page* page)
    {
        if (currentPage != page) currentPage = page;
    }

}
