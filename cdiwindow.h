#pragma once
#include <QMainWindow>
#include "uimanager.h"

namespace CDI
{
    class CDIWindow : public QMainWindow
    {
        Q_OBJECT

	public:
		QSize windowSize;


    public:
        UIManager *uimanager;

        explicit CDIWindow(QWidget *parent = 0);

		~CDIWindow();

		void InitWidgets();
    };
}
