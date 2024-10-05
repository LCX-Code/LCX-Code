

#ifndef HFSP_CRITICAL_PATH_SOLUTION_H
#define HFSP_CRITICAL_PATH_SOLUTION_H

#include <vector>
#include <unordered_map>
#include "HFS_Problem.h"
#include <algorithm>
class Node
{
public:
    int jobId = -1;
    int stageId = -1;
    int machineId = -1;

    int processTime = 0;
    int f_startTime = 0;
    int b_startTime = 0;
    Node *sucByStage = nullptr;
    Node *sucByMachine = nullptr;
    Node *preByStage = nullptr;
    Node *preByMachine = nullptr;

	//寻找关键路径上的节点用到
	bool is_visit = false;

	//寻找所有割点时用到
	int dfn = 0;
	int low = 0;
	int critical_adjacent_count = 0;
	std::vector<Node*> critical_adjacent = std::vector<Node*>(4);

    Node(int jobId, int stageId, int machineId, int processTime)
            : jobId(jobId), stageId(stageId), machineId(machineId), processTime(processTime)
    {}
	Node(const Node& node)
		: jobId(node.jobId), stageId(node.stageId), machineId(node.machineId), processTime(node.processTime),
		f_startTime(node.f_startTime), b_startTime(node.b_startTime)
	{}
};

class Solution
{
	const HFS_Problem& problem;
    std::vector<std::vector<Node *>> f_headOnMachinesInEachStage;
    std::vector<std::vector<Node *>> b_headOnMachinesInEachStage;
    std::vector<Node *> f_headJobs;
    std::vector<Node *> b_headJobs;
	std::vector<std::vector<Node *>> operations;
    int make_span;
public:
	enum Solution_Init_Type{RandomInit, Empty};

    explicit Solution(const HFS_Problem &problem, Solution_Init_Type init_type = RandomInit);
	Solution(const HFS_Problem& problem, const std::vector<int>& sequence);
	Solution(const HFS_Problem &problem, const std::string& fileName);
	Solution(const Solution &solution);
	Solution& operator=(const Solution &solution);
    //Solution(const HFS_Problem& problem);
	~Solution();

	void initBaseGraph();
	void reset_to_base_graph();
	void deleteGraph();

	int decode_forward();
	int decode_backward();
	int decode_forward_to_graph();
	int decode_backward_to_graph();
	int refreshAllGraph();
	int reCalculateMakeSpan();

    void findOneCriticalPathMachineFirstBackward(std::vector<Node *> &criticalPath, int span) const;
    void findOneCriticalPathJobFirstBackward(std::vector<Node*>& criticalPath, int span) const;

    void findAllCriticalNodesForward(std::vector<Node *> &criticalNodes, int span) const;
    void findAllCriticalNodesBackward(std::vector<Node *> &criticalNodes, int span) const;

	void findCutNodesForward(std::vector<Node*>& critical_nodes, std::vector<Node*>& cutNodes, int span) const;
	void findCutNodesBackward(std::vector<Node*>& critical_nodes, std::vector<Node*>& cutNodes, int span) const;

	void deleteNodePhysicalFromGraph(Node* deleted_node);
	void insertNodeInPosition(Node* deleted_node, Node* node_before_insert_pos, Node * node_in_pre_stage);

	void swap_node_in_graph(const std::vector<Node*>& critical_nodes);
	void insertion_node_in_graph0(const std::vector<Node*>& critical_nodes);
	void insertion_node_in_graph1(const std::vector<Node*>& critical_nodes);
	void disruption_node_at_one_stage(const std::vector<Node*>& critical_nodes);

	void copy_one_stage(const Solution& solution, int stage_id);
	void copy_stage_to_vector(int stage_id, std::vector<std::vector<int>>& vec);
	void copy_vector_to_stage(int stage_id, const std::vector<std::vector<int>>& vec);

	std::pair<int, int> find_best_insert_position_f(int inserted_job_id);
	std::pair<int, int> find_best_insert_position_b(int inserted_job_id);
	void destruction(std::vector<int>& erased_jobs, int d);

    [[nodiscard]] const std::vector<Node *> &getHeadOnMachinesInStage(int stageId) const;
    [[nodiscard]] const std::vector<Node *> &getTailOnMachinesInStage(int stageId) const;
	void setJobSequence(const std::vector<int>& job_sequence);
	[[nodiscard]] const std::vector<int>& getJobSequence() const;
	[[nodiscard]] const HFS_Problem& getProblem() const;
	[[nodiscard]] int get_job(int index) const;
	[[nodiscard]] int get_index(int job_id) const;
	void insert_job_at_position(int inserted_job_id, int position);
	void remove_one_job_by_index(int index);
	int remove_one_job_by_id(int job_id);
    void emplace_back_one_job(int inserted_job_id);
    void remove_last_job();

	void print() const;

    int get_span() const;

    void set_span(int span);

    int evaluate() const;

    void destruction_block(std::vector<int> &erased_jobs, int block_size);

    std::pair<int, int> find_best_position_block(const std::vector<int>& erased_jobs);

    void swap_block_in_graph(const std::vector<Node *> &critical_nodes, int block_size);

    void insertion_block_in_graph1(const std::vector<Node *> &critical_nodes, int block_size);

    std::vector<int> jobSequence;

    void insert_block_at_position(const std::vector<int> &erased_jobs, int position);

    std::pair<int, int> find_best_swap_f(int swapped_index);

    void swap_job_by_index(int index1, int index2);

    void Copy(std::vector<int> &v);
};

inline const std::vector<Node *>& Solution::getHeadOnMachinesInStage(int stageId) const
{
	return f_headOnMachinesInEachStage[stageId];
}

inline const std::vector<Node *>& Solution::getTailOnMachinesInStage(int stageId) const
{
	return b_headOnMachinesInEachStage[stageId];
}

inline void Solution::setJobSequence(const std::vector<int>& job_sequence)
{
	jobSequence = job_sequence;
}

inline const std::vector<int>& Solution::getJobSequence() const
{
	return jobSequence;
}

inline const HFS_Problem& Solution::getProblem() const
{
	return problem;
}

inline  int Solution::get_job(int index) const
{
	return jobSequence[index];
}
inline  int Solution::get_index(int job_id) const
{
	return std::find(jobSequence.begin(), jobSequence.end(), job_id) - jobSequence.begin();;
}

inline void Solution::insert_job_at_position(int inserted_job_id, int position)
{
	jobSequence.insert(jobSequence.begin() + position, inserted_job_id);
}

inline void Solution::remove_one_job_by_index(int index)
{
	jobSequence.erase(jobSequence.begin() + index);
}

inline int Solution::remove_one_job_by_id(int job_id)
{
	auto it = std::find(jobSequence.begin(), jobSequence.end(), job_id);
	int position = it - jobSequence.begin();
	jobSequence.erase(it);
	return position;
}

inline int Solution::get_span() const
{
    return make_span;
}
inline void Solution::set_span(int span)
{
    make_span = span;
}

inline void Solution::emplace_back_one_job(int inserted_job_id)
{
    jobSequence.emplace_back(inserted_job_id);
}
inline void Solution::remove_last_job()
{
    jobSequence.pop_back();
}
inline void Solution::insert_block_at_position(const std::vector<int>& erased_jobs, int position)
{
    jobSequence.insert(jobSequence.begin() + position, erased_jobs.begin(), erased_jobs.end());
}
inline void Solution::swap_job_by_index(int index1, int index2)
{
    std::swap(jobSequence[index1], jobSequence[index2]);
}

#endif //HFSP_CRITICAL_PATH_SOLUTION_H
