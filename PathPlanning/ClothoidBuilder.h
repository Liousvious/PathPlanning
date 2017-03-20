#pragma once

#define infcon 10000000
#define eps1 2.2204e-016

class ClothoidBuilder
{
public:
	ClothoidBuilder();
	~ClothoidBuilder();

protected:
	void FresnelCS(double *FresnelC, double *FresnelS, double y);
	


private:
	double _k;//б��
	double _dk;//б�ʱ仯��
	double _L;//Clothoid���߳���
};

