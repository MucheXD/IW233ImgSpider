#include "IW233ImgSpider.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    IW233ImgSpider w;
    w.show();
    return a.exec();
}
