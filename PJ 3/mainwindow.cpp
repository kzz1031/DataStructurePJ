#include "mainwindow.h"
#include "xml.h"
#include "menupage.h"
#include "kd_tree.h"
#include "ui_mainwindow.h"
#include "color.h"
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QScrollBar>
#include <QImageReader>
#include <QMouseEvent>
extern resolve_xml Xml;
extern std::map<QString,std::vector<QString>>mapWay;
extern std::map<QString,QString> roadname;
extern std::map<QString,bool> isRailway;
extern std::map<QString,bool> isWaterway;
extern std::map<QString,bool> isHighway;
extern std::map<QString,bool> isHospital;
extern std::map<QString,bool> isPark;
extern std::map<QString,bool> isOneway;
extern std::map<QString,bool> isFootway,isWater;
extern std::map<QString,int> HighwayLevel;
bool on_off = 1;
bool init = 0;
tree_Node* kdRoot;
std::vector<node> edge;
std::map<QString,int>start_edge;
std::vector<QString>points_cal;
std::map<QString,QString> nodetonode;

std::vector<std::vector<QPointF>>roads;
extern int Mode ;
long long avex,avey;
struct Node{
    QString ID;
    int x,y;

};

std::vector<Node> ways;
std::map<QString,bool> isPainted;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->graphicsView->centerOn(ui->graphicsView->width() / 2 - 1000, ui->graphicsView->height() / 2);
    ui->graphicsView_2->scale(0.07, 0.07);
    ui->graphicsView->scale(0.2, 0.2);
    ui->graphicsView_2->horizontalScrollBar()->setValue(ui->graphicsView->horizontalScrollBar()->value() - 200);
    ui->graphicsView_2->setRenderHint(QPainter::Antialiasing); //抗锯齿
    ui->graphicsView->setRenderHint(QPainter::Antialiasing); //抗锯齿
    scene = new QGraphicsScene(this);
    scene2 = new QGraphicsScene(this);
    ui->label->setText("Shortest Path Length: N/A");
    ui->label->setStyleSheet("background-color: lightblue; color: black; padding: 5px; border-radius: 3px;");
    ui->label_2->setText("Total Time: N/A");
    ui->label_2->setStyleSheet("background-color: white; color: black; padding: 5px; border-radius: 3px;");
    ui->label_3->setText("Total Time: N/A");
    ui->label_3->setStyleSheet("background-color: white; color: black; padding: 5px; border-radius: 3px;");
    menuPage = new MenuPage;
    connect(menuPage, &MenuPage::goBackToMainWindow, this, &MainWindow::onMenuPageBack);
    connect(menuPage, &MenuPage::storeWay, this, &MainWindow::store_ways);
    hide();
    menuPage->show();
}

MainWindow::~MainWindow()
{
    paths.clear();
    markers.clear();
    delete ui;
}

void MainWindow::onMenuPageBack() {
    menuPage->hide();
    show();
}


void MainWindow::add_edge(QString x,QString y,double dis,bool tag,int ll)
{
    node kk;
    kk.foot = tag;
    //if(tag) qDebug()<<"foot.....";
    kk.tonode = y;
    kk.dis = dis;
    if(ll == 1)  kk.level = 0.2;
    else if(ll == 2) kk.level = 0.3;
    else kk.level = 1;
    //qDebug()<<ll;
    if(start_edge.count(x)>0) kk.next = start_edge[x];
    else kk.next = -1;
    edge.push_back(kk);
    start_edge[x] = edge.size()-1;
}

