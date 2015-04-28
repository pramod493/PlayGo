#pragma once
#include <QObject>

namespace CDI
{
    /* Page can be initialize from text file but need to be initialized in order to load the data
     *
     * */
    class Page : public QObject
    {
        Q_OBJECT

    protected:
        QObject *_parent;

    public:
        explicit Page(QObject *parent = 0);

        ~Page();

        void Load();

        void Close();

        Page* Clone();

    signals:

    public slots:
    };
}
