//
// Created by 30565 on 2024/2/6.
//
//

#include <numeric>
#include <algorithm>
#include <iostream>
#include <cassert>
#include <chrono>
#include <queue>
#include <stack>
#include "HFS_Problem.h"
#include "Solution.h"
#include "utils.h"
#include "IGLCX.h"
#include <algorithm>

double IGLCX::m_lambda;
int IGLCX::m_d;
double IGLCX::m_T;


void
IGLCX::set_parameters(double lambda, double t, int d) {
    m_lambda = lambda;

    m_d = d;
    m_T = t;
}

Solution IGLCX::NEH(const HFS_Problem &problem, const std::vector<int> &guiding_job_sequence) {
    Solution solution(problem);
    int span;
    for (auto job_id: guiding_job_sequence) {
        auto re = solution.find_best_insert_position_f(job_id);
        span = re.first;
        solution.insert_job_at_position(job_id, re.second);
    }
    solution.set_span(span);
    return solution;
}

int IGLCX::NEH_f(const std::vector<int> &seed_job_sequence, Solution &solution) {
    int span;
    for (auto job_id: seed_job_sequence) {
        auto re = solution.find_best_insert_position_f(job_id);
        span = re.first;
        solution.insert_job_at_position(job_id, re.second);
    }
    return span;
}

int IGLCX::iterative_improvement_insertion(Solution &solution, int old_span) {
    const HFS_Problem &problem = solution.getProblem();
    int num_of_jobs = problem.getNumOfJobs();

    bool improvement = true;
    while (improvement) {
        improvement = false;
        for (int i = 0; i < num_of_jobs; ++i) {
            int job_id = solution.get_job(i);
            solution.remove_one_job_by_index(i);
            auto re = solution.find_best_insert_position_f(job_id);
            if (re.first < old_span) {
                solution.insert_job_at_position(job_id, re.second);
                improvement = true;
                old_span = re.first;
            } else {
                solution.insert_job_at_position(job_id, i);
            }
        }
    }
    return old_span;
}

void IGLCX::destruction(Solution &solution, std::vector<int> &erased_jobs) {
    solution.destruction(erased_jobs,5 );
}

void IGLCX::destruction1(Solution &solution, std::vector<int> &erased_jobs, bool flag,int r) {
    int d[3] = {3, 4, 5};
    if (flag)
        solution.destruction(erased_jobs, d[r % 3]);
    else
        solution.destruction(erased_jobs, d[++r % 3]);
}

int IGLCX::construction(Solution &solution, std::vector<int> &erased_jobs) {
    int span;
    for (auto job_id: erased_jobs) {
        auto re = solution.find_best_insert_position_f(job_id);
        span = re.first;
        solution.insert_job_at_position(job_id, re.second);
    }
    return span;
}

