#pragma once

#include <QWidget>
#include <QAction>
#include <QIcon>
#include <QString>
#include <QToolBar>
#include <QColor>
#include <QList>
#include <QActionGroup>

namespace CDI
{
    class ColorSelectorToolbar : public QToolBar
    {
        Q_OBJECT
    public:
       QList<QColor> toolbarColors;

    protected:
       QColor current_color;

    public:
        explicit ColorSelectorToolbar(QWidget *parent = 0);

       void InitToolbarItems();

    signals:
        void signalColorChange(QString name, QColor color);
    public slots:
       void slotColorChange(QString name, QColor color);
    };


    ///// Custom QColorAction
    class ColorAction : public QAction
    {
        Q_OBJECT
    protected:
        QColor actionColor;
    public:
        ColorAction(const QIcon& icon, const QString& name,QColor color, QObject* parent);
    private slots:
        void OnColorSelect();
    signals:
        void ColorSelected(QString name,QColor color);
    };

}
