/////////////////////////////////////////////////////////////
// snippets cdiwindow.cpp
// Testing out duplicate scenes. We can show same scene in multiple views.
// Zoom works too
QGraphicsView* view = new QGraphicsView();
view->setScene(sketchScene);
view->fitInView(0,0,1000,1000,Qt::KeepAspectRatio);
view->show();

view->setAttribute(Qt::WA_AcceptTouchEvents, true);
QLinearGradient gradient = QLinearGradient(0,0,0,1000);
gradient.setColorAt(0.0,QColor(255,175,175,100));
gradient.setColorAt(1.0, QColor(200,200,200,255));
view->setBackgroundBrush(QBrush(gradient));

/////////////////////////////////////////////////////////////
// 