#ifndef MAINSETTINGS_H
#define MAINSETTINGS_H

#include <QWidget>

namespace Ui {
	class MainSettings;
}

class MainSettings : public QWidget
{
	Q_OBJECT

public:
	explicit MainSettings(QWidget *parent = 0);
	~MainSettings();

private:
	Ui::MainSettings *ui;
};

#endif // MAINSETTINGS_H
