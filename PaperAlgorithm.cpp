
#include <vector>
#include <chrono>
#include <fstream>
#include <string>
#include <filesystem>
#include <iostream>
#include <algorithm>
#include "PaperAlgorithm.h"
#include "IGWS.h"
#include "utils.h"
#include "IGWYT.h"
#include "CSA.h"
#include "TCSNSA.h"
#include "MetaheuristicAlgorithms.h"
#include "IGLCX.h"

namespace fs = std::filesystem;

void IGWS_main() {
    int repeat_times = 5;
//	IGWS ig_ws{};
    IGWS::set_parameters(0.003, 0.4, 4);

    std::cout << "IGWS_Parameters:" << "\t"
              << "Lambda" << "," << IGWS::get_lambda() << "\t"
              << "T" << "," << IGWS::get_T() << "\t"
              << "d" << "," << IGWS::get_d() << std::endl;

    int instance_no = 0;
    for (const auto &file_path: fs::directory_iterator("../benchmark/Small_Size_Instances")) {
        //std::string file_name = file_path.path().filename().string();
        std::string file_name = "instancia_240_20_10";
        //HFS_Problem problem(file_path.path().string());

        //HFS_Problem problem(10, 4);

        //std::string file_name = "instancia_120_10_1";
        HFS_Problem problem("../benchmark/Big_Size_Instances/instancia_240_20_10.txt");
        //ig_ws.set_problem(problem);

        std::cout << instance_no + 1 << ":"
                  << file_name << ":" << "\t"
                  << problem.getNumOfJobs() << "*" << problem.getNumOfStages()
                  << std::endl;

        for (int i = 0; i < repeat_times; ++i) {
            long StartTime_Ins = GetElapsedProcessTime();
            std::pair<int, int> re = IGWS::Evolution(problem);
            long EndTime_Ins = GetElapsedProcessTime();

            std::cout << "\t"
                      << "Rep" << i + 1 << ":" << "\t"
                      << "make_span" << "," << re.first << "\t"
                      << "time" << "," << EndTime_Ins - StartTime_Ins << "\t"
                      << "iterate" << "," << re.second << std::endl;
        }
        ++instance_no;
    }
}

void IGWYT_main() {
    int repeat_times = 2;

    IGWYT::set_parameters(0.3, 0.4, 4);

    std::chrono::system_clock::time_point time_point_now = std::chrono::system_clock::now();
    std::chrono::system_clock::duration duration_since_epoch
            = time_point_now.time_since_epoch();
    time_t since_epoch = duration_since_epoch.count();
    //std::ofstream o_file_wyt;
    //o_file_wyt.open("../result/IGWYT_Small_Size_benchmark_result" + std::to_string(since_epoch) + ".csv");

    std::cout << "IGWYT_Parameters:" << "\t" << "Lambda" << "," << IGWYT::get_lambda() << "\t"
              << "T" << "," << IGWYT::get_T() << "\t" << "d" << "," << IGWYT::get_d() << std::endl;

    //o_file_wyt << "Instance_Name" << "," << "n" << "," << "s" << "," << "Instance" << ","
    //		   << "J*S" << "," << "make_span" << "," << "time" << "," << "iterate" << "," << std::endl;

    int instance_no = 0;
    for (const auto &file_path: fs::directory_iterator("../benchmark/Carlier_benchmark")) {
        std::string file_name = file_path.path().stem().string();
        std::vector names = StringSplit(file_name, '_');

        //HFS_Problem problem(file_path.path().string());
        HFS_Problem problem("../benchmark/Carlier_benchmark/j10c10c3.txt");
        //HFS_Problem problem(80,10);
        std::cout << instance_no + 1 << ":" << file_name << ":" << "\t"
                  << problem.getNumOfJobs() << "*" << problem.getNumOfStages()
                  << std::endl;

        for (int i = 0; i < repeat_times; ++i) {
            long StartTime_Ins = GetElapsedProcessTime();
            std::pair<int, int> re = IGWYT::Evolution(problem);
            long EndTime_Ins = GetElapsedProcessTime();
            //best_spans_wyt[instance_no][i] = re.first;

            std::cout << "\t" << "IG_WYT" << "\t" << "Rep" << i + 1 << ":" << "\t" << "make_span" << "," << re.first
                      << "\t"
                      << "time" << "," << EndTime_Ins - StartTime_Ins << "\t" << "iterate" << "," << re.second
                      << std::endl;

            //o_file_wyt << file_name << "," << names[1] << "," << names[2] << "," << names[3] << ","
            //		   << problem.getNumOfJobs() << "*" << problem.getNumOfStages() << "," << re.first << ","
            //		   << EndTime_Ins - StartTime_Ins << "," << re.second << "," << std::endl;
        }
    }
    //o_file_wyt.close();
}

