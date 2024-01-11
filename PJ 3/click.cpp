#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "xml.h"
#include "menupage.h"
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QScrollBar>
#include <QMouseEvent>
#include "kd_tree.h"

extern resolve_xml Xml;
extern std::map<QString,std::vector<QString>>mapWay;
extern std::map<QString,int>start_edge;
extern std::vector<QString>points_cal;
extern std::map<QString,bool> isHighway;
extern std::vector<node> edge;
extern MenuPage m;
extern tree_Node *kdRoot;
QString Home_str = "5447431183";
int cnt_home = 0,cnt_obstacle = 0,cnt_mhd,cnt_walk,cnt_priority;
int Mode = 0;
double plus_cnt = 0.8;
QGraphicsEllipseItem *marker1 = new QGraphicsEllipseItem(-15, -15, 30, 30);
QGraphicsEllipseItem *marker2 = new QGraphicsEllipseItem(-30, -30, 60, 60);
void MainWindow::click()
{
    connect(ui->PlusButton, &QPushButton::clicked, this, &MainWindow::PlusButton_click);
    connect(ui->MinusButton, &QPushButton::clicked, this, &MainWindow::MinusButton_click);
    connect(ui->LeftButton, &QPushButton::clicked, this, &MainWindow::LeftButton_click);
    connect(ui->RightButton, &QPushButton::clicked, this, &MainWindow::RightButton_click);
    connect(ui->OnOffButton, &QPushButton::clicked, this ,&MainWindow::OnOffButton_click);
    connect(ui->ClearButton, &QPushButton::clicked, this ,&MainWindow::points_clear);
    connect(ui->dijkstras,&QPushButton::clicked, this , &MainWindow::dij_calcu);
    connect(ui->Astar,&QPushButton::clicked, this , &MainWindow::Astar_calcu);
    connect(ui->darkmode,&QPushButton::clicked, this , &MainWindow::change_background);
    connect(ui->Text_On_Button,&QCheckBox::clicked, this , &MainWindow::text_on);
    connect(ui->CompareButton,&QPushButton::clicked, this , &MainWindow::draw);
    connect(ui->home,&QPushButton::clicked, this , &MainWindow::home);
    connect(ui->obstacle,&QPushButton::clicked, this , &MainWindow::set_obstacle);
    //connect(ui->menu_button, &QPushButton::clicked, this, &MainWindow::openMenuPage);
    connect(ui->mhd,&QPushButton::clicked, this , &MainWindow::mhd);
    connect(ui->walk,&QPushButton::clicked, this , &MainWindow::walk);
    connect(ui->priority,&QPushButton::clicked, this , &MainWindow::priority);

}

void MainWindow::openMenuPage()
{
    hide();

    menuPage->show();
}

void MainWindow::text_on()
{
    if(ui->Text_On_Button->checkState()==Qt::Checked)
    {
        for (auto &text : texts) {
            if(text == nullptr) continue;
            scene->addItem(text);
        }
    }
    else
    {
        for (auto &text : texts) {
            qDebug()<<"delete...";
            if(text == nullptr) continue;
            if (text->scene() == ui->graphicsView->scene()) {
                scene->removeItem(text);
            }
        }
    }
}

void MainWindow::priority()
{
    if(cnt_priority == 0)
    {
        cnt_priority = 1;
        ui->priority->setStyleSheet("border: 30px solid red;");
    }
    else
    {
        cnt_priority = 0;
        ui->priority->setStyleSheet("");
    }
}

void MainWindow::walk()
{
    if(cnt_walk == 0)
    {
        cnt_walk = 1;
        ui->walk->setStyleSheet("border: 30px solid red;");
    }
    else
    {
        cnt_walk = 0;
        ui->walk->setStyleSheet("");
    }
}

void MainWindow::mhd()
{
    if(cnt_mhd == 0)
    {
        cnt_mhd = 1;
        ui->mhd->setStyleSheet("border: 30px solid red;");
    }
    else
    {
        cnt_mhd = 0;
        ui->mhd->setStyleSheet("");
    }
}

