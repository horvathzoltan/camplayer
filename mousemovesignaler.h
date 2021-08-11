#ifndef MOUSEMOVESIGNALER_H
#define MOUSEMOVESIGNALER_H

#include <QtWidgets>

class MouseMoveSignaler : public QObject {
    Q_OBJECT
    bool eventFilter(QObject * obj, QEvent * ev) Q_DECL_OVERRIDE {
        //static int iii=0;
        //qDebug()<<iii++<<": "<<"obj:"<<obj<<" "<<"ev:"<<ev<<"\n";
        if ((ev->type() == QEvent::MouseMove
             || ev->type() == QEvent::Enter
             ||ev->type() == QEvent::Leave)
            && obj->isWidgetType())
        {
            emit mouseMoveEvent(static_cast<QWidget*>(obj),
                                static_cast<QMouseEvent*>(ev));
        }
        return false;
    }
public:
    Q_SIGNAL void mouseMoveEvent(QWidget *, QMouseEvent *);
    MouseMoveSignaler(QObject * parent = 0) : QObject(parent) {}
    void installOn(QWidget * widget)
    {
        widget->installEventFilter(this);
    }
};

#endif // MOUSEMOVESIGNALER_H
