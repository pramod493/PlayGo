#include "drawplotui.h"
#include "ui_drawplotui.h"

DrawPlotUI::DrawPlotUI(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DrawPlotUI)
{
	ui->setupUi(this);
	component = nullptr;
	physicsmanager = nullptr;
}

DrawPlotUI::~DrawPlotUI()
{
	delete ui;
}

void DrawPlotUI::on_buttonBox_accepted()
{
	if (component == nullptr || physicsmanager == nullptr) return;
	auto plot = new cdPlotWidget(component,
								 ui->x_position_checkbox->isChecked(),
								 ui->y_position_checkbox->isChecked(),
								 ui->velocity_x_checkbox->isChecked(),
								 ui->velocity_y_checkbox->isChecked(),
								 ui->ang_mom_checkbox->isChecked(),
								 ui->linear_mom_x_checkbox->isChecked(),
								 ui->linear_mom_y_checkbox->isChecked());

	connect(physicsmanager, SIGNAL(physicsStepComplete()),
			plot, SLOT(updatecdPlot()));
}

void DrawPlotUI::on_buttonBox_rejected()
{

}
