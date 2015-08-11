#include "colorselectortoolbar.h"
#include <QDebug>
#include "QsLog.h"
#include <QPixmap>
#include "commonfunctions.h"
#include <stdlib.h>

namespace CDI
{
    ColorSelectorToolbar::ColorSelectorToolbar(QWidget *parent)
        : QToolBar(parent)
    {
        toolbarColors = QList<QColor>();
        toolbarColors.append(Qt::black);
        toolbarColors.append(Qt::red);
        toolbarColors.append(Qt::darkRed);
        toolbarColors.append(Qt::green);
        toolbarColors.append(Qt::darkGreen);
        toolbarColors.append(Qt::blue);
        toolbarColors.append(Qt::darkBlue);
        toolbarColors.append(Qt::magenta);
		toolbarColors.append(Qt::darkYellow);

		srand(111111);	// initialize randomizer
    }

    void ColorSelectorToolbar::InitToolbarItems()
    {
        int iconSize = 32;
		colorGroup = new QActionGroup(this);
        for (int i=0; i< toolbarColors.size(); i++)
        {
            QColor color = toolbarColors[i];
            QPixmap image(iconSize,iconSize);
            image.fill(color);
            ColorAction* action = new ColorAction(QIcon(image),
                                                  QString::number(color.red())+QString::number(color.green())+QString::number(color.blue()),
                                                  color, colorGroup);
            action->setCheckable(true);
            action->setChecked(i==0);
            addAction(action);

            connect(action, SIGNAL(ColorSelected(QString,QColor)),
                    this, SLOT(slotColorChange(QString,QColor)));
        }
		actions = colorGroup->actions();
        setIconSize(QSize(iconSize,iconSize));
        current_color = toolbarColors[0];
    }

    void ColorSelectorToolbar::slotColorChange(QString name, QColor color)
    {
       if (!colorCompare(color,current_color))
       {
           current_color = color;
           emit signalColorChange(name, color);
       }
    }

	void ColorSelectorToolbar::slotRandomizeColor()
	{
		int size = actions.size();
		int index = rand() % size;
		QAction* action = actions[index];
		action->trigger();
		action->setChecked(true);
	}

    /*------------------------------------------------------
     * ------------------------------------------------------*/
    ColorAction::ColorAction(const QIcon& icon, const QString& name,QColor color, QObject* parent)
        : QAction(icon,name,parent)
    {
        actionColor = color;
        QObject::connect(this, SIGNAL(triggered()),
                         this, SLOT(OnColorSelect()));
    }

    void ColorAction::OnColorSelect()
    {
        emit ColorSelected(text(), actionColor);
    }
}
