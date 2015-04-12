#include "uimanager.h"
#include <QObject>

namesapce CDI
{
    UIManager::UIManager(QObject *parent)
        : QObject(parent)
    {

    }

    void UIManager::CreateActions()
    {

    }

    void SetUI(QWidget* widget)
    {
        Q_UNUSED(widget);
    }



}
