#include "xml.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <queue>
#include <QTimer>
#include <cmath>
extern std::vector<node> edge;
extern std::map<QString,int>start_edge;
extern std::map<QString,QString> nodetonode;
extern resolve_xml Xml;
extern std::vector<QString>points_cal;
extern int cnt_mhd,cnt_obstacle,cnt_walk,cnt_priority;
struct Node {
    QString name;
    double g;  // 从起始节点到当前节点的实际代价
    double h;  // 从当前节点到目标节点的估计代价
    double f;  // f = g + h，综合考虑实际代价和估计代价的总代价
    Node* parent;  // 父节点指针
};

struct CompareNode {
    bool operator()(const Node* lhs, const Node* rhs) const {
        return lhs->f > rhs->f;
    }
};

void MainWindow::clear_points()
{
    qDebug() << "clear points...";
    for (auto &marker : com_markers) {
        qDebug() << "clear points...";
        if (marker->scene() == ui->graphicsView->scene()) { // 检查图形项是否属于正确的场景
            if(marker!=nullptr)
            {
                scene->removeItem(marker);
                delete marker;
            }
        }
        com_markers.clear();
    }
}


void MainWindow::dij_calcu()
{
    clear_points();
   // clearPreviousPath();
    if(points_cal.size()<2) return ;
    QString start = points_cal[0];
    QString end = points_cal[1];
    nodetonode.clear();
    std::map<QString , double> ans_dis = dijkstra(start,end);
    if(ans_dis.count(end) && ans_dis[end] != std::numeric_limits<double>::max())
    {
        //qDebug()<<ans_dis[end];
        ui->label->setText("距离最短: " + QString::number(ans_dis[end]));
        QPen pathPen(Qt::red, 10);
        drawShortestPath(start,end,pathPen);
    }
    else
    {
        ui->label->setText("Cannot find the path!");

    }
}

