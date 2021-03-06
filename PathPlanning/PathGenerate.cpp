#include "PathGenerate.h"
#include <assert.h>
#include <math.h>
#include <fstream>
#include "lcmtype/PlanOutput.hpp"
#include "PathJoint.h"
#include "TrackFinder.h"
#include <chrono>
#include <algorithm>
#include <numeric>

using ckLcmType::PlanOutput;

#define LOG_CLOTHOID
bool PathGenerate::path_generate_grid(PosPoint startPt, PosPoint endPt, ckLcmType::VeloGrid_t& veloGrids, std::vector<RoadPoint>& rdPt) {

	//VeloGrid_t veloGrids=DataCenter::GetInstance().GetLidarData();

	// 
	int * grid_map_start = new int[MAP_HEIGHT];
	int * grid_map_end = new int[MAP_HEIGHT];
	int x_start = 0;
	int y_start = 0;
	int x_end = 0;
	int y_end = 0;
	// TRANSFORM TO GRID COORDINATE
	CoordTransform::LocaltoGrid(startPt, x_start, y_start);
	CoordTransform::LocaltoGrid(endPt, x_end, y_end);

	// create clothoid curve
	Clothoid path_clothoid(x_start, y_start, startPt.angle, x_end, y_end, endPt.angle);

	// get roadPoints
	int num_pt = 100;
	rdPt.resize(num_pt);// = new RoadPoint[num_pt];
	path_clothoid.PointsOnClothoid(rdPt, num_pt);

	//
	
	for (int i = 0; i < num_pt; i++)
	{
		// may be a bug can be saver
		/*for (int j = -4; j <= 4;j++)
		{
			if (((int)rdPt[i].x + j) >= 0 && ((int)rdPt[i].x + j) <= MAP_WIDTH -1)
				grid_map[MAP_WIDTH*(int)rdPt[i].y + (int)rdPt[i].x + j] = 1;
		}*/
		if (((int)rdPt[i].x + CAR_WIDTH) >= 0 && ((int)rdPt[i].x + CAR_WIDTH) <= MAP_WIDTH - 1)
		{
			grid_map_end[(int)rdPt[i].y] =
				(int)rdPt[i].x + CAR_WIDTH > grid_map_end[(int)rdPt[i].y] 
				?
				(int)rdPt[i].x + CAR_WIDTH : grid_map_end[(int)rdPt[i].y];
			
		}
		else {
			grid_map_end[(int)rdPt[i].y] = MAP_WIDTH - 1;
		}
		if (((int)rdPt[i].x - CAR_WIDTH) >= 0 && ((int)rdPt[i].x - CAR_WIDTH) <= MAP_WIDTH - 1)
		{
			grid_map_start[(int)rdPt[i].y] = 
				(int)rdPt[i].x - CAR_WIDTH < grid_map_start[(int)rdPt[i].y]
				?
				(int)rdPt[i].x - CAR_WIDTH : grid_map_start[(int)rdPt[i].y];
		}
		else {
			grid_map_start[(int)rdPt[i].y] = 0;
		}
		
	}

	//
#ifdef TEST
	for (int i = 0; i < num_pt; i++) {

		for (int j = -CAR_WIDTH; j <= CAR_WIDTH; j++)
		{
			if (((int)rdPt[i].x + j) >= 0 && ((int)rdPt[i].x + j) <= MAP_WIDTH - 1)
			{
				index = [MAP_WIDTH*(int)rdPt[i].y + (int)rdPt[i].x + j];
				if (veloGrids.velo_grid[index]) {
					return false
				}
			}

		}
	}
#endif // !TEST

	//int obstacle_size;
	for (size_t i = 0; i < veloGrids.height; i++)
	{
		for (size_t j = 0; j < veloGrids.width; j++)
		{
			int index = i*veloGrids.width + j;
			if (veloGrids.velo_grid[i])
			{
				
				bool flag = j >= grid_map_start[j] && grid_map_end[j] <= j;
				if (flag)
				{
					return false;
				}
			}
		}
	}
	delete[] grid_map_start;
	delete[] grid_map_end;

	return true;
	

}

bool PathGenerate::path_generate_local(PosPoint startPt, PosPoint endPt) {
	//// create clothoid curve
	//Clothoid path_clothoid(startPt.x, startPt.y, startPt.angle, endPt.x, endPt.y, endPt.angle);

	//// get roadPoints
	//int num_pt = 100;
	//RoadPoint *rdPt = new RoadPoint[num_pt];
	//RoadPoint *gridPt = new RoadPoint[num_pt];
	//path_clothoid.PointsOnClothoid(rdPt, num_pt);
	//// change to grid
	//for (auto i = 0; i < num_pt; i++)
	//{
	//	int x = 0;
	//	int y = 0;
	//	PosPoint pt;
	//	pt.x = rdPt[i].x;
	//	pt.y = rdPt[i].y;
	//	pt.angle = rdPt[i].angle;
	//	CoordTransform::LocaltoGrid(pt, x, y);
	//	gridPt[i].x = x;
	//	gridPt[i].y = y;
	//	gridPt[i].angle = rdPt[i].angle;
	//	gridPt[i].changeangle = rdPt[i].changeangle;
	//}
	//// define whether intersected or not

	//// step one get rightest point
	//int index = getRightestPoints(rdPt, num_pt);
	//// step two consider the car width
	//rdPt[index].x += 0.9;   

	//// TODO get the a,b,c
	//laserCurbs::pointXYZI coeff = DataCenter::GetInstance().GetRoadEdgeCoefficient(RIGHT);
	//double a, b, c;
	//a = coeff.x;
	//b = coeff.y;
	//c = coeff.z;
	//double distance = abs(a*rdPt[index].x + b*rdPt[index].y + c) / sqrtf(a*a + b*b);
	//if (distance<0.3)
	//{
	//	return false;
	//}
	//else
	//{
		return true;
	//}


}

void PathGenerate::path_generate() {

	// step one receive data
	if (!DataCenter::GetInstance().HasVeloGrid()) {
		std::cout << "Warning::not velogrid message" << std::endl;
		return;
	}
	if (!DataCenter::GetInstance().HasCurb()) {
		std::cout << "Warning::not curb message" << std::endl;
		return;
	}
	VeloGrid_t veloGrids = DataCenter::GetInstance().GetLidarData();
	// step two get the target points and target direction
	target_X = 75;
	target_Y = 400;
	double target_Angle = DataCenter::GetInstance().GetRoadEdgePoint(target_Y, RIGHT).angle;
	// step three generate the path
	int delta_Grid_start = -5;
	int delta_Grid_end = 20;
	PosPoint startPt, endPt;
	startPt.x = 75;
	startPt.y = 200;
	startPt.angle = 90 / 180.0 * PI;
	bool send_succeed = false;
	for (int i = delta_Grid_start; i < delta_Grid_end; i++)
	{
		endPt.y = target_Y;
		endPt.x = target_X - i;
		endPt.angle = target_Angle;
		//TODO set angle
		std::vector<RoadPoint> rdpt;
		if (path_generate_grid_obstacle(startPt, endPt, veloGrids, rdpt)) {
			std::cout << "congratulations a successful root" << std::endl;
			// TODO: step four send the data
#ifdef LOG_CLOTHOID

			FILE *fp = fopen("clothoid.txt", "a+");
#endif // LOG_CLOTHOID
			ckLcmType::DecisionDraw_t draw;
			draw.num = rdpt.size();
			draw.Path_x.reserve(draw.num);
			draw.Path_y.reserve(draw.num);
			for (RoadPoint& pt : rdpt) {
				double x, y;
				CoordTransform::GridtoLocal(pt.x + X_START, pt.y - Y_START, x, y);
				draw.Path_x.push_back(x);
				draw.Path_y.push_back(y);
				pt.x = x;
				pt.y = y;
				fprintf(fp, "%lf %lf %lf\n", x, y, pt.angle);
			}
#ifdef LOG_CLOTHOID
			fclose(fp);
#endif // 
			m_sendPath.SendDraw(draw);
			track.SetLocalPath(rdpt);
			send_succeed = true;
			break;

		}
	}
	if (!send_succeed)
	{
		std::cout << "no path" << std::endl;
		track.SetLocalPath(std::vector<RoadPoint>());
		//TODO: step five send message about how to stop
		CarInfo info;
		info.speed = 0;
		info.state = E_STOP;
		info.steerAngle = 0;
		info.gear = D;
		CarControl::GetInstance().SendCommand(info);
	}

}

double PathGenerate::getTargetDirection() {
	return 0.0;
}

int PathGenerate::getRightestPoints(RoadPoint *rdPt, int numPt) {

	int maxVal_x = rdPt[0].x;
	int index = 0;
	for (int i = 0; i < numPt; i++)
	{
		if (rdPt[i].x > maxVal_x)
		{
			maxVal_x = rdPt[i].x;
			index = i;
		}
	}
	return index;
}
bool  PathGenerate::path_generate_grid_obstacle(PosPoint startPt, PosPoint endPt, VeloGrid_t& veloGrids, std::vector<RoadPoint>& rdPt, int *y, int *x){
	// 
	int x_start = startPt.x;
	int y_start = startPt.y;
	int x_end = endPt.x;
	int y_end = endPt.y;
	// TRANSFORM TO GRID COORDINATE
	//	CoordTransform::LocaltoGrid(startPt, x_start, y_start);
	//	CoordTransform::LocaltoGrid(endPt, x_end, y_end);

	// create clothoid curve
	Clothoid path_clothoid(x_start, y_start, startPt.angle, x_end, y_end, endPt.angle);

	// get roadPoints
	int num_pt = 100;
	//rdPt.clear();
	rdPt.resize(num_pt);// = new RoadPoint[num_pt];
	path_clothoid.PointsOnClothoid(rdPt, num_pt);
	for (int i = 0; i < num_pt; i++)
	{
		// may be a bug can be saver
		for (int j = -CAR_WIDTH; j <= CAR_WIDTH; j++)
		{
			if ((int)(rdPt[i].x + 0.5 + j) >= 0 && ((int)(rdPt[i].x + 0.5 + j)) <= MAP_WIDTH - 1)
			{
				int index = MAP_WIDTH*(int)(rdPt[i].y + 0.5) + (int)(rdPt[i].x + 0.5) + j;
				if (veloGrids.velo_grid[index]) {
					*y = (int)rdPt[i].y;
					*x = (int)(rdPt[i].x + 0.5) + j;
					return false;
				}
			}
		}


	}
	return true;
}
bool PathGenerate::path_generate_grid_obstacle(PosPoint startPt, PosPoint endPt, VeloGrid_t& veloGrids, std::vector<RoadPoint>& rdPt)
{
	// 
	int x_start = startPt.x;
	int y_start = startPt.y;
	int x_end = endPt.x;
	int y_end = endPt.y;
	// TRANSFORM TO GRID COORDINATE
//	CoordTransform::LocaltoGrid(startPt, x_start, y_start);
//	CoordTransform::LocaltoGrid(endPt, x_end, y_end);

	// create clothoid curve
	Clothoid path_clothoid(x_start, y_start, startPt.angle, x_end, y_end, endPt.angle);

	// get roadPoints
	int num_pt = 100;
	//rdPt.clear();
	rdPt.resize(num_pt);// = new RoadPoint[num_pt];
	path_clothoid.PointsOnClothoid(rdPt, num_pt);
	for (int i = 0; i < num_pt; i++)
	{
		// may be a bug can be saver
		for (int j = -CAR_WIDTH; j <= CAR_WIDTH; j++)
		{
			if ((int)(rdPt[i].x + 0.5 + j) >= 0 && ((int)(rdPt[i].x + 0.5 + j)) <= MAP_WIDTH - 1)
			{
				int index = MAP_WIDTH*(int)(rdPt[i].y + 0.5) + (int)(rdPt[i].x + 0.5) + j;
				if (veloGrids.velo_grid[index]) {
					return false;
				}
			}
		}
	}

	
	return true;
}

