
/*
 *Fernandez-Viagas, V. (2022). A speed-up procedure for the hybrid flow shop scheduling problem. Expert Systems with Applications, 187, 115903. https://doi.org/10.1016/j.eswa.2021.115903
*/

#ifndef _IGWS_H_
#define _IGWS_H_

#include "HFS_Problem.h"
#include "Solution.h"
class IGWS
{
	static double m_lambda;
	static double m_T;
	static int m_d;
 public:
	static std::pair<int, int> Evolution(const HFS_Problem& problem);

	static void set_parameters(double lambda, double t, int d);
	[[nodiscard]] static double get_lambda() ;
	[[nodiscard]] static double get_T() ;
	[[nodiscard]] static int get_d() ;
	static int forward_decode_local_search_critical_nodes(Solution& solution, int originalMakeSpan);

    static std::pair<int, int> Evolution_Speed(const HFS_Problem &problem);

private:
	static int NEH_f(const std::vector<int>& seed_job_sequence, Solution& solution);
	static int iterative_improvement_insertion(Solution& solution, int old_span);
	static void destruction(Solution& solution, std::vector<int>& erased_jobs);
	static int construction(Solution& solution, std::vector<int>& erased_jobs);
	static int local_search(Solution& solution, const std::vector<Node*>& nodes, int originalMakeSpan);

};

inline void IGWS::set_parameters(double lambda, double t, int d)
{
	m_lambda = lambda;
	m_T = t;
	m_d = d;
}

inline double IGWS::get_lambda()
{
	return m_lambda;
}

inline double IGWS::get_T()
{
	return m_T;
}

inline int IGWS::get_d()
{
	return m_d;
}

#endif //_IGWS_H_
