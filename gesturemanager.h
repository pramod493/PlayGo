#pragma once
#include <QObject>
#include "pointcloudrecognizer.h"
#include "pdollargesture.h"
#include "gesturestroke.h"
#include "sketchscene.h"

namespace CDI
{
    class GestureManager : public QObject
    {
        Q_OBJECT
    protected:
        SketchScene* parentScene;
    public:
        explicit GestureManager(SketchScene* scene);
        ~GestureManager();

    signals:

    public slots:
        void OnStrokeEnd();
    };

}
