#pragma once
#include <QMainWindow>
#include <QMainWindow>
#include <QPainter>
#include <QXmlStreamReader>
#include <QFile>
#include <QLabel>
#include <QDebug>
#include <QString>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include "menupage.h"
#include "kd_tree.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QGraphicsScene *scene;
    QGraphicsScene *scene2;
    void click();
    void store_ways();
    void PlusButton_click();
    void MinusButton_click();
    void OnOffButton_click();
    void LeftButton_click();
    void RightButton_click();
    void addStartAndEndPoints(QPointF startPoint, QPointF endPoint);
    void add_edge(QString x,QString y,double dis,bool tag,int ll);
    void mousePressEvent(QMouseEvent *event) override;
    void points_clear();

    void dij_calcu();
    void Astar_calcu();

    std::map<QString, double> dijkstra(const QString& start,const QString& end);
    void drawShortestPath(const QString& start, const QString& end,const QPen pen);
    std::vector<QGraphicsEllipseItem*> markers;
    std::vector<QGraphicsEllipseItem*> com_markers;
    std::vector<QGraphicsEllipseItem*> obs_markers;

    std::vector<QGraphicsPixmapItem*> obstacle;
    std::vector<QString> obstacle_name;
    std::vector<QGraphicsPathItem*>paths;
    std::vector<QGraphicsLineItem*>lines;
    std::vector<QGraphicsTextItem*>texts;
    bool point_dis = 0;
    double heuristic(const QString& from, const QString& to);
    double heuristic2(const QString& from, const QString& to);

    std::map<QString, double> Astar(const QString& start, const QString& end);
    std::vector<QPointF>draw_node_dij;
    std::vector<QPointF>draw_node_Astar;
    void draw();
    void clearPreviousPath();
    void change_background();
    void clear_points();
    void text_on();
    void home();
    void update_node();
    void set_obstacle();
    void openMenuPage();
    void onMenuPageBack();
    void mhd();
    void walk();
    void priority();
   // Node* buildKdTree(std::vector<Node>& points, int depth);

private:
    Ui::MainWindow *ui;
    MenuPage *menuPage;

protected:
    void paintEvent(QPaintEvent *event) override;
};

