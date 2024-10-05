
#include <numeric>
#include <algorithm>
#include <iostream>
#include <cassert>
#include <chrono>
#include <algorithm>
#include "IGWS.h"
#include "HFS_Problem.h"
#include "Solution.h"
#include "utils.h"

double IGWS::m_lambda = 0.0;
double IGWS::m_T = 0.0;
int IGWS::m_d = 0;

int IGWS::NEH_f(const std::vector<int>& seed_job_sequence, Solution& solution)
{
	int span;
	for (auto job_id : seed_job_sequence)
	{
		auto re = solution.find_best_insert_position_f(job_id);
		span = re.first;
		solution.insert_job_at_position(job_id, re.second);
	}
	return span;
}

int IGWS::iterative_improvement_insertion(Solution& solution, int old_span)
{
	const HFS_Problem& problem = solution.getProblem();
	int num_of_jobs = problem.getNumOfJobs();

	bool improvement = true;
	while (improvement)
	{
		improvement = false;
		for (int i = 0; i < num_of_jobs; ++i)
		{
			int job_id = solution.get_job(i);
			solution.remove_one_job_by_index(i);
			auto re = solution.find_best_insert_position_f(job_id);
			if (re.first < old_span)
			{
				solution.insert_job_at_position(job_id, re.second);
				improvement = true;
				old_span = re.first;
			}
			else
			{
				solution.insert_job_at_position(job_id, i);
			}
		}
	}
	return old_span;
}

void IGWS::destruction(Solution& solution, std::vector<int>& erased_jobs)
{
	solution.destruction(erased_jobs, m_d);
}

int IGWS::construction(Solution& solution, std::vector<int>& erased_jobs)
{
	int span;
	for (auto job_id : erased_jobs)
	{
		auto re = solution.find_best_insert_position_f(job_id);
		span = re.first;
		solution.insert_job_at_position(job_id, re.second);
	}
	return span;
}

int IGWS::local_search(Solution& solution, const std::vector<Node*>& nodes, int originalMakeSpan)
{
	const HFS_Problem& problem = solution.getProblem();
	for (auto deleteCriticalNode : nodes)
	{
		Node* originalPreNodeOnMachine = deleteCriticalNode->preByMachine;
		Node* preNodeAtStage = deleteCriticalNode->preByStage;
		Node* sucNodeAtStage = deleteCriticalNode->sucByStage;

		int stageId = deleteCriticalNode->stageId;
		solution.deleteNodePhysicalFromGraph(deleteCriticalNode);
		int makeSpanAfterDeleteNode = solution.refreshAllGraph();

		const auto& heads = solution.getHeadOnMachinesInStage(stageId);
		const auto& tails = solution.getTailOnMachinesInStage(stageId);
		int machineCount = problem.getNumOfMachinesInStage(stageId);
		int bestSpan = originalMakeSpan;
		Node* nodeBeforeBestInsertPos;
		for (int mid = 0; mid < machineCount; ++mid)
		{
			Node* nodeBeforeInsertPos = heads[mid];
			Node* nodeAfterInsertPos = nodeBeforeInsertPos->sucByMachine;

			while (nodeBeforeInsertPos != tails[mid])
			{
				int longestValue =
					std::max(preNodeAtStage->f_startTime + preNodeAtStage->processTime,
						nodeBeforeInsertPos->f_startTime + nodeBeforeInsertPos->processTime)
					+ deleteCriticalNode->processTime +
					std::max(sucNodeAtStage->b_startTime + sucNodeAtStage->processTime,
						nodeAfterInsertPos->b_startTime + nodeAfterInsertPos->processTime);
				int makeSpanAfterInsert = std::max(longestValue, makeSpanAfterDeleteNode);
				if (makeSpanAfterInsert < bestSpan)
				{
					bestSpan = makeSpanAfterInsert;
					nodeBeforeBestInsertPos = nodeBeforeInsertPos;
				}
				nodeBeforeInsertPos = nodeAfterInsertPos;
				nodeAfterInsertPos = nodeBeforeInsertPos->sucByMachine;
			}
		}
		if (bestSpan < originalMakeSpan)
		{
			solution.insertNodeInPosition(deleteCriticalNode, nodeBeforeBestInsertPos, preNodeAtStage);
			//solution.setMakeSpan(bestSpan);
			int new_span = solution.refreshAllGraph();
			//assert(a == bestSpan);
			return new_span;
		}
		else
		{
			solution.insertNodeInPosition(deleteCriticalNode, originalPreNodeOnMachine, preNodeAtStage);
		}
	}
	return originalMakeSpan;
}