void CSA_main() {
    int repeat_times = 5;
    std::vector<std::vector<int>> best_spans;

    //原论文5*0.3*n*m秒
    CSA::set_parameters(0.003, 3.0, 3500, 0.99, 0.5);

    std::cout << "CSA_Parameters:" << "\t" << std::endl;

    int instance_no = 0;
    for (const auto &file_path: fs::directory_iterator("../benchmark/Carlier_benchmark")) {
        std::string file_name = file_path.path().filename().string();
        //std::string file_name = "instancia_240_20_10";
        HFS_Problem problem(file_path.path().string());

        //HFS_Problem problem(10, 4);

        //std::string file_name = "instancia_120_10_1";
        //HFS_Problem problem("../benchmark/Big_Size_Instances/instancia_240_20_10.txt");
        //ig_wyt.set_problem(problem);

        std::cout << instance_no + 1 << ":"
                  << file_name << ":" << "\t"
                  << problem.getNumOfJobs() << "*" << problem.getNumOfStages()
                  << std::endl;

        best_spans.emplace_back(repeat_times, 0);
        for (int i = 0; i < repeat_times; ++i) {
            long StartTime_Ins = GetElapsedProcessTime();
            std::pair<int, int> re = CSA::Evolution(problem);
            long EndTime_Ins = GetElapsedProcessTime();
            best_spans[instance_no][i] = re.first;

            std::cout << "\t"
                      << "Rep" << i + 1 << ":" << "\t"
                      << "make_span" << "," << best_spans[instance_no][i] << "\t"
                      << "time" << "," << EndTime_Ins - StartTime_Ins << "\t"
                      << "iterate" << "," << re.second << std::endl;
        }
        ++instance_no;
    }
}

void TCSNSA_main() {
    int repeat_times = 5;

    //原论文5*0.3*n*m秒
    TCSNSA::set_parameters(0.003, 3.0, 0.95, 3500, 10);;
    std::cout << "TCSNSA_Parameters:" << "\t" << std::endl;
    int instance_no = 0;
    for (const auto &file_path: fs::directory_iterator("../benchmark/Carlier_benchmark")) {
        std::string file_name = file_path.path().filename().string();
        //std::string file_name = "instancia_240_20_10";
        //HFS_Problem problem(file_path.path().string());

        //HFS_Problem problem(4, 1);

        //std::string file_name = "instancia_120_10_1";
        HFS_Problem problem("../benchmark/Carlier_benchmark/j10c10c3.txt");
        //ig_wyt.set_problem(problem);

        std::cout << instance_no + 1 << ":"
                  << file_name << ":" << "\t"
                  << problem.getNumOfJobs() << "*" << problem.getNumOfStages()
                  << std::endl;

        for (int i = 0; i < repeat_times; ++i) {
            long StartTime_Ins = GetElapsedProcessTime();
            std::pair<int, int> re = TCSNSA::Evolution(problem);
            long EndTime_Ins = GetElapsedProcessTime();

            std::cout << "\t"
                      << "Rep" << i + 1 << ":" << "\t"
                      << "make_span1" << "," << re.first << "\t"
                      << "make_span2" << "," << re.second << "\t"
                      << "time" << "," << EndTime_Ins - StartTime_Ins << "\t"
                      << std::endl;
        }
        ++instance_no;
    }
}

void IG_algorithm_main(MetaheuristicAlgorithms::AlgorithmType algorithm_type) {
    int repeat_times = 2;

    MetaheuristicAlgorithms::set_parameters(0.003, 2, 0.5, 0.4, 8);

    std::chrono::system_clock::time_point time_point_now = std::chrono::system_clock::now();
    std::chrono::system_clock::duration duration_since_epoch
            = time_point_now.time_since_epoch();
    time_t since_epoch = duration_since_epoch.count();
    //std::ofstream o_file_ig;
    //o_file_ig.open("../result/IG_Small_Size_benchmark_result" + std::to_string(since_epoch) + ".csv");

    //o_file_ig << "Instance_Name" << "," << "n" << "," << "s" << "," << "Instance" << ","
    //		   << "J*S" << "," << "make_span" << "," << "time" << "," << "iterate" << "," << std::endl;

    int instance_no = 0;
    for (const auto &file_path: fs::directory_iterator("../benchmark/Carlier_benchmark")) {
        std::string file_name = file_path.path().stem().string();
        std::vector names = StringSplit(file_name, '_');

        //HFS_Problem problem(file_path.path().string());
        HFS_Problem problem("../benchmark/Carlier_benchmark/J10c10c3.txt");
        //HFS_Problem problem(80,10);
        std::cout << instance_no + 1 << ":" << file_name << ":" << "\t"
                  << problem.getNumOfJobs() << "*" << problem.getNumOfStages()
                  << std::endl;

        for (int i = 0; i < repeat_times; ++i) {
            long StartTime_Ins = GetElapsedProcessTime();
            std::pair<int, int> re = MetaheuristicAlgorithms::IG_algorithm(problem, algorithm_type);
            long EndTime_Ins = GetElapsedProcessTime();

            std::cout << "\t" << "IG" << "\t" << "Rep" << i + 1 << ":" << "\t" << "make_span" << "," << re.first
                      << "\t"
                      << "time" << "," << EndTime_Ins - StartTime_Ins << "\t" << "iterate" << "," << re.second
                      << std::endl;

            //o_file_ig << file_name << "," << names[1] << "," << names[2] << "," << names[3] << ","
            //		   << problem.getNumOfJobs() << "*" << problem.getNumOfStages() << "," << re.first << ","
            //		   << EndTime_Ins - StartTime_Ins << "," << re.second << "," << std::endl;
        }
    }
    //o_file_ig.close();
}

