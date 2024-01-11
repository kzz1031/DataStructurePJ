#ifndef XML_H
#define XML_H

#endif // XML_H
#include <QMainWindow>
#include <QPainter>
#include <QXmlStreamReader>
#include <QFile>
#include <QDebug>
#include <vector>
#include <QString>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>

struct coordinates  //对应的经纬度坐标(double)
{
    int id;
    double lon;
    double lat;
};
struct coordinatesStr  //对应的经纬度坐标(QString)
{
    QString id;
    QString lon;
    QString lat;
};
struct coordinatesInt //对应的经纬度坐标(int)
{
    QString id;
    int x;
    int y;
};
struct node{
    QString tonode;
    double dis;
    int next;
    bool foot;
    double level;
};


class resolve_xml
{
public:
    double degreesToRadians(double degrees);
    double radiansToDegrees(double radians);
    double calculateDistance(coordinates node1, coordinates node2);
    void saveNode(int id,coordinates node);
    void strChangeDouble(coordinatesStr node);
    void findCommonNode();
    void changeCoor(QString strId);
    int imortant();
    void calculate();
    void min_max_lat();
    void store_ways();
    void Distance();
    std::map<QString,QPointF>doubleCoorMap;
    std::vector<coordinatesInt>doubleCoor;
    std::vector<coordinatesInt>doubleWay;
    double minLat;     //最小纬度
    double maxLat;     //最大经度
    double minLon;
    double maxLon;
};

