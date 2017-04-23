#ifndef _PATH_GENERATE_TOOL_H
#define _PATH_GENERATE_TOOL_H
#include "BaseType.h"
#include "Topology.h"
#include <Eigen/Dense>
class SXYSpline {
public:
	Eigen::MatrixX4d skX;
	Eigen::MatrixX4d skY;
	std::vector<double> S;//�ۻ�����
	std::vector<double> X;//��X
	std::vector<double> Y;//��Y
	int splineNum;
	void init(std::vector<PointPt> baseFrame);
	double getKappa(double Sf);
	RadAngle getTangent(double Sf);
	double getS(int index, double X, double Y);
};
#endif