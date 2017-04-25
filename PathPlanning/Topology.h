#ifndef _TOPOLOGY_MATH_METHOD
#define _TOPOLOGY_MATH_METHOD
#include "BaseType.h"
#include "Variables.h"
#include <cmath>
#include <algorithm>
#include <vector>
#include <assert.h>
#include "DataCenter.h"
#include <Eigen/Dense>
#include <math.h>
class Topology
{
public:
	static bool InsideConvexPolygon(int vcount, RoadPoint polygon[], RoadPoint q);//�������ι�ϵ�ж�
	static bool CurveInsects(RoadPoint Up[], RoadPoint Down[], LINESEG u, int GPSPtNum);//�ж������Ƿ��ཻ
	static bool intersect(LINESEG u, LINESEG v);//ֱ���ཻ�㷨
	static double Distance2(RoadPoint m1, RoadPoint m2);  //�������ƽ��
	static double cosangle(RoadPoint m1, RoadPoint m2, RoadPoint m3); //��������
	static double multiply(RoadPoint sp, RoadPoint ep, RoadPoint op);
	static int sign(double m);
	static RoadPoint getIntersectPoint(RoadPoint u1, RoadPoint u2, RoadPoint v1, RoadPoint v2);//�������߶εĽ���
	static double PointToSegDist(double x, double y, double x1, double y1, double x2, double y2);//��㣨x,y�����߶εľ���
	static void makeline(RoadPoint p1, RoadPoint p2, double &a, double &b, double &c);//��ֱ֪�������㣬��ֱ�߷���ax+by+c=0������a,b,c
	static bool Line_Seg_Intersect(double x1, double y1, double x2, double y2, double a, double b, double c);//�ж��߶κ�ֱ���Ƿ��ཻ
	static bool LineIntersection(double a1, double b1, double c1, double a2, double b2, double c2, RoadPoint &p);//��ֱ�ߺ�ֱ�ߵĽ���
	static bool Angle_Bisector_Intersect(RoadPoint p1, RoadPoint p2, RoadPoint p3, RoadPoint &p);//�������ν�ƽ���ߺͶԱߵĽ���(����p1���ڵĽǣ�
	static bool check_velogrid_rdPt_intersected(VeloGrid_t& veloGrids, std::vector<RoadPoint>& genPoints);
	static Eigen::MatrixXd rotate(double theta, Eigen::MatrixXd in);
	static std::vector<double> lufact(std::vector<std::vector<double>> A, std::vector<double> B);
	static Eigen::MatrixX4d CubicSpline(std::vector<double> X, std::vector<double> Y, double dds0 = 0, double ddsn = 0);
	static void Rotate_To_Decare(double theta, double x, double y, double& x_out, double& y_out);
	static void Rotate(double theta, double x, double y, double& x_out, double& y_out);
	static void Rotate_To_Guassian(double theta, double x, double y, double& x_out, double& y_out);
	static double toAngle(double dx, double dy);
};
#endif