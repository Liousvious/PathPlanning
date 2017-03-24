#ifndef _PATH_PLANNING_DATA_CENTER_H
#define _PATH_PLANNING_DATA_CENTER_H
#include "BaseType.h"
#include "lcmtype\Location_t.hpp"
#include "lcmtype\StatusBody_t.hpp"
#include "lcmtype\LcmRecvHelper.h"
#include "lcmtype\lasercurb_t.hpp"
#include "lcmtype\VeloGrid_t.hpp"
#include <mutex>
#include <condition_variable>

using ckLcmType::StatusBody_t;
using ckLcmType::Location_t;
using ckLcmType::VeloGrid_t;
using ckLcmType::cloudHandler;

enum CurbDirection{
	LEFT = 0,
	RIGHT
};

class DataCenter
{
private:
	PosPoint m_curPos;//x,y in Gauss; orientation by x
	CarInfo m_curCarInfo;//speed in km/h; steerAngle in deg
	LcmRecvHelper<Location_t> m_lcmLocation;
	LcmRecvHelper<StatusBody_t> m_lcmStatusBody;
	LcmRecvHelper<VeloGrid_t> m_lcmVeloGrid;
	LcmRecvHelper<cloudHandler> m_lcmCurb;
	Location_t m_lcmMsgLocation;
	StatusBody_t m_lcmMsgStatusBody;
	VeloGrid_t m_lcmMsgVeloGrid;
	cloudHandler m_lcmMsgCurb;
	std::mutex m_lockLocation;
	std::mutex m_lockStatusBody;
	std::mutex m_lockVeloGrid;
	std::mutex m_lockCurb;
	std::condition_variable m_waitLocation;
	//std::mutex m_waitLockLocation;
	std::condition_variable m_waitStatusBody;
	//std::mutex m_waitLockStatusBody;
	std::condition_variable m_waitVeloGrid;
	//std::mutex m_waitLockVeloGrid;
	std::condition_variable m_waitCurb;
	//std::mutex m_waitLockCurb;
	/*triggered while receiving Location_t */
	void LocationRecvOperation(const Location_t* msg, void*);
	/*triggered while receiving StatusBody */
	void StatusBodyRecvOperation(const StatusBody_t* msg, void*);
	/*triggered while receiving VeloGrid */
	void VeloGridRecvOperation(const VeloGrid_t* msg, void*);
	/*triggered while receiving Cloud */
	void CurbRecvOperation(const cloudHandler* msg, void*);
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
	/*end location*/
	void EndLocation();
	/*end statusboyd*/
	void EndStatusBody();
	/*end velogrid*/
	void EndVeloGrid();
	/*end curb*/
	void EndCurb();
	/*@return x,y in Gauss, orientation by x*/
	PosPoint GetCurPosition();
	/*@return speed in km/h�� steerAngle in deg*/
	CarInfo GetCarInfo();
	/*return Lidar Data*/
	VeloGrid_t& GetLidarData();
	/*return a point on road edge*/
	PosPoint GetRoadEdgePoint(double y, CurbDirection dir);
	/*@return The Coefficient of Road Edge Line*/ 
	laserCurbs::pointXYZI GetRoadEdgeCoefficient(CurbDirection dir);
	/*@return The Plan Target Point*/
	PosPoint GetTargetPoint();
	/*continue while Location Processing completed*/
	bool WaitForLocation(unsigned int milliseconds);
	/*continue while StatusBody Processing completed*/
	bool WaitForStatusBody(unsigned int milliseconds);
	/*continue while VeloGrid Processing completed*/
	bool WaitForVeloGrid(unsigned int milliseconds);
	/*continue while Curb Processing completed*/
	bool WaitForCurb(unsigned int milliseconds);
	/*@return if having Location_t during 500ms*/
	bool HasLocation();
	/*@return if having StatusBody_t during 500ms*/
	bool HasStatusBody();
	/*@return if having VeloGrid_t during 500ms*/
	bool HasVeloGrid();
	/*@return if having curb during 500ms*/
	bool HasCurb();

private:

};

#endif // !_PATH_PLANNING_DATA_CENTER_H