void MainWindow::store_ways()
{
    qDebug()<<"storing now...";
    for(auto it = mapWay.begin();it != mapWay.end();it++)
    {

        std::vector<QString> vectorStr = it->second;
        QString wayId = it->first;
        if(!isHighway[wayId]) continue ;
        for(int i = 0;i < int(vectorStr.size()-1);i++)
        {
            QString wayStr1 = vectorStr[i];
            QString wayStr2 = vectorStr[i+1];

            QPointF coorWay1 = Xml.doubleCoorMap[wayStr1];
            QPointF coorWay2 = Xml.doubleCoorMap[wayStr2];
            double dis_ = sqrt((coorWay1.x()-coorWay2.x())*(coorWay1.x()-coorWay2.x())+(coorWay1.y()-coorWay2.y())*(coorWay1.y()-coorWay2.y()));
            MainWindow::add_edge(wayStr1,wayStr2,dis_,isFootway[wayId],HighwayLevel[wayId]);
            if(!isOneway[wayId])
            {
                MainWindow::add_edge(wayStr2,wayStr1,dis_,isFootway[wayId],HighwayLevel[wayId]);
            }
        }
       // qDebug()<<roadname[wayId]<<" total edges:" << vectorStr.size();
    }
    std::vector<tree_Node> kdPoints;
    for (auto it = mapWay.begin(); it != mapWay.end(); it++) {
        if(!isHighway[it->first]) continue;
        std::vector<QString> vectorStr = it->second;
        for (int i = 0; i < int(vectorStr.size()); i++) {
            tree_Node node;
            QPointF pointWay = Xml.doubleCoorMap[vectorStr[i]];
            node.point = pointWay;
            node.name = vectorStr[i];
            kdPoints.push_back(node);
        }
    }
    kdRoot = buildKdTree(kdPoints, 0);
    qDebug()<<kdRoot->point.x();
    qDebug()<<"total edges :"<<edge.size();
    onMenuPageBack();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    qDebug()<<"drawing__";
    QMainWindow::paintEvent(event);  // 调用父类的 paintEvent 以确保其他部分绘制正确


    if(Mode == 1)
    {
        scene->setBackgroundBrush(QBrush(QColor(50, 50, 50)));
    }
    else
    {
        scene->setBackgroundBrush(Qt::white);
    }
    //QGraphicsScene *scene = ui->graphicsView->scene();

    if(!init)
    {
        ui->graphicsView->setScene(scene);
        ui->graphicsView_2->setScene(scene2);
        init = 1;
        for(auto it = mapWay.begin();it != mapWay.end();it++)
        {
            avex = 0,avey = 0;
            std::vector<QString> vectorStr = it->second;
            //qDebug()<<roadname[it ->first];
            std::vector<QPointF>roadWay;
            for(int i = 0;i < int(vectorStr.size());i++)
            {
                QString wayStr = vectorStr[i];
                QPointF coorWay;
                coorWay = Xml.doubleCoorMap[wayStr];
                QPoint pointWay;
                pointWay.setX(coorWay.x());
                pointWay.setY(coorWay.y());

                avex += coorWay.x();
                avey += coorWay.y();
                roadWay.push_back(pointWay);
            }
            avex /= vectorStr.size();
            avey /= vectorStr.size();
            Node kk ;
            kk.ID = it->first;
            kk.x = avex;
            kk.y = avey;
            ways.push_back(kk);
            roads.push_back(roadWay);
            roadWay.clear();
        }
        int cnt = 0;
        for (const std::vector<QPointF> &road : roads) {
            QString wayId = ways[cnt].ID;
            QString roadName = roadname[wayId];
            QPainterPath pathsss;
            pathsss.moveTo(road[0]);
            for (int i = 0; i < int(road.size()); ++i)
            {
                pathsss.lineTo(road[i]);
            }
            if(isHospital[wayId] || isPark[wayId] || isWater[wayId])
            {
                QPolygonF polygon = pathsss.toFillPolygon();
                QGraphicsPolygonItem *polygonItem = new QGraphicsPolygonItem(polygon);

                if(isHospital[wayId]) polygonItem->setBrush( QColor(255, 182, 193));
                else if(isPark[wayId]) polygonItem->setBrush(QColor(144, 238, 144));
                else if(isWater[wayId]) polygonItem->setBrush(customLightBlue);
                //else polygonItem->setBrush(customBlue);
                ui->graphicsView->scene()->addItem(polygonItem);
            }
            else
            {
                QGraphicsPathItem *pathItem = new QGraphicsPathItem(pathsss);
                QGraphicsPathItem *pathItem2 = new QGraphicsPathItem(pathsss);
                if (!scene->items().contains(pathItem)) {
                    scene->addItem(pathItem);
                    scene2->addItem(pathItem2);
                    //paths.push_back((pathItem));
                }
                if(isRailway[wayId]) pathItem->setPen(roadPen1);
                else if(isWaterway[wayId]) pathItem->setPen(waterPen);
                else if(isHighway[wayId])
                {
                    pathItem->setBrush(Qt::NoBrush);
                    pathItem->setPen(highwayPen);
                }
                else pathItem->setPen(roadPen2);

                if(isRailway[wayId]) pathItem2->setPen(roadPen1);
                else if(isWaterway[wayId]) pathItem2->setPen(waterPen);
                else if(isHighway[wayId]) pathItem2->setPen(highwayPen);
                else pathItem2->setPen(roadPen2);
            }

            int x = ways[cnt].x;
            int y = ways[cnt].y;
            cnt++;
            //qDebug()<<cnt<<":  "<<roadName;

            if(!roadName.isEmpty()&&!isPainted[roadName]){
                isPainted[roadName] = 1;
                QPointF midpoint;
                midpoint.setX(x);
                midpoint.setY(y);
                QGraphicsTextItem *textItem ;//= scene->addText(roadName);
                textItem = new QGraphicsTextItem(roadName);
                textItem->setPos(midpoint);
                 texts.push_back((textItem));

                QFont font("微软雅黑", 10);
                textItem->setFont(font);
            }
        }
    }
}