void MainWindow::set_obstacle()
{
    if(cnt_obstacle==0)
    {
        cnt_obstacle = 1;
        ui->obstacle->setStyleSheet("border: 30px solid red;");
    }
    else
    {
        cnt_obstacle = 0;

        for(auto &mark : obstacle)
        {
            ui->graphicsView->scene()->removeItem(mark);
            delete mark;
        }
        obstacle.clear();
        for(auto &mark : obs_markers)
        {
            ui->graphicsView->scene()->removeItem(mark);
            delete mark;
        }
        obs_markers.clear();
        qDebug()<<obstacle_name.size();
        for(auto &node : obstacle_name)
        {
            for(int kk = start_edge[node];kk != -1;kk = edge[kk].next)
            {
                QPointF coorWay1 = Xml.doubleCoorMap[node];
                QPointF coorWay2 = Xml.doubleCoorMap[edge[kk].tonode];
                double dis_ = sqrt((coorWay1.x()-coorWay2.x())*(coorWay1.x()-coorWay2.x())+(coorWay1.y()-coorWay2.y())*(coorWay1.y()-coorWay2.y()));
                edge[kk].dis = edge[kk+1].dis  = dis_;
            }
        }
        obstacle_name.clear();
        ui->obstacle->setStyleSheet("");
    }
}

void MainWindow::home()
{
    if(cnt_home==0)
    {
        cnt_home = 1;
        ui->home->setStyleSheet("border: 30px solid red;");
        points_cal.push_back(Home_str);
        QPointF coorWay;
        coorWay = Xml.doubleCoorMap[Home_str];
        marker1->setPos(coorWay.x(),coorWay.y());
        marker1->setBrush(Qt::yellow);
        marker2->setPos(coorWay.x(),coorWay.y());
        marker2->setBrush(Qt::yellow);
        ui->graphicsView->scene()->addItem(marker1);
        ui->graphicsView_2->scene()->addItem(marker2);
    }
    else
    {
        cnt_home = 0;
        auto it = std::find(points_cal.begin(), points_cal.end(), Home_str);
        if(it != points_cal.end())  points_cal.erase(it);
        ui->home->setStyleSheet("");
        ui->graphicsView->scene()->removeItem(marker1);
        ui->graphicsView_2->scene()->removeItem(marker2);
    }

}

void MainWindow::change_background()
{
    if(Mode == 0)
    {
        Mode = 1;
       scene->setBackgroundBrush(QBrush(QColor(50, 50, 50))); // 设置为深灰色背景
       ui->darkmode->setIcon(QIcon(":/new/night.png"));
       ui->darkmode->setIconSize(QSize(45,45));
    }
    else
    {
        Mode = 0;
        scene->setBackgroundBrush(Qt::white);
        ui->darkmode->setIcon(QIcon(":/new/light.png"));
        ui->darkmode->setIconSize(QSize(45,45));
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        QPoint mousePos = event->pos();
        QPointF scenePos = ui->graphicsView->mapToScene(mousePos);

        QPointF nearestPoint;
        QString PointName;

        double minDistance = std::numeric_limits<double>::max();

        nearestNeighborSearch(kdRoot, scenePos, nearestPoint, PointName, minDistance, 0);

        if(!cnt_obstacle)
        {
            points_cal.push_back(PointName);
            qDebug()<<PointName;
            QGraphicsEllipseItem *marker1 = new QGraphicsEllipseItem(-15, -15, 30, 30);
            QGraphicsEllipseItem *marker2 = new QGraphicsEllipseItem(-30, -30, 60, 60);
            QBrush brush;
            brush = Qt::red;
            marker1->setBrush(brush);
            marker1->setPos(nearestPoint);
            marker2->setPos(nearestPoint);
            marker2->setBrush(brush);
            ui->graphicsView->scene()->addItem(marker1);
            ui->graphicsView_2->scene()->addItem(marker2);
        }

        if(cnt_obstacle)
        {
            QGraphicsPixmapItem *item = new QGraphicsPixmapItem;
            item -> setPixmap(QPixmap(":/new/object2.png"));
            //item->setTransformOriginPoint(item->boundingRect().center());
            item -> setPos(nearestPoint);
            QGraphicsEllipseItem *marker1 = new QGraphicsEllipseItem(-10, -10, 20, 20);
            marker1->setPos(nearestPoint);
            marker1->setBrush( Qt::blue );
            ui->graphicsView->scene()->addItem(marker1);
            obs_markers.push_back(marker1);
            for(int kk = start_edge[PointName]; kk!=-1; kk = edge[kk].next )
            {
                edge[kk].dis = edge[kk+1].dis = 10000000;
            }
            item->setScale(0.5);
            ui->graphicsView->scene()->addItem(item);
            obstacle.push_back(item);
            obstacle_name.push_back(PointName);
        }
    }
}

