

#include <numeric>
#include <iostream>
#include <algorithm>
#include "CSA.h"
#include "utils.h"
SolutionCSA::SolutionCSA(const HFS_Problem& problem)
	: problem(problem)
{
	job_sequence_at_each_stage.resize(problem.getNumOfStages());
	for (auto& seq : job_sequence_at_each_stage)
	{
		seq.resize(problem.getNumOfJobs());
		std::iota(std::begin(seq), std::end(seq), 0);
		std::shuffle(std::begin(seq), std::end(seq), rand_generator());
	}
	d_code = wyt_rand(2);
	os_code = 0;
}

SolutionCSA& SolutionCSA::operator=(const SolutionCSA& solutionCsa)
{
	if (this == &solutionCsa)
	{
		return *this;
	}
	job_sequence_at_each_stage = solutionCsa.job_sequence_at_each_stage;
	d_code = solutionCsa.d_code;
	os_code = solutionCsa.os_code;
	return *this;
}

int SolutionCSA::list_scheduling_forward()
{
	int num_of_jobs = problem.getNumOfJobs();
	int num_of_stages = problem.getNumOfStages();
	const std::vector<Job>& jobs = problem.getJobs();

	//������������깤ʱ��
	std::vector<std::vector<int>> c_time(num_of_jobs, std::vector<int>(num_of_stages));
	//ÿ���׶�ÿ�������Ŀ���ʱ��
	std::vector<std::vector<int>> m_idle_time(num_of_stages);
	for (int s = 0; s < problem.getNumOfStages(); s++)
	{
		m_idle_time[s].resize(problem.getNumOfMachinesInStage(s));
	}

	int span = 0;
	//��һ�׶�
	for (auto job_id : job_sequence_at_each_stage[0])
	{
		int mt = min_element(m_idle_time[0].begin(), m_idle_time[0].end()) - m_idle_time[0].begin();
		c_time[job_id][0] = m_idle_time[0][mt] + jobs[job_id].getProcessTime(0);
		m_idle_time[0][mt] = c_time[job_id][0];
	}

	//�����׶Σ�FIFO��FAM
	for (int s = 1; s < num_of_stages; ++s)
	{
		job_sequence_at_each_stage[s] = job_sequence_at_each_stage[s - 1];
		std::sort(job_sequence_at_each_stage[s].begin(), job_sequence_at_each_stage[s].end(), [&](int job1, int job2)
		{
			return c_time[job1][s - 1] < c_time[job2][s - 1];
		});
		for (int first_come_job_id : job_sequence_at_each_stage[s])
		{
			int mt = min_element(m_idle_time[s].begin(), m_idle_time[s].end()) - m_idle_time[s].begin();
			c_time[first_come_job_id][s] =
				std::max(m_idle_time[s][mt], c_time[first_come_job_id][s - 1])
				+ jobs[first_come_job_id].getProcessTime(s);
			m_idle_time[s][mt] = c_time[first_come_job_id][s];
			if (s == num_of_stages - 1)
			{
				span = std::max(span, c_time[first_come_job_id][s]);
			}
		}
	}
	return span;
}

int SolutionCSA::permutation_scheduling_forward()
{
	int num_of_jobs = problem.getNumOfJobs();
	int num_of_stages = problem.getNumOfStages();
	const std::vector<Job>& jobs = problem.getJobs();

	//������������깤ʱ��
	std::vector<std::vector<int>> c_time(num_of_jobs, std::vector<int>(num_of_stages));
	//ÿ���׶�ÿ�������Ŀ���ʱ��
	std::vector<std::vector<int>> m_idle_time(num_of_stages);
	for (int s = 0; s < problem.getNumOfStages(); s++)
	{
		m_idle_time[s].resize(problem.getNumOfMachinesInStage(s));
	}

	int span = 0;
	//��һ�׶�
	for (auto job_id : job_sequence_at_each_stage[0])
	{
		int mt = min_element(m_idle_time[0].begin(), m_idle_time[0].end()) - m_idle_time[0].begin();
		c_time[job_id][0] = m_idle_time[0][mt] + jobs[job_id].getProcessTime(0);
		m_idle_time[0][mt] = c_time[job_id][0];
	}

	//�����׶�
	for (int s = 1; s < num_of_stages; ++s)
	{
		job_sequence_at_each_stage[s] = job_sequence_at_each_stage[0];
		for (auto job_id : job_sequence_at_each_stage[s])
		{
			int mt = min_element(m_idle_time[s].begin(), m_idle_time[s].end()) - m_idle_time[s].begin();
			c_time[job_id][s] = std::max(m_idle_time[s][mt], c_time[job_id][s - 1]) + jobs[job_id].getProcessTime(s);
			m_idle_time[s][mt] = c_time[job_id][s];

			if (s == num_of_stages - 1)
			{
				span = std::max(span, c_time[job_id][s]);
			}
		}
	}
	return span;
}