/*int IGLCX::local_search(Solution &solution, const std::vector<Node *> &nodes, int originalMakeSpan) {
    const HFS_Problem &problem = solution.getProblem();
    int stage_count = problem.getNumOfStages();
    for (auto deleteNode: nodes) {
        Node *originalPreNodeOnMachine = deleteNode->preByMachine;

        int current_stage_id = deleteNode->stageId;
        solution.deleteNodePhysicalFromGraph(deleteNode);
        //int makeSpanAfterDeleteNode = solution.refreshAllGraph();

        int f_start_time_of_pre_stage =
                deleteNode->preByStage->f_startTime + deleteNode->preByStage->processTime;

        int b_start_time_of_suc_stage =
                deleteNode->sucByStage->b_startTime + deleteNode->sucByStage->processTime;

        const auto &heads = solution.getHeadOnMachinesInStage(current_stage_id);
        const auto &tails = solution.getTailOnMachinesInStage(current_stage_id);
        int machineCount = problem.getNumOfMachinesInStage(current_stage_id);
        int bestPathLengthThroughDelNode = originalMakeSpan;
        Node *nodeBeforeBestInsertPos;
        for (int mid = 0; mid < machineCount; ++mid) {
            if (mid == deleteNode->machineId)//同一机器尝试
            {
                int a_point_offset = INT16_MIN;
                Node *nodeBeforeAPoint = heads[mid];
                int f_start_time_of_node_before_A = INT32_MIN;
                if (current_stage_id == 0) {
                    if (deleteNode->preByMachine == heads[mid]) {
                        a_point_offset = 0;
                    }
                } else {
                    a_point_offset = 0;
                    nodeBeforeAPoint = deleteNode->preByMachine;
                    //int f_start_time_of_node_before_A = INT32_MIN;
                    if (nodeBeforeAPoint->f_startTime + nodeBeforeAPoint->processTime
                        <= f_start_time_of_pre_stage) {
                        //从删除位置正向找A点
                        f_start_time_of_node_before_A = nodeBeforeAPoint->f_startTime;
                        Node *c_node = nodeBeforeAPoint->sucByMachine;
                        int f_star_time_c_node;
                        while (c_node->jobId != -1) {
                            f_star_time_c_node = std::max(
                                    f_start_time_of_node_before_A + nodeBeforeAPoint->processTime,
                                    c_node->preByStage->f_startTime + c_node->preByStage->processTime);
                            if (c_node->f_startTime + c_node->processTime > f_start_time_of_pre_stage) {
                                break;
                            }
                            nodeBeforeAPoint = c_node;
                            f_start_time_of_node_before_A = f_star_time_c_node;
                            c_node = nodeBeforeAPoint->sucByMachine;
                            ++a_point_offset;
                        }
                    } else {
                        //从删除位置反向找A点
                        --a_point_offset;
                        nodeBeforeAPoint = nodeBeforeAPoint->preByMachine;
                        while (nodeBeforeAPoint->jobId != -1) {
                            if (nodeBeforeAPoint->f_startTime + nodeBeforeAPoint->processTime
                                <= f_start_time_of_pre_stage) {
                                break;
                            }
                            nodeBeforeAPoint = nodeBeforeAPoint->preByMachine;
                            --a_point_offset;
                        }
                    }
                }

                int b_point_offset = INT16_MIN;
                Node *nodeAfterBPoint = tails[mid];
                int b_start_time_of_node_after_B = INT32_MIN;
                if (current_stage_id == stage_count - 1) {
                    if (deleteNode->sucByMachine == tails[mid]) {
                        b_point_offset = 0;
                    }
                } else {
                    b_point_offset = 0;
                    nodeAfterBPoint = deleteNode->sucByMachine;
                    //b_start_time_of_node_after_B = INT32_MIN;
                    if (nodeAfterBPoint->b_startTime + nodeAfterBPoint->processTime
                        <= b_start_time_of_suc_stage) {
                        //从删除位置反向向找b点
                        b_start_time_of_node_after_B = nodeAfterBPoint->b_startTime;
                        Node *c_node = nodeAfterBPoint->preByMachine;
                        int b_star_time_c_node;
                        while (c_node->jobId != -1) {
                            b_star_time_c_node = std::max(
                                    b_start_time_of_node_after_B + nodeAfterBPoint->processTime,
                                    c_node->sucByStage->b_startTime + c_node->sucByStage->processTime);
                            if (c_node->b_startTime + c_node->processTime > b_start_time_of_suc_stage) {
                                break;
                            }
                            nodeAfterBPoint = c_node;
                            b_start_time_of_node_after_B = b_star_time_c_node;
                            c_node = nodeAfterBPoint->preByMachine;
                            ++b_point_offset;
                        }
                    } else {
                        //从删除位置正向找B点
                        --b_point_offset;
                        nodeAfterBPoint = nodeAfterBPoint->sucByMachine;
                        while (nodeAfterBPoint->jobId != -1) {
                            if (nodeAfterBPoint->b_startTime + nodeAfterBPoint->processTime
                                <= b_start_time_of_suc_stage) {
                                break;
                            }
                            nodeAfterBPoint = nodeAfterBPoint->sucByMachine;
                            --b_point_offset;
                        }
                    }
                }

                if (a_point_offset >= 0 && b_point_offset >= 0) {
                    //有交集，且交集包含删除位置，不用尝试，不会变好，测试下一机器
                    continue;
                } else if (a_point_offset < 0 && b_point_offset < 0) {
                    //无交集，A点在删除位置前，B点在删除位置后，从删除位置开始正反向两段考虑
                    int longestValue;
                    Node *nodeBeforeInsertPos;
                    Node *nodeAfterInsertPos;

                    nodeBeforeInsertPos = deleteNode->sucByMachine;
                    nodeAfterInsertPos = nodeBeforeInsertPos->sucByMachine;
                    int f_start_time = nodeBeforeInsertPos->preByMachine->f_startTime;
                    while (nodeBeforeInsertPos != nodeAfterBPoint) {
                        f_start_time =
                                std::max(f_start_time
                                         + nodeBeforeInsertPos->preByMachine->processTime,
                                         nodeBeforeInsertPos->preByStage->f_startTime
                                         + nodeBeforeInsertPos->preByStage->processTime);
                        longestValue =
                                std::max(f_start_time_of_pre_stage,
                                         f_start_time + nodeBeforeInsertPos->processTime)
                                + deleteNode->processTime +
                                std::max(b_start_time_of_suc_stage,
                                         nodeAfterInsertPos->b_startTime + nodeAfterInsertPos->processTime);
                        if (longestValue < bestPathLengthThroughDelNode) {
                            bestPathLengthThroughDelNode = longestValue;
                            nodeBeforeBestInsertPos = nodeBeforeInsertPos;
                        }
                        nodeBeforeInsertPos = nodeAfterInsertPos;
                        nodeAfterInsertPos = nodeBeforeInsertPos->sucByMachine;
                    }

                    nodeAfterInsertPos = deleteNode->preByMachine;
                    nodeBeforeInsertPos = nodeAfterInsertPos->preByMachine;
                    int b_start_time = nodeAfterInsertPos->sucByMachine->b_startTime;
                    while (nodeAfterInsertPos != nodeBeforeAPoint) {
                        b_start_time =
                                std::max(b_start_time
                                         + nodeAfterInsertPos->sucByMachine->processTime,
                                         nodeAfterInsertPos->sucByStage->b_startTime
                                         + nodeAfterInsertPos->sucByStage->processTime);

                        longestValue =
                                std::max(f_start_time_of_pre_stage,
                                         nodeBeforeInsertPos->f_startTime + nodeBeforeInsertPos->processTime)
                                + deleteNode->processTime +
                                std::max(b_start_time_of_suc_stage,
                                         b_start_time + nodeAfterInsertPos->processTime);
                        if (longestValue < bestPathLengthThroughDelNode) {
                            bestPathLengthThroughDelNode = longestValue;
                            nodeBeforeBestInsertPos = nodeBeforeInsertPos;
                        }
                        nodeAfterInsertPos = nodeBeforeInsertPos;
                        nodeBeforeInsertPos = nodeAfterInsertPos->preByMachine;
                    }
                } else if (a_point_offset < 0) //a_point_offset < 0 && b_point_offset >= 0
                {
                    //A点在删除位置前，B点在删除位置前（B点可能就是删除位置）
                    if (-a_point_offset > b_point_offset) {
                        //无交集
                        int longestValue;
                        Node *nodeAfterInsertPos = nodeAfterBPoint;
                        Node *nodeBeforeInsertPos = nodeAfterInsertPos->preByMachine;
                        int b_start_time = b_start_time_of_node_after_B;
                        while (true) {
                            longestValue =
                                    std::max(f_start_time_of_pre_stage,
                                             nodeBeforeInsertPos->f_startTime + nodeBeforeInsertPos->processTime)
                                    + deleteNode->processTime +
                                    std::max(b_start_time_of_suc_stage,
                                             b_start_time + nodeAfterInsertPos->processTime);
                            if (longestValue < bestPathLengthThroughDelNode) {
                                bestPathLengthThroughDelNode = longestValue;
                                nodeBeforeBestInsertPos = nodeBeforeInsertPos;
                            }
                            if (nodeBeforeInsertPos == nodeBeforeAPoint) {
                                break;
                            }
                            nodeAfterInsertPos = nodeBeforeInsertPos;
                            nodeBeforeInsertPos = nodeAfterInsertPos->preByMachine;
                            b_start_time =
                                    std::max(b_start_time
                                             + nodeAfterInsertPos->sucByMachine->processTime,
                                             nodeAfterInsertPos->sucByStage->b_startTime //todo
                                             + nodeAfterInsertPos->sucByStage->processTime);
                        }
                    } else {
                        //有交集
                        int longestValue = f_start_time_of_pre_stage + b_start_time_of_suc_stage
                                           + deleteNode->processTime;
                        if (longestValue < bestPathLengthThroughDelNode) {
                            bestPathLengthThroughDelNode = longestValue;
                            nodeBeforeBestInsertPos = nodeBeforeAPoint;
                        }
                    }
                } else //a_point_offset >= 0 && b_point_offset < 0
                {
                    //B点在删除位置后，A点在删除位置后（B点可能就是删除位置）
                    if (-b_point_offset > a_point_offset) {
                        //无交集
                        int longestValue;
                        Node *nodeBeforeInsertPos = nodeBeforeAPoint;
                        Node *nodeAfterInsertPos = nodeBeforeInsertPos->sucByMachine;
                        int f_start_time = f_start_time_of_node_before_A;
                        while (true) {
                            longestValue =
                                    std::max(f_start_time_of_pre_stage,
                                             f_start_time + nodeBeforeInsertPos->processTime)
                                    + deleteNode->processTime +
                                    std::max(b_start_time_of_suc_stage,
                                             nodeAfterInsertPos->b_startTime + nodeAfterInsertPos->processTime);
                            if (longestValue < bestPathLengthThroughDelNode) {
                                bestPathLengthThroughDelNode = longestValue;
                                nodeBeforeBestInsertPos = nodeBeforeInsertPos;
                            }
                            if (nodeAfterInsertPos == nodeAfterBPoint) {
                                break;
                            }
                            nodeBeforeInsertPos = nodeAfterInsertPos;
                            nodeAfterInsertPos = nodeBeforeInsertPos->sucByMachine;
                            f_start_time =
                                    std::max(f_start_time
                                             + nodeBeforeInsertPos->preByMachine->processTime,
                                             nodeBeforeInsertPos->preByStage->f_startTime
                                             + nodeBeforeInsertPos->preByStage->processTime);
                        }
                    } else {
                        //有交集
                        int longestValue = f_start_time_of_pre_stage + b_start_time_of_suc_stage
                                           + deleteNode->processTime;
                        if (longestValue < bestPathLengthThroughDelNode) {
                            bestPathLengthThroughDelNode = longestValue;
                            nodeBeforeBestInsertPos = nodeAfterBPoint->preByMachine;
                        }
                    }
                }
            } else {
                //其他机器尝试
                int f_complete_time_machine = tails[mid]->f_startTime;
                int b_complete_time_machine = heads[mid]->b_startTime;

                if (f_complete_time_machine - f_start_time_of_pre_stage
                    >= b_complete_time_machine - b_start_time_of_suc_stage) {
                    //正向找A和B
                    //正向寻找插入位置
                    bool is_find_A_pos = false;
                    //bool is_find_B_pos = false;
                    int longestValue;
                    Node *nodeBeforeInsertPos = heads[mid];
                    Node *nodeAfterInsertPos = nodeBeforeInsertPos->sucByMachine;
                    while (nodeBeforeInsertPos != tails[mid]) {
                        if (!is_find_A_pos &&
                            nodeAfterInsertPos->f_startTime + nodeAfterInsertPos->processTime
                            > f_start_time_of_pre_stage) {
                            //发现A点
                            is_find_A_pos = true;
                        }

                        if (nodeAfterInsertPos->b_startTime + nodeAfterInsertPos->processTime
                            <= b_start_time_of_suc_stage) {
                            //发现B点
                            longestValue =
                                    std::max(f_start_time_of_pre_stage,
                                             nodeBeforeInsertPos->f_startTime + nodeBeforeInsertPos->processTime)
                                    + deleteNode->processTime
                                    + b_start_time_of_suc_stage;
                            if (longestValue < bestPathLengthThroughDelNode) {
                                bestPathLengthThroughDelNode = longestValue;
                                nodeBeforeBestInsertPos = nodeBeforeInsertPos;
                            }
                            break;
                        }

                        if (is_find_A_pos) {
                            longestValue =
                                    std::max(f_start_time_of_pre_stage,
                                             nodeBeforeInsertPos->f_startTime + nodeBeforeInsertPos->processTime)
                                    + deleteNode->processTime +
                                    std::max(b_start_time_of_suc_stage,
                                             nodeAfterInsertPos->b_startTime + nodeAfterInsertPos->processTime);
                            if (longestValue < bestPathLengthThroughDelNode) {
                                bestPathLengthThroughDelNode = longestValue;
                                nodeBeforeBestInsertPos = nodeBeforeInsertPos;
                            }
                        }
                        nodeBeforeInsertPos = nodeAfterInsertPos;
                        nodeAfterInsertPos = nodeBeforeInsertPos->sucByMachine;
                    }
                } else {
                    //正向找B和A
                    //std::cout << "<-";
                    //反向寻找插入位置
                    bool is_find_B_pos = false;
                    //bool is_find_A_pos = false;
                    int longestValue;
                    Node *nodeAfterInsertPos = tails[mid];
                    Node *nodeBeforeInsertPos = nodeAfterInsertPos->preByMachine;
                    while (nodeAfterInsertPos != heads[mid]) {
                        if (!is_find_B_pos &&
                            nodeBeforeInsertPos->b_startTime + nodeBeforeInsertPos->processTime
                            > b_start_time_of_suc_stage) {
                            //发现B点
                            is_find_B_pos = true;
                        }

                        if (nodeBeforeInsertPos->f_startTime + nodeBeforeInsertPos->processTime
                            <= f_start_time_of_pre_stage) {
                            //发现A点
                            longestValue =
                                    f_start_time_of_pre_stage
                                    + deleteNode->processTime
                                    + std::max(b_start_time_of_suc_stage,
                                               nodeAfterInsertPos->b_startTime + nodeAfterInsertPos->processTime);

                            if (longestValue < bestPathLengthThroughDelNode) {
                                bestPathLengthThroughDelNode = longestValue;
                                nodeBeforeBestInsertPos = nodeBeforeInsertPos;
                            }
                            break;
                        }

                        if (is_find_B_pos) {
                            longestValue =
                                    std::max(f_start_time_of_pre_stage,
                                             nodeBeforeInsertPos->f_startTime + nodeBeforeInsertPos->processTime)
                                    + deleteNode->processTime +
                                    std::max(b_start_time_of_suc_stage,
                                             nodeAfterInsertPos->b_startTime + nodeAfterInsertPos->processTime);
                            if (longestValue < bestPathLengthThroughDelNode) {
                                bestPathLengthThroughDelNode = longestValue;
                                nodeBeforeBestInsertPos = nodeBeforeInsertPos;
                            }
                        }
                        nodeAfterInsertPos = nodeBeforeInsertPos;
                        nodeBeforeInsertPos = nodeAfterInsertPos->preByMachine;
                    }
                }
            }
        }
        if (bestPathLengthThroughDelNode < originalMakeSpan) {
            solution
                    .insertNodeInPosition(deleteNode, nodeBeforeBestInsertPos, deleteNode->preByStage);
            int new_span = solution.refreshAllGraph();
            //assert(new_span < originalMakeSpan);
            return new_span;
        } else {
            solution
                    .insertNodeInPosition(deleteNode, originalPreNodeOnMachine, deleteNode->preByStage);
        }
    }
    return originalMakeSpan;
}*/

