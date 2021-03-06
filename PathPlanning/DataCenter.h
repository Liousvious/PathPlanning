#ifndef _PATH_PLANNING_DATA_CENTER_H
#define _PATH_PLANNING_DATA_CENTER_H
#include "BaseType.h"
#include "lcmtype\Location_t.hpp"
#include "lcmtype\StatusBody_t.hpp"
#include "lcmtype\LcmRecvHelper.h"
#include "lcmtype\lasercurb_t.hpp"
#include "lcmtype\VeloGrid_t.hpp"
#include "lcmtype\PlanOutput.hpp"
#include "lcmtype\ckMapStatus_t.hpp"
#include "lcmtype\Map_t.hpp"
#include "lcmtype\Lane_t.hpp"
#include "lcmtype\Scene_t.hpp"
#include <mutex>
#include <condition_variable>
#include <vector>

using ckLcmType::StatusBody_t;
using ckLcmType::Location_t;
using ckLcmType::VeloGrid_t;
using PCLcloudShow::cloudHandler;
using ckLcmType::PlanOutput;
using ckLcmType::ckMapStatus_t;
using ckLcmType::Map_t;
using ckLcmType::Lane_t;
using ckLcmType::Scene_t;

class SXYSpline;
enum CurbDirection{
	LEFT = 0,
	RIGHT
};

class DataCenter
{
private:
	PosPoint m_curPos;//x,y in Gauss; orientation by x
	CarInfo m_curCarInfo;//speed in km/h; steerAngle in deg
	std::vector<RoadPoint> m_referenceTrajectory;//reference trajectory information
	double m_carInitAngle;//get car angle at start point respect to the first point on reference trajectory
	double _qi;//init qi

	LcmRecvHelper<Location_t> m_lcmLocation;
	LcmRecvHelper<StatusBody_t> m_lcmStatusBody;
	LcmRecvHelper<VeloGrid_t> m_lcmVeloGrid;
	LcmRecvHelper<cloudHandler> m_lcmCurb;
	LcmRecvHelper<ckMapStatus_t> m_lcmRefTrajectory;
	LcmRecvHelper<Map_t> m_lcmMultiLane;
	LcmRecvHelper<Lane_t> m_lcmCamLane;
	LcmRecvHelper<Scene_t> m_lcmScene;

	Location_t m_lcmMsgLocation;
	StatusBody_t m_lcmMsgStatusBody;
	VeloGrid_t m_lcmMsgVeloGrid;
	cloudHandler m_lcmMsgCurb;
	ckMapStatus_t m_lcmMsgRefTrajectory;
	Map_t m_lcmMsgMultiLane;
	Lane_t m_lcmMsgCamLane;
	Scene_t m_lcmMsgScene;

	std::mutex m_lockLocation;
	std::mutex m_lockStatusBody;
	std::mutex m_lockVeloGrid;
	std::mutex m_lockCurb;
	std::mutex m_lockRefTrajectory;
	std::mutex m_lockMultiLane;
	std::mutex m_lockCamLane;
	std::mutex m_lockScene;

	std::condition_variable m_waitLocation;
	//std::mutex m_waitLockLocation;
	std::condition_variable m_waitStatusBody;
	//std::mutex m_waitLockStatusBody;
	std::condition_variable m_waitVeloGrid;
	//std::mutex m_waitLockVeloGrid;
	std::condition_variable m_waitCurb;
	std::condition_variable m_waitRefTrajectory;
	std::condition_variable m_waitMultiLane;
	std::condition_variable m_waitCamLane;
	std::condition_variable m_waitScene;

	//std::mutex m_waitLockCurb;
	/*triggered while receiving Location_t */
	void LocationRecvOperation(const Location_t* msg, void*);
	/*triggered while receiving StatusBody */
	void StatusBodyRecvOperation(const StatusBody_t* msg, void*);
	/*triggered while receiving VeloGrid */
	void VeloGridRecvOperation(const VeloGrid_t* msg, void*);
	/*triggered while receiving Cloud */
	void CurbRecvOperation(const cloudHandler* msg, void*);
	/*triggered while receiving Reference Trajectory*/
	void RefTrajectoryRecvOperation(const ckMapStatus_t* msg, void*);

