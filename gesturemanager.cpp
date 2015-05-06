#include "gesturemanager.h"


namespace CDI
{
    GestureManager::GestureManager(SketchScene* scene) : QObject(scene)
    {
        parentScene = scene;
    }

    GestureManager::~GestureManager()
    {

    }

    void GestureManager::OnStrokeEnd()
    {
        // Triggers symbol search based on all strokes
    }
}
