#ifndef KD_TREE_H
#define KD_TREE_H

#include <QPointF>
#include <QString>

struct tree_Node {
    QPointF point;
    QString name; // 保存点的名称
    tree_Node* left;
    tree_Node* right;
};

// 按照x坐标或y坐标进行排序
tree_Node *buildKdTree(std::vector<tree_Node>& points, int depth);
void nearestNeighborSearch(const tree_Node* root, QPointF& target, QPointF& nearest, QString& nearestName, double& minDist, int depth);
#endif // KD_TREE_H