int IGWS::forward_decode_local_search_critical_nodes(Solution& solution, int originalMakeSpan)
{
	std::vector<Node*> critical_nodes;
	std::vector<Node*> cut_nodes;

	while (true)
	{
		critical_nodes.clear();
		solution.findAllCriticalNodesForward(critical_nodes, originalMakeSpan);
       // solution.findCutNodesForward(critical_nodes,cut_nodes, originalMakeSpan);
		int new_span = local_search(solution, critical_nodes, originalMakeSpan);
		if (new_span < originalMakeSpan)
		{
			originalMakeSpan = new_span;
		}
		else
		{
			break;
		}
	}
	return originalMakeSpan;
}

std::pair<int, int> IGWS::Evolution(const HFS_Problem& problem)
{
	int num_of_jobs = problem.getNumOfJobs();
	int num_of_stages = problem.getNumOfStages();
	int total_process_time = problem.get_total_process_time();

	double time_limit = m_lambda * num_of_jobs * num_of_jobs * num_of_jobs * num_of_stages;
	//double time_limit = 2 * num_of_stages * num_of_jobs * 1000;

	long algorithm_start_time = GetElapsedProcessTime();

	int best_span = INT32_MAX;
	int span;

	std::vector<int> seed_job_sequence(num_of_jobs);
	std::iota(std::begin(seed_job_sequence), std::end(seed_job_sequence), 0);
	std::sort(seed_job_sequence.begin(), seed_job_sequence.end(), [&](int j1, int j2)
	{
		return problem.getJobs()[j1].get_total_process_time() > problem.getJobs()[j2].get_total_process_time();
	});

	Solution solution(problem, Solution::Solution_Init_Type::Empty);

	span = NEH_f(seed_job_sequence, solution);
	if (span < best_span)
	{
		best_span = span;
	}

	span = iterative_improvement_insertion(solution, span);
	if (span < best_span)
	{
		best_span = span;
	}

	std::vector<int> erased_jobs;
	std::vector<int> original_job_sequence = solution.getJobSequence();
	int original_span = span;

	int total_iteration_count = 0;
	//std::cout << "\titeration" << total_iteration_count << ":\t" << best_span << std::endl;
	while (true)
	{
            erased_jobs.clear();
            destruction(solution, erased_jobs);
            span = construction(solution, erased_jobs);

            solution.decode_forward_to_graph();
            span = forward_decode_local_search_critical_nodes(solution, span);

            if (span < best_span)
            {
                best_span = span;
                //std::cout << "\titeration" << total_iteration_count << ":\t" << best_span << std::endl;
            }

            if (span <= original_span)
            {
                original_span = span;
                original_job_sequence = solution.getJobSequence();
            }
            else
            {
                double rand_value = wyt_rand(0.0, 1.0);
                if (rand_value
                < std::exp(-(span - original_span) * num_of_jobs * num_of_stages * 10 / (m_T * total_process_time)))
                {
                    original_span = span;
                    original_job_sequence = solution.getJobSequence();
                }
                else
                {
                    solution.setJobSequence(original_job_sequence);
                    span = original_span;
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

	return { best_span, total_iteration_count };
}

std::pair<int, int> IGWS::Evolution_Speed(const HFS_Problem &problem) {
    auto start = std::chrono::system_clock::now();
    int num_of_jobs = problem.getNumOfJobs();
    int best_span = INT32_MAX;
    int span;

    std::vector<int> seed_job_sequence(num_of_jobs);
    std::iota(std::begin(seed_job_sequence), std::end(seed_job_sequence), 0);
    std::sort(seed_job_sequence.begin(), seed_job_sequence.end(), [&](int j1, int j2) {
        return problem.getJobs()[j1].get_total_process_time() > problem.getJobs()[j2].get_total_process_time();
    });
    Solution solution(problem, Solution::Solution_Init_Type::Empty);
    span = NEH_f(seed_job_sequence, solution);
    if (span < best_span) {
        best_span = span;
    }
    solution.decode_forward_to_graph();
    span = forward_decode_local_search_critical_nodes(solution, span);
    auto end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout <<duration.count()<<std::endl;
    return {1,1};
}