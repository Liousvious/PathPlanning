#ifndef _TOPOLOGY_MATH_METHOD
#define _TOPOLOGY_MATH_METHOD
#include "BaseType.h"
#include "Variables.h"
#include <cmath>
#include <algorithm>
int fileLength = 100;
bool InsideConvexPolygon(int vcount, RoadPoint polygon[], RoadPoint q);//�������ι�ϵ�ж�
bool CurveInsects(RoadPoint Up[], RoadPoint Down[], LINESEG u);//
bool intersect(LINESEG u, LINESEG v);//ֱ���ཻ�㷨
double Distance(RoadPoint m1, RoadPoint m2);  //ŷ����þ���
double cosangle(RoadPoint m1, RoadPoint m2, RoadPoint m3); //��������
double multiply(RoadPoint sp, RoadPoint ep, RoadPoint op);
int sign(double m);
RoadPoint getIntersectPoint(RoadPoint u1, RoadPoint u2, RoadPoint v1, RoadPoint v2);//�������߶εĽ���
double PointToSegDist(double x, double y, double x1, double y1, double x2, double y2);//��㣨x,y�����߶εľ���
void makeline(RoadPoint p1, RoadPoint p2, double &a, double &b, double &c);//��ֱ֪�������㣬��ֱ�߷���ax+by+c=0������a,b,c
bool Line_Seg_Intersect(double x1, double y1, double x2, double y2, double a, double b, double c);//�ж��߶κ�ֱ���Ƿ��ཻ
bool LineIntersection(double a1, double b1, double c1, double a2, double b2, double c2, RoadPoint &p);//��ֱ�ߺ�ֱ�ߵĽ���
bool Angle_Bisector_Intersect(RoadPoint p1, RoadPoint p2, RoadPoint p3, RoadPoint &p);//�������ν�ƽ���ߺͶԱߵĽ���(����p1���ڵĽǣ�
#endif