int SolutionCSA::non_permutation_scheduling_forward()
{
	int num_of_jobs = problem.getNumOfJobs();
	int num_of_stages = problem.getNumOfStages();
	const std::vector<Job>& jobs = problem.getJobs();

	//������������깤ʱ��
	std::vector<std::vector<int>> c_time(num_of_jobs, std::vector<int>(num_of_stages));
	//ÿ���׶�ÿ�������Ŀ���ʱ��
	std::vector<std::vector<int>> m_idle_time(num_of_stages);
	for (int s = 0; s < problem.getNumOfStages(); s++)
	{
		m_idle_time[s].resize(problem.getNumOfMachinesInStage(s));
	}

	int span = 0;
	//��һ�׶�
	for (auto job_id : job_sequence_at_each_stage[0])
	{
		int mt = min_element(m_idle_time[0].begin(), m_idle_time[0].end()) - m_idle_time[0].begin();
		c_time[job_id][0] = m_idle_time[0][mt] + jobs[job_id].getProcessTime(0);
		m_idle_time[0][mt] = c_time[job_id][0];
	}

	//�����׶�
	for (int s = 1; s < num_of_stages; ++s)
	{
		for (auto job_id : job_sequence_at_each_stage[s])
		{
			int mt = min_element(m_idle_time[s].begin(), m_idle_time[s].end()) - m_idle_time[s].begin();
			c_time[job_id][s] = std::max(m_idle_time[s][mt], c_time[job_id][s - 1]) + jobs[job_id].getProcessTime(s);
			m_idle_time[s][mt] = c_time[job_id][s];

			if (s == num_of_stages - 1)
			{
				span = std::max(span, c_time[job_id][s]);
			}
		}

	}
	return span;
}

int SolutionCSA::list_scheduling_backward()
{
	int num_of_jobs = problem.getNumOfJobs();
	int num_of_stages = problem.getNumOfStages();
	const std::vector<Job>& jobs = problem.getJobs();

	//������������깤ʱ��
	std::vector<std::vector<int>> c_time(num_of_jobs, std::vector<int>(num_of_stages));
	//ÿ���׶�ÿ�������Ŀ���ʱ��
	std::vector<std::vector<int>> m_idle_time(num_of_stages);
	for (int s = 0; s < problem.getNumOfStages(); s++)
	{
		m_idle_time[s].resize(problem.getNumOfMachinesInStage(s));
	}

	int span = 0;
	//���׶�
	for (auto it = job_sequence_at_each_stage[num_of_stages - 1].rbegin();
		 it != job_sequence_at_each_stage[num_of_stages - 1].rend(); ++it)
	{
		int job_id = *it;
		int mt = min_element(m_idle_time[num_of_stages - 1].begin(), m_idle_time[num_of_stages - 1].end())
				 - m_idle_time[num_of_stages - 1].begin();
		c_time[job_id][num_of_stages - 1] =
			m_idle_time[num_of_stages - 1][mt] + jobs[job_id].getProcessTime(num_of_stages - 1);
		m_idle_time[num_of_stages - 1][mt] = c_time[job_id][num_of_stages - 1];
	}

	//�����׶Σ�FIFO��FAM
	for (int s = num_of_stages - 2; s >= 0; --s)
	{
		job_sequence_at_each_stage[s] = job_sequence_at_each_stage[s + 1];
		std::sort(job_sequence_at_each_stage[s].begin(), job_sequence_at_each_stage[s].end(), [&](int job1, int job2)
		{
			return c_time[job1][s + 1] > c_time[job2][s + 1];
		});
		for (auto it = job_sequence_at_each_stage[s].rbegin(); it != job_sequence_at_each_stage[s].rend(); ++it)
		{
			int first_come_job_id = *it;
			int mt = min_element(m_idle_time[s].begin(), m_idle_time[s].end()) - m_idle_time[s].begin();
			c_time[first_come_job_id][s] =
				std::max(m_idle_time[s][mt], c_time[first_come_job_id][s + 1])
				+ jobs[first_come_job_id].getProcessTime(s);
			m_idle_time[s][mt] = c_time[first_come_job_id][s];
			if (s == 0)
			{
				span = std::max(span, c_time[first_come_job_id][s]);
			}
		}
	}
	return span;
}

