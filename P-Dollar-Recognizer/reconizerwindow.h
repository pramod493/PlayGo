#ifndef RECONIZERWINDOW_H
#define RECONIZERWINDOW_H

#include <QMainWindow>

namespace Ui {
	class ReconizerWindow;
}

class ReconizerWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit ReconizerWindow(QWidget *parent = 0);
	~ReconizerWindow();


private slots:
	void on_pushButton_clicked();

	void on_detectGestureSelection_clicked();


    void on_addgestureSelection_clicked();

    void on_addButon_clicked();

    void on_detectGestureSelection_toggled(bool checked);

    void on_DetectButton_clicked();

private:
	Ui::ReconizerWindow *ui;
};

#endif // RECONIZERWINDOW_H
