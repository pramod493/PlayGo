#include "drawplotui.h"
#include "ui_drawplotui.h"

DrawPlotUI::DrawPlotUI(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DrawPlotUI)
{
	ui->setupUi(this);
}

DrawPlotUI::~DrawPlotUI()
{
	delete ui;
}

void DrawPlotUI::on_buttonBox_accepted()
{

}

void DrawPlotUI::on_buttonBox_rejected()
{

}