void MainWindow::update_node()
{

    foreach (QGraphicsItem *item,ui->graphicsView->scene()->items()) {
        // 检查是否是QGraphicsEllipseItem
        if (QGraphicsEllipseItem *ellipseItem = qgraphicsitem_cast<QGraphicsEllipseItem *>(item)) {
            // 获取当前椭圆的矩形边界
            QRectF rect = ellipseItem->rect();
            // 将椭圆的大小增加1.15倍
            if(rect.width()<3 || rect.width()>12) continue ;
            rect.setWidth(rect.width() * plus_cnt);
            rect.setHeight(rect.height() *plus_cnt);
            // 设置新的椭圆大小
            ellipseItem->setRect(rect);
        }
    }

}

void MainWindow::points_clear()
{
    clearPreviousPath();
    lines.clear();
    draw_node_dij.clear();
    draw_node_Astar.clear();
    QGraphicsScene *scene = ui->graphicsView->scene();
    for (QGraphicsItem *item : scene->items()) {
        QGraphicsEllipseItem *marker = qgraphicsitem_cast<QGraphicsEllipseItem*>(item);
        if(marker == nullptr || marker == marker1) continue;
        if(std::find(obs_markers.begin(),obs_markers.end(),marker) != obs_markers.end() ) continue;
        scene->removeItem(marker);
        delete marker;
        }
    com_markers.clear();
    QGraphicsScene *scene2= ui->graphicsView_2->scene();
    for (QGraphicsItem *item : scene2->items()) {
        QGraphicsEllipseItem *marker = qgraphicsitem_cast<QGraphicsEllipseItem*>(item);
        if(marker == nullptr||marker == marker2) continue;
        scene2->removeItem(marker);
        delete marker;
       }
    points_cal.clear();
       if(cnt_home)   points_cal.push_back("5447431183");
}


void MainWindow::LeftButton_click()
{
    ui->graphicsView->horizontalScrollBar()->setValue(ui->graphicsView->horizontalScrollBar()->value() - 20); // 向左移动10个像素
}

void MainWindow::RightButton_click()
{
    ui->graphicsView->horizontalScrollBar()->setValue(ui->graphicsView->horizontalScrollBar()->value() + 20); // 向右移动10个像素
}

void MainWindow::OnOffButton_click()
{

}

void MainWindow::PlusButton_click()
{
    double scaleFactor = 1.15;
    plus_cnt = 0.8;
    QPointF center = ui->graphicsView->mapToScene(ui->graphicsView->viewport()->rect().center());

    ui->graphicsView->centerOn(center);

    ui->graphicsView->scale(scaleFactor, scaleFactor);

    //update_node();
}

void MainWindow::MinusButton_click()
{

    double scaleFactor = 1.15;
    plus_cnt = 1.25;
    QPointF center = ui->graphicsView->mapToScene(ui->graphicsView->viewport()->rect().center());

    ui->graphicsView->centerOn(center);

    ui->graphicsView->scale(1.0 / scaleFactor, 1.0 / scaleFactor);

   // update_node();
}

void MainWindow::draw()
{
    clear_points();
    for(auto &point: draw_node_dij)
    {
        QGraphicsEllipseItem *marker = new QGraphicsEllipseItem(-6, -6, 12, 12);
        marker->setPos(point);
        marker->setPen(Qt::NoPen);
        marker->setBrush(Qt::red);
        com_markers.push_back(marker);
        ui->graphicsView->scene()->addItem(marker);
    }
    for(auto &point: draw_node_Astar)
    {
        QGraphicsEllipseItem *marker = new QGraphicsEllipseItem(-6, -6, 12, 12);
        marker->setPos(point);
        marker->setPen(Qt::NoPen);
        marker->setBrush(Qt::blue);
        com_markers.push_back(marker);
        ui->graphicsView->scene()->addItem(marker);
    }
}

/*
        for(auto it = mapWay.begin();it != mapWay.end();it++)
        {
            std::vector<QString> vectorStr = it->second;
            QString name = it->first;
            if(!isHighway[name]) continue;
            std::vector<QPointF>roadWay;
            for(int i = 0;i < int(vectorStr.size());i++)
            {

                QString wayStr = vectorStr[i];
                QPointF coorWay;
                coorWay = Xml.doubleCoorMap[wayStr];
                QPointF pointWay;
                pointWay.setX(coorWay.x());
                pointWay.setY(coorWay.y());
                double distance = QLineF(scenePos, pointWay).length();

                if (distance < minDistance)
                {
                    PointName = vectorStr[i];
                    minDistance = distance;
                    nearestPoint.setX(coorWay.x());
                    nearestPoint.setY(coorWay.y());
                }
            }
        }*/
