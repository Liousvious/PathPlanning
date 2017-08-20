#pragma once

#include "BaseType.h"
#include <vector>
#include "ShortTimePlanner.h"
#include "PathDraw.h"
#include "TrackHelper.h"
#include <fstream>

enum Scene
{
	Avoidance = 0,//����ģʽ
	TrafficLight,//��ͨ�Ƽ��Ϊ���
	StopLine,//ǰ��ֹͣ��
	MultiLine,//�೵��
	Pedestrians,//���е�
	Follow��	//����ǰ����ʻ
};

class DecisionMaker
{
public:
	DecisionMaker();
	~DecisionMaker();
	//return true if make a decision successfully, default return true
	void Execute();
	bool updateTrajectory(std::vector<RoadPoint>& curPath, std::vector<RoadPoint>& prePath, double preFactor, double curFactor);
	void DrawPath(std::vector<std::vector<RoadPoint>>& paths, std::vector<RoadPoint>& bestPath);
	void SendInfo(std::vector<RoadPoint>& localPath, float speed = -1.0);
	std::vector<RoadPoint> BaseFrame2Local(const std::vector<RoadPoint>& baseFrame);//���ο�·��ת��������ڳ������꣬�������󷽵ĵ�ɾ��
	
	bool FollowFrontCar(const double& safe_distance, double& frontDis);//���ݰ�ȫ�������ж��Ƿ����ǰ��
	double SpeedRelativeDistance(double distance);//���ݳ��Ӻ�ǰ���ϰ���ľ�����������Ӧ�ٶ�
	double SafeDistance(double speed);//���ݳ�������������İ�ȫ����

protected:
	void makeDecision();
private:
	std::vector<RoadPoint> m_preRootLocal;//previous path stored in local coordinate system
	std::vector<RoadPoint> m_preRoot;//previous path, stored in global coordinate system
	std::vector<RoadPoint> m_plannedRoot;//trajectory planned by ShortTimePlanner
	std::vector<RoadPoint> m_baseFrame;
	std::vector<RoadPoint> m_baseFrameLocal;//����ڳ�������Ĳο�·��
	double m_safeDistance;//safe distance, if short than this distance, we change the lane, do not follow the car
	double m_disToFirstObstacle;//the distance to the first obstacle collides with the baseFrame
	Scene m_scene;//some common scene
	bool m_isSegmentMode;
	ShortTimePlanner m_pathPlanner;

	PathDraw m_pathDraw;
	TrackHelper m_track;
	double m_recommendSpeed;//recommend speed
	bool m_isUTurn;
	double m_targetSpeed[4];
	bool m_slowFlag;//��־���ڼ���
	double m_accer;//��������Ҫ�ļ��ٶ�
	double m_speedMax;//��ʻ������ٶ� km/hΪ��λ
	std::fstream m_logFile;
};

