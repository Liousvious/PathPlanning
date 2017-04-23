#pragma once
#include <vector>

class ShortTimePathPlanning
{
public:
	ShortTimePathPlanning(float theta);
	~ShortTimePathPlanning();
	void GetBaseFrame(const std::vector<float>& x, const std::vector<float>& y, int num);//��ȡ����·�����ο�·����
	bool GenerateCandidatePath();
private:
	float _Qi;//���Ĳ���ƫ����
	float _Qf;//·���յ�����ڻ���·���Ĳ���ƫ����
	std::vector<float> _BaseFrame_x;//����·���ĵ��x����
	std::vector<float> _BaseFrame_y;//����·���ϵ��y����
	int _BasePtCount;//����·���ϵ�ļ���
	float _theta;//��ǰλ�ó�ͷ�����·���ļн�
};