void VBIH_algorithm_main() {
    int repeat_times = 2;

    MetaheuristicAlgorithms::set_parameters(0.003, 2, 0.5, 0.4, 8);

    std::chrono::system_clock::time_point time_point_now = std::chrono::system_clock::now();
    std::chrono::system_clock::duration duration_since_epoch
            = time_point_now.time_since_epoch();
    time_t since_epoch = duration_since_epoch.count();
    //std::ofstream o_file_ig;
    //o_file_ig.open("../result/IGWYT_Small_Size_benchmark_result" + std::to_string(since_epoch) + ".csv");

    //o_file_ig << "Instance_Name" << "," << "n" << "," << "s" << "," << "Instance" << ","
    //		   << "J*S" << "," << "make_span" << "," << "time" << "," << "iterate" << "," << std::endl;

    int instance_no = 0;
    for (const auto &file_path: fs::directory_iterator("../benchmark/Carlier_benchmark")) {
        std::string file_name = file_path.path().stem().string();
        std::vector names = StringSplit(file_name, '_');

        //HFS_Problem problem(file_path.path().string());
        HFS_Problem problem("../benchmark/Carlier_benchmark/J10c10c3.txt");
        //HFS_Problem problem(80,10);
        std::cout << instance_no + 1 << ":" << file_name << ":" << "\t"
                  << problem.getNumOfJobs() << "*" << problem.getNumOfStages()
                  << std::endl;

        for (int i = 0; i < repeat_times; ++i) {
            long StartTime_Ins = GetElapsedProcessTime();
            std::pair<int, int> re = MetaheuristicAlgorithms::VBIH_algorithm(problem);
            long EndTime_Ins = GetElapsedProcessTime();

            std::cout << "\t" << "VBIH_algorithm" << "\t" << "Rep" << i + 1 << ":" << "\t" << "make_span" << ","
                      << re.first
                      << "\t"
                      << "time" << "," << EndTime_Ins - StartTime_Ins << "\t" << "iterate" << "," << re.second
                      << std::endl;

            //o_file_ig << file_name << "," << names[1] << "," << names[2] << "," << names[3] << ","
            //		   << problem.getNumOfJobs() << "*" << problem.getNumOfStages() << "," << re.first << ","
            //		   << EndTime_Ins - StartTime_Ins << "," << re.second << "," << std::endl;
        }
    }
    //o_file_ig.close();
}

enum Algorithm {
    IGWYT_ALG, IGWYT1_ALG, IGWS_ALG, CSA_ALG, TCANSA_ALG, IGRS_ALG, IGGR_ALG, IGT_ALG, IGALL_ALG, VBIH_ALG, IGLCX_ALG
};

