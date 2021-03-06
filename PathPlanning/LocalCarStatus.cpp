#include "LocalCarStatus.h"
#include "LoopThread.h"
#include "DataCenter.h"
#include "Topology.h"
class LocalPosThread :public ZBaseLoopThread {
private:
	LocalCarStatus* status;
	std::chrono::high_resolution_clock::time_point startTime;
	std::chrono::high_resolution_clock::time_point endTime;
public:
	LocalPosThread(void * parent)
		:ZBaseLoopThread()
		,status((LocalCarStatus*)parent)
	{

	}
	~LocalPosThread() {

	}

	void Start() {
		startTime = endTime = std::chrono::high_resolution_clock::now();
		ZBaseLoopThread::Start();
	}

	bool RunLoop() {
		return false;
		std::this_thread::sleep_for(std::chrono::microseconds(1000));
		endTime = std::chrono::high_resolution_clock::now();
		PosPoint pos = status->m_curPos;
		double speed = status->m_speed / 3.6;
		double angle = status->m_steerAngle / status->m_steerRatio * PI / 180;
		double seconds = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() / 1000000;
		pos.x += speed*cos(pos.angle)*seconds;
		pos.y += speed*sin(pos.angle)*seconds;
		pos.angle -= tan(angle) * speed / status->L *seconds;
		status->m_curPos = pos;
		startTime = std::chrono::high_resolution_clock::now();
		return true;
	}
};

LocalCarStatus::LocalCarStatus()
{
	thread = new LocalPosThread(this);
}

LocalCarStatus::~LocalCarStatus()
{
	thread->Stop();
	delete thread;
	thread = nullptr;
}

LocalCarStatus& LocalCarStatus::GetInstance()
{
	static LocalCarStatus instance;
	return instance;
}

void LocalCarStatus::SetOriginPos(PosPoint pos)
{
	orgPos = pos;
}

void LocalCarStatus::Start()
{
	thread->Start();
}

void LocalCarStatus::Suspend()
{
	thread->Suspend();
}

void LocalCarStatus::Resume()
{
	thread->Resume();
}

void LocalCarStatus::End()
{
	thread->Stop();
}

PosPoint LocalCarStatus::GetPosition()
{
	PosPoint pt = DataCenter::GetInstance().GetCurPosition();
	double dx = pt.x - orgPos.x;
	double dy = pt.y - orgPos.y;
	Topology::Rotate(PI / 2 - orgPos.angle, dx, dy, pt.x, pt.y);
	pt.angle = PI/2+pt.angle - orgPos.angle;
	return pt;
}

double LocalCarStatus::GetSpeed()
{
	return DataCenter::GetInstance().GetCarInfo().speed;
}

double LocalCarStatus::GetSteerAngle()
{
	return DataCenter::GetInstance().GetCarInfo().steerAngle;
}