	void MultiLaneRecvOperation(const Map_t* msg, void*);

	void CamLaneRecvOperation(const Lane_t* msg, void*);

	/*receive scene message from map part*/
	void SceneRecvOperation(const Scene_t* msg, void *);

protected:
	DataCenter();
	~DataCenter();
public:
	static DataCenter& GetInstance();
	DataCenter(DataCenter const&) = delete;             
	DataCenter(DataCenter&&) = delete;                  
	DataCenter& operator=(DataCenter const&) = delete;  
	DataCenter& operator=(DataCenter &&) = delete;
	/*start all sensors*/
	void StartAllSensor();
	/*end all sensors*/
	void EndAllSensor();
	/*start Location*/
	void StartLocation();
	/*start StatusBody*/
	void StartStatusBody();
	/*start VeloGrid*/
	void StartVeloGrid();
	/*start cloud*/
	void StartCurb();
	/**start reference trajectory*/
	void StartRefTrajectory();

	void StartMultiLane();

	void StartCamLane();

	void StartScene();


	/*end location*/
	void EndLocation();
	/*end statusboyd*/
	void EndStatusBody();
	/*end velogrid*/
	void EndVeloGrid();
	/*end curb*/
	void EndCurb();
	/*end reference trajectory*/
	void EndRefTrajectory();
	void EndMultiLane();
	void EndCamLane();
	void EndScene();

	/*@return x,y in Gauss, orientation by x*/
	PosPoint GetCurPosition();
	/*@return speed in km/h�� steerAngle in deg*/
	CarInfo GetCarInfo();
	/*return Lidar Data*/
	VeloGrid_t GetLidarData();
	/*return a point on road edge*/
	PosPoint GetRoadEdgePoint(double y, CurbDirection dir);
	/*@return The Coefficient of Road Edge Line*/ 
	PCLcloudShow::pointXYZI GetRoadEdgeCoefficient(CurbDirection dir);
	/*@return The Plan Target Point*/
	PosPoint GetTargetPoint();
	/*@return reference trajectory*/
	std::vector<RoadPoint> GetRefTrajectory();
	std::vector<RoadPoint> GetReferenceTrajectory(RoadPoint &car);
	std::vector<RoadPoint> GetRefTrajectories(); 
	std::vector<std::vector<RoadPoint>> GetMultiLanes(int & laneIndex);
	std::vector<RoadPoint> GetCamLanes();
	PosPoint GetStopLine(double& width);
	PosPoint GetCurOnTrajectory();

	Scene_t GetSceneMessage();

	//get init car angle and qi
	void Get_InitAngle_Qi(SXYSpline* spline,double& angle, double& qi);
	std::vector<RoadPoint> GetRefTrajectory_Qi(double& qi);
	void GetLanes_Qi(int laneIndex, double& qi, std::vector<RoadPoint>& lane);
	/*continue while Location Processing completed*/
	bool WaitForLocation(unsigned int milliseconds);
	/*continue while StatusBody Processing completed*/
	bool WaitForStatusBody(unsigned int milliseconds);
	/*continue while VeloGrid Processing completed*/
	bool WaitForVeloGrid(unsigned int milliseconds);
	/*continue while Curb Processing completed*/
	bool WaitForCurb(unsigned int milliseconds);
	/*continue while RefTrajectory Processing completed*/
	bool WaitForRefTrajectory(unsigned int milliseconds);

	bool WaitForMultiLane(unsigned int milliseconds);

	bool WaitForCamLane(unsigned int milliseconds);

	bool WaitForScene(unsigned int milliseconds);

	/*@return if having Location_t during 500ms*/
	bool HasLocation();
	/*@return if having StatusBody_t during 500ms*/
	bool HasStatusBody();
	/*@return if having VeloGrid_t during 500ms*/
	bool HasVeloGrid();
	/*@return if having curb during 500ms*/
	bool HasCurb();
	/*@return if having RefTrajectory during 500ms*/
	bool HasRefTrajectory();

	bool HasMultiLane();

	bool HasCamLane();

	bool HasSceneMsg();

private:

};

#endif // !_PATH_PLANNING_DATA_CENTER_H


