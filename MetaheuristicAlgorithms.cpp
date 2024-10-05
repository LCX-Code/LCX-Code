

#include <numeric>
#include "MetaheuristicAlgorithms.h"
#include "utils.h"
#include <algorithm>
SolutionMA::SolutionMA(const HFS_Problem& problem)
	: problem(problem)
{
	make_span = 0;
}

SolutionMA& SolutionMA::operator=(const SolutionMA& solution)
{
	if (this == &solution)
	{
		return *this;
	}
	job_sequence = solution.job_sequence;
	make_span = solution.make_span;
	return *this;
}

int SolutionMA::evaluate() const
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
	for (auto job_id : job_sequence)
	{
		int mt = min_element(m_idle_time[0].begin(), m_idle_time[0].end()) - m_idle_time[0].begin();
		c_time[job_id][0] = m_idle_time[0][mt] + jobs[job_id].getProcessTime(0);
		m_idle_time[0][mt] = c_time[job_id][0];
	}

	std::vector<int> sequence = job_sequence;
	//�����׶Σ�FIFO��FAM
	for (int s = 1; s < num_of_stages; ++s)
	{
		std::sort(sequence.begin(), sequence.end(), [&](int job1, int job2)
		{
			return c_time[job1][s - 1] < c_time[job2][s - 1];
		});
		for (int first_come_job_id : sequence)
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

std::pair<int, int> SolutionMA::find_best_position(int inserted_job_id)
{
	int min_span = INT_MAX;
	int best_pos;
	for (int pos = 0; pos <= job_sequence.size(); ++pos)
	{
		job_sequence.insert(job_sequence.begin() + pos, inserted_job_id);
		int span = evaluate();
		if (span < min_span)
		{
			min_span = span;
			best_pos = pos;
		}
		job_sequence.erase(job_sequence.begin() + pos);
	}
	return { min_span, best_pos };
}

std::pair<int, int> SolutionMA::find_best_position_block(const std::vector<int>& erased_jobs)
{
	int block_size = erased_jobs.size();
	int min_span = INT_MAX;
	int best_pos;
	for (int pos = 0; pos <= job_sequence.size(); ++pos)
	{
		job_sequence.insert(job_sequence.begin() + pos, erased_jobs.begin(), erased_jobs.end());
		int span = evaluate();
		if (span < min_span)
		{
			min_span = span;
			best_pos = pos;
		}
		job_sequence.erase(job_sequence.begin() + pos, job_sequence.begin() + pos + block_size);
	}
	return { min_span, best_pos };
}

std::pair<int, int> SolutionMA::find_best_swap(int swapped_index)
{
	int min_span = INT_MAX;
	int best_index;
	for (int index = 0; index < job_sequence.size(); ++index)
	{
		if (index == swapped_index)
		{
			continue;
		}
		std::swap(job_sequence[index], job_sequence[swapped_index]);
		int span = evaluate();
		if (span < min_span)
		{
			min_span = span;
			best_index = index;
		}
		std::swap(job_sequence[index], job_sequence[swapped_index]);
	}
	return { min_span, best_index };
}

void SolutionMA::destruction(std::vector<int>& erased_jobs, int d)
{

	int k = 0;
	while (k < d)
	{
		int pos = wyt_rand(job_sequence.size());
		erased_jobs.emplace_back(job_sequence[pos]);
		job_sequence.erase(job_sequence.begin() + pos);
		++k;
	}
}

void SolutionMA::destruction_block(std::vector<int>& erased_jobs, int block_size)
{
	int pos = wyt_rand(job_sequence.size() - block_size + 1);
	erased_jobs.insert(erased_jobs.begin(), job_sequence.begin() + pos, job_sequence.begin() + pos + block_size);
	job_sequence.erase(job_sequence.begin() + pos, job_sequence.begin() + pos + block_size);
}

double MetaheuristicAlgorithms::m_lambda;
int MetaheuristicAlgorithms::m_d;
double MetaheuristicAlgorithms::m_T;
double MetaheuristicAlgorithms::m_jP;
int MetaheuristicAlgorithms::m_block_max;

void MetaheuristicAlgorithms::set_parameters(double lambda, int d, double t, double jp, int block_max )
{
	m_lambda = lambda;
	m_d = d;
	m_T = t;
	m_jP = jp;
	m_block_max = block_max;
}

SolutionMA MetaheuristicAlgorithms::NEH(const HFS_Problem& problem, const std::vector<int>& guiding_job_sequence)
{
	SolutionMA solution(problem);
	int span;
	for (auto job_id : guiding_job_sequence)
	{
		auto re = solution.find_best_position(job_id);
		span = re.first;
		solution.insert_job_at_position(job_id, re.second);
	}
	solution.set_span(span);
	return solution;
}

SolutionMA
MetaheuristicAlgorithms::GRASP_NEH(const HFS_Problem& problem, const std::vector<int>& guiding_job_sequence, int x)
{
	int num_of_jobs = problem.getNumOfJobs();
	std::vector<SolutionMA> solution_vec(x, SolutionMA(problem));

	for (int h = 0; h < x; ++h)
	{
		std::vector<int> U(num_of_jobs);
		std::iota(U.begin(), U.end(), 0);
		int first_job = guiding_job_sequence[h];
		solution_vec[h].emplace_back_one_job(first_job);
		std::swap(U[0], U[first_job]);
		GRASP(solution_vec[h], U);
		SolutionMA solution2 = NEH(problem, solution_vec[h].get_job_sequence());
		if (solution2.get_span() < solution_vec[h].get_span())
		{
			solution_vec[h] = solution2;
		}
	}
	return *std::min_element(solution_vec.begin(), solution_vec.end(), [](const auto& sol1, const auto& sol2)
	{
		return sol1.get_span() < sol2.get_span();
	});
}

void MetaheuristicAlgorithms::GRASP(SolutionMA& solution, std::vector<int>& U)
{
	double alpha = 0.1;
	const HFS_Problem& problem = solution.get_problem();
	int num_of_jobs = problem.getNumOfJobs();

	std::vector<int> CF(num_of_jobs);
	std::vector<int> RCL(num_of_jobs);
	int span;
	int CF_min;
	int CF_max;
	for (int i = 1; i < num_of_jobs; ++i)
	{
		CF_min = INT32_MAX;
		CF_max = INT32_MIN;
		for (int j = i; j < num_of_jobs; ++j)
		{
			solution.emplace_back_one_job(U[j]);
			span = solution.evaluate();
			CF[j] = span;
			CF_min = std::min(span, CF_min);
			CF_max = std::max(span, CF_max);
			solution.remove_last_job();
		}

		int k = 0;
		for (int j = i; j < num_of_jobs; ++j)
		{
			if (CF[j] - CF_min <= alpha * (CF_max - CF_min))
			{
				RCL[k] = j;
				++k;
			}
		}

		int u_index = RCL[wyt_rand(k)];
		solution.emplace_back_one_job(U[u_index]);
		std::swap(U[i], U[u_index]);
	}
	solution.set_span(span);
}

void MetaheuristicAlgorithms::first_improvement_insertion_local_search(SolutionMA& solution)
{
	//const HFS_Problem& problem = solution.get_problem();
	int num_of_jobs = solution.get_job_sequence().size();

	bool improvement = true;
	while (improvement)
	{
		improvement = false;
		for (int i = 0; i < num_of_jobs; ++i)
		{
			int job_id = solution.get_job(i);
			solution.remove_one_job_by_index(i);
			auto re = solution.find_best_position(job_id);
			if (re.first < solution.get_span())
			{
				solution.insert_job_at_position(job_id, re.second);
				improvement = true;
				solution.set_span(re.first);
			}
			else
			{
				solution.insert_job_at_position(job_id, i);
			}
		}
	}
}

void MetaheuristicAlgorithms::RIS_local_search(SolutionMA& solution, const SolutionMA& best_solution)
{
	const HFS_Problem& problem = solution.get_problem();
	int num_of_jobs = problem.getNumOfJobs();

	int iter = 0;
	int pos = 0;
	while (iter < num_of_jobs)
	{
		int job_id = best_solution.get_job(pos);
		pos = (pos + 1) % num_of_jobs;
		int old_position = solution.remove_one_job_by_id(job_id);
		auto re = solution.find_best_position(job_id);
		if (re.first < solution.get_span())
		{
			solution.insert_job_at_position(job_id, re.second);
			solution.set_span(re.first);
			iter = 1;
		}
		else
		{
			solution.insert_job_at_position(job_id, old_position);
			++iter;
		}
	}
}

void MetaheuristicAlgorithms::RSS_local_search(SolutionMA& solution, const SolutionMA& best_solution)
{
	const HFS_Problem& problem = solution.get_problem();
	int num_of_jobs = problem.getNumOfJobs();

	int iter = 0;
	int pos = 0;
	while (iter < num_of_jobs)
	{
		int job_id = best_solution.get_job(pos);
		pos = (pos + 1) % num_of_jobs;

		int swapped_index = solution.get_index(job_id);
		auto re = solution.find_best_swap(swapped_index);

		if (re.first < solution.get_span())
		{
			solution.swap_job_by_index(swapped_index, re.second);
			solution.set_span(re.first);
			iter = 1;
		}
		else
		{
			++iter;
		}
	}
}

void MetaheuristicAlgorithms::destruction(SolutionMA& solution, std::vector<int>& erased_jobs)
{
	solution.destruction(erased_jobs, m_d);
}

void MetaheuristicAlgorithms::destruction_block(SolutionMA& solution, std::vector<int>& erased_jobs, int block_size)
{
	solution.destruction_block(erased_jobs, block_size);
}

void MetaheuristicAlgorithms::construction(SolutionMA& solution, std::vector<int>& erased_jobs)
{
	int span;
	for (auto job_id : erased_jobs)
	{
		auto re = solution.find_best_position(job_id);
		span = re.first;
		solution.insert_job_at_position(job_id, re.second);
	}
	solution.set_span(span);
}

void MetaheuristicAlgorithms::construction_block(SolutionMA& solution, std::vector<int>& erased_jobs)
{
	auto re = solution.find_best_position_block(erased_jobs);
	solution.insert_block_at_position(erased_jobs, re.second);
	solution.set_span(re.first);
}

std::pair<int, int> MetaheuristicAlgorithms::IG_algorithm(const HFS_Problem& problem, AlgorithmType algorithm_type)
{
	int num_of_jobs = problem.getNumOfJobs();
	int num_of_stages = problem.getNumOfStages();
	int total_process_time = problem.get_total_process_time();

	double time_limit = m_lambda * num_of_jobs * num_of_jobs * num_of_jobs * num_of_stages;
	//double time_limit = 2 * num_of_stages * num_of_jobs * 1000;

	long algorithm_start_time = GetElapsedProcessTime();

	std::vector<int> guiding_job_sequence(num_of_jobs);
	std::iota(std::begin(guiding_job_sequence), std::end(guiding_job_sequence), 0);
	std::shuffle(std::begin(guiding_job_sequence), std::end(guiding_job_sequence), rand_generator());
	std::sort(guiding_job_sequence.begin(), guiding_job_sequence.end(), [&](int j1, int j2)
	{
		return problem.getJobs()[j1].get_total_process_time() > problem.getJobs()[j2].get_total_process_time();
	});
	SolutionMA solution(problem);

	switch (algorithm_type)
	{
	case IGRS:
		solution = NEH(problem, guiding_job_sequence);
		break;
	case IGGR:
	case IGT:
	case IGALL:
		solution = GRASP_NEH(problem, guiding_job_sequence, num_of_jobs);
		break;
	}

	SolutionMA best_solution = solution;
	SolutionMA solution0 = solution;
	int total_iteration_count = 0;
	std::vector<int> erased_jobs;
	while (true)
	{
		erased_jobs.clear();
		destruction(solution, erased_jobs);
		switch (algorithm_type)
		{
		case IGRS:
		case IGGR:
		case IGT:
			break;
		case IGALL:
			first_improvement_insertion_local_search(solution);
			break;
		}
		construction(solution, erased_jobs);
		switch (algorithm_type)
		{
		case IGRS:
		case IGGR:
			first_improvement_insertion_local_search(solution);
			break;
		case IGT:
		case IGALL:
			double r1 = wyt_rand(0.0, 1.0);
			if (r1 < m_jP)
			{
				RIS_local_search(solution, best_solution);
			}
			else
			{
				RSS_local_search(solution, best_solution);
			}
			break;
		}

		if (solution.get_span() <= solution0.get_span())
		{
			solution0 = solution;
			if (solution.get_span() < best_solution.get_span())
			{
				best_solution = solution;
			}
		}
		else
		{
			double r2 = wyt_rand(0.0, 1.0);
			if (r2 < std::exp(-(solution.get_span() - solution0.get_span()) * num_of_jobs * num_of_stages * 10
							  / (m_T * total_process_time)))
			{
				solution0 = solution;
			}
			else
			{
				solution = solution0;
			}
		}
		++total_iteration_count;
		long cur_time = GetElapsedProcessTime();
		long ElapsedTime = cur_time - algorithm_start_time;
		if (ElapsedTime > time_limit)
		{
			break;
		}
	}
	return { best_solution.get_span(), total_iteration_count };
}

std::pair<int, int> MetaheuristicAlgorithms::VBIH_algorithm(const HFS_Problem& problem)
{
	int block_min = 2;
	int num_of_jobs = problem.getNumOfJobs();
	int num_of_stages = problem.getNumOfStages();
	int total_process_time = problem.get_total_process_time();

	double time_limit = m_lambda * num_of_jobs * num_of_jobs * num_of_jobs * num_of_stages;
	//double time_limit = 2 * num_of_stages * num_of_jobs * 1000;

	long algorithm_start_time = GetElapsedProcessTime();

	std::vector<int> guiding_job_sequence(num_of_jobs);
	std::iota(std::begin(guiding_job_sequence), std::end(guiding_job_sequence), 0);
	std::shuffle(std::begin(guiding_job_sequence), std::end(guiding_job_sequence), rand_generator());
	std::sort(guiding_job_sequence.begin(), guiding_job_sequence.end(), [&](int j1, int j2)
	{
		return problem.getJobs()[j1].get_total_process_time() > problem.getJobs()[j2].get_total_process_time();
	});

	SolutionMA solution = GRASP_NEH(problem, guiding_job_sequence, num_of_jobs);

	SolutionMA best_solution = solution;
	SolutionMA solution0 = solution;
	int total_iteration_count = 0;
	std::vector<int> erased_jobs;
	int block_size;
	while (true)
	{
		block_size = block_min;
		do
		{
			erased_jobs.clear();
			destruction_block(solution, erased_jobs, block_size);
			first_improvement_insertion_local_search(solution);
			construction_block(solution, erased_jobs);

			double r1 = wyt_rand(0.0, 1.0);
			if (r1 < m_jP)
			{
				RIS_local_search(solution, best_solution);
			}
			else
			{
				RSS_local_search(solution, best_solution);
			}

			if (solution.get_span() >= solution0.get_span())
			{
				++block_size;
			}

			if (solution.get_span() <= solution0.get_span())
			{
				solution0 = solution;
				if (solution.get_span() < best_solution.get_span())
				{
					best_solution = solution;
				}
			}
			else
			{
				double r2 = wyt_rand(0.0, 1.0);
				if (r2 < std::exp(-(solution.get_span() - solution0.get_span()) * num_of_jobs * num_of_stages * 10
								  / (m_T * total_process_time)))
				{
					solution0 = solution;
				}
				else
				{
					solution = solution0;
				}
			}
		} while (block_size <= m_block_max);

		long cur_time = GetElapsedProcessTime();
		long ElapsedTime = cur_time - algorithm_start_time;
		if (ElapsedTime > time_limit)
		{
			break;
		}
	}
	return { best_solution.get_span(), total_iteration_count };
}
