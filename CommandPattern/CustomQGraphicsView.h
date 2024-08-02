#ifndef CUSTOMQGRAPHICSVIEW_H
#define CUSTOMQGRAPHICSVIEW_H

class myQGraphicsView : public QGraphicsView
{
public:
    myQGraphicsView(QWidget * parent = nullptr);
    myQGraphicsView(QGraphicsScene * scene, QWidget * parent = nullptr);

protected:
    virtual void wheelEvent(QWheelEvent * event);
};

#endif // CUSTOMQGRAPHICSVIEW_H
