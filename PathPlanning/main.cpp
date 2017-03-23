/*
	author: heryms
	created on 2017-3-18
*/
#include<iostream>
#include "CoordTransform.h"
#include "BaseType.h"
#include "Variables.h"
#include <fstream>
#include "testModule.h"
#include <vector>
#include "DataCenter.h"
#include "PathGenerate.h"
#include <chrono>
int main()
{
	//CoordTransform transform;
	//test_Clothoid();
	DataCenter::GetInstance().StartAllSensor();
	__thread_sleep_for(1000);
	PathGenerate pathGen;
	while (true)
	{
		if (!DataCenter::GetInstance().WaitForVeloGrid(20)) {
			continue;
		}
		std::chrono::steady_clock::time_point startTime
			= std::chrono::steady_clock::now();
		pathGen.path_generate();
		std::chrono::steady_clock::time_point endTime
			= std::chrono::steady_clock::now();
		std::chrono::milliseconds time
			= std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
		std::cout << time.count() << std::endl;
	}
	DataCenter::GetInstance().EndAllSensor();
	return 0;
}