void
run_algorithm(const HFS_Problem &problem, int repeat_times, const std::string &file_name,
              const std::vector<std::string> &names, std::ofstream &o_file, Algorithm alg,std::vector<int>&Mi) {
    int makespan = INT_MAX;
    double AVG = 0;
    auto t = 0;
    for (int i = 0; i < repeat_times; ++i) {
        long StartTime_Ins = GetElapsedProcessTime();
        std::pair<int, int> re;
        switch (alg) {
            case IGWYT_ALG:
                //std::cout << "\t" << "IG_WYT";
                re = IGWYT::Evolution(problem);
                makespan = std::min(makespan, re.first);
                break;
            case IGWYT1_ALG:
                //std::cout << "\t" << "IG_WYT1";
                re = IGWYT::Evolution1(problem);
                break;
            case IGWS_ALG:
                //std::cout << "\t" << "IG_WS";
                re = IGWS::Evolution(problem);
                AVG += re.first;
                makespan = std::min(makespan, re.first);
                t+=re.second;
                break;
            case CSA_ALG:
                //std::cout << "\t" << "IG_CSA";
                re = CSA::Evolution(problem);
                makespan = std::min(makespan, re.first);
                AVG += re.first;

                break;
            case TCANSA_ALG:
                //std::cout << "\t" << "IG_TCANSA";
                re = TCSNSA::Evolution(problem);
                AVG  += re.first;
                makespan = std::min(makespan, re.first);

                break;
            case IGRS_ALG:
                //std::cout << "\t" << "IG_IGRS";
                re = MetaheuristicAlgorithms::IG_algorithm(problem, MetaheuristicAlgorithms::IGRS);
                makespan = std::min(makespan, re.first);
                AVG  += re.first;

                break;
            case IGGR_ALG:
                //std::cout << "\t" << "IG_IGGR";
                re = MetaheuristicAlgorithms::IG_algorithm(problem, MetaheuristicAlgorithms::IGGR);
                makespan = std::min(makespan, re.first);

                break;
            case IGT_ALG:
                //std::cout << "\t" << "IG_IGT";
                re = MetaheuristicAlgorithms::IG_algorithm(problem, MetaheuristicAlgorithms::IGT);
                makespan = std::min(makespan, re.first);

                break;
            case IGALL_ALG:
                //std::cout << "\t" << "IG_IGALL";
                re = MetaheuristicAlgorithms::IG_algorithm(problem, MetaheuristicAlgorithms::IGALL);
                makespan = std::min(makespan, re.first);

                break;
            case VBIH_ALG:
                //  std::cout << "\t" << "IG_VBIH";
                re = MetaheuristicAlgorithms::VBIH_algorithm(problem);
                makespan = std::min(makespan, re.first);
                break;
            case IGLCX_ALG:
                /*std::cout << "\t" << "IG_LCX";*/
                re = IGLCX::Evolution2(problem);
                makespan = std::min(makespan, re.first);
                AVG += re.first;
                //t += re.second;

                break;
        }
        long EndTime_Ins = GetElapsedProcessTime();

        /*   std::cout << "\t" << "Rep" << i + 1 << ":" << "\t" << "make_span" << "," << re.first
                    << "\t"<< "time" << "," << EndTime_Ins - StartTime_Ins << "\t" << "iterate" << "," << re.second
                    << std::endl;*/
/*
		o_file << file_name << "," << names[1] << "," << names[2] << "," << names[3] << ","
			   << problem.getNumOfJobs() << "*" << problem.getNumOfStages() << "," << re.first << ","
			   << EndTime_Ins - StartTime_Ins << "," << re.second << "," << std::endl;*/

    }
   // std::cout << t << std::endl;
     std::cout<<AVG/5<<std::endl;
     Mi.push_back(makespan);
}