void PathGenerate::createClothoidTable(){


	// step one define the start point and its directions
	PosPoint start_pt, endPt;
	start_pt.x = 75;
	start_pt.y = 200;
	start_pt.angle = PI / 2;

	clothoidMap.clear();
	clothoidMap.resize((GRID_END - GRID_START) *(ANGLE_END - ANGLE_START));
	// step two define the end point and create some directions
	target_Y = 250;
	target_X = 75;
	for (int i = GRID_START; i < GRID_END; i++)
	{

		endPt.y = target_Y;
		endPt.x = target_X - i;
		for (int angle = ANGLE_START; angle < ANGLE_END;angle++)
		{
			endPt.angle = angle / 180.0 *PI;
			std::vector<RoadPoint> rdPt;
			generateClothoidPoints(start_pt, endPt, rdPt);
			clothoidMap[(GRID_END - GRID_START)* (angle - ANGLE_START)+ (i - GRID_START)] = rdPt;
		}
		
	}


}
void PathGenerate::generateClothoidPoints(PosPoint startPt, PosPoint endPt, std::vector<RoadPoint>& rdPt){
	
	// create clothoid curve
	Clothoid path_clothoid(startPt.x, startPt.y, startPt.angle, endPt.x, endPt.y, endPt.angle);

	// get roadPoints
	int num_pt = 100;
	rdPt.clear();
	rdPt.resize(num_pt);// = new RoadPoint[num_pt];
	path_clothoid.PointsOnClothoid(rdPt, num_pt);
}
std::vector<RoadPoint> PathGenerate::getRdPtFromTable(int grid, int angle){

	int index = (GRID_END - GRID_START) * (angle - ANGLE_START)+ grid - GRID_START;
	assert(index >= 0 && index <= clothoidMap.size());
	return clothoidMap[index];
}
void PathGenerate::path_generate_using_bug()
{
	// step one receive data
	if (!DataCenter::GetInstance().HasVeloGrid()) {
		std::cout << "Warning::not velogrid message" << std::endl;
		return;
	}
	if (!DataCenter::GetInstance().HasCurb()) {
		std::cout << "Warning::not curb message" << std::endl;
		return;
	}
	VeloGrid_t veloGrids = DataCenter::GetInstance().GetLidarData();
	// step two get the target points and target direction
	target_X = 75;
	target_Y = 250;
	double target_Angle = DataCenter::GetInstance().GetRoadEdgePoint(target_Y, RIGHT).angle;
	// step three generate the path
	int delta_Grid_start = -5;
	int delta_Grid_end = 20;
	PosPoint startPt, endPt;
	startPt.x = 75;
	startPt.y = 200;
	startPt.angle = 90 / 180.0 * PI;
	std::vector<std::vector<RoadPoint>> success_root;
	for (int i = GRID_START; i < GRID_END; i++)
	{

		endPt.y = target_Y;
		endPt.x = target_X - i;
		for (int angle = ANGLE_START; angle < ANGLE_END; angle++)
		{
			
			std::vector<RoadPoint> rdPt;
			rdPt = getRdPtFromTable(i, angle);
			if (Topology::check_velogrid_rdPt_intersected(veloGrids, rdPt))
			{
				success_root.push_back(rdPt);
			}
			
		}
	}

	//store and compare the difference of the road
	if (pre_Root.size() == 0)
	{
		if (success_root.size() == 0)
		{
			std::cout << "no path" << std::endl;
			track.SetLocalPath(std::vector<RoadPoint>());
			//TODO: step five send message about how to stop
			CarInfo info;
			info.speed = 0;
			info.state = E_STOP;
			info.steerAngle = 0;
			info.gear = D;
			CarControl::GetInstance().SendCommand(info);
			return;
		}
		else
		{
			// random choose first one
			// or you can give a direction 
			// then i can compute the cost
			// then choose a better one
			// random choose may cause a problem
			pre_Root = success_root[0];
		}
		
	}
	else
	{
		if (success_root.size() == 0 )
		{
			// choice using the pre the go 
			// or stop
			std::cout << "no path" << std::endl;
			track.SetLocalPath(std::vector<RoadPoint>());
			//TODO: step five send message about how to stop
			CarInfo info;
			info.speed = 0;
			info.state = E_STOP;
			info.steerAngle = 0;
			info.gear = D;
			CarControl::GetInstance().SendCommand(info);
		}
		else{
			int simi_max = -1000;
			std::vector<RoadPoint> path=success_root[0];
			for (auto root : success_root)
			{
				double simi = similarity(pre_Root, root);
				if (simi > simi_max){
					simi_max = simi;
					path = root;
				}
			}
			// send the data
			ckLcmType::DecisionDraw_t draw;
			draw.num = path.size();
			draw.Path_x.reserve(draw.num);
			draw.Path_y.reserve(draw.num);
			for (RoadPoint& pt : path) {
				double x, y;
				CoordTransform::GridtoLocal(pt.x + X_START, pt.y - Y_START, x, y);
				draw.Path_x.push_back(x);
				draw.Path_y.push_back(y);
				//fprintf(fp, "%lf %lf %lf\n", x, y, pt.angle);
			}
			pre_Root = path;
			track.SetLocalPath(path);
			m_sendPath.SendDraw(draw);
			
		}
		
	}

}
bool PathGenerate::path_generate_turning(VeloGrid_t& veloGrids){
	// define search region
	// lane width 3 so 6 grid
	int RegionWidth = 0;
	int RegionHeight = 0;
	int RegionStartX = 0;
	int RegionStartY = 0;

	PosPoint endPt, startPt;
	startPt.x = 75;
	startPt.y = 200;
	startPt.angle = PI / 2.;


	if (PreDirection == TURN_LEFT)
	{
		RegionStartX = 75 - 12;
		RegionStartY = 200 + 12;
		RegionWidth = 4;
		RegionHeight = 4;
		endPt.angle = PI;

	}
	else if (PreDirection == TUN_RIGHT)
	{
		RegionStartX = 75 + 6;
		RegionStartY = 200 + 12;
		RegionWidth = 4;
		RegionHeight = 4;
		endPt.angle = 0;
	}
	else
	{
		return false;
	}

	//
	


	for (int i = 0; i < RegionWidth;i++)
	{
		for (int j = 0; j < RegionHeight;j++)
		{
			endPt.x = RegionStartX + i;
			endPt.y = RegionStartY + j;
			std::vector<RoadPoint> rdpt;
			if (path_generate_grid_obstacle(startPt, endPt, veloGrids, rdpt)) {
				std::cout << "congratulations a successful root" << std::endl;

				// pass the data
			}

		}
	}
	
}
bool PathGenerate::path_generate_for_fun(){
	// step one receive data
	if (!DataCenter::GetInstance().HasVeloGrid()) {
		std::cout << "Warning::not velogrid message" << std::endl;
		return false;
	}
	if (!DataCenter::GetInstance().HasCurb()) {
		std::cout << "Warning::not curb message" << std::endl;
		return false;
	}
	VeloGrid_t veloGrids = DataCenter::GetInstance().GetLidarData();
	// step two get the target points and target direction
	target_X = 75;
	target_Y = 300;
	double target_Angle = DataCenter::GetInstance().GetRoadEdgePoint(target_Y, RIGHT).angle;

	//
	std::vector<PosPoint> obstacles;
	PosPoint startPt;
	PosPoint endPt;

	startPt.x = 75;
	startPt.y = 200;
	startPt.angle = 90 / 180.0 * PI;
	endPt.angle = startPt.angle;
	endPt.x = 75;
	endPt.y = 300;
	std::vector<PosPoint> root;
	bool result = path_generate_recursive(startPt, endPt, veloGrids,root, 0);
	if (!result)
	{
		std::cout << "no root "<<std::endl;
		return false;
	}
	if (posPtOnRoot.size() == 0)
	{
		return false;
	}
	else
	{
		ckLcmType::DecisionDraw_t draw;
		std::cout << "we have roads number" << posPtOnRoot.size() << std::endl;
		// how to choose one ?
		int total_num = 0.0;
		for (int i = 0; i < posPtOnRoot[0].size();i++)

		{
			endPt = posPtOnRoot[0][i];
			std::vector<RoadPoint> rdpt;
			if (path_generate_grid_obstacle(startPt, endPt, veloGrids, rdpt)) {
				//std::cout << "congratulations a successful root" << std::endl;

				
				
				for (RoadPoint& pt : rdpt) {
					double x, y;
					CoordTransform::GridtoLocal(pt.x + X_START, pt.y - Y_START, x, y);
					draw.Path_x.push_back(x);
					draw.Path_y.push_back(y);
					pt.x = x;
					pt.y = y;
					total_num += rdpt.size();

				}
			}
			startPt = endPt;
		}
		draw.num = total_num;
		//draw.Path_x.reserve(draw.num);
		//draw.Path_y.reserve(draw.num);
		m_sendPath.SendDraw(draw);
	}


	
}
bool PathGenerate::path_generate_recursive(PosPoint startPt, PosPoint endPt, VeloGrid_t veloGrids, std::vector<PosPoint> &root, int count){

	// first generate a path
	if (count>5)
	{
		return false;
	}
	std::vector<RoadPoint> rdPt;
	int obstacle_y = -1, obstacle_x = -1;

	// search Left
	for (int i = SAFE_REGION_START; i <= SAFE_REGION_END;i++)
	{
		endPt.x = endPt.x - i;
		if (path_generate_grid_obstacle(startPt, endPt, veloGrids, rdPt, &obstacle_y, &obstacle_x))
		{
			root.push_back(endPt);
			if (endPt.y != target_Y)
			{
				PosPoint startPt_tmp = endPt;
				endPt.x = target_X;
				endPt.y = target_Y;
				path_generate_recursive(startPt_tmp, endPt, veloGrids, root, count + 1);
				root.pop_back();
			}
			else
			{
				// reach the target point;
				posPtOnRoot.push_back(root);
			}
			return true;
		}
		else
		{
			endPt.y = obstacle_y;
			endPt.x = obstacle_x;
			root.push_back(endPt);
			path_generate_recursive(startPt, endPt, veloGrids, root, count+1);
			root.pop_back();
		}
	}

}
//bool PathGenerate
//void PathGenerate::motion_model(){
//
//	double theta, x, y, kappa, vt;
//	double deltaT = 0.1;
//	get_current_state();
//	double x_deltaT = x + vt * cos(theta) * deltaT;
//	double y_deltaT = y + vt * sin(theta) * deltaT;
//	double theta_deltaT = theta + vt*kappa*deltaT;
//
//}
double PathGenerate::dynamic_response(double k_next, double k, double v_next, double v, double t, 
	double &k_response, double & v_response){


	double kmax = 0.1900;
	double kmin = -kmax;
	double delta_k_max = 0.1021;
	double delta_k_min = -delta_k_max;
	double delta_v_max = 2.000;
	double delta_v_min = -6.000;

	double delta_k = (k_next - k) / t;
	delta_k = fmin(delta_k, delta_k_max);
	delta_k = fmax(delta_k, delta_k_min);

	k_response = k + delta_k * t;
	k_response = fmin(k_response, kmax);
	k_response = fmax(k_response, kmin);

	speed_logic_control(k_next, v_next, v, v_response);
	double delta_v = (v_response - v) / t;
	delta_v = fmin(delta_v, delta_v_max);
	delta_v = fmax(delta_v, delta_v_min);
	v_response = v + delta_v * t;

	return true;

}


bool PathGenerate::speed_logic_control(double k_next, double v_next, double v_pre, double &v_response){
	double v = v_next;
	double a = 0.1681;
	double b = -0.0049;
	double safefactor = 1.0;
	double vscl = 4.00;
	double k_max = 0.1900;
	double kv_max = 0.1485;
	double v_max = fmax(vscl, (k_next - a) / b);
	double k_max_scl = fmin(k_max, a + b * v);
	if (k_next > k_max_scl)
		v = abs(safefactor * v_max);
	v_response = v * v_pre / abs(v_pre);
	//value = [v_response];
	return true;
}

void PathGenerate::gps_tracking()
{
	if (!DataCenter::GetInstance().HasRefTrajectory()) {
		std::cout << "Warning::not ref message" << std::endl;
		return;
	}
	PosPoint curPos = DataCenter::GetInstance().GetCurPosition();
	std::vector<RoadPoint> path = DataCenter::GetInstance().GetRefTrajectories();
	for (RoadPoint& rpt : path) {
		CoordTransform::WorldToLocal(curPos, rpt, &rpt);
	}
	track.SetLocalPath(path);
	DecisionDraw_t draw;
	draw.num = path.size();
	for (int i = 0; i < path.size(); i++)
	{
		draw.Path_x.push_back(path[i].x);
		draw.Path_y.push_back(path[i].y);
	}
	m_sendPath.SendDraw(draw);
}

int PathGenerate::CheckCollision_2(VeloGrid_t & grids, std::vector<RoadPoint>& localPath, bool hasAngle)
{//未考虑车屁股
	const double gridL = Grid;
	const int gridW = MAP_WIDTH;
	const int gridH = MAP_HEIGHT;
	const double carW = 2.6;//2.8
	const double carH = 4.5;
	if (!hasAngle) {
		for (int i = 0; i + 1 < localPath.size(); i++) {
			localPath[i].angle = atan2((localPath[i + 1].y - localPath[i].y), (localPath[i + 1].x - localPath[i].x));
		}
		localPath.rbegin()->angle = (localPath.rbegin() + 1)->angle;
	}
	for (int ptIndex = 0; ptIndex < localPath.size();ptIndex++) {
		RoadPoint rpt = localPath[ptIndex];
		//{
		//	PointPt dPt;
		//	dPt.x = carH / 2 * cos(rpt.angle);
		//	dPt.y = carH / 2 * sin(rpt.angle);
		//	PointPt carCenter;
		//	carCenter.x = (rpt.x + dPt.x) / gridL + gridW / 2;
		//	carCenter.y = (rpt.y + dPt.y) / gridL + gridH / 2;
		//	int indexes[] = { 13,13,13,13,13,12,12,11,11,10,9,7,5 };
		//	//上半圆
		//	for (int i = fmax(0, -carCenter.y); i < fmin(13, gridH - carCenter.y); i++) {
		//		for (int j = fmax(0, carCenter.x - indexes[i]); j < fmin(gridW, carCenter.x + indexes[i]); j++) {
		//			int index = (carCenter.y + i)*gridW + j;
		//			if (grids.velo_grid[index]) {
		//				return ptIndex;
		//			}
		//		}
		//	}
		//	//下半圆
		//	for (int i = fmax(0, carCenter.y - gridH); i < fmin(13, carCenter.y); i++) {
		//		for (int j = fmax(0, carCenter.x - indexes[i]); j < fmin(gridW, carCenter.x + indexes[i]); j++) {
		//			int index = (carCenter.y - i)*gridW + j;
		//			if (grids.velo_grid[index]) {
		//				return ptIndex;
		//			}
		//		}
		//	}
		//}
		//continue;

		//double disss = rpt.x*rpt.x + rpt.y*rpt.y;
		//disss
		//	= disss > 20 ? 20 : disss;
		double tcarW = carW;// + disss / 20 * 0.6;
		PointPt dLeftFront, dRightFront, dLeftBack, dRightBack;
		dLeftBack.x = -tcarW / 2 * sin(rpt.angle);
		dLeftBack.y = tcarW / 2 * cos(rpt.angle);
		dRightBack.x = -dLeftBack.x;
		dRightBack.y = -dLeftBack.y;
		PointPt dFront;
		dFront.x = carH*cos(rpt.angle);
		dFront.y = carH*sin(rpt.angle);
		dLeftFront.x = dFront.x + dLeftBack.x;
		dLeftFront.y = dFront.y + dLeftBack.y;
		dRightFront.x = dFront.x + dRightBack.x;
		dRightFront.y = dFront.y + dRightBack.y;
		
		PosPoint rb, lb,third0,lf,rf,third1;
		rb.x = rpt.x + dRightBack.x;
		rb.y = rpt.y + dRightBack.y;
		lb.x = rpt.x + dLeftBack.x;
		lb.y = rpt.y + dLeftBack.y;
		lf.x = rpt.x + dLeftFront.x;
		lf.y = rpt.y + dLeftFront.y;
		rf.x = rpt.x + dRightFront.x;
		rf.y = rpt.y + dRightFront.y;
		rb.x = rb.x / gridL + gridW / 2;
		rb.y = rb.y / gridL + gridH / 2;
		lb.x = lb.x / gridL + gridW / 2;
		lb.y = lb.y / gridL + gridH / 2;
		lf.x = lf.x / gridL + gridW / 2;
		lf.y = lf.y / gridL + gridH / 2;
		rf.x = rf.x / gridL + gridW / 2;
		rf.y = rf.y / gridL + gridH / 2;

		PosPoint bigLT,bigRB;
		bigLT.x = fmin(rb.x, fmin(rf.x, fmin(lf.x, lb.x)));
		bigLT.y = fmax(rb.y, fmax(rf.y, fmax(lf.y, lb.y)));
		bigRB.y = fmin(rb.y, fmin(rf.y, fmin(lf.y, lb.y)));
		bigRB.x = fmax(rb.x, fmax(rf.x, fmax(lf.x, lb.x)));
		for (int i = fmax(bigLT.x, 0); i < fmin(bigRB.x, gridW); i++) {
			for (int j = fmax(bigRB.y, 0); j < fmin(bigLT.y, gridH); j++) {
				int index = j*gridW + i;
				if (grids.velo_grid[index]) {

					return ptIndex;
				}
			}
		}
				}
	return -1;
}



