#ifndef __CDSTATE_H__
#define __CDSTATE_H__

#include <QState>

class QGraphicsScene;
namespace CDI
{
    class PlayGoController;
    class Page;
    class SketchView;
    /**
     * @brief The cdState class is the base class for all the state to be used in here.
     * It sets up the necessary parameters required by all the states which we are going
     * to use
     */
    class cdState : public QState
    {
    protected:
        Page            *_page;
        SketchView      *_view;
        QGraphicsScene  *_scene;
        PlayGoController *_controller;
    public:
        cdState(PlayGoController* parentController, QState *parentState = 0);

        cdState(PlayGoController *parentController, ChildMode childMode, QState *parentState);

        virtual void onViewEvent(QEvent *event) {Q_UNUSED(event)}

        virtual void onSceneEvent(QEvent *event) {Q_UNUSED(event)}

    private:
        void init() ;
    };
}
#endif //__CDSTATE_H__
