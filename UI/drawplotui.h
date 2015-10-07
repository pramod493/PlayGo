#ifndef DRAWPLOTUI_H
#define DRAWPLOTUI_H

#include <QDialog>
#include <component.h>
#include "qCustomPlot/qcustomplot.h"
#include "component.h"
#include "ui_drawplotui.h"

namespace Ui {
	class DrawPlotUI;
}

class DrawPlotUI : public QDialog
{
	Q_OBJECT

public:
	explicit DrawPlotUI(QWidget *parent = 0);
	~DrawPlotUI();
	CDI::Component* component;
private slots:
	void on_buttonBox_accepted();

	void on_buttonBox_rejected();

private:
	Ui::DrawPlotUI *ui;
};

class cdPlotWidget : public QCustomPlot
{
	Q_OBJECT
	CDI::Component* c;
	QVector<double> steps;
	QVector<double> px, py, vx, vy, ang_mom, lin_mom_x, lin_mom_y;
	QCPGraph *px_graph, *py_graph, *vx_graph, *vy_graph, *ang_mom_graph, *lin_mom_x_graph, *lin_mom_y_graph;
	bool b_px, b_py, b_vx, b_vy, b_ang_mom, b_lin_mom_x, b_lin_mom_y;

public:
	explicit cdPlotWidget(CDI::Component* component, bool enable_px, bool enable_py, bool enable_vx, bool enable_vy,
				 bool enable_ang_mom, bool enable_lin_mom_x, bool enable_lin_mom_y, QWidget *parent = nullptr)
		: QCustomPlot(parent),
		  c(component),
		  b_px(enable_px),
		  b_py(enable_py),
		  b_vx(enable_vx),
		  b_vy(enable_vy),
		  b_ang_mom(enable_ang_mom),
		  b_lin_mom_x(enable_lin_mom_x),
		  b_lin_mom_y(enable_lin_mom_y)
	{
		setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
		xAxis->setVisible(true);
		xAxis2->setVisible(false);
		yAxis->setVisible(true);
		yAxis2->setVisible(false);
		xAxis->setLabel("time(steps)--->");
		yAxis->setLabel("Part params");
		auto physicsbody = c->physicsBody();
		steps.push_back(0);
		if (b_px || true)
		{
			px_graph = addGraph();
			px_graph->setPen(QPen(Qt::blue));
			px.push_back(component->pos().x());
		}
		show();
	}

	virtual ~cdPlotWidget()
	{

	}
public slots:
	void updatecdPlot()
	{
		steps.push_back(steps.last() + 1);
		if (b_px || true)
		{
			px.push_back(c->pos().x());
			px_graph->setData(steps, px);
		}
	}
};
#endif // DRAWPLOTUI_H