int PathGenerate::CheckCollision(VeloGrid_t& grids, std::vector<RoadPoint>& localPath, bool hasAngle /*= false*/)
{
	double gridSize = 0.2;
	double carWidth = 2.8;
	double carLength = 4.5;

	double rear2Back = 0.8;//后轮轴到车屁股的长度

	for (int i = 0; i < localPath.size(); i++)
	{
		RoadPoint pt = localPath[i];
		RoadPoint co;
		bool flag1 = DetectGridObs(pt, grids, co);
		if (flag1 == 1)
			return i;
	}
	return -1;
}

//typedef struct  Car
//{
//	RoadPoint Position;
//	double rearx;
//	double reary;
//	double phi;
//	double frontx;
//	double fronty;
//	double theta;
//	double width;
//	double length;
//	double L;
//	double RtoT;
//};
bool PathGenerate::DetectGridObs(RoadPoint Cur, VeloGrid_t & grids, RoadPoint &collisionpoint)
{
	/*for (int i = 0; i < localPath.size(); i++)
	{*/
	Car myCar;
	myCar.Position = Cur;
	myCar.length = 4.5;
	myCar.width = 2.8;
	myCar.RtoT = 1.5;


	//RoadPoint curPo = DataCenter::GetInstance().GetCurPosition();
	//double dx = myCar.Position.x;//- curPo.x;
	//double dy = myCar.Position.y;//- curPo.y;
	//double angle11 = Topology::AngleNormalnize1(PI / 2.0 - curPo.angle);
	//double cx = dx*cos(angle11) - dy*sin(angle11);
	//double cy = dx*sin(angle11) + dy*cos(angle11);

	//double angle = Topology::AngleNormalnize1(myCar.Position.angle + angle11);

	////double angle=myCar.Position.angle-g_CurrentLocation.angle;

	//myCar.Position.x = cx;
	//myCar.Position.y = cy;
	//myCar.Position.angle = angle;

	myCar.rearx = myCar.Position.x;
	myCar.reary = myCar.Position.y;
	myCar.phi = myCar.Position.angle;

	RoadPoint LeftRear;
	LeftRear.x = myCar.rearx - myCar.RtoT*cos(myCar.phi) - 0.5*myCar.width*sin(myCar.phi);
	LeftRear.y = myCar.reary - myCar.RtoT*sin(myCar.phi) + 0.5*myCar.width*cos(myCar.phi);

	//unsigned int LRx = LeftRear.x / 0.2 + GRID_WidthNum / 2;
	//unsigned int LRy = LeftRear.y / 0.2;

	double grid_widthX = 0.2;
	double grid_widthY = 0.2;
	//转化车所占的所有点到格网，并对其做检查
	for (double i = 0; i<myCar.width; i += grid_widthX)
		for (double j = 0; j<myCar.length; j += grid_widthY)
		{

			RoadPoint point;
			int GRID_WidthNum = 150;
			point.x = i*sin(myCar.phi) + j*cos(myCar.phi) + LeftRear.x;
			point.y = -i*cos(myCar.phi) + j*sin(myCar.phi) + LeftRear.y;
			int grid_X = point.x / grid_widthX + GRID_WidthNum / 2;
			int grid_Y = point.y / grid_widthY + 200;

			if (grid_X <= 0)
			{
				return 0;
			}
			if (grid_Y <= 0)
			{
				return 0;
			}
			if (grid_X >= 150)
			{
				return 0;
			}
			if (grid_Y >= 400)
			{
				return 0;
			}
			int aaa = grids.velo_grid[grid_Y*GRID_WidthNum + grid_X];
			if (aaa>0)
			{
				collisionpoint.x = point.x;
				collisionpoint.y = point.y;
				return 1;//说明有障碍物
			}
		}

	return 0;
}

double CalLineLength(const std::vector<RoadPoint> &line)
{
	if (line.empty())
		return -1;
	double length = 0.0;
	for (int i = 0; i < line.size() - 1; i++)
	{
		RoadPoint cur = line[i];
		RoadPoint next = line[i + 1];
		length += std::sqrt(Topology::Distance2(cur, next));
	}
	return length;
}

double CalculateDisTOGPS(std::vector<RoadPoint>N1, std::vector<RoadPoint>N_GPS)
{
	std::vector<double>DistanceToGPS;//用来存储最后的距离
	std::vector<RoadPoint>::iterator iter;
	double minDIs = 0;

	for (iter = N1.begin(); iter != N1.end(); iter++)
	{
		double temp_Distance = 1000000;
		std::vector<RoadPoint>::iterator iter1;
		//一个点到GPS线上的每一个点求距离
		for (iter1 = N_GPS.begin(); iter1 != N_GPS.end(); iter1++)
		{
			double ljy = Topology::Distance2(*iter, *iter1);/*(iter->x-iter1->x)*(iter->x-iter1->x)
															+(iter->y-iter1->y)*(iter->y-iter1->y);*/
			if (abs(ljy)<temp_Distance)
				temp_Distance = abs(ljy);
		}
		if (temp_Distance > minDIs)
			minDIs = temp_Distance;
		DistanceToGPS.push_back(temp_Distance);//每次存入一个点
	}

	//return sum value
	//return std::accumulate(DistanceToGPS.begin(), DistanceToGPS.end(), 0.0);
	double sum = std::accumulate(DistanceToGPS.begin(), DistanceToGPS.end(), 0.0);

	//return max value
	std::sort(DistanceToGPS.begin(), DistanceToGPS.end());
	return DistanceToGPS.back() / sum;
}

double CalculateAveDisTOGPS(std::vector<RoadPoint>N1, std::vector<RoadPoint>N_GPS)
{
	std::vector<double>DistanceToGPS;//用来存储最后的距离
	std::vector<RoadPoint>::iterator iter;
	double minDIs = 0;

	for (iter = N1.begin(); iter != N1.end(); iter++)
	{
		double temp_Distance = 1000000;
		std::vector<RoadPoint>::iterator iter1;
		//一个点到GPS线上的每一个点求距离
		for (iter1 = N_GPS.begin(); iter1 != N_GPS.end(); iter1++)
		{
			double ljy = Topology::Distance2(*iter, *iter1);/*(iter->x-iter1->x)*(iter->x-iter1->x)
															+(iter->y-iter1->y)*(iter->y-iter1->y);*/
			if (abs(ljy)<temp_Distance)
				temp_Distance = abs(ljy);
		}
		if (temp_Distance > minDIs)
			minDIs = temp_Distance;
		DistanceToGPS.push_back(temp_Distance);//每次存入一个点
	}
	//average
	return std::accumulate(DistanceToGPS.begin(), DistanceToGPS.end(), 0.0) / (DistanceToGPS.size() + 0.00000001);
}

double CalculateFinalDisTOGPS(std::vector<RoadPoint>N1, std::vector<RoadPoint>N_GPS)
{
	std::vector<double> DistanceToGPS;//用来存储最后的距离
	std::vector<double> FinalDis;
	std::vector<RoadPoint>::iterator iter;
	double minDIs = 0;

	int region = N1.size() / 10.0;
	if (region < 3)
		region = 3;
	if (region == N1.size())
		region = 0;
	//std::cout << "Region Size : " << region << std::endl;
	int startpos = N1.size() - region;
	int curpos = 0;
	for (iter = N1.begin(); iter != N1.end(); iter++)
	{
		double temp_Distance = 1000000;
		std::vector<RoadPoint>::iterator iter1;
		//一个点到GPS线上的每一个点求距离
		for (iter1 = N_GPS.begin(); iter1 != N_GPS.end(); iter1++)
		{
			double ljy = Topology::Distance2(*iter, *iter1);
														
			if (abs(ljy)<temp_Distance)
				temp_Distance = abs(ljy);
		}
		if (temp_Distance > minDIs)
			minDIs = temp_Distance;
		DistanceToGPS.push_back(temp_Distance);//每次存入一个点
		if (curpos >= startpos)
			FinalDis.push_back(temp_Distance);
		curpos++;
	}
	//average energy
	//return (std::accumulate(FinalDis.begin(), FinalDis.end(), 0.0) / std::accumulate(DistanceToGPS.begin(), DistanceToGPS.end(), 0.0001)) / (region + 0.00001);

	//std::cout <<"Calculate Final Region Aveage Distance : "<< DistanceToGPS.size() << std::endl;
	return std::accumulate(FinalDis.begin(), FinalDis.end(), 0.0) / (region + 0.00000001);
}


std::vector<RoadPoint> PathGenerate::SelectTra(std::vector<std::vector<RoadPoint>>& paths, std::vector<RoadPoint>& prePath, std::vector<RoadPoint>& refPath, double& min_maxDis)
{
	RoadPoint curPt = DataCenter::GetInstance().GetCurPosition();
	//init last path in this relative coordinate system
	//std::vector<RoadPoint> pre_r_cur(prePath.begin(), prePath.end());
	std::vector<RoadPoint> path_tmp;
	int collision;
	if (!prePath.empty())
	{
		VeloGrid_t velol = DataCenter::GetInstance().GetLidarData();
		collision = CheckCollision(velol, prePath, true);
		
		if (collision == -1 && m_isSegmentMode)
		{
			std::cout << "与上一条路径比较！！！" << std::endl;
			path_tmp = prePath;
		}
		else
		{
			path_tmp = refPath;
		}
	}
	else
	{
		path_tmp = refPath;
	}
	//select best trajectory in this term
	std::vector<double > disIndex;
	for (int i = 0; i < paths.size(); i++)
	{
		double DisI = CalculateDisTOGPS(paths[i], path_tmp);
		disIndex.push_back(DisI);
	}
	double aaa = DBL_MIN;
	int index = 0;
	for (int u = 0; u < disIndex.size(); u++)
	{
		if (aaa  < disIndex[u])
		{
			aaa = disIndex[u];
			index = u;
		}
	}
	min_maxDis = CalculateAveDisTOGPS(paths[index], path_tmp);
	return paths[index];
}

bool  PathGenerate::UpdateOrNot(std::vector<RoadPoint>& curPath, std::vector<RoadPoint>& prePath, std::vector<RoadPoint>& refPath)
{
	//update or not
	VeloGrid_t velol = DataCenter::GetInstance().GetLidarData();
	int collision = CheckCollision(velol, prePath, true);
	double pre_length = CalLineLength(prePath);
	double cur_length = CalLineLength(curPath);
	//last path empty or has collison or in segment mode or too short
	if (prePath.empty() || collision != -1 || pre_length <= 0.6*cur_length || m_isSegmentMode || pre_length < 25)
	{
		return true;
	}
	else if (!prePath.empty())
	{
		double pre_factor = CalculateFinalDisTOGPS(prePath, refPath);
		double ths_factor = CalculateFinalDisTOGPS(curPath, refPath);
		std::cout << "last path distance to ref path : " << pre_factor << std::endl;
		std::cout << "this path distance to ref path : " << ths_factor << std::endl;

		if (pre_factor > ths_factor)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return true;
	}
}


std::vector<RoadPoint> PathGenerate::BestPathSelector(const std::vector<std::vector<RoadPoint>> cadidatePath)
{
	return std::vector<RoadPoint>();
}

