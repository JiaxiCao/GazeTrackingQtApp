#include "GazeTrackingQtApp.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GazeTrackingQtApp w;
    w.show();
    return a.exec();
}