void experimental_small_instance() {
    int repeat_times = 5;

    IGWYT::set_parameters(0.003, 0.4, 4);
    IGWS::set_parameters(0.003, 0.4, 4);
    CSA::set_parameters(0.003, 3.0, 3500, 0.99, 0.5);
    TCSNSA::set_parameters(0.003, 3.0, 0.95, 3500, 10);
    MetaheuristicAlgorithms::set_parameters(0.003, 2, 0.5, 0.4, 8);
    IGLCX::set_parameters(0.003, 0.4, 4);

    std::chrono::system_clock::time_point time_point_now = std::chrono::system_clock::now();
    std::chrono::system_clock::duration duration_since_epoch
            = time_point_now.time_since_epoch();
    time_t since_epoch = duration_since_epoch.count();

    IGWYT::set_parameters(0.003, 0.4, 4);
    IGWS::set_parameters(0.003, 0.4, 4);
    CSA::set_parameters(0.003, 3.0, 3500, 0.99, 0.5);
    TCSNSA::set_parameters(0.003, 3.0, 0.95, 3500, 10);;
    MetaheuristicAlgorithms::set_parameters(0.003, 2, 0.5, 0.4, 8);
    IGLCX::set_parameters(0.003, 0.4, 4);

    std::ofstream o_file_wyt;
    std::ofstream o_file_wyt1;
    std::ofstream o_file_ws;
    std::ofstream o_file_csa;
    std::ofstream o_file_tcsnsa;
    std::ofstream o_file_igrs;
    std::ofstream o_file_iggr;
    std::ofstream o_file_igt;
    std::ofstream o_file_igall;
    std::ofstream o_file_vbih;
    std::ofstream o_file_lcx;

    o_file_wyt.open("../result/IGWYT_Small_result" + std::to_string(since_epoch) + ".csv");
    o_file_wyt1.open("../result/IGWYT1_Small_result" + std::to_string(since_epoch) + ".csv");
    o_file_ws.open("../result/IGWS_Small_result" + std::to_string(since_epoch) + ".csv");
    o_file_csa.open("../result/CSA_Small_result" + std::to_string(since_epoch) + ".csv");
    o_file_tcsnsa.open("../result/TCSNSA_Small_result" + std::to_string(since_epoch) + ".csv");
    o_file_igrs.open("../result/IGRS_Small_result" + std::to_string(since_epoch) + ".csv");
    o_file_iggr.open("../result/IGGR_Small_result" + std::to_string(since_epoch) + ".csv");
    o_file_igt.open("../result/IGT_Small_result" + std::to_string(since_epoch) + ".csv");
    o_file_igall.open("../result/IGALL_Small_result" + std::to_string(since_epoch) + ".csv");
    o_file_vbih.open("../result/VBIH_Small_result" + std::to_string(since_epoch) + ".csv");
    o_file_lcx.open("../result/IGLCX_Small_result" + std::to_string(since_epoch) + ".csv");

    o_file_wyt << "Instance_Name" << "," << "n" << "," << "s" << "," << "Instance" << "," << "J*S" << ","
               << "make_span" << "," << "time" << "," << "iterate" << "," << std::endl;
    o_file_wyt1 << "Instance_Name" << "," << "n" << "," << "s" << "," << "Instance" << "," << "J*S" << ","
                << "make_span" << "," << "time" << "," << "iterate" << "," << std::endl;
    o_file_ws << "Instance_Name" << "," << "n" << "," << "s" << "," << "Instance" << "," << "J*S" << ","
              << "make_span" << "," << "time" << "," << "iterate" << "," << std::endl;
    o_file_csa << "Instance_Name" << "," << "n" << "," << "s" << "," << "Instance" << "," << "J*S" << ","
               << "make_span" << "," << "time" << "," << "iterate" << "," << std::endl;
    o_file_tcsnsa << "Instance_Name" << "," << "n" << "," << "s" << "," << "Instance" << "," << "J*S" << ","
                  << "make_span" << "," << "time" << "," << "iterate" << "," << std::endl;
    o_file_igrs << "Instance_Name" << "," << "n" << "," << "s" << "," << "Instance" << "," << "J*S" << ","
                << "make_span" << "," << "time" << "," << "iterate" << "," << std::endl;
    o_file_iggr << "Instance_Name" << "," << "n" << "," << "s" << "," << "Instance" << "," << "J*S" << ","
                << "make_span" << "," << "time" << "," << "iterate" << "," << std::endl;
    o_file_igt << "Instance_Name" << "," << "n" << "," << "s" << "," << "Instance" << "," << "J*S" << ","
               << "make_span" << "," << "time" << "," << "iterate" << "," << std::endl;
    o_file_igall << "Instance_Name" << "," << "n" << "," << "s" << "," << "Instance" << "," << "J*S" << ","
                 << "make_span" << "," << "time" << "," << "iterate" << "," << std::endl;
    o_file_vbih << "Instance_Name" << "," << "n" << "," << "s" << "," << "Instance" << "," << "J*S" << ","
                << "make_span" << "," << "time" << "," << "iterate" << "," << std::endl;
    o_file_lcx << "Instance_Name" << "," << "n" << "," << "s" << "," << "Instance" << "," << "J*S" << ","
               << "make_span" << "," << "time" << "," << "iterate" << "," << std::endl;
    int instance_no = 0;
    std::vector<int> Min;
    //auto start = std::chrono::system_clock::now();
    for (const auto &file_path: fs::directory_iterator("../benchmark/Small_Size_Instances")) {
        std::string file_name = file_path.path().stem().string();
        std::vector<std::string> names = StringSplit(file_name, '_');
        HFS_Problem problem(file_path.path().string());
        /* std::cout << instance_no + 1 << ":" << file_name<<std::endl; << ":" << "\t"
                   << problem.getNumOfJobs() << "*" << problem.getNumOfStages()
                   << std::endl;*/
        //run_algorithm(problem, repeat_times, file_name, names, o_file_iggr, IGGR_ALG);
        // run_algorithm(problem, repeat_times, file_name, names, o_file_wyt, IGWYT_ALG);
        //run_algorithm(problem, repeat_times, file_name, names, o_file_wyt1, IGWYT1_ALG);
        // run_algorithm(problem, repeat_times, file_name, names, o_file_csa, CSA_ALG,Min);
        // run_algorithm(problem, repeat_times, file_name, names, o_file_ws, IGWS_ALG,Min);
       //   run_algorithm(problem, repeat_times, file_name, names, o_file_tcsnsa, TCANSA_ALG,Min);
        //run_algorithm(problem, repeat_times, file_name, names, o_file_igrs, IGRS_ALG);

        // run_algorithm(problem, repeat_times, file_name, names, o_file_igt, IGT_ALG);
        //run_algorithm(problem, repeat_times, file_name, names, o_file_igall, IGALL_ALG);
        //run_algorithm(problem, repeat_times, file_name, names, o_file_vbih, VBIH_ALG);
        run_algorithm(problem, repeat_times, file_name, names, o_file_lcx, IGLCX_ALG,Min);
        //  ++instance_no;
    }
    std::cout<<"________________________________________________________________"<<std::endl;
    for(int i=0;i<Min.size();i++)
        std::cout<<Min[i]<<std::endl;
    /* auto end = std::chrono::system_clock::now();
     auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
     std::cout << "运行时间" << duration.count() << " ms\n";*/
    o_file_wyt.close();
    o_file_wyt1.close();
    o_file_ws.close();
    o_file_csa.close();
    o_file_tcsnsa.close();
    o_file_igrs.close();
    o_file_iggr.close();
    o_file_igt.close();
    o_file_igall.close();
    o_file_vbih.close();
    o_file_lcx.close();
}