void MainWindow::Astar_calcu()
{
    clearPreviousPath();
    clear_points();
    if(cnt_obstacle) clearPreviousPath();
    if(points_cal.size()<2) return ;
    QString start = points_cal[0];
    QString end = points_cal[1];
    nodetonode.clear();
    std::map<QString , double> ans_dis = Astar(start,end);
    qDebug()<<"end....";
    if(ans_dis.count(end)&&ans_dis[end] != std::numeric_limits<double>::max())
    {
        qDebug()<<ans_dis[end];
        if(!cnt_priority)   ui->label->setText("距离最短: " + QString::number(ans_dis[end]));
        else ui->label->setText("大路优先: " + QString::number(ans_dis[end]));
        QPen pathPen(QColor(0, 100, 0), 10);
        drawShortestPath(start,end,pathPen);
    }
    else
    {
        ui->label->setText("Cannot find the path!");
    }
}
std::map<QString, double> MainWindow::dijkstra(const QString& start,const QString& end) {
    std::map<QString, double> distance;
    std::priority_queue<std::pair<double, QString>, std::vector<std::pair<double, QString>>, std::greater<>> pq;
    draw_node_dij.clear();
    auto start_time = std::chrono::high_resolution_clock::now();
    for (const auto& it : start_edge) {
        distance[it.first] = std::numeric_limits<double>::max();
    }
    distance[start] = 0;
    pq.push({0, start});

    while (!pq.empty()) {
        auto [dist, node] = pq.top();
        pq.pop();
        if (dist > distance[node]) continue;
        if(node == end)
        {
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
            ui->label_2->setText("Total Time: "+ QString::number(duration.count())+" ms");
            return distance;
        }
        QPointF point;
        point.setX(Xml.doubleCoorMap[node].x());
        point.setY(Xml.doubleCoorMap[node].y());
        draw_node_dij.push_back(point);

        for (int i = start_edge.at(node); i != -1; i = edge[i].next) {
            const auto& e = edge[i];
            if(e.foot == 1 && cnt_walk == 0) continue;
            if (distance[node] + e.dis < distance[e.tonode]) {
                nodetonode[e.tonode] = node;
                distance[e.tonode] = distance[node] + e.dis;
                pq.push({distance[e.tonode], e.tonode});
            }
        }
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    ui->label_2->setText("Total Time: "+ QString::number(duration.count())+" ms");
    return distance;
}


std::map<QString, double> MainWindow::Astar(const QString& start, const QString& end) {
    std::map<QString, double> distance;
    std::map<QString, Node*> openSet;
    std::map<QString, Node*> closedSet;
    std::priority_queue<Node*, std::vector<Node*>, CompareNode> openQueue;
    double value = ui->doubleSpinBox->value();
    draw_node_Astar.clear();
    qDebug()<<"Astaring...";
    Node* startNode;
    if(!cnt_mhd)     startNode = new Node{start, 0, heuristic2(start, end), heuristic2(start, end), nullptr};
    else      startNode = new Node{start, 0, heuristic(start, end), heuristic(start, end), nullptr};

    openSet[start] = startNode;
    openQueue.push(startNode);
    auto start_time = std::chrono::high_resolution_clock::now();
    int cnt = 0;
    while (!openQueue.empty() && cnt < 10000000) {
        cnt ++;
        Node* current = openQueue.top();
        openQueue.pop();
        openSet.erase(current->name);
        QPointF point;
        point.setX(Xml.doubleCoorMap[current->name].x());
        point.setY(Xml.doubleCoorMap[current->name].y());
        draw_node_Astar.push_back(point);
        if (current->name == end) {

            std::map<QString, double> result;
            while (current->parent != nullptr) {
                nodetonode[current->name] = current->parent->name;
                //qDebug()<<current->name;
                result[current->name] = current->g;
                current = current->parent;
            }
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
            ui->label_3->setText("Total Time: "+ QString::number(duration.count())+" ms");
            return result;
        }

        closedSet[current->name] = current;
        if(!start_edge.count(current -> name))  continue;
        for (int i = start_edge.at(current->name); i != -1; i = edge[i].next) {
            const auto& e = edge[i];
            if(e.foot == 1 && cnt_walk == 0) continue;
            double tentative_g = current->g + e.dis;
            if (closedSet.find(e.tonode) != closedSet.end() && tentative_g >= distance[e.tonode]) {
                continue;
            }

            if (openSet.find(e.tonode) == openSet.end() || tentative_g < distance[e.tonode]) {
                double val = 0;
                if(!cnt_mhd) val = heuristic2(e.tonode, end);
                else  val = heuristic(e.tonode, end);
                double kk = 1;
                if(cnt_priority)
                {
                    qDebug()<<"prio...."<<e.level;
                    kk = e.level;
                }
                Node* neighbor = new Node{e.tonode, tentative_g , value*val, 0, current};
                neighbor->f = (neighbor->g + neighbor->h) * kk;
                distance[e.tonode] = tentative_g ;
                openSet[e.tonode] = neighbor;
                openQueue.push(neighbor);
            }
        }
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    ui->label_3->setText("Ttttotal Time: "+ QString::number(duration.count())+" ms");
    return std::map<QString, double>();
}

void MainWindow::clearPreviousPath() {
    for(auto &item : paths)
    {
        ui->graphicsView->scene()->removeItem(item);
        delete item;
    }
    paths.clear();
}

void MainWindow::drawShortestPath(const QString& start, const QString& end, const QPen pen) {
    //clearPreviousPath();
    std::vector<QPointF> pathPoints;
    qDebug()<<"drawing shortest path";
    QString current = end;
    QPointF pp ;
    pp.setX(Xml.doubleCoorMap[current].x());
    pp.setY(Xml.doubleCoorMap[current].y());
    pathPoints.push_back(pp);

    while (nodetonode.count(current))
    {
        current = nodetonode[current];
        pp.setX(Xml.doubleCoorMap[current].x());
        pp.setY(Xml.doubleCoorMap[current].y());
        pathPoints.push_back(pp);
    }
    QPainterPath path;
    for (size_t i = 0; i < pathPoints.size(); ++i) {
        if(i == 0) path.moveTo(pathPoints[0]);
        path.lineTo(pathPoints[i]);
    }
    QGraphicsPathItem *pathitem = new QGraphicsPathItem(path);
    pathitem -> setPen(pen);
    ui->graphicsView->scene()->addItem(pathitem);
    paths.push_back(pathitem);
}

double MainWindow::heuristic2(const QString& from, const QString& to) {
    QPointF coorFrom = Xml.doubleCoorMap[from];
    QPointF coorTo = Xml.doubleCoorMap[to];
    double distance = sqrt(pow(coorTo.x() - coorFrom.x(), 2) + pow(coorTo.y() - coorFrom.y(), 2));
    return distance;
}


double MainWindow::heuristic(const QString& from, const QString& to) {
    return abs(Xml.doubleCoorMap[from].x()-Xml.doubleCoorMap[to].x())+abs(Xml.doubleCoorMap[from].y()-Xml.doubleCoorMap[to].y());
}
