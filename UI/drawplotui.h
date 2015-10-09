#ifndef DRAWPLOTUI_H
#define DRAWPLOTUI_H

#include <QDialog>
#include <component.h>
#include "qCustomPlot/qcustomplot.h"
#include "component.h"
#include "physicsmanager.h"
#include "ui_drawplotui.h"
#include "Box2D/Dynamics/b2Body.h"

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
	CDI::PhysicsManager *physicsmanager;
private slots:
	void on_buttonBox_accepted();

	void on_buttonBox_rejected();

private:
	Ui::DrawPlotUI *ui;
};
//TODO In order to plot between different components, we should have an UI where we can capture the
// touch/pen input to select component and show the available properties. Also, can we have legend
// with other info??
class cdPlotWidget : public QCustomPlot
{
	Q_OBJECT
	CDI::Component* c;
	b2Body* physicsbody;
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
		connect(c, SIGNAL(destroyed(QObject*)),
				this, SLOT(deleteLater()));
		setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
		xAxis->setVisible(true);
		xAxis2->setVisible(false);
		yAxis->setVisible(true);
		yAxis2->setVisible(false);
		xAxis->setLabel("time(steps)--->");
		yAxis->setLabel("Part params");
		physicsbody = c->physicsBody();
		steps.push_back(0);
		QPen pen;
		pen.setWidth(4);
		if (b_px)
		{
			px_graph = addGraph();
			px_graph->setName("X");
			pen.setColor(QColor(000,000,153));
			px_graph->setPen(pen);
			px.push_back(physicsbody->GetPosition().x);
		}
		if (b_py)
		{
			py_graph = addGraph();
			py_graph->setName("Y");
			pen.setColor(QColor(128,000,000));
			py_graph->setPen(pen);
			px.push_back(physicsbody->GetPosition().y);
		}
		if (b_vx)
		{
			vx_graph = addGraph();
			vx_graph->setName("Velocity - X");
			pen.setColor(QColor(102,000,102));
			vx_graph->setPen(pen);
			vx.push_back(physicsbody->GetLinearVelocity().x);
		}
		if (b_vy)
		{
			vy_graph = addGraph();
			vy_graph->setName("Velocity - Y");
			vy_graph->setPen(QPen(QColor(000,153,204)));
			vy.push_back(physicsbody->GetLinearVelocity().y);
		}
		if (b_ang_mom)
		{
			ang_mom_graph = addGraph();
			pen.setColor(QColor(051,051,000));
			ang_mom_graph->setPen(pen);
			ang_mom.push_back(physicsbody->GetAngularVelocity() * physicsbody->GetInertia());
		}
		if (b_lin_mom_x)
		{
			lin_mom_x_graph = addGraph();
			lin_mom_x_graph->setPen(QPen(QColor(000,102,000)));
			lin_mom_x.push_back(physicsbody->GetLinearVelocity().x * physicsbody->GetMass());
		}
		if (b_lin_mom_y)
		{
			lin_mom_y_graph = addGraph();
			lin_mom_y_graph->setPen(QPen(QColor(000,153,051)));
			lin_mom_y.push_back(physicsbody->GetLinearVelocity().y * physicsbody->GetMass());
		}
		legend->setVisible(true);
		show();
	}

	virtual ~cdPlotWidget()
	{

	}
public slots:
	void updatecdPlot()
	{
		steps.push_back(steps.last() + 1);
		if (b_px)
		{
			px.push_back(physicsbody->GetPosition().x);
			px_graph->setData(steps, px);
		}
		if (b_py)
		{
			py.push_back(physicsbody->GetPosition().y);
			py_graph->setData(steps, py);
		}
		if (b_vx)
		{
			vx.push_back(physicsbody->GetLinearVelocity().x);
			vx_graph->setData(steps, vx);
		}
		if (b_vy)
		{
			vy.push_back(physicsbody->GetLinearVelocity().y);
			vy_graph->setData(steps, vy);
		}
		if (b_ang_mom)
		{
			ang_mom.push_back(physicsbody->GetAngularVelocity() * physicsbody->GetMass());
			ang_mom_graph->setData(steps, ang_mom);
		}
		if (b_lin_mom_x)
		{
			lin_mom_x.push_back(physicsbody->GetLinearVelocity().x * physicsbody->GetMass());
			lin_mom_x_graph->setData(steps, lin_mom_x);
		}
		if (b_lin_mom_y)
		{
			lin_mom_y.push_back(physicsbody->GetLinearVelocity().y * physicsbody->GetMass());
			lin_mom_y_graph->setData(steps, lin_mom_y);
		}
		if (steps.length() > 300)
		{
			auto stripper = [](QVector<double>& tostrip)
			{
				auto startiter = tostrip.begin();
				auto enditer = startiter + 150;
				tostrip.erase(startiter, enditer);
			};
			stripper(steps);
			if (b_px) stripper(px);
			if (b_py) stripper(py);
			if (b_vx) stripper(vx);
			if (b_vy) stripper(vy);
			if (b_ang_mom) stripper(ang_mom);
			if (b_lin_mom_x) stripper(lin_mom_x);
			if (b_lin_mom_y) stripper(lin_mom_y);
		}
		rescaleAxes();
		xAxis->setRangeLower(steps.last() - 150);
		xAxis->setRangeUpper(steps.last());
		replot();
	}
};
#endif // DRAWPLOTUI_H
