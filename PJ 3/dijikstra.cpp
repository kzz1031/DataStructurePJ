#include "mainwindow.h"

void MainWindow::dij_calcu()
{
    if(points_cal.size()<2) return ;
    QString start = points_cal[0];
    QString end = points_cal[1];
    nodetonode.clear();
    std::map<QString , double> ans_dis = dijkstra(start);
    if(ans_dis[end] != std::numeric_limits<double>::max())
    {
        qDebug()<<ans_dis[end];
        drawShortestPath(start,end);
    }
    else
    {

    }
}

std::map<QString, double> MainWindow::dijkstra(const QString& start) {
    std::map<QString, double> distance;
    std::priority_queue<std::pair<double, QString>, std::vector<std::pair<double, QString>>, std::greater<>> pq;

    for (const auto& it : start_edge) {
        distance[it.first] = std::numeric_limits<double>::max();
    }
    distance[start] = 0;
    pq.push({0, start});

    while (!pq.empty()) {
        auto [dist, node] = pq.top();
        pq.pop();
        if (dist > distance[node]) continue;

        for (int i = start_edge.at(node); i != -1; i = edge[i].next) {
            const auto& e = edge[i];
            if (distance[node] + e.dis < distance[e.tonode]) {
                nodetonode[e.tonode] = node;
                distance[e.tonode] = distance[node] + e.dis;
                pq.push({distance[e.tonode], e.tonode});
            }
        }
    }

    return distance;
}

void MainWindow::drawShortestPath(const QString& start, const QString& end) {
    std::vector<QPointF> pathPoints;

    QString current = end;
    QPointF pp ;
    pp.setX(Xml.doubleCoorMap[current].x);
    pp.setY(Xml.doubleCoorMap[current].y);
    pathPoints.push_back(pp);
    int cnt = 0;
    /*
    while (current != start) {
        qDebug()<<cnt++;
        for (int i = start_edge.at(current); i != -1; i = edge[i].next) {
            if (distance.at(current) == distance.at(edge[i].tonode) + edge[i].dis) {
                pp.setX(Xml.doubleCoorMap[edge[i].tonode].x);
                pp.setY(Xml.doubleCoorMap[edge[i].tonode].y);
                pathPoints.push_back(pp);
                current = edge[i].tonode;
                break;
            }
        }
    }*/
    while (current != start)
    {
        current = nodetonode[current];
        pp.setX(Xml.doubleCoorMap[current].x);
        pp.setY(Xml.doubleCoorMap[current].y);
        pathPoints.push_back(pp);
    }
    // 绘制路径
    QGraphicsScene* scene = ui->graphicsView->scene();
    QPen pathPen(Qt::red, 1); // 绿色笔
    for (size_t i = 1; i < pathPoints.size(); ++i) {
        QGraphicsLineItem* line = scene->addLine(QLineF(pathPoints[i - 1], pathPoints[i]), pathPen);
        ui->graphicsView->scene()->addItem(line);
    }
}
