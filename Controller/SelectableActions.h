#ifndef __SELECTABLEACTIONS_H__
#define __SELECTABLEACTIONS_H__

#include <QGraphicsEllipseItem>
#include <QAction>
#include <QObject>
#include "cdi2qtwrapper.h"
#include "commonfunctions.h"

namespace CDI
 {
	/**
	* @brief The SelectableActions class defines a set of interactive
	* QGraphicsItem. It typically comprises of an icon on top of ellipse
	* which can be tapped with stylus as well as touch.
	* Override sceneEvent(QEvent*) in derived classes to change the behavior
	*/
   class SelectableActions : public QGraphicsEllipseItem
   {
   protected:
	 QAction *_action;	// Do not destroy on component delete
	 QGraphicsSimpleTextItem *textItem;

  public:
	 // Custom Type value of the item (for downcasting)
	enum {Type = UserType + UI_SELECTABLE_ACTIONS };

	static int dim;	/**< Defines global dimension of all the Selectable items */

	/**
	 * @brief Create a selectable item in scene. This will respond to mouse and touch
	 * @param action QAction which be triggered on selection of item
	 * @param parent Parent QGraphicsItem
	 */
	explicit SelectableActions(QAction *action, QGraphicsItem* parent);

	/**
	 * @brief ~SelectableActions
	 */
	virtual ~SelectableActions() {}

	/**
	 * @brief Returns the SelectableActions type. Use for  downcasting
	 * @return QGraphicsItem::UserType + UI_SELECTABLE_ACTIONS
	 */
	int type() const;

	// triggers the associated action. This need not be associated with any action
	virtual void trigger();

protected:
	/**
	 * @brief Capture the mouse(stylus) and touch input received by the object.
	 * Note that events of type mouse for items is different from widget
	 * @param event Input event to item
	 * @return true when accepted (AFAIK this value is not that useful)
	 */
	bool sceneEvent(QEvent *event);
  };

   class LayerSelectorActions : public QObject,  public SelectableActions
   {
	   Q_OBJECT
   protected:
	   cdLayerIndex _index;
   public :
	   LayerSelectorActions (QString text, cdLayerIndex index,
									  QAction *action, QGraphicsItem* parent);

	   ~LayerSelectorActions();

		void trigger();
   signals:
		void onTrigger(cdLayerIndex newIndex);
   };
 }

#endif //__SELECTABLEACTIONS_H__
