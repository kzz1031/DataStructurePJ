#ifndef COLOR_H
#define COLOR_H

#endif // COLOR_H
#include<QColor>
#include<QPen>
QColor customOrange(255, 165, 0);
QColor customBlue(65, 105, 225); // 使用RGB数值创建稍浅的深蓝色

QColor customLightBlue(173, 206, 250);  // 使用RGB数值创建浅蓝色
QColor customLightGreen(144, 238, 144);  // 使用RGB数值创建浅绿色
QPen roadPen1(customBlue, 2.5);
QPen roadPen2(customOrange, 2);
QPen waterPen(customLightBlue,15);
QPen highwayPen(customLightGreen,3);