int IGLCX::local_search(Solution& solution, const std::vector<Node*>& nodes, int originalMakeSpan)
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

int IGLCX::forward_decode_local_search_cut_nodes(Solution &solution, int originalMakeSpan) {
    std::vector<Node *> critical_nodes;
    std::vector<Node *> cut_nodes;

    while (true) {
        cut_nodes.clear();
        solution.findCutNodesBackward(critical_nodes, cut_nodes, originalMakeSpan);
        int new_span = local_search(solution, cut_nodes, originalMakeSpan);
        if (new_span < originalMakeSpan) {
            originalMakeSpan = new_span;
        } else {
            break;
        }
    }
    return originalMakeSpan;
}

int IGLCX::forward_decode_local_search(Solution &solution, Solution &assistant_solution) {
    const HFS_Problem &problem = solution.getProblem();

    int num_of_jobs = problem.getNumOfJobs();
    int num_of_stages = problem.getNumOfStages();
    int total_process_time = problem.get_total_process_time();

    int span = solution.decode_forward_to_graph();
    int best_span = forward_decode_local_search_cut_nodes(solution, span);
    assistant_solution = solution;

    int iteration_count = 0;
    int block_size = 2;
    //int count = 0;
    while (iteration_count < 2 * num_of_jobs) {
        ++iteration_count;
        std::vector<Node *> critical_nodes;
        solution.findAllCriticalNodesForward(critical_nodes, best_span);

        int is_swap = wyt_rand(3);
        if (is_swap == 0) {
            solution.swap_node_in_graph(critical_nodes);
        } else if (is_swap == 1) {
            solution.insertion_node_in_graph1(critical_nodes);
        } else if (is_swap == 2) {
            solution.swap_block_in_graph(critical_nodes, block_size);
        }/*else if (is_swap == 3) {
              solution.insertion_block_in_graph1(critical_nodes, block_size);
          }*/
        span = solution.refreshAllGraph();
        span = forward_decode_local_search_cut_nodes(solution, span);
        if (span < best_span) {
            iteration_count = 0;
        }
        if (span <= best_span) {
            best_span = span;
            assistant_solution = solution;
        } else {
            solution = assistant_solution;
        }
    }
    //++count;
    //std::cout << count << std::endl;
    return best_span;
}

