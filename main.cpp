#include "mainwindow.h"
#include "mousebuttonsignaler.h"
#include "mousemovesignaler.h"

#include <QApplication>
#include "settings.h"

Settings settings;
MouseButtonSignaler signaler;
MouseMoveSignaler mouse_move_signaler;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    settings.Load();
    CamPlayer::init();

    MainWindow w;
    w.show();
    bool e =  a.exec();

    settings.Save();
    return e;
}

