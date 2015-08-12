#ifndef __SELECTABLEACTIONS_H__
#define __SELECTABLEACTIONS_H__

#include <QGraphicsEllipseItem>
#include  <QAction>
#include "cdi2qtwrapper.h"

namespace CDI
 {
   class SelectableActions : public QGraphicsEllipseItem
   {
   protected:
     QAction *_action;
     QString _text;

  public:
    enum {Type = UserType + UI_SELECTABLE_ACTIONS };
    static int dim;

    SelectableActions(QAction *action, QGraphicsItem* parent);
    int type() const;
    virtual void trigger();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

    bool sceneEvent(QEvent *event);
  };
 }

#endif //__SELECTABLEACTIONS_H__
