#include "xml.h"
#include <map>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
std::map<QString,coordinates>doubleMap;
std::map<QString,std::vector<QString>>mapWay;
std::map<QString,coordinatesStr>mp;
std::map<QString,QString> roadname;//ID to name
std::vector<QString>nodeamenity;
resolve_xml Xml;
std::map<QString,bool>isRailway;
std::map<QString,bool>isWaterway;
std::map<QString,bool>isHospital;
std::map<QString,bool> isHighway;
std::map<QString,bool> isPark;
std::map<QString,bool> isOneway;
std::map<QString,bool> isFootway;
std::map<QString,bool> isWater;
std::map<QString,int> HighwayLevel;
std::vector<QString>strNode;

QString file_str;

double resolve_xml::degreesToRadians(double degrees) {
    return degrees * M_PI / 180;
}
double resolve_xml::radiansToDegrees(double radians) {
    return radians * 180 / M_PI;
}
double resolve_xml::calculateDistance(coordinates node1, coordinates node2) {
    double dLat = degreesToRadians(node2.lat - node1.lat);
    double dLon = degreesToRadians(node2.lon - node1.lon);
    double a = std::sin(dLat / 2) * std::sin(dLat / 2) +
               std::cos(degreesToRadians(node1.lat)) * std::cos(degreesToRadians(node2.lat)) *
                   std::sin(dLon / 2) * std::sin(dLon / 2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
    double distance = 6378137 * c;
    return distance;
}

void resolve_xml::calculate()
{
    for(auto it = mp.begin();it != mp.end();it++)
    {
        QString Id = it->first;
        coordinatesStr jw = it->second;
        Xml.strChangeDouble(jw);
        Xml.changeCoor(Id);
    }
}

void resolve_xml::min_max_lat()
{
    for(auto it = mapWay.begin();it != mapWay.end();it++)
    {
        std::vector<QString> Strway = it->second;
        for(int i = 0;i < int(Strway.size());i++)
        {
            QString Qstr = Strway[i];
            double x = doubleMap[Qstr].lat;
            double y = doubleMap[Qstr].lon;
            minLat = std::min(minLat,x);
            minLon = std::min(minLon,y);
            maxLat = std::max(maxLat,x);
            maxLon = std::max(maxLon,y);
        }
    }
    qDebug()<<"minlat: "<<minLat<<" maxlat: "<<maxLat<<Qt::endl;
    qDebug()<<"minLon: "<<minLon<<" maxLon: "<<maxLon<<Qt::endl;

}

void resolve_xml::Distance()
{

}

void resolve_xml::store_ways()
{

}

void resolve_xml::strChangeDouble(coordinatesStr node)
{
    coordinates coor;
    coor.id = node.id.toInt();
    coor.lat = node.lat.toDouble();
    coor.lon = node.lon.toDouble();
    doubleMap[node.id] = coor;
}
void resolve_xml::changeCoor(QString strId)
{
    coordinates coo = doubleMap[strId];
    QPointF cooint ;
    int x = (maxLat - coo.lat ) * 50000 ;
    int y = (coo.lon - minLon) * 50000 ;
    cooint.setX(y);
    cooint.setY(x);
    Xml.doubleCoorMap[strId] = cooint;
}

int resolve_xml::  imortant()
{
    mp.clear(); strNode.clear(); isRailway.clear(); isWaterway.clear(); isHospital.clear() ; isHighway.clear(); isPark.clear();
    std::vector<QString>strWay;
    coordinatesStr coorStr;
    QString wayId;
    QString tagKey;
    QString tagValue;
    QString nodeId;
    QString nodeLon;
    QString nodeLat;
    QFile fileName(file_str);
    bool tagWay = 0;
    if (!fileName.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "cannot open this file\n";
        return 1;
    }
    else qDebug() << "open successfully\n";

    QXmlStreamReader xml(&fileName);
    while (!xml.atEnd() ) {
        if( xml.hasError())
        {
            qDebug()<<"xml error\n";
            break ;
        }
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token == QXmlStreamReader::StartElement) {
            if(xml.name() == QString("node")){
                nodeId = xml.attributes().value("id").toString();
                nodeLon = xml.attributes().value("lon").toString();
                nodeLat = xml.attributes().value("lat").toString();
                coorStr.id = nodeId;
                coorStr.lat = nodeLat;
                coorStr.lon = nodeLon;
                mp[nodeId] = coorStr;
                strNode.push_back(nodeId);
            }
            else if (xml.name() == QString("way")) {
                tagWay = 1;
                strWay.clear();
                wayId = xml.attributes().value("id").toString();
            } else if (xml.name() ==QString("nd")) {
                QString ndRef = xml.attributes().value("ref").toString();
                strWay.push_back(ndRef);
                mapWay[wayId] = strWay;
            } else if (xml.name() == QString("tag")) {
                tagKey = xml.attributes().value("k").toString();
                tagValue = xml.attributes().value("v").toString();
                if(tagKey == "railway") isRailway[wayId] = 1;
                if(tagKey == "waterway"||tagValue == "water")  isWaterway[wayId] = 1;
                if(tagKey == "water" ) isWater[wayId] = 1;
                if(tagKey == "highway")
                {
                    isHighway[wayId] = 1;
                    if(tagValue == "primary" || tagValue == "primary_link")  HighwayLevel[wayId] = 1;
                    else if(tagValue == "secondary" || tagValue == "secondary_link")  HighwayLevel[wayId] = 2;
                    else if(tagValue == "tertiary" || tagValue == "tertiary_link")  HighwayLevel[wayId] = 3;
                    else HighwayLevel[wayId] = 4;

                }
                if(tagValue == "hospital")  isHospital[wayId] = 1;
                if(tagValue == "park")  isPark[wayId] = 1;
                if(tagKey == "name" && tagWay)
                {
                    roadname[wayId] = tagValue;
                    //qDebug()<<"Way ID:"<<wayId<<", name:"<< tagValue ;
                }
                if(tagKey == "oneway" && tagValue == "yes")  isOneway[wayId] = 1;
                if(tagValue == "footway" || tagValue == "pedestrian")  isFootway[wayId] = 1;

            }
            else if(xml.name() == QString("bounds"))
            {
                //提取数据中的最大最小经纬度
                Xml.minLat = xml.attributes().value("minlat").toString().toDouble();
                Xml.minLon = xml.attributes().value("minlon").toString().toDouble();
                Xml.maxLat = xml.attributes().value("maxlat").toString().toDouble();
                Xml.maxLon = xml.attributes().value("maxlon").toString().toDouble();
            }
        }
    }
    resolve_xml::calculate();
    resolve_xml::min_max_lat();
   // resolve_xml::store_ways();
   // resolve_xml::Distance();

    fileName.close();
    return 0;
}