int PathGenerate::short_time_planning(float qf, float qi, float theta, double sf,
	VeloGrid_t veloGrids, SXYSpline spline, std::vector<RoadPoint> & pts, PosPoint curPt,
	double& firstObstacle_x, double& firstObstacle_y, int& index, double& new_sf) {
	//std::vector<float> s(x_ref.size(), 0);
	//std::vector<float> delta_x(x_ref.size(), 0);
	//std::vector<float> delta_y(x_ref.size(), 0);
	//std::vector<float> length(x_ref.size(), 0);
	//std::vector<PointPt> pt;

	// need vector operator?
	/*for (int i = 1; i < x_ref.size();i++)
	{
		s[i] = sqrt((x_ref[i] - x_ref[i - 1])*(x_ref[i] - x_ref[i - 1])+
			(y_ref[i] - y_ref[i - 1])*(y_ref[i] - y_ref[i - 1])) + s[i - 1];
		delta_x[i] = x_ref[i] - x_ref[i - 1];
		delta_y[i] = y_ref[i] - y_ref[i - 1];
		length[i] = sqrt(delta_x[i] * delta_x[i] + delta_y[i] * delta_y[i]);

	}*/
	//??
	float c = tan(theta);
	
	float a = 2 * (qi - qf) / (pow(sf, 3)) + c / pow(sf, 2);
	float b = 3 * (qf - qi) / (pow(sf, 2)) - 2 * c / sf;

	/*bool firstObstacle = false;
	double firstObstacle_x = 0.0;
	double firstObstacle_y = 0.0;*/

	bool firstObstacle = true;

	for (int i = 0; i < 50;i++)
	{
		double delta_s = sf / 50 * i;
		float q = a * pow(delta_s, 3) + b * pow(delta_s, 2) + c * delta_s + qi;
		double x=0.0, y=0.0;
		spline.getXY(delta_s, x, y);
		double _delta_x_, _delta_y_;
		spline.getDeriveXY(delta_s, _delta_x_, _delta_y_);
		//if (i == 0)
		//	std::cout << "delta_X: " << _delta_x_ << " delta_Y: " << _delta_y_ << std::endl;
		double _length_ = sqrt(pow(_delta_x_, 2) + pow(_delta_y_, 2));
		Eigen::Matrix2Xd norm_vec(2, 1), pre(2, 1);
		norm_vec(0, 0) = _delta_x_ / _length_;
		norm_vec(1, 0) = _delta_y_ / _length_;
		pre(0, 0) = x;
		pre(1, 0) = y;
		Eigen::Matrix2Xd result = Topology::rotate(PI / 2, norm_vec)*q + pre;
		RoadPoint tmp;
		tmp.x = result(0, 0);
		tmp.y = result(1, 0);

		tmp.angle = Topology::toAngle(_delta_x_, _delta_y_);

		int Grid_x = tmp.x / Grid + 75;
		int Grid_y = tmp.y / Grid + 200;



		////collision detection
		//for (int j = -CAR_WIDTH; j <= CAR_WIDTH; j++)
		//{
		//	//for (int jj = 0; jj < 2 * CAR_HEIGHT;jj++)
		//	{
		//		if ((int)(Grid_x + j) >= 0 && ((int)(Grid_x + j)) <= MAP_WIDTH - 1 && (int)(Grid_y + i) <= MAP_HEIGHT - 1)
		//		{
		//			int index = MAP_WIDTH*(int)(Grid_y + i)+(int)(Grid_x)+j;
		//			if (veloGrids.velo_grid[index]) {
		//				//record first obstacle
		//				if (firstObstacle)
		//				{
		//					firstObstacle_x = ((int)(Grid_x + j) - 75) * Grid;
		//					firstObstacle_y = ((int)(Grid_y + i + 2) - 200) * Grid;
		//					//std::cout << "collision:\t" << firstObstacle_x << "\t" << firstObstacle_y << std::endl;
		//					index = i;
		//		 			new_sf = delta_s;
		//				}
		//				return false;
		//			}
		//		}
		//	}
		//	
		//}
		///*if (i != 0){
		//	tmp.x -= pts[0].x;
		//	tmp.y -= pts[0].y;
		//}*/
		
		pts.push_back(tmp);
	}
	PosPoint pp;
	return CheckCollision(veloGrids, pts);
	//pts[0].x = 0;
	//pts[0].y = 0;
	//return true;
	/*for (int i = 1; i < x_ref.size();i++)
	{
		float delta_s = s[i];
		float q = a * pow(delta_s , 3) + b * pow(delta_s ,2) + c *delta_s + qi;
		Eigen::Matrix2Xd norm_vec(1,2), pre(1,2);
		norm_vec(0, 0) = delta_x[i] / length[i];
		norm_vec(0, 1) = delta_y[i] / length[i];
		pre(0, 0) = x_ref[i-1];
		pre(0, 1) = y_ref[i-1];
		Eigen::Matrix2Xd result = Topology::rotate(PI / 2, norm_vec)*q + pre;
		PointPt tmp;
		tmp.x = result(0, 0);
		tmp.y = result(0, 1);
		int Grid_x = tmp.x / Grid;
		int Grid_y = tmp.y / Grid;
		for (int j = -CAR_WIDTH; j <= CAR_WIDTH; j++)
		{
			if ((int)(Grid_x + j) >= 0 && ((int)(Grid_x + j)) <= MAP_WIDTH - 1)
			{
				int index = MAP_WIDTH*(int)(Grid_y) + (int)(Grid_x) + j;
				if (veloGrids.velo_grid[index]) {

					return false;
				}
			}
		}
		pt.push_back(tmp);

	}*/


}
void PathGenerate::short_time_planning() {
	// step one receive data
	if (!DataCenter::GetInstance().HasVeloGrid()) {
		std::cout << "Warning::not velogrid message" << std::endl;
		return;
	}
	//if (!DataCenter::GetInstance().HasCurb()) {
	//	std::cout << "Warning::not curb message" << std::endl;
	//	return ;
	//}
	if (!DataCenter::GetInstance().HasRefTrajectory()) {
		std::cout << "Warning::not ref message" << std::endl;
		return;
	}
	VeloGrid_t veloGrids = DataCenter::GetInstance().GetLidarData();
	PosPoint curCar;
	curCar = DataCenter::GetInstance().GetCurPosition();
	//在相对于车辆的坐标系里规划
	curCar.x = .0;
	curCar.y = .0;
	//std::vector<RoadPoint> refTrajectory = DataCenter::GetInstance().GetReferenceTrajectory(curCar);
	double tmp_qi = 0.0;
	std::vector<RoadPoint> refTrajectory = DataCenter::GetInstance().GetRefTrajectory_Qi(tmp_qi);


	SXYSpline spline;
	spline.init(refTrajectory);

	// get data and process
	// float theta, double sf

	 //get car data
	//car.angle = PI / 2 - DataCenter::GetInstance().GetCurPosition().angle;
	//PosPoint tmp_Pt = DataCenter::GetInstance().GetCurOnTrajectory();
	//car.x = tmp_Pt.x;
	//car.y = tmp_Pt.y;

	//if (Root_On_Gaussian.size() != 0)
	//{
	//	std::vector<RoadPoint> pre_to_now = PathJoint::Gaussian_To_Decare(Root_On_Gaussian, car.angle, car.x, car.y);
	//}
	//qi here is not used
	double qi = 0;

	double theta = PI / 2 - refTrajectory[0].angle;//0;
	//DataCenter::GetInstance().Get_InitAngle_Qi(&spline, theta, qi);

	double sf = spline.S[spline.S.size() - 1];
	//std::cout << "Sf:" << sf << std::endl;
	std::vector<std::vector<RoadPoint>> _root_;
	std::vector<RoadPoint> rdpt;
	std::vector<float> road_qf;
	//生成候选路径
	double firstObstacle_x = 0.0;
	double firstObstacle_y = 0.0;
	int firstObsIndex;
	double new_sf = 0.0;
	for (float i = 0; i < 12; i += 0.2) {
		float qf = i - 6.;
		std::vector<RoadPoint> pts;
		if (short_time_planning(qf, tmp_qi, theta, sf, veloGrids, spline, pts, curCar, firstObstacle_x, firstObstacle_y, firstObsIndex, new_sf))
		{
			_root_.push_back(pts);
			road_qf.push_back(qf);
		}
	}
	if (_root_.size() == 0)
	{
		std::cout << "no root to use " << std::endl;
		return;
	}
	std::vector<RoadPoint> _best_root_;
	if (pre_Root.empty()) {
		float min = 10000;
		float minI = 1000;
		for (int i = 0; i < road_qf.size(); i++) {
			if (fabs(road_qf[i]) < min) {
				min = fabs(road_qf[i]);
				minI = i;
			}
		}
		_best_root_ = _root_[minI];
	}
	else {
		float min = (double)LONG_MAX;
		int minI = 0;
		for (int n = 0; n < _root_.size(); n++) {
			std::vector<RoadPoint>& tmpRoot = _root_[n];
			float distanceSum = 0;
			for (int i = 0; i < tmpRoot.size(); i++) {
				double delta_s = sf / tmpRoot.size() * i;
				RoadPoint pt;
				spline.getXY(delta_s, pt.x, pt.y);
				distanceSum += Topology::Distance2(pt, tmpRoot[i]);
			}
			float distanceSum2 = 0;
			for (int i = 0; i<tmpRoot.size(); i++) {
				distanceSum2 += Topology::Distance2(pre_Root[i], tmpRoot[i]);
			}
			distanceSum2 *=1;
			distanceSum += distanceSum2;
			if (distanceSum < min) {
				min = distanceSum;
				minI = n;
			}
		}
		_best_root_ = _root_[minI];
	}

	pre_Root = _best_root_;
	////选择最优路径
	//std::vector<RoadPoint> _best_root_;
	//if (pre_Root.size()==0)
	//{
	//	pre_Root = _root_[0];
	//	_best_root_ = _root_[0];
	//	int diff_max = 1000000;
	//	_best_root_ = _root_[0];
	//	int qf_index = 0;
	//	double qf_max = 100000;
	//	int index = 0;
	//	RoadPoint lastPt = *refTrajectory.rbegin();
	//	lastPt.x -= curCar.x;
	//	lastPt.y -= curCar.y;
	//	for (auto root : _root_){
	//		double distance = Topology::Distance2(root[root.size() - 1], lastPt);// refTrajectory[refTrajectory.size() - 1]);
	//		//distance = abs(root[root.size() - 1].x - refTrajectory[refTrajectory.size() - 1].x);
	//		if (distance < diff_max)
	//		{
	//			diff_max = distance;
	//			_best_root_ = root;
	//		}
	//		/*if (abs(road_qf[index])<qf_max)
	//		{
	//			qf_max = abs(road_qf[index]);
	//			qf_index = index;
	//		}
	//		index++;*/
	//	}
	//	//_best_root_ = _root_[qf_index];
	//}
	//else
	//{
	//	int diff_max = 1000000;
	//	_best_root_ = _root_[0];
	//	int qf_index=0;
	//	double qf_max = 100000;
	//	int index = 0;
	//	RoadPoint lastPt = *refTrajectory.rbegin();
	//	lastPt.x -= curCar.x;
	//	lastPt.y -= curCar.y;
	//	for (auto root : _root_)
	//	{
	//		double _diff_ = similarity(pre_Root, root, sf);
	//		//_diff_ *= (abs(road_qf[index]) + 5) / 10;
	//		double distance = Topology::Distance2(root[root.size() - 1], lastPt);// refTrajectory[refTrajectory.size() - 1]);
	//		//distance = abs(root[root.size() - 1].x - refTrajectory[refTrajectory.size() - 1].x);
	//		if (distance < diff_max)
	//		{
	//			diff_max = distance;
	//			_best_root_ = root;
	//		}
	//		
	//	}
	//	pre_Root = _best_root_;
	//	/*_best_root_ = _root_[qf_index];
	//	std::cout << "qf max:" << qf_max << std::endl;*/
	//	Root_On_Gaussian = PathJoint::Decare_To_Gaussian(_best_root_, car.angle, car.x, car.y);
	//	// send the data
	//	//ckLcmType::DecisionDraw_t draw;
	//	
	//}
	//long long tp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
	//std::ofstream of("plan_" + std::to_string(tp) + ".txt");
	//_best_root_ = _root_[];
	ckLcmType::DecisionDraw_t draw;
	//draw.num = _best_root_.size();
	//draw.Path_x.reserve(draw.num);
	//draw.Path_y.reserve(draw.num);
	//for (RoadPoint& pt : _best_root_) {
	//	//double x, y;
	//	//CoordTransform::GridtoLocal(pt.x + X_START, pt.y - Y_START, x, y);
	//	draw.Path_x.push_back(pt.x);
	//	draw.Path_y.push_back(pt.y);
	//	//of << pt.x << "\t" << pt.y << "\n";
	//	//fprintf(fp, "%lf %lf %lf\n", x, y, pt.angle);
	//}
	track.SetLocalPath(_best_root_);

	//draw all lines
		int total_num = 0;
	for (int i = 0; i < _root_.size(); i++)
	{
		for (int j = 0; j < _root_[i].size(); j++)
		{
			double x = _root_[i][j].x;
			double y = _root_[i][j].y;
			draw.Path_x.push_back(x);
			draw.Path_y.push_back(y);
			if (i == 0)
			{
				RoadPoint pt_for_track;
				pt_for_track.x = x;
				pt_for_track.y = y;
				pt_for_track.angle = 0;
				rdpt.push_back(pt_for_track);
			}
		}
		total_num += _root_[i].size();
	}
	//track.SetLocalPath(rdpt);
	std::cout << "root number" << _root_.size() << std::endl;
	draw.num = total_num;
	
	//std::ofstream ff("ref_" + std::to_string(tp) + ".txt");
	int total_refnum = refTrajectory.size();
	draw.refnum = total_refnum + _best_root_.size();
	draw.Refer_x.reserve(total_refnum);
	draw.Refer_y.reserve(total_refnum);
	for (int i = 0; i < total_refnum; i++)
	{
		draw.Refer_x.push_back(refTrajectory[i].x - curCar.x);
		draw.Refer_y.push_back(refTrajectory[i].y - curCar.y);
		//ff << refTrajectory[i].x << "\t" << refTrajectory[i].y << "\n";
	}
	for (RoadPoint& pt : _best_root_) {
		//double x, y;
		//CoordTransform::GridtoLocal(pt.x + X_START, pt.y - Y_START, x, y);
		draw.Refer_x.push_back(pt.x);
		draw.Refer_y.push_back(pt.y);
		//of << pt.x << "\t" << pt.y << "\n";
		//fprintf(fp, "%lf %lf %lf\n", x, y, pt.angle);
	}
	//of.close();
	//ff.close();
	m_sendPath.SendDraw(draw);

}