void experimental_big_instance() {
    int repeat_times = 5;

    IGWYT::set_parameters(0.003, 0.4, 4);
    IGWS::set_parameters(0.003, 0.4, 4);
    CSA::set_parameters(0.003, 3.0, 3500, 0.99, 0.5);
    TCSNSA::set_parameters(0.003, 3.0, 0.95, 3500, 10);;
    MetaheuristicAlgorithms::set_parameters(0.003, 2, 0.5, 0.4, 8);
    IGLCX::set_parameters(0.003, 0.4, 4);
    std::chrono::system_clock::time_point time_point_now = std::chrono::system_clock::now();
    std::chrono::system_clock::duration duration_since_epoch
            = time_point_now.time_since_epoch();
    time_t since_epoch = duration_since_epoch.count();

    std::ofstream o_file_wyt;
    std::ofstream o_file_wyt1;
    std::ofstream o_file_ws;
    std::ofstream o_file_csa;
    std::ofstream o_file_tcsnsa;
    std::ofstream o_file_igrs;
    std::ofstream o_file_iggr;
    std::ofstream o_file_igt;
    std::ofstream o_file_igall;
    std::ofstream o_file_vbih;
    std::ofstream o_file_lcx;
    o_file_wyt.open("../result/IGWYT_BIg_result" + std::to_string(since_epoch) + ".csv");
    o_file_wyt1.open("../result/IGWYT1_BIg_result" + std::to_string(since_epoch) + ".csv");
    o_file_ws.open("../result/IGWS_BIg_result" + std::to_string(since_epoch) + ".csv");
    o_file_csa.open("../result/CSA_BIg_result" + std::to_string(since_epoch) + ".csv");
    o_file_tcsnsa.open("../result/TCSNSA_BIg_result" + std::to_string(since_epoch) + ".csv");
    o_file_igrs.open("../result/IGRS_BIg_result" + std::to_string(since_epoch) + ".csv");
    o_file_iggr.open("../result/IGGR_BIg_result" + std::to_string(since_epoch) + ".csv");
    o_file_igt.open("../result/IGT_BIg_result" + std::to_string(since_epoch) + ".csv");
    o_file_igall.open("../result/IGALL_BIg_result" + std::to_string(since_epoch) + ".csv");
    o_file_vbih.open("../result/VBIH_BIg_result" + std::to_string(since_epoch) + ".csv");
    o_file_wyt.open("../result/IGLCX_BIg_result" + std::to_string(since_epoch) + ".csv");
    o_file_wyt << "Instance_Name" << "," << "n" << "," << "s" << "," << "Instance" << "," << "J*S" << ","
               << "make_span" << "," << "time" << "," << "iterate" << "," << std::endl;
    o_file_wyt1 << "Instance_Name" << "," << "n" << "," << "s" << "," << "Instance" << "," << "J*S" << ","
                << "make_span" << "," << "time" << "," << "iterate" << "," << std::endl;
    o_file_ws << "Instance_Name" << "," << "n" << "," << "s" << "," << "Instance" << "," << "J*S" << ","
              << "make_span" << "," << "time" << "," << "iterate" << "," << std::endl;
    o_file_csa << "Instance_Name" << "," << "n" << "," << "s" << "," << "Instance" << "," << "J*S" << ","
               << "make_span" << "," << "time" << "," << "iterate" << "," << std::endl;
    o_file_tcsnsa << "Instance_Name" << "," << "n" << "," << "s" << "," << "Instance" << "," << "J*S" << ","
                  << "make_span" << "," << "time" << "," << "iterate" << "," << std::endl;
    o_file_igrs << "Instance_Name" << "," << "n" << "," << "s" << "," << "Instance" << "," << "J*S" << ","
                << "make_span" << "," << "time" << "," << "iterate" << "," << std::endl;
    o_file_iggr << "Instance_Name" << "," << "n" << "," << "s" << "," << "Instance" << "," << "J*S" << ","
                << "make_span" << "," << "time" << "," << "iterate" << "," << std::endl;
    o_file_igt << "Instance_Name" << "," << "n" << "," << "s" << "," << "Instance" << "," << "J*S" << ","
               << "make_span" << "," << "time" << "," << "iterate" << "," << std::endl;
    o_file_igall << "Instance_Name" << "," << "n" << "," << "s" << "," << "Instance" << "," << "J*S" << ","
                 << "make_span" << "," << "time" << "," << "iterate" << "," << std::endl;
    o_file_vbih << "Instance_Name" << "," << "n" << "," << "s" << "," << "Instance" << "," << "J*S" << ","
                << "make_span" << "," << "time" << "," << "iterate" << "," << std::endl;
    o_file_lcx << "Instance_Name" << "," << "n" << "," << "s" << "," << "Instance" << "," << "J*S" << ","
               << "make_span" << "," << "time" << "," << "iterate" << "," << std::endl;
    int instance_no = 0;
    std::vector<int> Min;
    // auto start = std::chrono::system_clock::now();
    for (const auto &file_path: fs::directory_iterator("../benchmark/Small_Size_Instances")) {
        std::string file_name = file_path.path().stem().string();
        std::vector<std::string> names = StringSplit(file_name, '_');
        HFS_Problem problem(file_path.path().string());
        /*  std::cout << instance_no + 1 << ":" << file_name << ":" << "\t"
                    << problem.getNumOfJobs() << "*" << problem.getNumOfStages()
                    << std::endl;*/
        // run_algorithm(problem, repeat_times, file_name, names, o_file_wyt, IGWYT_ALG);
        // run_algorithm(problem, repeat_times, file_name, names, o_file_wyt1, IGWYT1_ALG);
       // run_algorithm(problem, repeat_times, file_name, names, o_file_ws, IGWS_ALG,Min);
         // run_algorithm(problem, repeat_times, file_name, names, o_file_csa, CSA_ALG,Min);
         //run_algorithm(problem, repeat_times, file_name, names, o_file_tcsnsa, TCANSA_ALG,Min);
        // run_algorithm(problem, repeat_times, file_name, names, o_file_igrs, IGRS_ALG);
        //run_algorithm(problem, repeat_times, file_name, names, o_file_iggr, IGGR_ALG);
        //run_algorithm(problem, repeat_times, file_name, names, o_file_igt, IGT_ALG);
        // run_algorithm(problem, repeat_times, file_name, names, o_file_igall, IGALL_ALG);
        //run_algorithm(problem, repeat_times, file_name, names, o_file_vbih, VBIH_ALG);
        //run_algorithm(problem, repeat_times, file_name, names, o_file_wyt, IGLCX_ALG,Min);
        // ++instance_no;
    }
    std::cout<<"________________________________________________________________"<<std::endl;
    for(int i=0;i<Min.size();i++)
        std::cout<<Min[i]<<std::endl;
    /*   auto end = std::chrono::system_clock::now();
       auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
       std::cout << "运行时间" << duration.count() << " ms\n";*/
    o_file_wyt.close();
    o_file_wyt1.close();
    o_file_ws.close();
    o_file_csa.close();
    o_file_tcsnsa.close();
    o_file_igrs.close();
    o_file_iggr.close();
    o_file_igt.close();
    o_file_igall.close();
    o_file_vbih.close();
    o_file_lcx.close();
}

