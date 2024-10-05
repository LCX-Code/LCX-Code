#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <numeric>
#include <chrono>
#include <cassert>
#include <filesystem>
#include <algorithm>
#include "HFS_Problem.h"
#include "Solution.h"
#include "PyGantt.h"
#include "PaperAlgorithm.h"
#include "IGWS.h"
#include "IGWYT.h"
#include "utils.h"
#include "TCSNSA.h"
#include "CSA.h"
namespace fs = std::filesystem;


int main(int argc, char** argv)
{
	std::cout << "Carlier_benchmark: 1" << std::endl;
	std::cout << "Small instance: 2" << std::endl;
	std::cout << "Big instance: 3" << std::endl;
	int v;
	std::cin >> v;
	switch (v)
	{
	case 1:
		experimental_Carlier_benchmark();
		break;
	case 2:
		experimental_small_instance();
		break;
	case 3:
		experimental_big_instance();
		break;
	default:
		std::cout << "input error" << std::endl;
		break;
	}

	//VBIH_algorithm_main();
	//IGWYT_main();
	//IG_algorithm_main(MetaheuristicAlgorithms::IGT);
	//local_search();
	/*int count = 0;
	int span = 0;
	for(int i = 0; i < 1000; ++i)
	{
		auto re = test_swap_graph1();
		count += re.second;
		span += re.first;
	}
	std::cout << count << "," << span << std::endl;*/
	/*int count = 0;
	int span = 0;
	for(int i = 0; i < 10000; ++i)
	{
		auto re = test_swap_graph();
		count += re.second;
		span += re.first;
	}
	std::cout << count << "," << span << std::endl;*/
	//test_solution_decode();
	//std::cout << d1 << "," << d2 << std::endl;
	//int span2f = s1.decode_forward_to_graph();

	//正向甘特图
	//PyGantt::ganttAllCriticalNodes(s1, {},{}, "Instance", span1f,
	//	false, true);

	/*std::cout << "IGWS: 1" << std::endl;
	std::cout << "IGWYT: 2" << std::endl;
	std::cout << "CSA: 3" << std::endl;
	std::cout << "TCSNSA: 4" << std::endl;
	int v;
	std::cin >> v;
	switch (v)
	{
	case 1:
		IGWS_main();
		break;
	case 2:
		IGWYT_main();
		break;
	case 3:
		CSA_main();
		break;
	case 4:
		TCSNSA_main();
		break;
	default:
		std::cout << "input error" << std::endl;
		break;
	}*/


	//IGWS_main();
	//IGWYT_main();
	//CSA_main();
	//TCSNSA_main();

	//CSA_main();
	//experimental_small_instance();
	//std::chrono::time_point<std::chrono::steady_clock> st;
	//std::chrono::time_point<std::chrono::steady_clock> et;
	//test_cut();
	//IGWS_main();
	//IGWYT_main();
	return 0;

	return 0;
}