int Solution::evaluate() const {
    int num_of_jobs = problem.getNumOfJobs();
    int num_of_stages = problem.getNumOfStages();
    const std::vector<Job> &jobs = problem.getJobs();

    //各工序的正向完工时间
    std::vector<std::vector<int>> c_time(num_of_jobs, std::vector<int>(num_of_stages));
    //每个阶段每个机器的可用时间
    std::vector<std::vector<int>> m_idle_time(num_of_stages);
    for (int s = 0; s < problem.getNumOfStages(); s++) {
        m_idle_time[s].resize(problem.getNumOfMachinesInStage(s));
    }

    int span = 0;
    //第一阶段
    for (auto job_id: jobSequence) {
        int mt = min_element(m_idle_time[0].begin(), m_idle_time[0].end()) - m_idle_time[0].begin();
        c_time[job_id][0] = m_idle_time[0][mt] + jobs[job_id].getProcessTime(0);
        m_idle_time[0][mt] = c_time[job_id][0];
    }

    std::vector<int> sequence = jobSequence;
    //其他阶段，FIFO和FAM
    for (int s = 1; s < num_of_stages; ++s) {
        std::sort(sequence.begin(), sequence.end(), [&](int job1, int job2) {
            return c_time[job1][s - 1] < c_time[job2][s - 1];
        });
        for (int first_come_job_id: sequence) {
            int mt = min_element(m_idle_time[s].begin(), m_idle_time[s].end()) - m_idle_time[s].begin();
            c_time[first_come_job_id][s] =
                    std::max(m_idle_time[s][mt], c_time[first_come_job_id][s - 1])
                    + jobs[first_come_job_id].getProcessTime(s);
            m_idle_time[s][mt] = c_time[first_come_job_id][s];
            if (s == num_of_stages - 1) {
                span = std::max(span, c_time[first_come_job_id][s]);
            }
        }
    }
    return span;
}