void experimental_Carlier_benchmark() {
    int repeat_times = 5;

    CSA::set_parameters(0.003, 3.0, 3500, 0.99, 0.5);
    IGWS::set_parameters(0.003, 0.4, 4);
    IGWYT::set_parameters(0.003, 0.4, 4);
    TCSNSA::set_parameters(0.003, 3.0, 0.95, 3500, 10);;
    MetaheuristicAlgorithms::set_parameters(0.003, 2, 0.5, 0.4, 8);
    IGLCX::set_parameters(0.003, 0.4, 4);
    std::chrono::system_clock::time_point time_point_now = std::chrono::system_clock::now();
    std::chrono::system_clock::duration duration_since_epoch
            = time_point_now.time_since_epoch();
    time_t since_epoch = duration_since_epoch.count();
    std::ofstream o_file_ws;
    std::ofstream o_file_csa;
    std::ofstream o_file_wyt;
    std::ofstream o_file_wyt1;
    std::ofstream o_file_tcsnsa;
    std::ofstream o_file_igrs;
    std::ofstream o_file_iggr;
    std::ofstream o_file_igt;
    std::ofstream o_file_igall;
    std::ofstream o_file_vbih;
    std::ofstream o_file_lcx;
    o_file_wyt.open("../result/IGWYT_Carlier_result" + std::to_string(since_epoch) + ".csv");
    o_file_wyt1.open("../result/IGWYT1_Carlier_result" + std::to_string(since_epoch) + ".csv");
    o_file_ws.open("../result/IGWS_Carlier_result" + std::to_string(since_epoch) + ".csv");
    o_file_csa.open("../result/CSA_Carlier_result" + std::to_string(since_epoch) + ".csv");
    o_file_tcsnsa.open("../result/TCSNSA_Carlier_result" + std::to_string(since_epoch) + ".csv");
    o_file_igrs.open("../result/IGRS_Carlier_result" + std::to_string(since_epoch) + ".csv");
    o_file_iggr.open("../result/IGGR_Carlier_result" + std::to_string(since_epoch) + ".csv");
    o_file_igt.open("../result/IGT_Carlier_result" + std::to_string(since_epoch) + ".csv");
    o_file_igall.open("../result/IGALL_Carlier_result" + std::to_string(since_epoch) + ".csv");
    o_file_vbih.open("../result/VBIH_Carlier_result" + std::to_string(since_epoch) + ".csv");

    o_file_lcx.open("../result/IGLCX_Carlier_result" + std::to_string(since_epoch) + ".csv");
    o_file_wyt << "Instance_Name" << "," << "J*S" << "," << "make_span" << "," << "time" << "," << "iterate" << ","
               << std::endl;
    o_file_wyt1 << "Instance_Name" << "," << "J*S" << "," << "make_span" << "," << "time" << "," << "iterate" << ","
                << std::endl;
    o_file_ws << "Instance_Name" << "," << "J*S" << "," << "make_span" << "," << "time" << "," << "iterate" << ","
              << std::endl;
    o_file_csa << "Instance_Name" << "," << "J*S" << "," << "make_span" << "," << "time" << "," << "iterate" << ","
               << std::endl;
    o_file_tcsnsa << "Instance_Name" << "," << "J*S" << "," << "make_span" << "," << "time" << "," << "iterate" << ","
                  << std::endl;
    o_file_igrs << "Instance_Name" << "," << "J*S" << "," << "make_span" << "," << "time" << "," << "iterate" << ","
                << std::endl;
    o_file_iggr << "Instance_Name" << "," << "J*S" << "," << "make_span" << "," << "time" << "," << "iterate" << ","
                << std::endl;
    o_file_igt << "Instance_Name" << "," << "J*S" << "," << "make_span" << "," << "time" << "," << "iterate" << ","
               << std::endl;
    o_file_igall << "Instance_Name" << "," << "J*S" << "," << "make_span" << "," << "time" << "," << "iterate" << ","
                 << std::endl;
    o_file_vbih << "Instance_Name" << "," << "J*S" << "," << "make_span" << "," << "time" << "," << "iterate" << ","
                << std::endl;
    o_file_lcx << "Instance_Name" << "," << "J*S" << "," << "make_span" << "," << "time" << "," << "iterate" << ","
               << std::endl;
    int instance_no = 0;
    std::vector<int> Min;
    int t=0;
    double sum=0;
    double span=0;
    //auto start = std::chrono::system_clock::now();
    for (const auto &file_path: fs::directory_iterator("../benchmark/Carlier_benchmark")) {
        std::string file_name = file_path.path().stem().string();
        HFS_Problem problem(file_path.path().string());
        std::vector<std::string> names = {std::string("Instance"), std::to_string(problem.getNumOfJobs()),
                                          std::to_string(problem.getNumOfStages()), std::to_string(1)};
        /*	std::cout << instance_no + 1 << ":" << file_name << ":" << "\t"
                      << problem.getNumOfJobs() << "*" << problem.getNumOfStages()
                      << std::endl;*/
        // std::cout <<  file_name<<std::endl;
        //    run_algorithm(problem, repeat_times, file_name, names, o_file_wyt, IGWYT_ALG);
        //	run_algorithm(problem, repeat_times, file_name, names, o_file_wyt1, IGWYT1_ALG);
        // run_algorithm(problem, repeat_times, file_name, names, o_file_ws, IGWS_ALG,Min);
        // run_algorithm(problem, repeat_times, file_name, names, o_file_csa, CSA_ALG,Min);
         //run_algorithm(problem, repeat_times, file_name, names, o_file_tcsnsa, TCANSA_ALG,Min);
        //run_algorithm(problem, repeat_times, file_name, names, o_file_igrs, IGRS_ALG);
        // run_algorithm(problem, repeat_times, file_name, names, o_file_iggr, IGGR_ALG);
        //run_algorithm(problem, repeat_times, file_name, names, o_file_igt, IGT_ALG);
        //run_algorithm(problem, repeat_times, file_name, names, o_file_igall, IGALL_ALG);
        // run_algorithm(problem, repeat_times, file_name, names, o_file_vbih, VBIH_ALG);
        run_algorithm(problem, repeat_times, file_name, names, o_file_lcx, IGLCX_ALG,Min);
       // t++;
        // ++instance_no;
    }
    std::cout<<"________________________________________________________________"<<std::endl;
    for(int i=0;i<Min.size();i++)
        std::cout<<Min[i]<<std::endl;
    /*  auto end = std::chrono::system_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
      std::cout << "运行时间" << duration.count() << " ms\n";*/
    o_file_ws.close();
    o_file_csa.close();
    o_file_wyt.close();
    o_file_wyt1.close();
    o_file_tcsnsa.close();
    o_file_igrs.close();
    o_file_iggr.close();
    o_file_igt.close();
    o_file_igall.close();
    o_file_vbih.close();
    o_file_lcx.close();
}