int SolutionCSA::permutation_scheduling_backward()
{
	int num_of_jobs = problem.getNumOfJobs();
	int num_of_stages = problem.getNumOfStages();
	const std::vector<Job>& jobs = problem.getJobs();

	//������������깤ʱ��
	std::vector<std::vector<int>> c_time(num_of_jobs, std::vector<int>(num_of_stages));
	//ÿ���׶�ÿ�������Ŀ���ʱ��
	std::vector<std::vector<int>> m_idle_time(num_of_stages);
	for (int s = 0; s < problem.getNumOfStages(); s++)
	{
		m_idle_time[s].resize(problem.getNumOfMachinesInStage(s));
	}

	int span = 0;
	//���׶�
	for (auto it = job_sequence_at_each_stage[num_of_stages - 1].rbegin();
		 it != job_sequence_at_each_stage[num_of_stages - 1].rend(); ++it)
	{
		int job_id = *it;
		int mt = min_element(m_idle_time[num_of_stages - 1].begin(), m_idle_time[num_of_stages - 1].end())
				 - m_idle_time[num_of_stages - 1].begin();
		c_time[job_id][num_of_stages - 1] =
			m_idle_time[num_of_stages - 1][mt] + jobs[job_id].getProcessTime(num_of_stages - 1);
		m_idle_time[num_of_stages - 1][mt] = c_time[job_id][num_of_stages - 1];
	}

	//�����׶�
	for (int s = num_of_stages - 2; s >= 0; --s)
	{
		job_sequence_at_each_stage[s] = job_sequence_at_each_stage[num_of_stages - 1];
		for (auto it = job_sequence_at_each_stage[s].rbegin(); it != job_sequence_at_each_stage[s].rend(); ++it)
		{
			int job_id = *it;
			int mt = min_element(m_idle_time[s].begin(), m_idle_time[s].end()) - m_idle_time[s].begin();
			c_time[job_id][s] = std::max(m_idle_time[s][mt], c_time[job_id][s + 1]) + jobs[job_id].getProcessTime(s);
			m_idle_time[s][mt] = c_time[job_id][s];

			if (s == 0)
			{
				span = std::max(span, c_time[job_id][s]);
			}
		}
	}
	return span;
}

