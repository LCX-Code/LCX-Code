//
// Created by 30565 on 2024/2/6.
//

#ifndef HFSP_CRITICAL_PATH_IGLCX_H
#define HFSP_CRITICAL_PATH_IGLCX_H

#endif //HFSP_CRITICAL_PATH_IGLCX_H

#include "HFS_Problem.h"
#include "Solution.h"


class IGLCX
{
    static double m_T;
    static int m_d;
    static double m_lambda;


public:
    static std::pair<int, int> Evolution1(const HFS_Problem &problem);
    static std::pair<int, int> Evolution2(const HFS_Problem &problem);
    static void set_parameters(double lambda,double t,int d);
public:
    static Solution NEH(const HFS_Problem &problem, const std::vector<int> &guiding_job_sequence);
    static int NEH_f(const std::vector<int>& seed_job_sequence, Solution& solution);
    static int iterative_improvement_insertion(Solution& solution, int old_span);
    static void destruction(Solution& solution, std::vector<int>& erased_jobs);
    static int construction(Solution& solution, std::vector<int>& erased_jobs);
    static int local_search(Solution& solution, const std::vector<Node*>& nodes, int originalMakeSpan);
    static int forward_decode_local_search_cut_nodes(Solution &solution, int originalMakeSpan);
    static int forward_decode_local_search(Solution &solution, Solution &assistant_solution);
    static void GRASP(Solution& solution, std::vector<int>& U);
    static Solution GRASP_NEH(const HFS_Problem& problem, const std::vector<int>& guiding_job_sequence, int x);
    static void destruction_block(Solution &solution, std::vector<int> &erased_jobs, int block_size);
    static void construction_block(Solution& solution, std::vector<int>& erased_jobs);

    static int RIS_local_search_f(Solution &solution, int old_span, const Solution &best_solution);

    static int RSS_local_search_f(Solution &solution, int old_span, const Solution &best_solution);

    static int Swap(Solution &solution);

    static int Shift(Solution &solution);

    static int ZBSwap(Solution &solution);

    static int localSearch(Solution &solution);

   // static void destruction1(Solution &solution, std::vector<int> &erased_jobs, bool flag);


    static void destruction1(Solution &solution, std::vector<int> &erased_jobs, bool flag, int r);

    static std::pair<int, int> Evolution_Speed(const HFS_Problem &problem);
};



