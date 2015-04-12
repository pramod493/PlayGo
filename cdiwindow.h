#pragma once
#include <QMainWindow>

namespace CDI
{
    class CDIWindow : public QMainWindow
    {
        Q_OBJECT
    public:
        explicit CDIWindow(QWidget *parent = 0);
        ~CDIWindow();

    signals:

    public slots:
    };
}
