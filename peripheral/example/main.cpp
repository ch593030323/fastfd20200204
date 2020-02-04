#include <QApplication>
#include "barscale_main_dialog.h"
#include "labelprint_main_dialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    barscale_main_dialog w;
    w.show();
    labelprint_main_dialog w2;
    w2.show();
    return a.exec();
}