void PathGenerate::short_time_segment()
{
	//receive data from sensor
	if (!DataCenter::GetInstance().HasVeloGrid()) {
		std::cout << "Warning::not velogrid message" << std::endl;
		return;
	}
	if (!DataCenter::GetInstance().HasRefTrajectory()) {
		std::cout << "Warning::not ref message" << std::endl;
		return;
	}
	VeloGrid_t veloGrids = DataCenter::GetInstance().GetLidarData();
	PosPoint curPos = DataCenter::GetInstance().GetCurPosition();
	double qi = 0.0;
	std::vector<RoadPoint> refTrajectory = DataCenter::GetInstance().GetRefTrajectory_Qi(qi);
	SXYSpline spline;
	spline.init(refTrajectory);
	double theta = PI / 2 - refTrajectory[0].angle;
	double sf = *spline.S.rbegin();
	std::vector<std::vector<RoadPoint>> cadidatePath;
	std::vector<RoadPoint> rdpt;
	std::vector<float> road_qf;
	do {
	//generate candidate path
	for (float i = 0; i < 12; i += 0.2) {
			float qf = i - 6;
		std::vector<RoadPoint> pts;
			pts = trajectory_build(qf, qi, theta, sf, spline);
			if (CheckCollision(veloGrids, pts, false) < 0) {
			cadidatePath.push_back(pts);
			road_qf.push_back(qf);
		}
	}
		if (!cadidatePath.empty()) break;
		std::vector<RoadPoint> searchRoot;
		if (pre_Root.empty()) {
			searchRoot = trajectory_build(0, qi, theta, sf, spline);
			}
		else {
			for (RoadPoint& rpt : pre_Root) {
				RoadPoint srpt;
				CoordTransform::WorldToLocal(curPos, rpt, &srpt);
				searchRoot.push_back(srpt);
			}
		}
		qi = CoordTransform::TrimLocalPathToCurPt(searchRoot);
		spline.init(searchRoot);
		theta = PI / 2 - searchRoot[0].angle;
		sf = *spline.S.rbegin();
				std::vector<RoadPoint> pts;
		pts = trajectory_build(0, qi, theta, sf, spline);
		int findOb = CheckCollision(veloGrids, pts, false);
		//if (findOb < 0) {
		//	cadidatePath.push_back(pts);
		//	road_qf.push_back(0);
		//	break;
		//}
		double tmpSf =findOb<0?*spline.S.rbegin():spline.S[findOb];
		if (tmpSf < 4) {
			break;
				}
		tmpSf += 4;
		if (sf == tmpSf) break;
		sf = fmin(tmpSf, sf);
	} while (cadidatePath.empty());

	////if we can not find a path, we replace the goal point with first obstacle(a little further)
	////and do the replanning work
	//if (cadidatePath.size() == 0)
	//{
	//	segSf = sf;
	//	std::vector<RoadPoint> searchRoot;
	//	//if (pre_Root.empty()){
	//		searchRoot = trajectory_build(0, qi, theta, sf, spline);
	//	//}
	//	//else{
	//	//	searchRoot = pre_Root;
	//	//}
	//	PosPoint ob;
	//	int findOb = CheckCollision(veloGrids, searchRoot);
	//	if (findOb >= 1){
	//		if (pre_Root.empty()){
	//			segSf = (findOb + 2) < spline.S.size() ? spline.S[findOb + 2] : *spline.S.rbegin();
	//		}
	//		else{
	//			segSf = segSf*(findOb + 2) / pre_Root.size();
	//		}
	//		for (float i = 0; i < 12; i += 0.2) {
	//			float qf = i - 6.0;
	//			std::vector<RoadPoint> pts = trajectory_build(qf, qi, theta, segSf, spline);
	//			if (CheckCollision(veloGrids,pts,true) < 0)
	//			{
	//				cadidatePath.push_back(pts);
	//				road_qf.push_back(qf);
	//			}
	//		}
	//	}
	//	else if(findOb<0){
	//		cadidatePath.push_back(searchRoot);
	//		road_qf.push_back(0);
	//	}
	//}
	if (cadidatePath.empty())
	{
		std::cout << "No validate path found" << std::endl;
		return;
	}

	std::vector<RoadPoint> _best_root_;
	if (pre_Root.empty()) {
		float min = 10000;
		float minI = 1000;
		for (int i = 0; i < road_qf.size(); i++) {
			if (fabs(road_qf[i]) < min) {
				min = fabs(road_qf[i]);
				minI = i;
			}
		}
		_best_root_ = cadidatePath[minI];
	}
	else {
		float min = (double)LONG_MAX;
		int minI = 0;
		std::vector<RoadPoint> ppp;
		for (RoadPoint& rpt : pre_Root) {
			RoadPoint srpt;
			CoordTransform::WorldToLocal(curPos, rpt, &srpt);
			ppp.push_back(srpt);
		}
		for (int n = 0; n < cadidatePath.size(); n++) {
			std::vector<RoadPoint>& tmpRoot = cadidatePath[n];
			float distanceSum = 0;
			for (int i = 0; i < tmpRoot.size(); i++) {
				double delta_s = sf / tmpRoot.size() * i;
				RoadPoint pt;
				spline.getXY(delta_s, pt.x, pt.y);
				distanceSum += Topology::Distance2(pt, tmpRoot[i]);
			}
			float distanceSum2 = 0;
			for (int i = 0; i < tmpRoot.size(); i++) {
				distanceSum2 += Topology::Distance2(ppp[i], tmpRoot[i]);
			}
			distanceSum2 *= 1;
			distanceSum += distanceSum2;
			if (distanceSum < min) {
				min = distanceSum;
				minI = n;
			}
		}
		_best_root_ = cadidatePath[minI];
	}
	pre_Root.clear();
	pre_Root.resize(_best_root_.size());
	for (int i = 0; i < _best_root_.size(); i++) {
		CoordTransform::LocalToWorld(curPos, _best_root_[i],&pre_Root[i] );
	}
	//pre_Root = _best_root_;

	ckLcmType::DecisionDraw_t draw;
	track.SetLocalPath(_best_root_);

	//draw all lines
	int total_num = 0;
	for (int i = 0; i < cadidatePath.size(); i++)
	{
		for (int j = 0; j < cadidatePath[i].size(); j++)
		{
			double x = cadidatePath[i][j].x;
			double y = cadidatePath[i][j].y;
			draw.Path_x.push_back(x);
			draw.Path_y.push_back(y);
			if (i == 0)
			{
				RoadPoint pt_for_track;
				pt_for_track.x = x;
				pt_for_track.y = y;
				pt_for_track.angle = 0;
				rdpt.push_back(pt_for_track);
			}
		}
		total_num += cadidatePath[i].size();
	}
	std::cout << "root number" << cadidatePath.size() << std::endl;
	draw.num = total_num;

	int total_refnum = refTrajectory.size();
	draw.refnum = total_refnum + _best_root_.size();
	draw.Refer_x.reserve(total_refnum);
	draw.Refer_y.reserve(total_refnum);
	for (int i = 0; i < total_refnum; i++)
	{
		draw.Refer_x.push_back(refTrajectory[i].x);
		draw.Refer_y.push_back(refTrajectory[i].y);
	}
	for (RoadPoint& pt : _best_root_) {
		draw.Refer_x.push_back(pt.x);
		draw.Refer_y.push_back(pt.y);
	}
	m_sendPath.SendDraw(draw);
}

bool PathGenerate::cmu_planning(std::vector<double> k, double vt, double sf, 
	double theta, double x_start, double y_start, double delta_t){
	std::vector<double> x(k.size(), 0);
	std::vector<double> y(k.size(), 0);
	double k_response;
	double v_response;
	for (int i = 1; i < k.size();i++)
	{
		x[i] = x[i - 1] + vt * cos(theta)*delta_t;
		y[i] = y[i - 1] + vt * sin(theta)*delta_t;
		double k_tmp;
		if (i == 1)
			k_tmp = k[i - 1];
		else
			k_tmp = k_response;
		theta = theta + vt * k_tmp * delta_t;
		//s = sqrt((x(i + 1) - x(i)). ^ 2 + (y(i + 1) - y(i)). ^ 2) + s;
		double k_next = k[i];
		dynamic_response(k_next, k_tmp, vt, vt, delta_t, k_response, v_response);
		//k_response = value(1);
	}
	return true;
}

void PathGenerate::short_time_several_lanes(){
	if (!DataCenter::GetInstance().HasVeloGrid()) {
		std::cout << "Warning::not velogrid message" << std::endl;
		return;
	}
	if (!DataCenter::GetInstance().HasMultiLane()){
		std::cout << "Warning::not lanes message" << std::endl;
		return;
	}
	VeloGrid_t veloGrids = DataCenter::GetInstance().GetLidarData();
	PosPoint curPos = DataCenter::GetInstance().GetCurPosition();
	{
		double width = 0;
		PosPoint pt = DataCenter::GetInstance().GetStopLine(width);
		if (width != 0){
			std::cout << "on stopline" << std::endl;
			double height = 4;
			CoordTransform::WorldToLocal(curPos, pt, &pt);
			PosPoint LT, RB, LB, RT;
			double dxb = sin(pt.angle)*width;
			double dyb = cos(pt.angle)*width;
			LB.x = pt.x - dxb / 2;
			LB.y = pt.y + dyb / 2;
			RB.x = pt.x + dxb / 2;
			RB.y = pt.y - dyb / 2;
			double dxt = cos(pt.angle)*height;
			double dyt = sin(pt.angle)*height;
			LT.x = LB.x + dxt;
			LT.y = LB.y + dyt;
			RT.x = RB.x + dxt;
			RT.y = RB.y + dyt;
			PosPoint bigLT, bigRB;
			bigLT.x = fmin(LB.x, fmin(LT.x, fmin(RB.x, RT.x)));
			bigLT.y = fmax(LB.y, fmax(LT.y, fmax(RB.y, RT.y)));
			bigRB.x = fmax(LB.x, fmax(LT.x, fmax(RB.x, RT.x)));
			bigRB.y = fmin(LB.y, fmin(LT.y, fmin(RB.y, RT.y)));
			bigLT.x = bigLT.x / Grid + MAP_WIDTH / 2;
			bigLT.y = bigLT.y / Grid + MAP_HEIGHT / 2;
			bigRB.x = bigRB.x / Grid + MAP_WIDTH / 2;
			bigRB.y = bigRB.y / Grid + MAP_HEIGHT / 2;

			for (int i = bigLT.x; i < bigRB.x; i++){
				for (int j = bigLT.y; j > bigRB.y; j--){
					int index = j*MAP_WIDTH + i;
					if (veloGrids.velo_grid[index]){
						std::cout << "something on stopline" << std::endl;
						CarControl::GetInstance().StopCommand();
						return;
					}
				}
			}
		}
	}
	int laneI = 0;
	std::vector<std::vector<RoadPoint>> lanes = DataCenter::GetInstance().GetMultiLanes(laneI);
	for (std::vector<RoadPoint>& lane : lanes){
		for (RoadPoint& rpt : lane){
			CoordTransform::WorldToLocal(curPos, rpt, &rpt);
		}
	}
	
	for (int laneIndex = laneI;;){
		
		//double min = DBL_MAX;
		//int minIndex = 0;
		//for (int i = 0; i < lanes[laneIndex].size(); i++){
		//	RoadPoint pt = lanes[laneIndex][i];
		//	double dis = pt.x*pt.x + pt.y*pt.y;
		//	if (dis < min){
		//		min = dis;
		//		minIndex = i;
		//	}
		//}
		////求参考轨迹上离车当前点最近的点
		//std::pair<double, double> v1;
		//v1.first = lanes[laneIndex][minIndex + 1].x - lanes[laneIndex][minIndex].x;
		//v1.second = lanes[laneIndex][minIndex + 1].y - lanes[laneIndex][minIndex].y;
		//RoadPoint firstPt;
		//firstPt.y = (-v1.first * v1.second *  lanes[laneIndex][minIndex].x
		//	+ v1.first * v1.first *  lanes[laneIndex][minIndex].y)
		//	/ (v1.first * v1.first + v1.second * v1.second);
		//firstPt.x = (-v1.second * firstPt.y) / (v1.first);
		////firstPt.angle = trajectory[minIndex].angle;
		//firstPt.angle = atan2(v1.second, v1.first);
		////std::cout << "angle: " << tan(firstPt.angle) << std::endl;
		//lanes[laneIndex].erase(lanes[laneIndex].begin(), lanes[laneIndex].begin() + minIndex);
		//lanes[laneIndex][0] = firstPt;
		//double qi = sqrt(lanes[laneIndex][0].x *  lanes[laneIndex][0].x + lanes[laneIndex][0].y *  lanes[laneIndex][0].y);
		//if (firstPt.x < 0)
		//{
		//	qi = -qi;
		//}

		double qi = CoordTransform::TrimLocalPathToCurPt(lanes[laneIndex]);
		double theta = PI / 2 - lanes[laneIndex][0].angle;
		SXYSpline spline;
		spline.init(lanes[laneIndex]);
		double sf = *spline.S.rbegin();
		std::vector<std::vector<RoadPoint>> _root_;
		std::vector<double> road_qf;
		for (float i = 0; i < 6; i += 0.2) {
			float qf = i - 3;
			std::vector<RoadPoint> pts=trajectory_build(qf, qi, theta, sf, spline);
			if (CheckCollision(veloGrids, pts, true)<0) {
				_root_.push_back(pts);
				road_qf.push_back(qf);
			}
		}
		if (_root_.empty()){
			laneIndex++;
			if (laneIndex >= lanes.size()){
				laneIndex = 0;
			}
			if (laneIndex == laneI){
				break;
			}
			continue;
		}
		std::vector<RoadPoint> _best_root_;
		if (pre_Root.empty()) {
			float min = 10000;
			float minI = 1000;
			for (int i = 0; i < road_qf.size(); i++) {
				if (fabs(road_qf[i]) < min) {
					min = fabs(road_qf[i]);
					minI = i;
				}
			}
			_best_root_ = _root_[minI];
		}
		else {
			float min = (double)LONG_MAX;
			int minI = 0;
			for (int n = 0; n < _root_.size(); n++) {
				std::vector<RoadPoint>& tmpRoot = _root_[n];
				float distanceSum = 0;
				for (int i = 0; i < tmpRoot.size(); i++) {
					double delta_s = sf / tmpRoot.size() * i;
					RoadPoint pt;
					spline.getXY(delta_s, pt.x, pt.y);
					distanceSum += Topology::Distance2(pt, tmpRoot[i]);
				}
				float distanceSum2 = 0;
				for (int i = 0; i<tmpRoot.size(); i++) {
					distanceSum2 += Topology::Distance2(pre_Root[i], tmpRoot[i]);
				}
				distanceSum2 *= 1;
				distanceSum += distanceSum2;
				if (distanceSum < min) {
					min = distanceSum;
					minI = n;
				}
			}
			_best_root_ = _root_[minI];
		}
		pre_Root = _best_root_;
		track.SetLocalPath(_best_root_);
		ckLcmType::DecisionDraw_t draw;
		int total_num = 0;
		for (int i = 0; i < _root_.size(); i++)
		{
			for (int j = 0; j < _root_[i].size(); j++)
			{
				double x = _root_[i][j].x;
				double y = _root_[i][j].y;
				draw.Path_x.push_back(x);
				draw.Path_y.push_back(y);
			}
			total_num += _root_[i].size();
		}
		std::cout << "root number" << _root_.size() << std::endl;
		draw.num = total_num;
		for (std::vector<RoadPoint> lane : lanes){
			for (RoadPoint rpt : lane){
				draw.Refer_x.push_back(rpt.x);
				draw.Refer_y.push_back(rpt.y);
			}
		}
		for (RoadPoint& pt : _best_root_) {
			//double x, y;
			//CoordTransform::GridtoLocal(pt.x + X_START, pt.y - Y_START, x, y);
			draw.Refer_x.push_back(pt.x);
			draw.Refer_y.push_back(pt.y);
			//of << pt.x << "\t" << pt.y << "\n";
			//fprintf(fp, "%lf %lf %lf\n", x, y, pt.angle);
		}
		draw.refnum = draw.Refer_x.size();
		m_sendPath.SendDraw(draw);
		break;
	}
}

