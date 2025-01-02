#include "AsdEngine.h"
#include <QtWidgets/QApplication>
#include "log.h"

int main(int argc, char *argv[])
{
 //   aaa();
    QApplication a(argc, argv);
    AsdEngine w;
    w.show();
    return a.exec();
}
