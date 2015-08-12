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
        // Check out http://www.w3schools.com/cssref/css_colors.asp for some of
        // the color codes
        toolbarColors.append(QColor(064,064,064));

        toolbarColors.append(QColor(128,000,000));
        toolbarColors.append(QColor(153,000,051));
        toolbarColors.append(QColor(102,000,102));
        toolbarColors.append(QColor(153,051,102));

        toolbarColors.append(QColor(000,000,153));
        toolbarColors.append(QColor(051,051,255));
        toolbarColors.append(QColor(000,102,255));
        toolbarColors.append(QColor(000,153,204));

        toolbarColors.append(QColor(051,051,000));
        toolbarColors.append(QColor(000,102,000));
        toolbarColors.append(QColor(000,153,051));

        toolbarColors.append(QColor(204,102,000));
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
