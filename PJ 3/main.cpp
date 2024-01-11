#include "mainwindow.h"
#include "xml.h"
#include <QApplication>

int page_change;
resolve_xml XML;
int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    qDebug()<<"RUNNING";
    MainWindow w;
    //XML.imortant();
    //w.store_ways();
    w.click();
    //w.show();
    return a.exec();
}