void PathGenerate::short_time_uturn()
{
	// step one receive data
	if (!DataCenter::GetInstance().HasVeloGrid()) {
		std::cout << "Warning::not velogrid message" << std::endl;
		return;
	}
	//if (!DataCenter::GetInstance().HasCurb()) {
	//	std::cout << "Warning::not curb message" << std::endl;
	//	return ;
	//}
	if (!DataCenter::GetInstance().HasRefTrajectory()) {
		std::cout << "Warning::not ref message" << std::endl;
		return;
	}
	VeloGrid_t veloGrids = DataCenter::GetInstance().GetLidarData();
	double qi = 0.0;
	std::vector<RoadPoint> refTrajectory = DataCenter::GetInstance().GetRefTrajectory_Qi(qi);


	SXYSpline spline;
	spline.init(refTrajectory);


	double theta = PI / 2 - refTrajectory[0].angle;

	double sf = spline.S[spline.S.size() - 1];
	//std::cout << "Sf:" << sf << std::endl;
	std::vector<std::vector<RoadPoint>> _root_;
	std::vector<RoadPoint> rdpt;
	std::vector<float> road_qf;
	//生成候选路径
	for (float i = 0; i < 12; i += 0.2) {
		float qf = i - 6;
		std::vector<RoadPoint> pts;
		pts = trajectory_build(qf, qi, theta, sf, spline);
		int findOb = CheckCollision(veloGrids, pts, false);
		if(findOb < 0) {
			_root_.push_back(pts);
			road_qf.push_back(qf);
		}
	}
	if (_root_.size() == 0)
	{
		bool onUturn = true;
		if (onUturn) {
			RadAngle go_angle = refTrajectory[0].angle;
			RadAngle go_angle0 = go_angle + 0.3;
			RadAngle go_angle1 = go_angle - 0.3;
			RadAngle back_angle = 2 * PI - go_angle;
			RadAngle back_angle0 = back_angle + 0.3;
			RadAngle back_angle1 = back_angle - 0.3;
			int back_num = 0;
			int go_num = 0;
			int back_start = -1;
			int go_end = -1;
			for (int j = 0; j < refTrajectory.size(); j++) {
				if (go_end < 0) {
					if (!refTrajectory[j].angle.belong(go_angle1, go_angle0)) {
						go_num++;
						if (go_num >= 5) {
							go_end = j;
						}
					}
					else {
						go_num = 0;
					}
				}
				else {
					if (refTrajectory[j].angle.belong(back_angle1, back_angle0)) {
						back_num++;
						if (back_num >= 5) {
							back_start = j;
							break;
						}
					}
					else {
						back_num = 0;
					}
				}
			}
			if (back_start < 0) {
				std::cout << "未找到掉头" << std::endl;
			}
			else {
				std::cout << "发现掉头" << std::endl;
				for (int i = 0; i < go_end; i++) {
					PosPoint startPt = refTrajectory[go_end - i];
					int searchSt = back_start - 3;
					for (int j = 0; j < 6; j++) {
						std::vector<RoadPoint> tmpRef(refTrajectory.begin(), refTrajectory.begin() + go_end - i);
						tmpRef.insert(tmpRef.end(), refTrajectory.begin() + searchSt + 1, refTrajectory.end());
						PosPoint target = refTrajectory[searchSt + j];
						Clothoid clothoid(startPt.x, startPt.y, startPt.angle, target.x, target.y, target.angle);
						std::vector<RoadPoint> pts(10, RoadPoint());
						clothoid.PointsOnClothoid(pts, 10);
						//ckLcmType::DecisionDraw_t draw;
						//draw.num =100;
						//for (RoadPoint rpt:pts){
						//	draw.Path_x.push_back(rpt.x);
						//	draw.Path_y.push_back(rpt.y);
						//}
						//m_sendPath.SendDraw(draw);
						int findob = CheckCollision(veloGrids, pts, false);
						if (findob < 0) {
							tmpRef.insert(tmpRef.begin() + go_end - i, pts.begin() + 1, pts.end() - 1);
							double qi = CoordTransform::TrimLocalPathToCurPt(tmpRef);
							spline.init(tmpRef);
							sf = *spline.S.rbegin();
							for (float ii = 0; ii < 12; ii += 0.2) {
								float qf = ii - 6;
								std::vector<RoadPoint> apts;
								apts = trajectory_build(0, qi, theta, sf, spline);
								int coll = CheckCollision(veloGrids, apts, false);
								if (coll < 0) {
									_root_.push_back(apts);
									road_qf.push_back(0);
								}
							}
							if (_root_.size() == 0)
							{
								continue;
							}
							else {
								i = go_end;
								break;
							}

						}
					}
				}
			}
		}
		else {
			std::cout << "no root to use " << std::endl;
			return;
		}
	}
	if (_root_.empty()) {
		std::cout << "not root to use" << std::endl;
		return;
	}
	std::vector<RoadPoint> _best_root_;
	if (pre_Root.empty()) {
		float min = 10000;
		float minI = 1000;
		for (int i = 0; i < road_qf.size(); i++) {
			if (fabs(road_qf[i]) < min) {
				min = fabs(road_qf[i]);
				minI = i;
			}
		}
		_best_root_ = _root_[minI];
	}
	else {
		float min = (double)LONG_MAX;
		int minI = 0;
		for (int n = 0; n < _root_.size(); n++) {
			std::vector<RoadPoint>& tmpRoot = _root_[n];
			float distanceSum = 0;
			for (int i = 0; i < tmpRoot.size(); i++) {
				double delta_s = sf / tmpRoot.size() * i;
				RoadPoint pt;
				spline.getXY(delta_s, pt.x, pt.y);
				distanceSum += Topology::Distance2(pt, tmpRoot[i]);
			}
			float distanceSum2 = 0;
			for (int i = 0; i<tmpRoot.size(); i++) {
				distanceSum2 += Topology::Distance2(pre_Root[i], tmpRoot[i]);
			}
			distanceSum2 *= 1;
			distanceSum += distanceSum2;
			if (distanceSum < min) {
				min = distanceSum;
				minI = n;
			}
		}
		_best_root_ = _root_[minI];
	}

	pre_Root = _best_root_;
	ckLcmType::DecisionDraw_t draw;
	track.SetLocalPath(_best_root_);

	//draw all lines
	int total_num = 0;
	for (int i = 0; i < _root_.size(); i++)
	{
		for (int j = 0; j < _root_[i].size(); j++)
		{
			double x = _root_[i][j].x;
			double y = _root_[i][j].y;
			draw.Path_x.push_back(x);
			draw.Path_y.push_back(y);
			if (i == 0)
			{
				RoadPoint pt_for_track;
				pt_for_track.x = x;
				pt_for_track.y = y;
				pt_for_track.angle = 0;
				rdpt.push_back(pt_for_track);
			}
		}
		total_num += _root_[i].size();
	}
	std::cout << "root number" << _root_.size() << std::endl;
	draw.num = total_num;

	int total_refnum = refTrajectory.size();
	draw.refnum = total_refnum + _best_root_.size();
	draw.Refer_x.reserve(total_refnum);
	draw.Refer_y.reserve(total_refnum);
	for (int i = 0; i < total_refnum; i++)
	{
		draw.Refer_x.push_back(refTrajectory[i].x);
		draw.Refer_y.push_back(refTrajectory[i].y);
	}
	for (RoadPoint& pt : _best_root_) {
		draw.Refer_x.push_back(pt.x);
		draw.Refer_y.push_back(pt.y);
	}
	m_sendPath.SendDraw(draw);
}

std::vector<std::vector<RoadPoint>> PathGenerate::generateTrajectories(std::vector<RoadPoint>& baseFrame, bool local)
{
	std::vector<std::vector<RoadPoint>> trajectories;
	if (!local) {
		PosPoint curPos = DataCenter::GetInstance().GetCurPosition();
		for (RoadPoint& rpt : baseFrame) {
			CoordTransform::WorldToLocal(curPos, rpt, &rpt);
		}
	}
	VeloGrid_t veloGrids = DataCenter::GetInstance().GetLidarData();
	double qi = CoordTransform::TrimLocalPathToCurPt(baseFrame);
	double theta = PI / 2 - baseFrame.begin()->angle;
	SXYSpline spline;
	spline.init(baseFrame);
	if (spline.splineNum <= 4) 
		return trajectories;
	double Sf = *spline.S.rbegin();
	baseFrame.clear();
	//将BaseFrame等间隔划分为50个点
	for (int i = 0; i <= 50; i++) {
		double deltaS = i*Sf / 50;
		RoadPoint rpt;
		spline.getXY(deltaS, rpt.x, rpt.y);
		double deltaX, deltaY;
		spline.getDeriveXY(deltaS, deltaX, deltaY);
		rpt.angle = atan2(deltaY, deltaX);
		baseFrame.push_back(rpt);
	}
	for (double step = 0; step < 12; step += 0.2) {
		double qf = step - 6;
		std::vector<RoadPoint> trajectory = trajectory_build(qf, qi, theta, *spline.S.rbegin(), spline);
		int findOb = CheckCollision(veloGrids, trajectory, true);
		if (findOb < 0) {
			trajectories.push_back(trajectory);
		}
	}
	return trajectories;
}

