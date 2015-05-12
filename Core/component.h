#pragma once

#include <QObject>
#include <Vector>
#include "cdicommon.h"

using namespace std;

namespace CDI
{
    class Component : public QObject
    {
        Q_OBJECT

    public:
        vector<Shape*> shapes;
        vector<Item*> items;
    public:
        explicit Component(QObject *parent = 0);
        ~Component();

    signals:

    public slots:
    };
}
