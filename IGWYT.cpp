//
// Created by wangy on 2024/1/17.
//

#include <vector>
#include <numeric>
#include <iostream>
#include <cassert>
#include <chrono>
#include "IGWYT.h"
#include "Solution.h"
#include "utils.h"
#include "PyGantt.h"
#include <algorithm>

double IGWYT::m_lambda = 0.0;
double IGWYT::m_T = 0.0;
int IGWYT::m_d = 0;

int IGWYT::NEH_f(const std::vector<int> &seed_job_sequence, Solution &solution) {

    return 0;
}

int IGWYT::NEH_b(const std::vector<int> &seed_job_sequence, Solution &solution) {

    return 0;
}

int IGWYT::iterative_improvement_insertion_ls_f(Solution &solution, int old_span) {

    return 0;
}

int IGWYT::iterative_improvement_insertion_ls_b(Solution &solution, int old_span) {

    return 0;
}

void IGWYT::destruction(Solution &solution, std::vector<int> &erased_jobs) {

}

int IGWYT::construction_f(Solution &solution, std::vector<int> &erased_jobs) {

    return 0;
}

int IGWYT::construction_b(Solution &solution, std::vector<int> &erased_jobs) {
    return 0;
}

int IGWYT::forward_decode_local_search(Solution &solution, Solution &assistant_solution) {

    return 0;
}

int IGWYT::backward_decode_local_search(Solution &solution, Solution &assistant_solution) {

    return 0;
}

int IGWYT::forward_decode_local_search_cut_nodes(Solution &solution, int originalMakeSpan) {

    return 0;
}

int IGWYT::backward_decode_local_search_cut_nodes(Solution &solution, int originalMakeSpan) {

    return 0;
}

std::pair<int, int> IGWYT::Evolution(const HFS_Problem &problem) {
    return {0,0};
}

std::pair<int, int> IGWYT::Evolution1(const HFS_Problem &problem) {
    return {0,0};
}

int local_search_t_f(Solution &solution_f) {
    return 0;
}

int local_search_t_b(Solution &solution_b) {
    return 0;
}

std::pair<int, int> local_search_t(Solution &solution_f, Solution &solution_b) {

    return {1, 2};
}

std::pair<int, int> IGWYT::Evolution2(const HFS_Problem &problem) {


    return {1, 2};
}