void PathGenerate::plan()
{
	//{
	//	if (!DataCenter::GetInstance().HasCamLane()) {
	//		std::cout << "没有车道线" << std::endl;
	//		return;
	//	}
	//	std::vector<RoadPoint> baseFrame = DataCenter::GetInstance().GetCamLanes();
	//	if (baseFrame.empty()) {
	//		std::cout << "没有足够的车道线信息" << std::endl;
	//		return;
	//	}
	//	//SendPath(baseFrame, baseFrame, std::vector<std::vector<RoadPoint>>());	
	//	//return;
	//	double qi = CoordTransform::TrimLocalPathToCurPt(baseFrame);
	//	double theta = PI / 2 - baseFrame.begin()->angle;
	//	SXYSpline spline;
	//	spline.init(baseFrame);
	//	double Sf = *spline.S.rbegin();
	//	baseFrame.clear();
	//	for (int i = 0; i < 50; i++) {
	//		double deltaS = i*Sf / 50;
	//		RoadPoint rpt;
	//		spline.getXY(deltaS, rpt.x, rpt.y);
	//		double deltaX, deltaY;
	//		spline.getDeriveXY(deltaS, deltaX, deltaY);
	//		rpt.angle = atan2(deltaY, deltaX);
	//		baseFrame.push_back(rpt);
	//	}
	//	//for (double step = 0; step < 12; step += 0.2) {
	//	double qf = 0;
	//	std::vector<RoadPoint> trajectory = trajectory_build(qf, qi, theta, *spline.S.rbegin(), spline);
	//	//trajectories.push_back(trajectory);
	//	//}
	//	track.SetLocalPath(trajectory);
	//	std::vector<std::vector<RoadPoint>> rrr;
	//	rrr.push_back(trajectory);
	//	SendPath(baseFrame, std::vector<RoadPoint>(), rrr);
	//	return;
	//}
	std::cout << "------------------Plan Start------------------------" << std::endl;
	std::vector<std::vector<RoadPoint>> baseFrames;
	int laneI;
	if (DataCenter::GetInstance().HasMultiLane()) {
		baseFrames = DataCenter::GetInstance().GetMultiLanes(laneI);
	}
	else {
		if (DataCenter::GetInstance().HasRefTrajectory()) {
			baseFrames.push_back(DataCenter::GetInstance().GetRefTrajectories());
			laneI = 0;
		}
		else {
			std::cout << "没有参考路径" << std::endl;
			return;
		}
	}
	if (DataCenter::GetInstance().HasMultiLane())
	{
		VeloGrid_t veloGrids = DataCenter::GetInstance().GetLidarData();
		PosPoint curPos = DataCenter::GetInstance().GetCurPosition();
		double width = 0;
		PosPoint pt = DataCenter::GetInstance().GetStopLine(width);
		if (width != 0) {
			std::cout << "前方有停止线" << std::endl;
			double height = 4;
			CoordTransform::WorldToLocal(curPos, pt, &pt);
			PosPoint LT, RB, LB, RT;
			double dxb = sin(pt.angle)*width;
			double dyb = cos(pt.angle)*width;
			LB.x = pt.x - dxb / 2;
			LB.y = pt.y + dyb / 2;
			RB.x = pt.x + dxb / 2;
			RB.y = pt.y - dyb / 2;
			double dxt = cos(pt.angle)*height;
			double dyt = sin(pt.angle)*height;
			LT.x = LB.x + dxt;
			LT.y = LB.y + dyt;
			RT.x = RB.x + dxt;
			RT.y = RB.y + dyt;
			PosPoint bigLT, bigRB;
			bigLT.x = fmin(LB.x, fmin(LT.x, fmin(RB.x, RT.x)));
			bigLT.y = fmax(LB.y, fmax(LT.y, fmax(RB.y, RT.y)));
			bigRB.x = fmax(LB.x, fmax(LT.x, fmax(RB.x, RT.x)));
			bigRB.y = fmin(LB.y, fmin(LT.y, fmin(RB.y, RT.y)));
			bigLT.x = bigLT.x / Grid + MAP_WIDTH / 2;
			bigLT.y = bigLT.y / Grid + MAP_HEIGHT / 2;
			bigRB.x = bigRB.x / Grid + MAP_WIDTH / 2;
			bigRB.y = bigRB.y / Grid + MAP_HEIGHT / 2;

			for (int i = bigLT.x; i < bigRB.x; i++) {
				for (int j = bigLT.y; j > bigRB.y; j--) {
					int index = j*MAP_WIDTH + i;
					if (veloGrids.velo_grid[index]) {
						std::cout << "停止线前方存在障碍物" << std::endl;
						CarControl::GetInstance().StopCommand();
						return;
					}
				}
			}
		}
	}

	//init last path in this relative coordinate system
	PosPoint curPos = DataCenter::GetInstance().GetCurPosition();
	std::vector<RoadPoint> path_tmp;
	int collision;
	if (!pre_r.empty())
	{
		int indexI = -1;
		for (PosPoint& rpt : pre_r)
		{
			CoordTransform::WorldToLocal(curPos, rpt, &rpt);
			if (rpt.y < 0)
				indexI++;
		}
		if (indexI >= 0)
			pre_r.erase(pre_r.begin(), pre_r.begin() + indexI);
	}

	double best_cost = DBL_MAX;
	std::vector<RoadPoint> best_bestRoot;
	std::vector<RoadPoint> best_baseframe;
	std::vector<std::vector<RoadPoint>> candidateTrajs;
	for (int laneIndex = 0; laneIndex < baseFrames.size();laneIndex++)
	//for (int laneIndex = laneI; laneIndex < baseFrames.size(); laneIndex = (++laneIndex) >= baseFrames.size() ? (0) : ((laneIndex == laneI) ? baseFrames.size() : laneIndex)) 
	{
		std::vector<RoadPoint> baseFrame = baseFrames[laneIndex];
		
		for (RoadPoint& rpt : baseFrame) {
			CoordTransform::WorldToLocal(curPos, rpt, &rpt);
		}
		std::vector<std::vector<RoadPoint>> tmp_candidateTraj = planRef(baseFrame);
		if (tmp_candidateTraj.empty()) {
			std::cout << "纯路径走不了" << std::endl;
			CarControl::GetInstance().StopCommand();
			bool onUTurn = false;
			if (onUTurn) {
				tmp_candidateTraj = planRefInUTurn(baseFrame);
				if (tmp_candidateTraj.empty()) {
					std::cout << "调不了头" << std::endl;
					CarControl::GetInstance().StopCommand();
					continue;
				}
			}
			else {
				tmp_candidateTraj = planRefWithSegment(baseFrame);
				if (tmp_candidateTraj.empty()) {
					std::cout << "无路径" << std::endl;
					CarControl::GetInstance().StopCommand();
					continue;
				}
			}
		}

		candidateTrajs.insert(candidateTrajs.end(), tmp_candidateTraj.begin(), tmp_candidateTraj.end());

		double tmp_cost = 0.0;
		//select best trajectory in this baseframe
		//std::vector<RoadPoint> bestRoot = selectBestTraj(candidateTraj, pre_Root, baseFrame);
		std::vector<RoadPoint> bestRoot = SelectTra(tmp_candidateTraj, pre_r, baseFrame, tmp_cost);

		if (best_cost > tmp_cost)
		{
			best_bestRoot = bestRoot; 
			best_cost = tmp_cost;
			best_baseframe = baseFrame;
		}

	}

	//update
	bool updateflag = true;
	updateflag = UpdateOrNot(best_bestRoot, pre_r, best_baseframe);
	if (updateflag)
	{
		pre_Root = best_bestRoot;
		track.SetLocalPath(best_bestRoot);
		pre_r = best_bestRoot;
	}
	else
		std::cout << "Not Update Path !!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;

	SendPath(best_baseframe, best_bestRoot, candidateTrajs);

	//transform last path into global coordinate system
	for (int i = 0; i < pre_r.size(); i++)
	{
		PosPoint tmp;
		CoordTransform::LocalToWorld(curPos, pre_r[i], &tmp);
		pre_r[i] = tmp;
	}

	std::cout << "------------------Plan End------------------------" << std::endl << std::endl;
	return;
}

void PathGenerate::planJoint(){
	std::vector<std::vector<RoadPoint>> baseFrames;
	int laneI;
	if (DataCenter::GetInstance().HasMultiLane()) {
		baseFrames = DataCenter::GetInstance().GetMultiLanes(laneI);
	}
	else {
		if (DataCenter::GetInstance().HasRefTrajectory()) {
			baseFrames.push_back(DataCenter::GetInstance().GetRefTrajectories());
			laneI = 0;
		}
		else {
			std::cout << "没有参考路径" << std::endl;
			return;
		}
	}
	for (int laneIndex = laneI; laneIndex < baseFrames.size(); laneIndex = (++laneIndex) >= baseFrames.size() ? (0) : ((laneIndex == laneI) ? baseFrames.size() : laneIndex)) {
		std::vector<RoadPoint> baseFrame = baseFrames[laneIndex];
		PosPoint curPos = DataCenter::GetInstance().GetCurPosition();
		for (RoadPoint& rpt : baseFrame) {
			CoordTransform::WorldToLocal(curPos, rpt, &rpt);
		}
		std::vector<std::vector<RoadPoint>> candidateTraj;
		if (pre_Root.empty()){
			plan();
			if (pre_Root.empty()){
				std::cout << "no path to go" << std::endl;
				continue;
			}
			double disSum = 0;
			for (int i = 1; i < pre_Root.size(); i++){
				double dis = sqrt(Topology::Distance2(pre_Root[i], pre_Root[0]));
				if (dis > 8){
					pre_Root.erase(pre_Root.begin() + i + 1, pre_Root.end());
					break;
				}
			}
			for (RoadPoint & rpt : pre_Root){
				CoordTransform::LocalToWorld(curPos, rpt, &rpt);
			}
			return;
		}
		else{
			VeloGrid_t veloGrids = DataCenter::GetInstance().GetLidarData();
			for (RoadPoint& rpt : pre_Root) {
				CoordTransform::WorldToLocal(curPos, rpt, &rpt);
			}
			if (CheckCollision(veloGrids, pre_Root, true) < 0){
				RoadPoint planStart = *pre_Root.rbegin();
				{
					for (int i = 0; i < pre_Root.size(); i++){
						double dis = sqrt(Topology::Distance2(pre_Root[i], pre_Root[0]));
						if (dis > 8){
							planStart = pre_Root[i];
							pre_Root.erase(pre_Root.begin() + i, pre_Root.end());
							break;
						}
					}
				}
				for (RoadPoint& rpt : baseFrame){
					CoordTransform::WorldToLocal(planStart, rpt, &rpt);
				}
				std::vector<std::vector<RoadPoint>> candidateTraj;
				///planRef(baseFrame);
				do{
					std::vector<std::vector<RoadPoint>> trajectories;
					VeloGrid_t veloGrids = DataCenter::GetInstance().GetLidarData();
					double qi = CoordTransform::TrimLocalPathToCurPt(baseFrame);
					double theta = PI / 2 - baseFrame.begin()->angle;
					SXYSpline spline;
					spline.init(baseFrame);
					if (spline.splineNum <= 4) break;
					double Sf = *spline.S.rbegin();
					baseFrame.clear();
					for (int i = 0; i <= 50; i++) {
						double deltaS = i*Sf / 50;
						RoadPoint rpt;
						spline.getXY(deltaS, rpt.x, rpt.y);
						double deltaX, deltaY;
						spline.getDeriveXY(deltaS, deltaX, deltaY);
						rpt.angle = atan2(deltaY, deltaX);
						baseFrame.push_back(rpt);
					}
					for (double step = 0; step < 12; step += 0.2) {
						double qf = step - 6;
						std::vector<RoadPoint> trajectory = trajectory_build(qf, qi, theta, *spline.S.rbegin(), spline);
						std::vector<RoadPoint> ttrajectory = trajectory;
						for (RoadPoint& rpt : trajectory){
							CoordTransform::LocalToWorld(planStart, rpt, &rpt);
						}
						int findOb = CheckCollision(veloGrids, trajectory, true);
						if (findOb < 0) {
							trajectories.push_back(ttrajectory);
						}
					}
					candidateTraj= trajectories;
				} while (false);
				if (candidateTraj.empty()) {
					std::cout << "纯路径走不了" << std::endl;
					CarControl::GetInstance().StopCommand();
					bool onUTurn = false;
					if (onUTurn) {
						candidateTraj = planRefInUTurn(baseFrame);
						if (candidateTraj.empty()) {
							std::cout << "调不了头" << std::endl;
							CarControl::GetInstance().StopCommand();
							pre_Root.clear();
							continue;
						}
					}
					else {
						candidateTraj;
						///planRefWithSegment(baseFrame);
						do{
							int count = 0;
							do
							{
								VeloGrid_t veloGrids = DataCenter::GetInstance().GetLidarData();
								std::vector<RoadPoint> searchRoot;
								if (pre_Root.empty()) {
									searchRoot = baseFrame;
								}
								else {
									searchRoot = pre_Root;
								}
								double qi = CoordTransform::TrimLocalPathToCurPt(searchRoot);
								SXYSpline spline;
								spline.init(searchRoot);
								if (spline.splineNum <= 4) break;
								double Sf = *spline.S.rbegin();
								searchRoot = trajectory_build(0, qi, PI / 2 - searchRoot.begin()->angle, Sf, spline);
								int findOb = CheckCollision(veloGrids, searchRoot, true);
								if (findOb >= 0) {
									findOb = (findOb + 2) < spline.S.size() ? findOb + 2 : spline.S.size();
									searchRoot.erase(searchRoot.begin() + findOb, searchRoot.end());
								}
								if (searchRoot.size() <= 5) break;
								candidateTraj;
								/// generateTrajectories(searchRoot, true);
								do{
									std::vector<std::vector<RoadPoint>> trajectories;
									VeloGrid_t veloGrids = DataCenter::GetInstance().GetLidarData();
									double qi = CoordTransform::TrimLocalPathToCurPt(baseFrame);
									double theta = PI / 2 - baseFrame.begin()->angle;
									SXYSpline spline;
									spline.init(baseFrame);
									if (spline.splineNum <= 4) break;
									double Sf = *spline.S.rbegin();
									baseFrame.clear();
									for (int i = 0; i <= 50; i++) {
										double deltaS = i*Sf / 50;
										RoadPoint rpt;
										spline.getXY(deltaS, rpt.x, rpt.y);
										double deltaX, deltaY;
										spline.getDeriveXY(deltaS, deltaX, deltaY);
										rpt.angle = atan2(deltaY, deltaX);
										baseFrame.push_back(rpt);
									}
									for (double step = 0; step < 12; step += 0.2) {
										double qf = step - 6;
										std::vector<RoadPoint> trajectory = trajectory_build(qf, qi, theta, *spline.S.rbegin(), spline);
										std::vector<RoadPoint> ttrajectory = trajectory;
										for (RoadPoint& rpt : trajectory){
											CoordTransform::LocalToWorld(planStart, rpt, &rpt);
										}
										int findOb = CheckCollision(veloGrids, trajectory, true);
										if (findOb < 0) {
											trajectories.push_back(ttrajectory);
										}
									}
									candidateTraj = trajectories;
								} while (false);
								if (!candidateTraj.empty()){
									double deltaS = Sf*findOb / 50;
									qi = CoordTransform::TrimLocalPathToCurPt(baseFrame);
									spline.init(baseFrame);
									baseFrame.clear();
									for (int i = 0; i <= 50; i++){
										RoadPoint rpt;
										spline.getXY(deltaS*i / 50, rpt.x, rpt.y);
										double dx, dy;
										spline.getDeriveXY(deltaS*i / 50, dx, dy);
										rpt.angle = atan2(dy, dx);
										baseFrame.push_back(rpt);
									}
									break;
								}
							} while (count++ < 10);
						} while (false);
						if (candidateTraj.empty()) {
							std::cout << "无路径" << std::endl;
							CarControl::GetInstance().StopCommand();
							pre_Root.clear();
							continue;
						}
					}
				}

				std::vector<RoadPoint> bestRoot = selectBestTraj(candidateTraj, pre_Root, baseFrame);
				for (RoadPoint & rpt : bestRoot){
					CoordTransform::LocalToWorld(planStart, rpt, &rpt);
				}
				double disSum = 0;
				for (int i = 1; i < bestRoot.size(); i++){
					double dis = sqrt(Topology::Distance2(bestRoot[i], bestRoot[0]));
					if (dis > 8){
						bestRoot.erase(bestRoot.begin() + i + 1, bestRoot.end());
						break;
					}
				}
				pre_Root.insert(pre_Root.end(), bestRoot.begin(), bestRoot.end());
				track.SetLocalPath(pre_Root); std::vector<std::vector<RoadPoint>> rrr;
				rrr.push_back(pre_Root);
				for (RoadPoint& rpt : baseFrame){
					CoordTransform::LocalToWorld(planStart, rpt, &rpt);
				}
				SendPath(baseFrame, std::vector<RoadPoint>(), rrr);
				for (RoadPoint & rpt : pre_Root){
					CoordTransform::LocalToWorld(curPos, rpt, &rpt);
				}
				return;
			}
			else{
				pre_Root.clear();
				continue;
			}
		}
	}
}