void IGLCX::GRASP(Solution &solution, std::vector<int> &U) {
    double alpha = 0.1;
    const HFS_Problem &problem = solution.getProblem();
    int num_of_jobs = problem.getNumOfJobs();

    std::vector<int> CF(num_of_jobs);
    std::vector<int> RCL(num_of_jobs);
    int span;
    int CF_min;
    int CF_max;
    for (int i = 1; i < num_of_jobs; ++i) {
        CF_min = INT32_MAX;
        CF_max = INT32_MIN;
        for (int j = i; j < num_of_jobs; ++j) {
            solution.emplace_back_one_job(U[j]);
            span = solution.evaluate();
            CF[j] = span;
            CF_min = std::min(span, CF_min);
            CF_max = std::max(span, CF_max);
            solution.remove_last_job();
        }

        int k = 0;
        for (int j = i; j < num_of_jobs; ++j) {
            if (CF[j] - CF_min <= alpha * (CF_max - CF_min)) {
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

Solution IGLCX::GRASP_NEH(const HFS_Problem &problem, const std::vector<int> &guiding_job_sequence, int x) {
    int num_of_jobs = problem.getNumOfJobs();
    std::vector<Solution> solution_vec(x, Solution(problem));
    for (int i = 0; i < x; i++)
        solution_vec[i].jobSequence.clear();
    for (int h = 0; h < x; ++h) {
        std::vector<int> U(num_of_jobs);
        std::iota(U.begin(), U.end(), 0);
        int first_job = guiding_job_sequence[h];
        solution_vec[h].emplace_back_one_job(first_job);
        std::swap(U[0], U[first_job]);
        GRASP(solution_vec[h], U);
        Solution solution2 = NEH(problem, solution_vec[h].getJobSequence());
        if (solution2.get_span() < solution_vec[h].get_span()) {
            solution_vec[h] = solution2;
        }
    }
    return *std::min_element(solution_vec.begin(), solution_vec.end(), [](const auto &sol1, const auto &sol2) {
        return sol1.get_span() < sol2.get_span();
    });

}

void IGLCX::destruction_block(Solution &solution, std::vector<int> &erased_jobs, int block_size) {
    solution.destruction_block(erased_jobs, block_size);
}

void IGLCX::construction_block(Solution &solution, std::vector<int> &erased_jobs) {
    auto re = solution.find_best_position_block(erased_jobs);
    solution.insert_block_at_position(erased_jobs, re.second);
    solution.set_span(re.first);
}

int IGLCX::RIS_local_search_f(Solution &solution, int old_span, const Solution &best_solution) {
    const HFS_Problem &problem = solution.getProblem();
    int num_of_jobs = problem.getNumOfJobs();

    int iter = 0;
    int pos = 0;
    while (iter < num_of_jobs) {
        int job_id = best_solution.get_job(pos);
        pos = (pos + 1) % num_of_jobs;
        int old_position = solution.remove_one_job_by_id(job_id);
        auto re = solution.find_best_insert_position_f(job_id);
        if (re.first < old_span) {
            solution.insert_job_at_position(job_id, re.second);
            old_span = re.first;
            iter = 0;
        } else {
            solution.insert_job_at_position(job_id, old_position);
            ++iter;
        }
    }
    return old_span;
}

int IGLCX::RSS_local_search_f(Solution &solution, int old_span, const Solution &best_solution) {
    const HFS_Problem &problem = solution.getProblem();
    int num_of_jobs = problem.getNumOfJobs();

    int iter = 0;
    int pos = 0;
    while (iter < num_of_jobs) {
        int job_id = best_solution.get_job(pos);
        pos = (pos + 1) % num_of_jobs;

        int swapped_index = solution.get_index(job_id);
        auto re = solution.find_best_swap_f(swapped_index);

        if (re.first < old_span) {
            solution.swap_job_by_index(swapped_index, re.second);
            old_span = re.first;
            iter = 0;
        } else {
            ++iter;
        }
    }
    return old_span;
}
std::pair<int, int> IGLCX::Evolution1(const HFS_Problem &problem) {
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
    std::sort(seed_job_sequence.begin(), seed_job_sequence.end(), [&](int j1, int j2) {
        return problem.getJobs()[j1].get_total_process_time() > problem.getJobs()[j2].get_total_process_time();
    });

    Solution solution(problem, Solution::Solution_Init_Type::Empty);

    span = NEH_f(seed_job_sequence, solution);
    if (span < best_span) {
        best_span = span;
    }

    span = iterative_improvement_insertion(solution, span);
    if (span < best_span) {
        best_span = span;
    }

    std::vector<int> erased_jobs;
    std::vector<int> original_job_sequence = solution.getJobSequence();
    int original_span = span;

    int total_iteration_count = 0;
    //std::cout << "\titeration" << total_iteration_count << ":\t" << best_span << std::endl;
    while (true) {
        erased_jobs.clear();

        destruction(solution, erased_jobs);
        span = construction(solution, erased_jobs);

        /*    solution.set_span(span);
            span = localSearch(solution);*/


        solution.decode_forward_to_graph();
        span = forward_decode_local_search_cut_nodes(solution, span);

        // span = iterative_improvement_insertion(solution, span);
        if (span < best_span) {
            best_span = span;
            //std::cout << "\titeration" << total_iteration_count << ":\t" << best_span << std::endl;
        }

        if (span <= original_span) {
            original_span = span;
            original_job_sequence = solution.getJobSequence();
        } else {
            double rand_value = wyt_rand(0.0, 1.0);
            if (rand_value
                < std::exp(-(span - original_span) * num_of_jobs * num_of_stages * 10 / (m_T * total_process_time))) {
                original_span = span;
                original_job_sequence = solution.getJobSequence();
            } else {
                solution.setJobSequence(original_job_sequence);
                span = original_span;
            }
        }

        ++total_iteration_count;

        long cur_time = GetElapsedProcessTime();
        long ElapsedTime = cur_time - algorithm_start_time;
        if (ElapsedTime > time_limit) {
            break;
        }
    }
    return {best_span, total_iteration_count};
}

std::pair<int, int> IGLCX::Evolution2(const HFS_Problem &problem) {
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
    std::sort(seed_job_sequence.begin(), seed_job_sequence.end(), [&](int j1, int j2) {
        return problem.getJobs()[j1].get_total_process_time() > problem.getJobs()[j2].get_total_process_time();
    });

    Solution solution(problem, Solution::Solution_Init_Type::Empty);

    span = NEH_f(seed_job_sequence, solution);
    if (span < best_span) {
        best_span = span;
    }

 /*   span = iterative_improvement_insertion(solution, span);
    if (span < best_span) {
        best_span = span;
    }*/

    std::vector<int> erased_jobs;
    std::vector<int> original_job_sequence = solution.getJobSequence();
    int original_span = span;

    int total_iteration_count = 0;
    bool flag = true;
    int r=0;
    //std::cout << "\titeration" << total_iteration_count << ":\t" << best_span << std::endl;
    while (true) {
        erased_jobs.clear();

        destruction1(solution, erased_jobs, flag,r);
        span = construction(solution, erased_jobs);


     /*  solution.decode_forward_to_graph();
        span = forward_decode_local_search_cut_nodes(solution, span);*/

        // span = iterative_improvement_insertion(solution, span);
        if (span < best_span) {
            best_span = span;
            //std::cout << "\titeration" << total_iteration_count << ":\t" << best_span << std::endl;
        }

        if (span <= original_span) {
            original_span = span;
            original_job_sequence = solution.getJobSequence();
        } else {
            flag = false;
            double rand_value = wyt_rand(0.0, 1.0);
            if (rand_value
                < std::exp(-(span - original_span) * num_of_jobs * num_of_stages * 10 / (m_T * total_process_time))) {
                original_span = span;
                original_job_sequence = solution.getJobSequence();
                // flag=false;
            } else {
                solution.setJobSequence(original_job_sequence);
                span = original_span;
            }
        }

        ++total_iteration_count;

        long cur_time = GetElapsedProcessTime();
        long ElapsedTime = cur_time - algorithm_start_time;
        if (ElapsedTime > time_limit) {
            break;
        }
    }

    return {best_span, total_iteration_count};
}

std::pair<int, int> IGLCX::Evolution_Speed(const HFS_Problem &problem) {
    auto start = std::chrono::system_clock::now();
    int num_of_jobs = problem.getNumOfJobs();
    int span;

    std::vector<int> seed_job_sequence(num_of_jobs);
    std::iota(std::begin(seed_job_sequence), std::end(seed_job_sequence), 0);
    std::sort(seed_job_sequence.begin(), seed_job_sequence.end(), [&](int j1, int j2) {
        return problem.getJobs()[j1].get_total_process_time() > problem.getJobs()[j2].get_total_process_time();
    });
    Solution solution(problem, Solution::Solution_Init_Type::Empty);
    span = NEH_f(seed_job_sequence, solution);
    solution.decode_forward_to_graph();
    span = forward_decode_local_search_cut_nodes(solution, span);
    //span= iterative_improvement_insertion(solution,span);
    auto end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    return {span,duration.count()};
}