int SolutionCSA::non_permutation_scheduling_backward()
{
	int num_of_jobs = problem.getNumOfJobs();
	int num_of_stages = problem.getNumOfStages();
	const std::vector<Job>& jobs = problem.getJobs();

	//������������깤ʱ��
	std::vector<std::vector<int>> c_time(num_of_jobs, std::vector<int>(num_of_stages));
	//ÿ���׶�ÿ�������Ŀ���ʱ��
	std::vector<std::vector<int>> m_idle_time(num_of_stages);
	for (int s = 0; s < problem.getNumOfStages(); s++)
	{
		m_idle_time[s].resize(problem.getNumOfMachinesInStage(s));
	}

	int span = 0;
	//���׶�
	for (auto it = job_sequence_at_each_stage[num_of_stages - 1].rbegin();
		 it != job_sequence_at_each_stage[num_of_stages - 1].rend(); ++it)
	{
		int job_id = *it;
		int mt = min_element(m_idle_time[num_of_stages - 1].begin(), m_idle_time[num_of_stages - 1].end())
				 - m_idle_time[num_of_stages - 1].begin();
		c_time[job_id][num_of_stages - 1] =
			m_idle_time[num_of_stages - 1][mt] + jobs[job_id].getProcessTime(num_of_stages - 1);
		m_idle_time[num_of_stages - 1][mt] = c_time[job_id][num_of_stages - 1];
	}

	//�����׶�
	for (int s = num_of_stages - 2; s >= 0; --s)
	{
		for (auto it = job_sequence_at_each_stage[s].rbegin();
			 it != job_sequence_at_each_stage[s].rend(); ++it)
		{
			int job_id = *it;
			int mt = min_element(m_idle_time[s].begin(), m_idle_time[s].end()) - m_idle_time[s].begin();
			c_time[job_id][s] = std::max(m_idle_time[s][mt], c_time[job_id][s + 1]) + jobs[job_id].getProcessTime(s);
			m_idle_time[s][mt] = c_time[job_id][s];
			if (s == 0)
			{
				span = std::max(span, c_time[job_id][s]);
			}
		}
	}
	return span;
}

void SolutionCSA::swap(int stage_index)
{
	int num_Of_jobs = problem.getNumOfJobs();
	int p1 = wyt_rand(num_Of_jobs);
	int p2;
	do
	{
		p2 = wyt_rand(num_Of_jobs);
	} while (p1 == p2);
	std::swap(job_sequence_at_each_stage[stage_index][p1], job_sequence_at_each_stage[stage_index][p2]);
}

void SolutionCSA::insertion(int stage_index)
{
	int num_Of_jobs = problem.getNumOfJobs();
	int p1 = wyt_rand(num_Of_jobs);
	int p2;
	do
	{
		p2 = wyt_rand(num_Of_jobs);
	} while (p1 == p2);
	int job_id = job_sequence_at_each_stage[stage_index][p2];
	job_sequence_at_each_stage[stage_index].erase(job_sequence_at_each_stage[stage_index].begin() + p2);
	job_sequence_at_each_stage[stage_index].insert(job_sequence_at_each_stage[stage_index].begin() + p1, job_id);
}

void SolutionCSA::reversion(int stage_index)
{
	int num_Of_jobs = problem.getNumOfJobs();
	int p1 = wyt_rand(num_Of_jobs);
	int p2;
	do
	{
		p2 = wyt_rand(num_Of_jobs);
	} while (p1 == p2);

	if (p1 > p2)
	{
		std::reverse(
			job_sequence_at_each_stage[stage_index].begin() + p2,
			job_sequence_at_each_stage[stage_index].begin() + p1 + 1);
	}
	else
	{
		std::reverse(
			job_sequence_at_each_stage[stage_index].begin() + p1,
			job_sequence_at_each_stage[stage_index].begin() + p2 + 1);
	}
}

void SolutionCSA::neighborhood_search()
{
	int num_of_stages = problem.getNumOfStages();
	int ns_mode = wyt_rand(3);
	switch (ns_mode)
	{
	case 0:
		if (os_code == 2)
		{
			int stage_index = wyt_rand(num_of_stages);
			swap(stage_index);
		}
		else
		{
			if (d_code == 0)
			{
				swap(0);
			}
			else
			{
				swap(num_of_stages - 1);
			}
		}
		break;
	case 1:
		if (os_code == 2)
		{
			int stage_index = wyt_rand(num_of_stages);
			insertion(stage_index);
		}
		else
		{
			if (d_code == 0)
			{
				insertion(0);
			}
			else
			{
				insertion(num_of_stages - 1);
			}
		}
		break;
	case 2:
		if (os_code == 2)
		{
			int stage_index = wyt_rand(num_of_stages);
			reversion(stage_index);
		}
		else
		{
			if (d_code == 0)
			{
				reversion(0);
			}
			else
			{
				reversion(num_of_stages - 1);
			}
		}
		break;
	default:
		break;
	}
}