std::vector<std::vector<RoadPoint>> PathGenerate::planRef(std::vector<RoadPoint>& baseFrame)
{
	m_isSegmentMode = false;
	m_testVelocityFactor = 1.5;
	std::vector<std::vector<RoadPoint>> candidateTraj = generateTrajectories(baseFrame, true);
	return candidateTraj;
}

std::vector<std::vector<RoadPoint>> PathGenerate::planRefWithSegment(std::vector<RoadPoint>& baseFrame)
{
	m_isSegmentMode = true;
	m_testVelocityFactor = 0.8;
	std::vector<std::vector<RoadPoint>> candidateTraj;
	int count = 0;
	do
	{
		VeloGrid_t veloGrids = DataCenter::GetInstance().GetLidarData();
		std::vector<RoadPoint> searchRoot;
		if (pre_Root.empty()) {
			searchRoot = baseFrame;
		}
		else {
			searchRoot = pre_Root;
		}
		double qi = CoordTransform::TrimLocalPathToCurPt(searchRoot);
		SXYSpline spline;
		spline.init(searchRoot);
		if (spline.splineNum <= 4) break;
		double Sf = *spline.S.rbegin();
		searchRoot = trajectory_build(0, qi, PI / 2 - searchRoot.begin()->angle, Sf, spline);
		int findOb = CheckCollision(veloGrids, searchRoot, true);
		if (findOb >= 0) {
			findOb = (findOb + 2) < spline.S.size() ? findOb + 2 : spline.S.size();
			searchRoot.erase(searchRoot.begin() + findOb, searchRoot.end());
		}
		if (searchRoot.size() <= 5) break;
		candidateTraj = generateTrajectories(searchRoot, true);
		if (!candidateTraj.empty()){
			double deltaS = Sf*findOb / 50;
			qi = CoordTransform::TrimLocalPathToCurPt(baseFrame);
			spline.init(baseFrame);
			baseFrame.clear();
			for (int i = 0; i <= 50; i++){
				RoadPoint rpt;
				spline.getXY(deltaS*i / 50, rpt.x, rpt.y);
				double dx, dy;
				spline.getDeriveXY(deltaS*i / 50, dx, dy);
				rpt.angle = atan2(dy, dx);
				baseFrame.push_back(rpt);
			}
			break;
		}
	} while (count++ < 10);
	return candidateTraj;
}

std::vector<std::vector<RoadPoint>> PathGenerate::planRefInUTurn(std::vector<RoadPoint>& baseFrame)
{
	RadAngle go_angle = baseFrame[0].angle;
	RadAngle go_angle0 = go_angle + 0.2;
	RadAngle go_angle1 = go_angle - 0.2;
	RadAngle back_angle = 2 * PI - go_angle;
	RadAngle back_angle0 = back_angle + 0.2;
	RadAngle back_angle1 = back_angle - 0.2;
	int back_num = 0;
	int go_num = 0;
	int back_start = -1;
	int go_end = -1;
	for (int j = 0; j < baseFrame.size(); j++) {
		if (go_end < 0) {
			if (!baseFrame[j].angle.belong(go_angle1, go_angle0)) {
				go_num++;
				if (go_num >= 5) {
					go_end = j;
				}
			}
			else {
				go_num = 0;
			}
		}
		else {
			if (baseFrame[j].angle.belong(back_angle1, back_angle0)) {
				back_num++;
				if (back_num >= 5) {
					back_start = j;
					break;
				}
			}
			else {
				back_num = 0;
			}
		}
	}
	if (back_start < 0) {
		std::cout << "未找到掉头" << std::endl;
		return planRefWithSegment(baseFrame);
	}
	else {
		std::cout << "发现掉头" << std::endl;
		VeloGrid_t veloGrids = DataCenter::GetInstance().GetLidarData();
		for (int i = 0; i < go_end; i++) {
			PosPoint startPt = baseFrame[go_end - i];
			int searchSt = back_start - 3;
			for (int j = 0; j < 6; j++) {
				std::vector<RoadPoint> tmpRef(baseFrame.begin(), baseFrame.begin() + go_end - i);
				tmpRef.insert(tmpRef.end(), baseFrame.begin() + searchSt + 1, baseFrame.end());
				PosPoint target = baseFrame[searchSt + j];
				Clothoid clothoid(startPt.x, startPt.y, startPt.angle, target.x, target.y, target.angle);
				std::vector<RoadPoint> pts(10, RoadPoint());
				clothoid.PointsOnClothoid(pts, 10);
				int findob = CheckCollision(veloGrids, pts, false);
				if (findob < 0) {
					tmpRef.insert(tmpRef.begin() + go_end - i, pts.begin() + 1, pts.end() - 1);
					std::vector<std::vector<RoadPoint>> candicatePaths = planRef(tmpRef);
					if (candicatePaths.empty()) {
						return planRefWithSegment(tmpRef);
					}
					return candicatePaths;
				}
			}
			back_start++;
		}
	}
	return std::vector<std::vector<RoadPoint>>();
}

std::vector<RoadPoint> PathGenerate::selectBestTraj(std::vector<std::vector<RoadPoint>>& paths, std::vector<RoadPoint>& prePath,std::vector<RoadPoint>& refPath)
{
	std::vector<RoadPoint> bestRoot;
	float min = FLT_MAX;
	float minI = 0;
	//if (pre_Root.empty()) {
		for (int i = 0; i < paths.size(); i++) {
			float disSum = 0;
			for (int j = 0; j < paths[i].size() && j < refPath.size(); j++) {
				float dis = Topology::Distance2(paths[i][j], refPath[j]);
				disSum += dis;
			}
			if (disSum < min) {
				min = disSum;
				minI = i;
			}
		}
//	}
	//else {
	//	for (int n = 0; n < paths.size(); n++) {
	//		std::vector<RoadPoint>& tmpRoot = paths[n];
	//		float distanceSum = 0;
	//		for (int i = 0; i < tmpRoot.size(); i++) {
	//			int index=TrackFinder::FindPointIndex(refPath, tmpRoot[i]);
	//			if (index >= 0){
	//				distanceSum += Topology::Distance2(refPath[index], tmpRoot[i]);
	//			}
	//		}
	//		float distanceSum2 = 0;
	//		for (int i = 0; i<tmpRoot.size(); i++) {
	//			distanceSum2 += Topology::Distance2(prePath[i], tmpRoot[i]);
	//		}
	//		distanceSum2 *= 0;
	//		distanceSum += distanceSum2;
	//		if (distanceSum < min) {
	//			min = distanceSum;
	//			minI = n;
	//		}
	//	}
	//}
	bestRoot = paths[minI];
	return bestRoot;
}

void PathGenerate::SendPath(std::vector<RoadPoint>& ref, std::vector<RoadPoint>& best, std::vector<std::vector<RoadPoint>>& paths)
{

	m_sendPath.SendDraw(paths, best);
	return;

	DecisionDraw_t draw;
	for (int i = 0; i < paths.size(); i++)
	{
		for (int j = 0; j < paths[i].size(); j++)
		{
			double x = paths[i][j].x;
			double y = paths[i][j].y;
			draw.Path_x.push_back(x);
			draw.Path_y.push_back(y);
		}
	}
	std::cout << "root number" << paths.size() << std::endl;
	draw.num = draw.Path_x.size();
		for (RoadPoint rpt : ref) {
			draw.Refer_x.push_back(rpt.x);
			draw.Refer_y.push_back(rpt.y);
		}
	for (RoadPoint& pt : best) {
		draw.Refer_x.push_back(pt.x);
		draw.Refer_y.push_back(pt.y);
	}
	draw.refnum = draw.Refer_x.size();
	m_sendPath.SendDraw(draw);
}


std::vector<RoadPoint> PathGenerate::trajectory_build(float qf, float qi, float theta, double sf, SXYSpline& spline){
	std::vector<RoadPoint> pts;
	float c = tan(theta);
	float a = 2 * (qi - qf) / (pow(sf, 3)) + c / pow(sf, 2);
	float b = 3 * (qf - qi) / (pow(sf, 2)) - 2 * c / sf;
	bool firstObstacle = true;

	for (int i = 0; i <= 50; i++)
	{
		double delta_s = sf / 50 * i;
		m_testVelocityFactor = 1.0;
		double temp = m_testVelocityFactor * delta_s;
		if (temp > sf)
			temp = sf;

		float q = a * pow(temp, 3) + b * pow(temp, 2) + c * temp + qi;
		double x = 0.0, y = 0.0;
		spline.getXY(delta_s, x, y);
		double _delta_x_, _delta_y_;
		spline.getDeriveXY(delta_s, _delta_x_, _delta_y_);
		double _length_ = sqrt(pow(_delta_x_, 2) + pow(_delta_y_, 2));
		Eigen::Matrix2Xd norm_vec(2, 1), pre(2, 1);
		norm_vec(0, 0) = _delta_x_ / _length_;
		norm_vec(1, 0) = _delta_y_ / _length_;
		pre(0, 0) = x;
		pre(1, 0) = y;
		Eigen::Matrix2Xd result = Topology::rotate(PI / 2, norm_vec)*q + pre;
		RoadPoint tmp;
		tmp.x = result(0, 0);
		tmp.y = result(1, 0);
		tmp.angle = Topology::toAngle(_delta_x_, _delta_y_);
		pts.push_back(tmp);
	}
	return pts;
}


bool PathGenerate::check_circle_area_collision(VeloGrid_t & grids, std::vector<RoadPoint> localPath){

	//liyong yuanxing jinxing pengzhuang jiance
	int index[13] = { 13, 13, 13, 13, 12, 12, 11, 11, 10, 9, 8, 6, 4 };
	for (int ptIndex = 0; ptIndex < localPath.size(); ptIndex++) {
		RoadPoint rpt = localPath[ptIndex];

		for (int i = 0; i < 13; i++){

			int colnum = index[i];
			for (int j = -colnum; j < colnum; j++)
			{
				int pos = (rpt.y - i) * MAP_WIDTH + rpt.x + j;
				if (grids.velo_grid[pos])
					return false;
			}
		}

		//检查下半圆
		for (int i = -12; i <= 0; i++){

			int colnum = index[abs(i)];
			for (int j = -colnum; j < colnum; j++)
			{
				int pos = (rpt.y - i) * MAP_WIDTH + rpt.x + j;
				if (grids.velo_grid[pos])
					return false;
			}
		}
	}
	return true;
	
}
