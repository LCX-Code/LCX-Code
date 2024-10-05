//
// Created by wangy on 2024/1/17.
//

#ifndef _IGWYT_H_
#define _IGWYT_H_

#include "HFS_Problem.h"
#include "Solution.h"
class IGWYT
{
	static double m_lambda;
	static double m_T;
	static int m_d;
 public:
	static std::pair<int, int> Evolution(const HFS_Problem& problem);
	static std::pair<int, int> Evolution1(const HFS_Problem& problem);
	static std::pair<int, int> Evolution2(const HFS_Problem& problem);

	static void set_parameters(double lambda, double t, int d);
	[[nodiscard]] static double get_lambda() ;
	[[nodiscard]] static double get_T() ;
	[[nodiscard]] static int get_d() ;
	static int forward_decode_local_search(Solution& solution, Solution& assistant_solution);
	static int backward_decode_local_search(Solution& solution, Solution& assistant_solution);
	static int forward_decode_local_search_cut_nodes(Solution& solution, int originalMakeSpan);
	static int backward_decode_local_search_cut_nodes(Solution& solution, int originalMakeSpan);

	static int iterative_improvement_insertion_ls_f(Solution& solution, int old_span);
	static int iterative_improvement_insertion_ls_b(Solution& solution, int old_span);
	static int NEH_f(const std::vector<int>& seed_job_sequence, Solution& solution);
	static int NEH_b(const std::vector<int>& seed_job_sequence, Solution& solution);
	static void destruction(Solution& solution, std::vector<int>& erased_jobs) ;
	static int construction_f(Solution& solution, std::vector<int>& erased_jobs);
	static int construction_b(Solution& solution, std::vector<int>& erased_jobs);
	static int local_search(Solution& solution, const std::vector<Node*>& nodes, int originalMakeSpan);
};

inline void IGWYT::set_parameters(double lambda, double t, int d)
{
	m_lambda = lambda;
	m_T = t;
	m_d = d;
}

inline double IGWYT::get_lambda()
{
	return m_lambda;
}

inline double IGWYT::get_T()
{
	return m_T;
}

inline int IGWYT::get_d()
{
	return m_d;
}

#endif //_IGWYT_H_