void SolutionCSA::mutation_d_code()
{
	double p = wyt_rand(0.0, 1.0);
	if (p < 0.005)
	{
		d_code = !d_code;
	}
}

void SolutionCSA::mutation_os_code()
{
	double p = wyt_rand(0.0, 1.0);
	if (p < 0.005)
	{
		int r = wyt_rand(1, 2);
		os_code = (os_code + r) % 3;
	}
}

int SolutionCSA::evaluate()
{
	int span;
	switch (d_code)
	{
	case 0:
		switch (os_code)
		{
		case 0:
			span = list_scheduling_forward();
			break;
		case 1:
			span = permutation_scheduling_forward();
			break;
		case 2:
			span = non_permutation_scheduling_forward();
			break;
		}
		break;
	case 1:
		switch (os_code)
		{
		case 0:
			span = list_scheduling_backward();
			break;
		case 1:
			span = permutation_scheduling_backward();
			break;
		case 2:
			span = non_permutation_scheduling_backward();
			break;
		}
		break;
	}
	return span;
}

double CSA::m_lambda;
double CSA::m_t_initial;
int CSA::m_i_iter;
double CSA::m_alpha;
double CSA::m_c_factor;

void CSA::set_parameters( double lambda, double t_initial, int i_iter, double alpha, double c_factor)
{
	m_lambda = lambda;
	m_t_initial = t_initial;
	m_i_iter = i_iter;
	m_alpha = alpha;
	m_c_factor = c_factor;
}

std::pair<int, int> CSA::Evolution(const HFS_Problem& problem)
{
	int num_of_jobs = problem.getNumOfJobs();
	int num_of_stages = problem.getNumOfStages();

	double time_limit = m_lambda * num_of_jobs * num_of_jobs * num_of_jobs * num_of_stages;
	//double time_limit = 2 * num_of_stages * num_of_jobs * 1000;

	long algorithm_start_time = GetElapsedProcessTime();

	int best_span = INT32_MAX;
	int span;

	SolutionCSA solution(problem);
	span = solution.evaluate();
	best_span = span;

	double mu = 1.0;
	double Y_iterator = 0.152;
	double T = m_t_initial;
	double T_iterator;
	int total_iteration_count = 0;
	int iteration_count = 0;

	SolutionCSA original_solution(solution);
	int original_span = span;
	//std::cout << "\titeration" << total_iteration_count << ":\t" << best_span << std::endl;
	while (true)
	{
		T_iterator = T;
		solution.neighborhood_search();
		span = solution.evaluate();

		if (span < best_span)
		{
			best_span = span;
			//std::cout << "\titeration" << total_iteration_count << ":\t" << best_span << std::endl;
		}

		if (span <= original_span)
		{
			original_solution = solution;
			original_span = span;
		}
		else
		{
			Y_iterator = std::fmod(mu / Y_iterator, 1.0);
			T_iterator = T_iterator * (1 + (Y_iterator - 0.5) * m_c_factor);
			double r = wyt_rand(0.0, 1.0);
			if (r < exp(-(span - original_span) / T_iterator))
			{
				original_solution = solution;
				original_span = span;
			}
			else
			{
				solution = original_solution;
				span = original_span;
			}
		}



		++iteration_count;
		++total_iteration_count;

		if (iteration_count >= m_i_iter)
		{
			iteration_count = 0;
			T = T * m_alpha;
		}

		long cur_time = GetElapsedProcessTime();
		long ElapsedTime = cur_time - algorithm_start_time;

		solution.mutation_d_code();
		if (ElapsedTime > 0.4 * time_limit)
		{
			solution.mutation_os_code();
		}

		if (ElapsedTime > time_limit)
		{
			break;
		}
	}

	return { best_span, total_iteration_count };
}
