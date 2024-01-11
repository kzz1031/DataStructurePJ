#include "kd_tree.h"
#include <vector>
#include <QDebug>
double distance(const QPointF& a, const QPointF& b) {
    return std::sqrt(std::pow(a.x() - b.x(), 2) + std::pow(a.y() - b.y(), 2));
}

bool sortByX(const tree_Node& a, const tree_Node& b) {
    return a.point.x() < b.point.x();
}

bool sortByY(const tree_Node& a, const tree_Node& b) {
    return a.point.y() < b.point.y();
}

void nearestNeighborSearch(const tree_Node* root, QPointF& target, QPointF& nearest, QString& nearestName, double& minDist, int depth) {
    if (root == nullptr) {
        return;
    }
    double dist = distance(root->point, target);
    //qDebug()<<root->point.x();
    if (dist < minDist) {
        qDebug()<<"searching    "<<dist;
        minDist = dist;
        nearest = root->point;
        nearestName = root->name;
        qDebug()<<nearest.x()<<"     "<<nearest.y();
    }

    int axis = depth % 2;
    const tree_Node* nearSubtree = (axis == 0 && target.x() < root->point.x()) || (axis == 1 && target.y() < root->point.y()) ? root->left : root->right;
    const tree_Node* farSubtree = (axis == 0 && target.x() >= root->point.x()) || (axis == 1 && target.y() >= root->point.y()) ? root->left : root->right;

    nearestNeighborSearch(nearSubtree, target, nearest, nearestName, minDist, depth + 1);

    if ((axis == 0 && std::abs(target.x() - root->point.x()) < minDist) || (axis == 1 && std::abs(target.y() - root->point.y()) < minDist)) {
        nearestNeighborSearch(farSubtree, target, nearest, nearestName, minDist, depth + 1);
    }
}

tree_Node *buildKdTree(std::vector<tree_Node>& points, int depth) {
    //qDebug()<<"kd_treeing";
    if (points.empty()) {
        return nullptr;
    }

    int axis = depth % 2;
    if (axis == 0) {
        std::sort(points.begin(), points.end(), sortByX);
    } else {
        std::sort(points.begin(), points.end(), sortByY);
    }

    int median = points.size() / 2;
    tree_Node* node = new tree_Node;
    node->point = points[median].point;
    node->name = points[median].name;
    std::vector<tree_Node> leftPoints(points.begin(), points.begin() + median);
    node->left = buildKdTree(leftPoints, depth + 1);
    std::vector<tree_Node> rightPoints(points.begin() + median + 1, points.end());
    node->right = buildKdTree(rightPoints, depth + 1);
    return node;
}
// 最近点搜索



