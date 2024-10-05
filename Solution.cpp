
#include <numeric>
#include <iostream>
#include <stack>
#include <queue>
#include <fstream>
#include <cassert>
#include "Solution.h"
#include "PyGantt.h"
#include <algorithm>

Solution::Solution(const HFS_Problem &problem, Solution_Init_Type init_type)
        : problem(problem) {
    switch (init_type) {
        case RandomInit:
            jobSequence.resize(problem.getNumOfJobs());
            std::iota(std::begin(jobSequence), std::end(jobSequence), 0);
            std::shuffle(std::begin(jobSequence), std::end(jobSequence), rand_generator());
            break;
        case Empty:
            break;
    }
    initBaseGraph();
}

Solution::Solution(const HFS_Problem &problem, const std::vector<int> &sequence)
        : problem(problem), jobSequence(sequence) {
    initBaseGraph();
}

Solution::Solution(const HFS_Problem &problem, const std::string &fileName)
        : problem(problem) {
    jobSequence.resize(problem.getNumOfJobs());
    std::ifstream fin(fileName);
    for (auto &job: jobSequence) {
        fin >> job;
    }
    fin.close();

    initBaseGraph();
}

/*Solution::Solution(const HFS_Problem &problem)
    : problem(problem)
{
    make_span=0;
}*/
Solution::Solution(const Solution &solution)
        : problem(solution.problem), jobSequence(solution.jobSequence), make_span(solution.make_span) {
    initBaseGraph();
    int num_of_stages = problem.getNumOfStages();
    int num_of_jobs = problem.getNumOfJobs();
    for (int s = 0; s < num_of_stages; ++s) {
        int machineCount = problem.getNumOfMachinesInStage(s);
        for (int m = 0; m < machineCount; ++m) {
            f_headOnMachinesInEachStage[s][m]->f_startTime = solution.f_headOnMachinesInEachStage[s][m]->f_startTime;
            f_headOnMachinesInEachStage[s][m]->b_startTime = solution.f_headOnMachinesInEachStage[s][m]->b_startTime;
            b_headOnMachinesInEachStage[s][m]->f_startTime = solution.b_headOnMachinesInEachStage[s][m]->f_startTime;
            b_headOnMachinesInEachStage[s][m]->b_startTime = solution.b_headOnMachinesInEachStage[s][m]->b_startTime;
        }
    }

    /*for (int j = 0; j < num_of_jobs; ++j)
    {
        f_headJobs[j]->f_startTime = solution.f_headJobs[j]->f_startTime;
        f_headJobs[j]->b_startTime = solution.f_headJobs[j]->b_startTime;
        b_headJobs[j]->f_startTime = solution.b_headJobs[j]->f_startTime;
        b_headJobs[j]->b_startTime = solution.b_headJobs[j]->b_startTime;
    }*/

    //std::vector<Node*> preOperationsByStage = f_headJobs;
    for (int s = 0; s < num_of_stages; ++s) {
        int machineCount = problem.getNumOfMachinesInStage(s);
        for (int m = 0; m < machineCount; ++m) {
            Node *f_head_node_at_machine = solution.f_headOnMachinesInEachStage[s][m];
            Node *node_at_machine = f_head_node_at_machine->sucByMachine;

            Node *pre_node_at_machine = f_headOnMachinesInEachStage[s][m];
            while (node_at_machine->jobId != -1) {
                int job_id = node_at_machine->jobId;
                //Node* operation = new Node(*node_at_machine);
                //Node* operation = operations[node_at_machine->stageId][job_id];
                Node *operation = operations[s][job_id];
                operation->f_startTime = node_at_machine->f_startTime;
                operation->b_startTime = node_at_machine->b_startTime;
                operation->machineId = node_at_machine->machineId;

                pre_node_at_machine->sucByMachine = operation;
                operation->sucByMachine = b_headOnMachinesInEachStage[s][m];

                b_headOnMachinesInEachStage[s][m]->preByMachine = operation;
                operation->preByMachine = pre_node_at_machine;

                //preOperationsByStage[job_id]->sucByStage = operation;
                //operation->sucByStage = b_headJobs[job_id];

                //b_headJobs[job_id]->preByStage = operation;
                //operation->preByStage = preOperationsByStage[job_id];

                pre_node_at_machine = operation;
                //preOperationsByStage[job_id] = operation;
                node_at_machine = node_at_machine->sucByMachine;
            }
        }
    }
}

Solution &Solution::operator=(const Solution &solution) {
    if (this == &solution) {
        return *this;
    }

    jobSequence = solution.jobSequence;

    int num_of_stages = problem.getNumOfStages();
    int num_of_jobs = problem.getNumOfJobs();

    for (int s = 0; s < num_of_stages; ++s) {
        int machineCount = problem.getNumOfMachinesInStage(s);
        for (int m = 0; m < machineCount; ++m) {
            f_headOnMachinesInEachStage[s][m]->f_startTime = solution.f_headOnMachinesInEachStage[s][m]->f_startTime;
            f_headOnMachinesInEachStage[s][m]->b_startTime = solution.f_headOnMachinesInEachStage[s][m]->b_startTime;
            b_headOnMachinesInEachStage[s][m]->f_startTime = solution.b_headOnMachinesInEachStage[s][m]->f_startTime;
            b_headOnMachinesInEachStage[s][m]->b_startTime = solution.b_headOnMachinesInEachStage[s][m]->b_startTime;
        }
    }

    /*for (int j = 0; j < num_of_jobs; ++j)
    {
        f_headJobs[j]->f_startTime = solution.f_headJobs[j]->f_startTime;
        f_headJobs[j]->b_startTime = solution.f_headJobs[j]->b_startTime;
        b_headJobs[j]->f_startTime = solution.b_headJobs[j]->f_startTime;
        b_headJobs[j]->b_startTime = solution.b_headJobs[j]->b_startTime;
    }*/

    //std::vector<Node*> preOperationsByStage = f_headJobs;
    for (int s = 0; s < num_of_stages; ++s) {
        int machineCount = problem.getNumOfMachinesInStage(s);
        for (int m = 0; m < machineCount; ++m) {
            Node *f_head_node_at_machine = solution.f_headOnMachinesInEachStage[s][m];
            Node *node_at_machine = f_head_node_at_machine->sucByMachine;
            if (node_at_machine->jobId == -1) {
                f_headOnMachinesInEachStage[s][m]->sucByMachine = b_headOnMachinesInEachStage[s][m];
                b_headOnMachinesInEachStage[s][m]->preByMachine = f_headOnMachinesInEachStage[s][m];
                continue;
            }
            Node *pre_node_at_machine = f_headOnMachinesInEachStage[s][m];
            while (node_at_machine->jobId != -1) {
                int job_id = node_at_machine->jobId;
                //Node* operation = new Node(*node_at_machine);
                Node *operation = operations[s][job_id];
                operation->f_startTime = node_at_machine->f_startTime;
                operation->b_startTime = node_at_machine->b_startTime;
                operation->machineId = node_at_machine->machineId;

                pre_node_at_machine->sucByMachine = operation;
                operation->sucByMachine = b_headOnMachinesInEachStage[s][m];

                b_headOnMachinesInEachStage[s][m]->preByMachine = operation;
                operation->preByMachine = pre_node_at_machine;

                //preOperationsByStage[job_id]->sucByStage = operation;
                //operation->sucByStage = b_headJobs[job_id];

                //b_headJobs[job_id]->preByStage = operation;
                //operation->preByStage = preOperationsByStage[job_id];

                pre_node_at_machine = operation;
                //preOperationsByStage[job_id] = operation;
                node_at_machine = node_at_machine->sucByMachine;
            }
        }
    }

    return *this;
}

Solution::~Solution() {
    //deleteGraph();

    for (const auto &operations_st_stage: operations) {
        for (auto op: operations_st_stage) {
            delete op;
        }
    }

    int num_of_stages = problem.getNumOfStages();
    for (int s = 0; s < num_of_stages; ++s) {
        int machineCount = problem.getNumOfMachinesInStage(s);
        for (int m = 0; m < machineCount; ++m) {
            delete f_headOnMachinesInEachStage[s][m];
            delete b_headOnMachinesInEachStage[s][m];
        }
    }

    std::for_each(std::begin(f_headJobs), std::end(f_headJobs), [](const Node *ptr) {
        delete ptr;
    });
    std::for_each(std::begin(b_headJobs), std::end(b_headJobs), [](const Node *ptr) {
        delete ptr;
    });
}

void Solution::initBaseGraph() {
    int num_of_jobs = problem.getNumOfJobs();
    int num_of_stages = problem.getNumOfStages();
    const std::vector<Job> &jobs = problem.getJobs();

    operations.resize(num_of_stages);
    for (int s = 0; s < num_of_stages; ++s) {
        for (const auto &job: jobs) {
            operations[s].emplace_back(new Node(job.getId(), s, -1, job.getProcessTime(s)));
        }
    }

    for (int jobId = 0; jobId < num_of_jobs; ++jobId) {
        f_headJobs.emplace_back(new Node(jobId, -1, -1, 0));
        b_headJobs.emplace_back(new Node(jobId, -1, -1, 0));
        f_headJobs[jobId]->sucByStage = b_headJobs[jobId];
        b_headJobs[jobId]->preByStage = f_headJobs[jobId];
    }

    for (int s = 0; s < num_of_stages; ++s) {
        for (int jobId = 0; jobId < num_of_jobs; ++jobId) {
            operations[s][jobId]->sucByStage = b_headJobs[jobId];
            operations[s][jobId]->preByStage = b_headJobs[jobId]->preByStage;
            b_headJobs[jobId]->preByStage->sucByStage = operations[s][jobId];
            b_headJobs[jobId]->preByStage = operations[s][jobId];
        }
    }

    f_headOnMachinesInEachStage.resize(num_of_stages);
    b_headOnMachinesInEachStage.resize(num_of_stages);
    for (int s = 0; s < num_of_stages; ++s) {
        int machineCount = problem.getNumOfMachinesInStage(s);
        for (int m = 0; m < machineCount; ++m) {
            f_headOnMachinesInEachStage[s].emplace_back(new Node(-1, s, m, 0));
            b_headOnMachinesInEachStage[s].emplace_back(new Node(-1, s, m, 0));

            f_headOnMachinesInEachStage[s][m]->sucByMachine = b_headOnMachinesInEachStage[s][m];
            b_headOnMachinesInEachStage[s][m]->preByMachine = f_headOnMachinesInEachStage[s][m];
        }
    }
}

void Solution::reset_to_base_graph() {
    int num_of_jobs = problem.getNumOfJobs();
    int num_of_stages = problem.getNumOfStages();

    /*for (int jobId = 0; jobId < num_of_jobs; ++jobId)
    {
        f_headJobs[jobId]->sucByStage = b_headJobs[jobId];
        b_headJobs[jobId]->preByStage = f_headJobs[jobId];
    }*/

    for (int s = 0; s < num_of_stages; ++s) {
        int machineCount = problem.getNumOfMachinesInStage(s);
        for (int m = 0; m < machineCount; ++m) {
            f_headOnMachinesInEachStage[s][m]->sucByMachine = b_headOnMachinesInEachStage[s][m];
            b_headOnMachinesInEachStage[s][m]->preByMachine = f_headOnMachinesInEachStage[s][m];
        }
    }
}


int Solution::decode_forward() {
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
        c_time[job_id][0] = m_idle_time[0][mt] + operations[0][job_id]->processTime;
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
                    + operations[s][first_come_job_id]->processTime;
            m_idle_time[s][mt] = c_time[first_come_job_id][s];
            if (s == num_of_stages - 1) {
                span = std::max(span, c_time[first_come_job_id][s]);
            }
        }
    }
    return span;
}

int Solution::decode_backward() {
    int num_of_jobs = problem.getNumOfJobs();
    int num_of_stages = problem.getNumOfStages();

    //各工序的逆向完工时间
    std::vector<std::vector<int>> c_time(num_of_jobs, std::vector<int>(num_of_stages));
    //每个阶段每个机器的可用时间
    std::vector<std::vector<int>> m_idle_time(num_of_stages);
    for (int s = 0; s < problem.getNumOfStages(); s++) {
        m_idle_time[s].resize(problem.getNumOfMachinesInStage(s));
    }

    int span = 0;
    //最后阶段
    for (auto it = jobSequence.rbegin(); it != jobSequence.rend(); ++it) {
        int job_id = *it;
        int mt = min_element(m_idle_time[num_of_stages - 1].begin(), m_idle_time[num_of_stages - 1].end())
                 - m_idle_time[num_of_stages - 1].begin();
        c_time[job_id][num_of_stages - 1] =
                m_idle_time[num_of_stages - 1][mt] + operations[num_of_stages - 1][job_id]->processTime;
        m_idle_time[num_of_stages - 1][mt] = c_time[job_id][num_of_stages - 1];
    }

    std::vector<int> sequence = jobSequence;
    //其他阶段，FIFO和FAM
    for (int s = num_of_stages - 2; s >= 0; --s) {
        std::sort(sequence.begin(), sequence.end(), [&](int job1, int job2) {
            return c_time[job1][s + 1] > c_time[job2][s + 1];
        });
        for (auto it = sequence.rbegin(); it != sequence.rend(); ++it) {
            int first_come_job_id = *it;
            int mt = min_element(m_idle_time[s].begin(), m_idle_time[s].end()) - m_idle_time[s].begin();
            c_time[first_come_job_id][s] =
                    std::max(m_idle_time[s][mt], c_time[first_come_job_id][s + 1])
                    + operations[s][first_come_job_id]->processTime;
            m_idle_time[s][mt] = c_time[first_come_job_id][s];
            if (s == 0) {
                span = std::max(span, c_time[first_come_job_id][s]);
            }
        }
    }
    return span;
}

int Solution::decode_forward_to_graph() {
    reset_to_base_graph();
    int num_of_jobs = problem.getNumOfJobs();
    int num_of_stages = problem.getNumOfStages();

    //各工序的逆向完工时间
    std::vector<std::vector<int>> c_time(num_of_jobs, std::vector<int>(num_of_stages));
    //每个阶段每个机器的可用时间
    std::vector<std::vector<int>> m_idle_time(num_of_stages);
    for (int s = 0; s < problem.getNumOfStages(); s++) {
        m_idle_time[s].resize(problem.getNumOfMachinesInStage(s));
    }

    auto assign_job_fun = [&](Node *cur_operation_node, int job_id, int stage_id, int machine_id) {
        //cur_operation_node->is_visit = false;
        //cur_operation_node->dfn = 0;
        //cur_operation_node->low = 0;
        //cur_operation_node->critical_adjacent_count = 0;
        cur_operation_node->machineId = machine_id;

        cur_operation_node->preByMachine = b_headOnMachinesInEachStage[stage_id][machine_id]->preByMachine;
        cur_operation_node->sucByMachine = b_headOnMachinesInEachStage[stage_id][machine_id];

        b_headOnMachinesInEachStage[stage_id][machine_id]->preByMachine->sucByMachine = cur_operation_node;
        b_headOnMachinesInEachStage[stage_id][machine_id]->preByMachine = cur_operation_node;

        //cur_operation_node->preByStage = b_headJobs[job_id]->preByStage;
        //cur_operation_node->sucByStage = b_headJobs[job_id];
        //b_headJobs[job_id]->preByStage->sucByStage = cur_operation_node;
        //b_headJobs[job_id]->preByStage = cur_operation_node;

        b_headOnMachinesInEachStage[stage_id][machine_id]->f_startTime =
                cur_operation_node->f_startTime + cur_operation_node->processTime;
    };

    //第一阶段
    for (auto job_id: jobSequence) {
        Node *cur_operation_node = operations[0][job_id];

        int mt = min_element(m_idle_time[0].begin(), m_idle_time[0].end()) - m_idle_time[0].begin();
        cur_operation_node->f_startTime = m_idle_time[0][mt];
        c_time[job_id][0] = cur_operation_node->f_startTime + cur_operation_node->processTime;
        m_idle_time[0][mt] = c_time[job_id][0];

        assign_job_fun(cur_operation_node, job_id, 0, mt);
    }

    std::vector<int> sequence = jobSequence;
    //其他阶段，FIFO和FAM
    for (int s = 1; s < num_of_stages; ++s) {
        std::sort(sequence.begin(), sequence.end(), [&](int job1, int job2) {
            return c_time[job1][s - 1] < c_time[job2][s - 1];
        });
        for (int first_come_job_id: sequence) {
            Node *cur_operation_node = operations[s][first_come_job_id];

            int mt = min_element(m_idle_time[s].begin(), m_idle_time[s].end()) - m_idle_time[s].begin();
            cur_operation_node->f_startTime = std::max(m_idle_time[s][mt], c_time[first_come_job_id][s - 1]);

            c_time[first_come_job_id][s] = cur_operation_node->f_startTime + cur_operation_node->processTime;
            m_idle_time[s][mt] = c_time[first_come_job_id][s];

            assign_job_fun(cur_operation_node, first_come_job_id, s, mt);
        }
    }

    //计算逆向时间
    for (auto sit = std::rbegin(b_headOnMachinesInEachStage); sit != std::rend(b_headOnMachinesInEachStage); ++sit) {
        const std::vector<Node *> &tailOnMachines = *sit;
        for (auto mit = std::rbegin(tailOnMachines); mit != std::rend(tailOnMachines); ++mit) {
            Node *node = (*mit)->preByMachine;
            while (node->jobId != -1) {
                node->b_startTime = std::max(node->sucByMachine->b_startTime + node->sucByMachine->processTime,
                                             node->sucByStage->b_startTime + node->sucByStage->processTime);
                node = node->preByMachine;
            }
            node->b_startTime = node->sucByMachine->b_startTime + node->sucByMachine->processTime;
        }
    }
    int lastStageId = num_of_stages - 1;
    int span = (*std::max_element(std::begin(b_headOnMachinesInEachStage[lastStageId]),
                                  std::end(b_headOnMachinesInEachStage[lastStageId]), [](const Node *l, const Node *r) {
                return l->f_startTime < r->f_startTime;
            }))->f_startTime;
    return span;
}

int Solution::decode_backward_to_graph() {
    reset_to_base_graph();
    int num_of_jobs = problem.getNumOfJobs();
    int num_of_stages = problem.getNumOfStages();

    //各工序的逆向完工时间
    std::vector<std::vector<int>> c_time(num_of_jobs, std::vector<int>(num_of_stages));
    //每个阶段每个机器的可用时间
    std::vector<std::vector<int>> m_idle_time(num_of_stages);
    for (int s = 0; s < problem.getNumOfStages(); s++) {
        m_idle_time[s].resize(problem.getNumOfMachinesInStage(s));
    }

    auto assign_job_fun = [&](Node *cur_operation_node, int job_id, int stage_id, int machine_id) {
        //cur_operation_node->is_visit = false;
        //cur_operation_node->dfn = 0;
        //cur_operation_node->low = 0;
        //cur_operation_node->critical_adjacent_count = 0;
        cur_operation_node->machineId = machine_id;

        cur_operation_node->sucByMachine = f_headOnMachinesInEachStage[stage_id][machine_id]->sucByMachine;
        cur_operation_node->preByMachine = f_headOnMachinesInEachStage[stage_id][machine_id];

        f_headOnMachinesInEachStage[stage_id][machine_id]->sucByMachine->preByMachine = cur_operation_node;
        f_headOnMachinesInEachStage[stage_id][machine_id]->sucByMachine = cur_operation_node;

        //cur_operation_node->sucByStage = f_headJobs[job_id]->sucByStage;
        //cur_operation_node->preByStage = f_headJobs[job_id];
        //f_headJobs[job_id]->sucByStage->preByStage = cur_operation_node;
        //f_headJobs[job_id]->sucByStage = cur_operation_node;

        f_headOnMachinesInEachStage[stage_id][machine_id]->b_startTime =
                cur_operation_node->b_startTime + cur_operation_node->processTime;
    };

    //最后阶段
    for (auto it = jobSequence.rbegin(); it != jobSequence.rend(); ++it) {
        int job_id = *it;
        Node *cur_operation_node = operations[num_of_stages - 1][job_id];
        int mt = min_element(m_idle_time[num_of_stages - 1].begin(), m_idle_time[num_of_stages - 1].end())
                 - m_idle_time[num_of_stages - 1].begin();
        cur_operation_node->b_startTime = m_idle_time[num_of_stages - 1][mt];
        c_time[job_id][num_of_stages - 1] = cur_operation_node->b_startTime + cur_operation_node->processTime;
        m_idle_time[num_of_stages - 1][mt] = c_time[job_id][num_of_stages - 1];

        assign_job_fun(cur_operation_node, job_id, num_of_stages - 1, mt);
    }

    std::vector<int> sequence = jobSequence;
    //其他阶段，FIFO和FAM
    for (int s = num_of_stages - 2; s >= 0; --s) {
        std::sort(sequence.begin(), sequence.end(), [&](int job1, int job2) {
            return c_time[job1][s + 1] > c_time[job2][s + 1];
        });
        for (auto it = sequence.rbegin(); it != sequence.rend(); ++it) {
            int first_come_job_id = *it;
            Node *cur_operation_node = operations[s][first_come_job_id];

            int mt = min_element(m_idle_time[s].begin(), m_idle_time[s].end()) - m_idle_time[s].begin();
            cur_operation_node->b_startTime = std::max(m_idle_time[s][mt], c_time[first_come_job_id][s + 1]);

            c_time[first_come_job_id][s] = cur_operation_node->b_startTime + cur_operation_node->processTime;
            m_idle_time[s][mt] = c_time[first_come_job_id][s];

            assign_job_fun(cur_operation_node, first_come_job_id, s, mt);
        }
    }

    //计算正向时间
    for (auto &headOnMachines: f_headOnMachinesInEachStage) {
        for (auto &head: headOnMachines) {
            Node *node = head->sucByMachine;
            while (node->jobId != -1) {
                node->f_startTime = std::max(node->preByMachine->f_startTime + node->preByMachine->processTime,
                                             node->preByStage->f_startTime + node->preByStage->processTime);
                node = node->sucByMachine;
            }
            node->f_startTime = node->preByMachine->f_startTime + node->preByMachine->processTime;
        }
    }
    int span = (*std::max_element(std::begin(f_headOnMachinesInEachStage[0]),
                                  std::end(f_headOnMachinesInEachStage[0]), [](const Node *l, const Node *r) {
                return l->b_startTime < r->b_startTime;
            }))->b_startTime;
    return span;
}

int Solution::refreshAllGraph() {
    //重新计算正向时间
    for (const auto &headOnMachines: f_headOnMachinesInEachStage) {
        for (auto headOnMachine: headOnMachines) {
            Node *node = headOnMachine->sucByMachine;
            while (node->jobId != -1) {
                node->f_startTime = std::max(node->preByMachine->f_startTime + node->preByMachine->processTime,
                                             node->preByStage->f_startTime + node->preByStage->processTime);

                //node->is_visit = false;
                //node->dfn = 0;
                //node->low = 0;
                //node->critical_adjacent_count = 0;

                node = node->sucByMachine;
            }
            node->f_startTime = node->preByMachine->f_startTime + node->preByMachine->processTime;
        }
    }

    //int b_span = 0;
    //重新计算逆向时间
    for (auto sit = std::rbegin(b_headOnMachinesInEachStage); sit != std::rend(b_headOnMachinesInEachStage); ++sit) {
        const std::vector<Node *> &tailOnMachines = *sit;
        for (auto mit = std::rbegin(tailOnMachines); mit != std::rend(tailOnMachines); ++mit) {
            Node *node = (*mit)->preByMachine;
            while (node->jobId != -1) {
                node->b_startTime = std::max(node->sucByMachine->b_startTime + node->sucByMachine->processTime,
                                             node->sucByStage->b_startTime + node->sucByStage->processTime);
                node = node->preByMachine;
            }
            node->b_startTime = node->sucByMachine->b_startTime + node->sucByMachine->processTime;
            //b_span = std::max(b_span, node->b_startTime);
        }
    }
    //assert(f_span == b_span);
    int lastStageId = problem.getNumOfStages() - 1;
    int span = (*std::max_element(std::begin(b_headOnMachinesInEachStage[lastStageId]),
                                  std::end(b_headOnMachinesInEachStage[lastStageId]),
                                  [](const Node *n1, const Node *n2) {
                                      return n1->f_startTime < n2->f_startTime;
                                  }))->f_startTime;
    return span;
}

int Solution::reCalculateMakeSpan() {
    int lastStageId = problem.getNumOfStages() - 1;
    int span = (*std::max_element(std::begin(b_headOnMachinesInEachStage[lastStageId]),
                                  std::end(b_headOnMachinesInEachStage[lastStageId]), [](const Node *l, const Node *r) {
                return l->f_startTime < r->f_startTime;
            }))->f_startTime;
    return span;
}

void Solution::findOneCriticalPathMachineFirstBackward(std::vector<Node *> &criticalPath, int span) const {
    criticalPath.clear();
    int num_of_stages = problem.getNumOfStages();
    Node *critical_node = *std::max_element(std::begin(b_headOnMachinesInEachStage[num_of_stages - 1]),
                                            std::end(b_headOnMachinesInEachStage[num_of_stages - 1]),
                                            [](const Node *l, const Node *r) {
                                                return l->f_startTime < r->f_startTime;
                                            });
    critical_node = critical_node->preByMachine;
    while (critical_node->jobId != -1) {
        criticalPath.emplace_back(critical_node);
        if (critical_node->preByMachine->f_startTime + critical_node->preByMachine->processTime ==
            critical_node->f_startTime) {
            critical_node = critical_node->preByMachine;
        } else {
            critical_node = critical_node->preByStage;
        }
    }
}

void Solution::findOneCriticalPathJobFirstBackward(std::vector<Node *> &criticalPath, int span) const {
    criticalPath.clear();
    int num_of_stages = problem.getNumOfStages();
    Node *critical_node = *std::max_element(std::begin(b_headOnMachinesInEachStage[num_of_stages - 1]),
                                            std::end(b_headOnMachinesInEachStage[num_of_stages - 1]),
                                            [](const Node *l, const Node *r) {
                                                return l->f_startTime < r->f_startTime;
                                            });
    critical_node = critical_node->preByMachine;
    while (critical_node->jobId != -1) {
        criticalPath.emplace_back(critical_node);
        if (critical_node->preByStage->stageId != -1
            && critical_node->preByStage->f_startTime + critical_node->preByStage->processTime ==
               critical_node->f_startTime) {
            critical_node = critical_node->preByStage;
        } else {
            critical_node = critical_node->preByMachine;
        }
    }
}

/*void Solution::findCutNodesForward(std::vector<Node *> &critical_nodes, std::vector<Node *> &cutNodes, int span) const {

    int num_of_stages = problem.getNumOfStages();

    std::queue<Node *> nodesQueue;

    Node critical_graph_start_node(INT32_MIN, INT32_MIN, INT32_MIN, INT32_MIN);
    Node critical_graph_end_node(INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX);

    for (Node *f_head: f_headOnMachinesInEachStage[0]) {
        Node *node = f_head->sucByMachine;
        if (node->jobId != -1 && node->b_startTime + node->processTime == span) {
            critical_graph_start_node.critical_adjacent[critical_graph_start_node.critical_adjacent_count] = node;
            ++critical_graph_start_node.critical_adjacent_count;
            node->critical_adjacent[node->critical_adjacent_count] = &critical_graph_start_node;
            ++node->critical_adjacent_count;
            node->is_visit = true;
            critical_nodes.emplace_back(node);
            nodesQueue.push(node);
        }
    }

    while (!nodesQueue.empty()) {
        Node *node = nodesQueue.front();
        nodesQueue.pop();
        Node *suc_node_on_machine = node->sucByMachine;
        if (suc_node_on_machine->jobId != -1
            && suc_node_on_machine->b_startTime + suc_node_on_machine->processTime == node->b_startTime) {
            node->critical_adjacent[node->critical_adjacent_count] = suc_node_on_machine;
            ++node->critical_adjacent_count;
            suc_node_on_machine->critical_adjacent[suc_node_on_machine->critical_adjacent_count] = node;
            ++suc_node_on_machine->critical_adjacent_count;
            if (!suc_node_on_machine->is_visit) {
                suc_node_on_machine->is_visit = true;
                critical_nodes.emplace_back(suc_node_on_machine);
                nodesQueue.push(suc_node_on_machine);
            }
        }

        Node *suc_node_at_stage = node->sucByStage;
        if (suc_node_at_stage->stageId != -1
            && suc_node_at_stage->b_startTime + suc_node_at_stage->processTime == node->b_startTime) {
            node->critical_adjacent[node->critical_adjacent_count] = suc_node_at_stage;
            ++node->critical_adjacent_count;
            suc_node_at_stage->critical_adjacent[suc_node_at_stage->critical_adjacent_count] = node;
            ++suc_node_at_stage->critical_adjacent_count;
            if (!suc_node_at_stage->is_visit) {
                suc_node_at_stage->is_visit = true;
                critical_nodes.emplace_back(suc_node_at_stage);
                nodesQueue.push(suc_node_at_stage);
            }
        }
    }

    for (Node *b_head: b_headOnMachinesInEachStage[num_of_stages - 1]) {
        Node *node = b_head->preByMachine;
        if (node->jobId != -1 && node->f_startTime + node->processTime == span) {
            critical_graph_end_node.critical_adjacent[critical_graph_end_node.critical_adjacent_count] = node;
            ++critical_graph_end_node.critical_adjacent_count;
            node->critical_adjacent[node->critical_adjacent_count] = &critical_graph_end_node;
            ++node->critical_adjacent_count;
        }
    }

    std::stack<std::pair<Node *, Node *>> nodeStack;
    nodeStack.emplace(&critical_graph_start_node, nullptr);
    int timeStamp = 1;

    while (!nodeStack.empty()) {
        std::pair<Node *, Node *> &cur_pair = nodeStack.top();
        Node *curNode = cur_pair.first;
        Node *parentNode = cur_pair.second;
        if (curNode->dfn == 0) {
            curNode->dfn = curNode->low = timeStamp;
            ++timeStamp;
        }

        bool flag = false;

        while (true) {
            if (curNode->critical_adjacent_count == 0) {
                break;
            }
            Node *childNode = curNode->critical_adjacent[curNode->critical_adjacent_count - 1];;
            --curNode->critical_adjacent_count;
            if (childNode == parentNode) {
                continue;
            }
            if (childNode->dfn == 0) {
                nodeStack.emplace(childNode, curNode);
                flag = true;
                break;
            } else {
                curNode->low = std::min(curNode->low, childNode->dfn);
            }
        }

        if (!flag) {
            std::pair<Node *, Node *> tp = nodeStack.top();
            nodeStack.pop();
            Node *node = std::get<0>(tp);
            Node *pNode = std::get<1>(tp);
            if (!nodeStack.empty()) {
                if (pNode->dfn <= node->low && pNode != &critical_graph_start_node) {
                    cutNodes.emplace_back(pNode);
                }
                pNode->low = std::min(node->low, pNode->low);
            }
        }
    }

    for (auto node_ptr: critical_nodes) {
        node_ptr->is_visit = false;
        node_ptr->dfn = 0;
        node_ptr->low = 0;
        node_ptr->critical_adjacent_count = 0;
    }
}*/

void Solution::findCutNodesForward(std::vector<Node *> &critical_nodes, std::vector<Node *> &cutNodes, int span) const {

    int num_of_stages = problem.getNumOfStages();

    std::queue<Node *> nodesQueue;

    // 创建关键路径图的起始和终止节点
    Node critical_graph_start_node(INT32_MIN, INT32_MIN, INT32_MIN, INT32_MIN);
    Node critical_graph_end_node(INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX);

    // 初始化关键路径图的起始节点
    for (Node *f_head: f_headOnMachinesInEachStage[0]) {
        Node *node = f_head->sucByMachine;
        if (node->jobId != -1 && node->b_startTime + node->processTime == span) {
            // 更新起始节点和当前节点的邻接信息
            critical_graph_start_node.critical_adjacent[critical_graph_start_node.critical_adjacent_count] = node;
            ++critical_graph_start_node.critical_adjacent_count;
            node->critical_adjacent[node->critical_adjacent_count] = &critical_graph_start_node;
            ++node->critical_adjacent_count;
            node->is_visit = true;
            critical_nodes.emplace_back(node);
            nodesQueue.push(node);
        }
    }

    // 在关键路径图上执行BFS
    while (!nodesQueue.empty()) {
        Node *node = nodesQueue.front();
        nodesQueue.pop();
        Node *suc_node_on_machine = node->sucByMachine;
        if (suc_node_on_machine->jobId != -1
            && suc_node_on_machine->b_startTime + suc_node_on_machine->processTime == node->b_startTime) {
            // 更新当前节点和机器上的后继节点的邻接信息
            node->critical_adjacent[node->critical_adjacent_count] = suc_node_on_machine;
            ++node->critical_adjacent_count;
            suc_node_on_machine->critical_adjacent[suc_node_on_machine->critical_adjacent_count] = node;
            ++suc_node_on_machine->critical_adjacent_count;
            if (!suc_node_on_machine->is_visit) {
                suc_node_on_machine->is_visit = true;
                critical_nodes.emplace_back(suc_node_on_machine);
                nodesQueue.push(suc_node_on_machine);
            }
        }

        Node *suc_node_at_stage = node->sucByStage;
        if (suc_node_at_stage->stageId != -1
            && suc_node_at_stage->b_startTime + suc_node_at_stage->processTime == node->b_startTime) {
            // 更新当前节点和阶段上的后继节点的邻接信息
            node->critical_adjacent[node->critical_adjacent_count] = suc_node_at_stage;
            ++node->critical_adjacent_count;
            suc_node_at_stage->critical_adjacent[suc_node_at_stage->critical_adjacent_count] = node;
            ++suc_node_at_stage->critical_adjacent_count;
            if (!suc_node_at_stage->is_visit) {
                suc_node_at_stage->is_visit = true;
                critical_nodes.emplace_back(suc_node_at_stage);
                nodesQueue.push(suc_node_at_stage);
            }
        }
    }

    // 初始化关键路径图的结束节点
    for (Node *b_head: b_headOnMachinesInEachStage[num_of_stages - 1]) {
        Node *node = b_head->preByMachine;
        if (node->jobId != -1 && node->f_startTime + node->processTime == span) {
            // 更新结束节点和当前节点的邻接信息
            critical_graph_end_node.critical_adjacent[critical_graph_end_node.critical_adjacent_count] = node;
            ++critical_graph_end_node.critical_adjacent_count;
            node->critical_adjacent[node->critical_adjacent_count] = &critical_graph_end_node;
            ++node->critical_adjacent_count;
        }
    }

    // 使用堆栈进行DFS
    std::stack<std::pair<Node *, Node *>> nodeStack;
    nodeStack.emplace(&critical_graph_start_node, nullptr);
    int timeStamp = 1;

    while (!nodeStack.empty()) {
        std::pair<Node *, Node *> &cur_pair = nodeStack.top();
        Node *curNode = cur_pair.first;
        Node *parentNode = cur_pair.second;
        if (curNode->dfn == 0) {
            curNode->dfn = curNode->low = timeStamp;
            ++timeStamp;
        }

        bool flag = false;

        while (true) {
            if (curNode->critical_adjacent_count == 0) {
                break;
            }
            Node *childNode = curNode->critical_adjacent[curNode->critical_adjacent_count - 1];;
            --curNode->critical_adjacent_count;
            if (childNode == parentNode) {
                continue;
            }
            if (childNode->dfn == 0) {
                nodeStack.emplace(childNode, curNode);
                flag = true;
                break;
            } else {
                curNode->low = std::min(curNode->low, childNode->dfn);
            }
        }

        if (!flag) {
            std::pair<Node *, Node *> tp = nodeStack.top();
            nodeStack.pop();
            Node *node = std::get<0>(tp);
            Node *pNode = std::get<1>(tp);
            if (!nodeStack.empty()) {
                if (pNode->dfn <= node->low && pNode != &critical_graph_start_node) {
                    cutNodes.emplace_back(pNode);
                }
                pNode->low = std::min(node->low, pNode->low);
            }
        }
    }

    // 重置节点信息
    for (auto node_ptr: critical_nodes) {
        node_ptr->is_visit = false;
        node_ptr->dfn = 0;
        node_ptr->low = 0;
        node_ptr->critical_adjacent_count = 0;
    }
}


void
Solution::findCutNodesBackward(std::vector<Node *> &critical_nodes, std::vector<Node *> &cutNodes, int span) const {
    int num_of_stages = problem.getNumOfStages();

    std::queue<Node *> nodesQueue;

    Node critical_graph_start_node(INT32_MIN, INT32_MIN, INT32_MIN, INT32_MIN);
    Node critical_graph_end_node(INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX);

    for (Node *b_head: b_headOnMachinesInEachStage[num_of_stages - 1]) {
        Node *node = b_head->preByMachine;
        if (node->jobId != -1 && node->f_startTime + node->processTime == span) {
            critical_graph_start_node.critical_adjacent[critical_graph_start_node.critical_adjacent_count] = node;
            ++critical_graph_start_node.critical_adjacent_count;
            node->critical_adjacent[node->critical_adjacent_count] = &critical_graph_start_node;
            ++node->critical_adjacent_count;
            node->is_visit = true;
            critical_nodes.emplace_back(node);
            nodesQueue.push(node);
        }
    }

    while (!nodesQueue.empty()) {
        Node *node = nodesQueue.front();
        nodesQueue.pop();
        Node *pre_node_on_machine = node->preByMachine;
        if (pre_node_on_machine->jobId != -1
            && pre_node_on_machine->f_startTime + pre_node_on_machine->processTime == node->f_startTime) {
            node->critical_adjacent[node->critical_adjacent_count] = pre_node_on_machine;
            ++node->critical_adjacent_count;
            pre_node_on_machine->critical_adjacent[pre_node_on_machine->critical_adjacent_count] = node;
            ++pre_node_on_machine->critical_adjacent_count;
            if (!pre_node_on_machine->is_visit) {
                pre_node_on_machine->is_visit = true;
                critical_nodes.emplace_back(pre_node_on_machine);
                nodesQueue.push(pre_node_on_machine);
            }
        }

        Node *pre_node_at_stage = node->preByStage;
        if (pre_node_at_stage->stageId != -1
            && pre_node_at_stage->f_startTime + pre_node_at_stage->processTime == node->f_startTime) {
            node->critical_adjacent[node->critical_adjacent_count] = pre_node_at_stage;
            ++node->critical_adjacent_count;
            pre_node_at_stage->critical_adjacent[pre_node_at_stage->critical_adjacent_count] = node;
            ++pre_node_at_stage->critical_adjacent_count;
            if (!pre_node_at_stage->is_visit) {
                pre_node_at_stage->is_visit = true;
                critical_nodes.emplace_back(pre_node_at_stage);
                nodesQueue.push(pre_node_at_stage);
            }
        }
    }

    for (Node *f_head: f_headOnMachinesInEachStage[0]) {
        Node *node = f_head->sucByMachine;
        if (node->jobId != -1 && node->b_startTime + node->processTime == span) {
            critical_graph_end_node.critical_adjacent[critical_graph_end_node.critical_adjacent_count] = node;
            ++critical_graph_end_node.critical_adjacent_count;
            node->critical_adjacent[node->critical_adjacent_count] = &critical_graph_end_node;
            ++node->critical_adjacent_count;
        }
    }
// 创建一个存储节点指针对的栈
    std::stack<std::pair<Node *, Node *>> nodeStack;
// 将起始节点和nullptr压入栈中
    nodeStack.emplace(&critical_graph_start_node, nullptr);
// 初始化时间戳为1
    int timeStamp = 1;

// 当栈不为空时执行循环
    while (!nodeStack.empty()) {
        // 获取栈顶节点对的引用
        std::pair<Node *, Node *> &cur_pair = nodeStack.top();
        // 当前节点指针
        Node *curNode = cur_pair.first;
        // 当前节点的父节点指针
        Node *parentNode = cur_pair.second;
        // 如果当前节点的dfn为0，表示当前节点未被访问
        if (curNode->dfn == 0) {
            // 更新当前节点的dfn和low值，并递增时间戳
            curNode->dfn = curNode->low = timeStamp;
            ++timeStamp;
        }

        // 用于标记是否找到新的子节点加入栈
        bool flag = false;

        // 处理当前节点的邻接节点
        while (true) {
            // 如果当前节点的关键邻接节点数量为0，则跳出循环
            if (curNode->critical_adjacent_count == 0) {
                break;
            }
            // 获取当前节点的一个关键邻接节点
            Node *childNode = curNode->critical_adjacent[curNode->critical_adjacent_count - 1];
            // 减少当前节点的关键邻接节点数量
            --curNode->critical_adjacent_count;
            // 如果邻接节点是父节点，则继续下一个邻接节点
            if (childNode == parentNode) {
                continue;
            }
            // 如果邻接节点未被访问过，则将其加入栈，并标记找到新的子节点
            if (childNode->dfn == 0) {
                nodeStack.emplace(childNode, curNode);
                flag = true;
                break;
            }
                // 否则更新当前节点的low值
            else {
                curNode->low = std::min(curNode->low, childNode->dfn);
            }
        }

        // 如果没有找到新的子节点加入栈
        if (!flag) {
            // 弹出栈顶节点对
            std::pair<Node *, Node *> tp = nodeStack.top();
            nodeStack.pop();
            // 获取弹出节点对应的节点指针
            Node *node = std::get<0>(tp);
            Node *pNode = std::get<1>(tp);
            // 如果栈不为空
            if (!nodeStack.empty()) {
                // 如果父节点的dfn小于当前节点的low，并且父节点不是起始节点
                if (pNode->dfn <= node->low && pNode != &critical_graph_start_node) {
                    // 将父节点加入cutNodes数组
                    cutNodes.emplace_back(pNode);
                }
                // 更新父节点的low值
                pNode->low = std::min(node->low, pNode->low);
            }
        }
    }

// 对所有关键节点进行重置操作
    for (auto node_ptr: critical_nodes) {
        // 将节点标记为未访问状态
        node_ptr->is_visit = false;
        // 重置节点的dfn、low和关键邻接节点数量
        node_ptr->dfn = 0;
        node_ptr->low = 0;
        node_ptr->critical_adjacent_count = 0;
    }
}

void Solution::findAllCriticalNodesForward(std::vector<Node *> &criticalNodes, int span) const {
    std::queue<Node *> nodesQueue;

    for (Node *f_head: f_headOnMachinesInEachStage[0]) {
        Node *node = f_head->sucByMachine;
        if (node->jobId != -1 && node->b_startTime + node->processTime == span) {
            node->is_visit = true;
            criticalNodes.emplace_back(node);
            nodesQueue.push(node);
        }
    }

    while (!nodesQueue.empty()) {
        Node *node = nodesQueue.front();
        nodesQueue.pop();
        Node *suc_node_on_machine = node->sucByMachine;
        if (suc_node_on_machine->jobId != -1 && !suc_node_on_machine->is_visit
            && suc_node_on_machine->b_startTime + suc_node_on_machine->processTime == node->b_startTime) {

            suc_node_on_machine->is_visit = true;
            criticalNodes.emplace_back(suc_node_on_machine);
            nodesQueue.push(suc_node_on_machine);
        }

        Node *suc_node_at_stage = node->sucByStage;
        if (suc_node_at_stage->stageId != -1 && !suc_node_at_stage->is_visit
            && suc_node_at_stage->b_startTime + suc_node_at_stage->processTime == node->b_startTime) {
            suc_node_at_stage->is_visit = true;
            criticalNodes.emplace_back(suc_node_at_stage);
            nodesQueue.push(suc_node_at_stage);
        }
    }

    for (auto node_ptr: criticalNodes) {
        node_ptr->is_visit = false;
    }
}

void Solution::findAllCriticalNodesBackward(std::vector<Node *> &criticalNodes, int span) const {
    int num_of_stages = problem.getNumOfStages();
    std::queue<Node *> nodesQueue;

    for (Node *b_head: b_headOnMachinesInEachStage[num_of_stages - 1]) {
        Node *node = b_head->preByMachine;
        if (node->jobId != -1 && node->f_startTime + node->processTime == span) {
            node->is_visit = true;
            criticalNodes.emplace_back(node);
            nodesQueue.push(node);
        }
    }

    while (!nodesQueue.empty()) {
        Node *node = nodesQueue.front();
        nodesQueue.pop();
        Node *pre_node_on_machine = node->preByMachine;
        if (pre_node_on_machine->jobId != -1 && !pre_node_on_machine->is_visit
            && pre_node_on_machine->f_startTime + pre_node_on_machine->processTime == node->f_startTime) {

            pre_node_on_machine->is_visit = true;
            criticalNodes.emplace_back(pre_node_on_machine);
            nodesQueue.push(pre_node_on_machine);
        }

        Node *pre_node_at_stage = node->preByStage;
        if (pre_node_at_stage->stageId != -1 && !pre_node_at_stage->is_visit
            && pre_node_at_stage->f_startTime + pre_node_at_stage->processTime == node->f_startTime) {
            pre_node_at_stage->is_visit = true;
            criticalNodes.emplace_back(pre_node_at_stage);
            nodesQueue.push(pre_node_at_stage);
        }
    }
    for (auto node_ptr: criticalNodes) {
        node_ptr->is_visit = false;
    }
}

void Solution::deleteNodePhysicalFromGraph(Node *deleted_node) {
    //同一机器断开
    deleted_node->preByMachine->sucByMachine = deleted_node->sucByMachine;
    deleted_node->sucByMachine->preByMachine = deleted_node->preByMachine;

    //前后阶段断开
    deleted_node->preByStage->sucByStage = deleted_node->sucByStage;
    deleted_node->sucByStage->preByStage = deleted_node->preByStage;
}

void Solution::insertNodeInPosition(Node *deleted_node, Node *node_before_insert_pos, Node *node_in_pre_stage) {
    node_before_insert_pos->sucByMachine->preByMachine = deleted_node;
    deleted_node->sucByMachine = node_before_insert_pos->sucByMachine;

    node_before_insert_pos->sucByMachine = deleted_node;
    deleted_node->preByMachine = node_before_insert_pos;

    node_in_pre_stage->sucByStage->preByStage = deleted_node;
    deleted_node->sucByStage = node_in_pre_stage->sucByStage;

    node_in_pre_stage->sucByStage = deleted_node;
    deleted_node->preByStage = node_in_pre_stage;

    deleted_node->machineId = node_before_insert_pos->machineId;
}

//随机交换两个点的位置
void Solution::swap_node_in_graph(const std::vector<Node *> &critical_nodes) {
    int num_of_jobs = getProblem().getNumOfJobs();
    int num_of_stages = getProblem().getNumOfStages();
    int job1_id = critical_nodes[wyt_rand(critical_nodes.size())]->jobId;
    int job2_id;
    do {
        job2_id = wyt_rand(num_of_jobs);
        //job2_id = critical_nodes[wyt_rand(critical_nodes.size())]->jobId;
    } while (job2_id == job1_id);

    Node *job1_node = f_headJobs[job1_id];
    Node *job2_node = f_headJobs[job2_id];
    Node *temp1;
    Node *temp2;
    for (int s = 0; s < num_of_stages; ++s) {
        job1_node = job1_node->sucByStage;
        job2_node = job2_node->sucByStage;
        if (job1_node->sucByMachine == job2_node) {
            /*temp1 = job1_node->preByMachine;
            temp2 = job2_node->sucByMachine;

            job1_node->sucByMachine = temp2;
            temp2->preByMachine = job1_node;
            job2_node->sucByMachine = job1_node;

            job2_node->preByMachine = temp1;
            temp1->sucByMachine = job2_node;
            job1_node->preByMachine = job2_node;*/

            job2_node->sucByMachine->preByMachine = job1_node;
            job1_node->sucByMachine = job2_node->sucByMachine;

            job2_node->preByMachine = job1_node->preByMachine;
            job2_node->sucByMachine = job1_node;
            job1_node->preByMachine->sucByMachine = job2_node;
            job1_node->preByMachine = job2_node;
        } else if (job2_node->sucByMachine == job1_node) {
            /*temp1 = job1_node->sucByMachine;
            temp2 = job2_node->preByMachine;

            job2_node->sucByMachine = temp1;
            temp1->preByMachine = job2_node;
            job1_node->sucByMachine = job2_node;

            job1_node->preByMachine = temp2;
            temp2->sucByMachine = job1_node;
            job2_node->preByMachine = job1_node;*/

            job2_node->preByMachine->sucByMachine = job1_node;
            job1_node->preByMachine = job2_node->preByMachine;

            job2_node->sucByMachine = job1_node->sucByMachine;
            job2_node->preByMachine = job1_node;
            job1_node->sucByMachine->preByMachine = job2_node;
            job1_node->sucByMachine = job2_node;

        } else {
            temp1 = job1_node->sucByMachine;
            temp2 = job2_node->sucByMachine;

            job1_node->sucByMachine = temp2;
            job2_node->sucByMachine = temp1;
            temp1->preByMachine = job2_node;
            temp2->preByMachine = job1_node;

            temp1 = job1_node->preByMachine;
            temp2 = job2_node->preByMachine;
            job1_node->preByMachine = temp2;
            job2_node->preByMachine = temp1;
            temp1->sucByMachine = job2_node;
            temp2->sucByMachine = job1_node;
        }
        std::swap(job1_node->machineId, job2_node->machineId);
    }
}

void Solution::swap_block_in_graph(const std::vector<Node *> &critical_nodes, int block_size) {
    int num_of_jobs = getProblem().getNumOfJobs();
    int num_of_stages = getProblem().getNumOfStages();
    int job1_id = critical_nodes[wyt_rand(critical_nodes.size())]->jobId;
    int job2_id;
    do {
        job2_id = wyt_rand(num_of_jobs);
        //job2_id = critical_nodes[wyt_rand(critical_nodes.size())]->jobId;
    } while (job2_id == job1_id);

    Node *job1_node = f_headJobs[job1_id];
    Node *job2_node = f_headJobs[job2_id];
    Node *temp1;
    Node *temp2;
    for (int s = 0; s < num_of_stages; ++s) {
        job1_node = job1_node->sucByStage;
        job2_node = job2_node->sucByStage;
        if (job1_node->sucByMachine == job2_node)//j1和j2换位置
        {

            job2_node->sucByMachine->preByMachine = job1_node;
            job1_node->sucByMachine = job2_node->sucByMachine;

            job2_node->preByMachine = job1_node->preByMachine;
            job2_node->sucByMachine = job1_node;
            job1_node->preByMachine->sucByMachine = job2_node;
            job1_node->preByMachine = job2_node;
        } else if (job2_node->sucByMachine == job1_node)//j1和j2换位置
        {
            job2_node->preByMachine->sucByMachine = job1_node;
            job1_node->preByMachine = job2_node->preByMachine;

            job2_node->sucByMachine = job1_node->sucByMachine;
            job2_node->preByMachine = job1_node;
            job1_node->sucByMachine->preByMachine = job2_node;
            job1_node->sucByMachine = job2_node;
        } else if (job1_node->sucByMachine->jobId != -1 && job2_node->sucByMachine->jobId != -1) {
            int size = block_size;
            Node *p = job1_node;
            while (size != 1) {
                p = p->sucByMachine;
                size--;
            }
            size = block_size;
            Node *q = job2_node;
            while (size != 1) {
                q = q->sucByMachine;
                size--;
            }
            Node *t1 = p->sucByMachine;
            Node *t2 = q->sucByMachine;

            p->sucByMachine = t2;
            t2->preByMachine = p;
            q->sucByMachine = t1;
            t1->preByMachine = q;

            t1 = job1_node->preByMachine;
            t2 = job2_node->preByMachine;

            t1->sucByMachine = job2_node;
            job2_node->preByMachine = t1;
            t2->sucByMachine = job1_node;
            job1_node->preByMachine = t2;

            p = job1_node->sucByMachine;
            q = job2_node->sucByMachine;
            while (block_size != 1) {
                block_size--;
                std::swap(p->machineId, q->machineId);
                p = p->sucByMachine;
                q = q->sucByMachine;
            }
        } else {
            temp1 = job1_node->sucByMachine;
            temp2 = job2_node->sucByMachine;

            job1_node->sucByMachine = temp2;
            job2_node->sucByMachine = temp1;
            temp1->preByMachine = job2_node;
            temp2->preByMachine = job1_node;

            temp1 = job1_node->preByMachine;
            temp2 = job2_node->preByMachine;
            job1_node->preByMachine = temp2;
            job2_node->preByMachine = temp1;
            temp1->sucByMachine = job2_node;
            temp2->sucByMachine = job1_node;
        }
        std::swap(job1_node->machineId, job2_node->machineId);
    }
}

//j2在j1后面
void Solution::insertion_node_in_graph0(const std::vector<Node *> &critical_nodes) {
    int num_of_jobs = getProblem().getNumOfJobs();
    int num_of_stages = getProblem().getNumOfStages();
    int job1_id = critical_nodes[wyt_rand(critical_nodes.size())]->jobId;
    int job2_id;
    do {
        job2_id = wyt_rand(num_of_jobs);
        //job2_id = critical_nodes[wyt_rand(critical_nodes.size())]->jobId;
    } while (job2_id == job1_id);

    Node *job1_node = f_headJobs[job1_id];
    Node *job2_node = f_headJobs[job2_id];
    bool after = wyt_rand();
    if (after) {
        for (int s = 0; s < num_of_stages; ++s) {
            job1_node = job1_node->sucByStage;
            job2_node = job2_node->sucByStage;
            if (job1_node->sucByMachine == job2_node) {
                continue;
            } else {
                job2_node->preByMachine->sucByMachine = job2_node->sucByMachine;
                job2_node->sucByMachine->preByMachine = job2_node->preByMachine;

                job2_node->sucByMachine = job1_node->sucByMachine;
                job2_node->preByMachine = job1_node;

                job1_node->sucByMachine->preByMachine = job2_node;
                job1_node->sucByMachine = job2_node;

                job2_node->machineId = job1_node->machineId;
            }
        }
    } else {
        for (int s = 0; s < num_of_stages; ++s) {
            job1_node = job1_node->sucByStage;
            job2_node = job2_node->sucByStage;
            if (job1_node->preByMachine == job2_node) {
                continue;
            } else {
                job2_node->sucByMachine->preByMachine = job2_node->preByMachine;
                job2_node->preByMachine->sucByMachine = job2_node->sucByMachine;

                job2_node->preByMachine = job1_node->preByMachine;
                job2_node->sucByMachine = job1_node;

                job1_node->preByMachine->sucByMachine = job2_node;
                job1_node->preByMachine = job2_node;

                job2_node->machineId = job1_node->machineId;
            }
        }
    }
}

//连续则换位置，否则根据判断来使其连续
void Solution::insertion_node_in_graph1(const std::vector<Node *> &critical_nodes) {
    int num_of_jobs = getProblem().getNumOfJobs();
    int num_of_stages = getProblem().getNumOfStages();
    int job1_id = critical_nodes[wyt_rand(critical_nodes.size())]->jobId;
    int job2_id;
    do {
        job2_id = wyt_rand(num_of_jobs);
        //job2_id = critical_nodes[wyt_rand(critical_nodes.size())]->jobId;
    } while (job2_id == job1_id);

    Node *job1_node = f_headJobs[job1_id];
    Node *job2_node = f_headJobs[job2_id];
    for (int s = 0; s < num_of_stages; ++s) {
        job1_node = job1_node->sucByStage;
        job2_node = job2_node->sucByStage;
        //换位置
        if (job1_node->sucByMachine == job2_node) {
            job2_node->sucByMachine->preByMachine = job2_node->preByMachine;
            job2_node->preByMachine->sucByMachine = job2_node->sucByMachine;

            job2_node->preByMachine = job1_node->preByMachine;
            job2_node->sucByMachine = job1_node;

            job1_node->preByMachine->sucByMachine = job2_node;
            job1_node->preByMachine = job2_node;

            job2_node->machineId = job1_node->machineId;
        }
            //换位置
        else if (job1_node->preByMachine == job2_node) {
            job2_node->preByMachine->sucByMachine = job2_node->sucByMachine;
            job2_node->sucByMachine->preByMachine = job2_node->preByMachine;

            job2_node->sucByMachine = job1_node->sucByMachine;
            job2_node->preByMachine = job1_node;

            job1_node->sucByMachine->preByMachine = job2_node;
            job1_node->sucByMachine = job2_node;

            job2_node->machineId = job1_node->machineId;
        } else {
            bool after = wyt_rand();
            //2在1后面
            if (after) {
                job2_node->preByMachine->sucByMachine = job2_node->sucByMachine;
                job2_node->sucByMachine->preByMachine = job2_node->preByMachine;

                job2_node->sucByMachine = job1_node->sucByMachine;
                job2_node->preByMachine = job1_node;

                job1_node->sucByMachine->preByMachine = job2_node;
                job1_node->sucByMachine = job2_node;

                job2_node->machineId = job1_node->machineId;
            }
                //1在2后面
            else {
                job2_node->sucByMachine->preByMachine = job2_node->preByMachine;
                job2_node->preByMachine->sucByMachine = job2_node->sucByMachine;

                job2_node->preByMachine = job1_node->preByMachine;
                job2_node->sucByMachine = job1_node;

                job1_node->preByMachine->sucByMachine = job2_node;
                job1_node->preByMachine = job2_node;

                job2_node->machineId = job1_node->machineId;
            }
        }
    }
}

void Solution::insertion_block_in_graph1(const std::vector<Node *> &critical_nodes, int block_size) {
    int num_of_jobs = getProblem().getNumOfJobs();
    int num_of_stages = getProblem().getNumOfStages();
    int job1_id = critical_nodes[wyt_rand(critical_nodes.size())]->jobId;
    int job2_id;
    do {
        job2_id = wyt_rand(num_of_jobs);
        //job2_id = critical_nodes[wyt_rand(critical_nodes.size())]->jobId;
    } while (job2_id == job1_id);

    Node *job1_node = f_headJobs[job1_id];
    Node *job2_node = f_headJobs[job2_id];
    for (int s = 0; s < num_of_stages; ++s) {
        job1_node = job1_node->sucByStage;
        job2_node = job2_node->sucByStage;
        //换位置
        if (job1_node->sucByMachine == job2_node) {
            job2_node->sucByMachine->preByMachine = job2_node->preByMachine;
            job2_node->preByMachine->sucByMachine = job2_node->sucByMachine;

            job2_node->preByMachine = job1_node->preByMachine;
            job2_node->sucByMachine = job1_node;

            job1_node->preByMachine->sucByMachine = job2_node;
            job1_node->preByMachine = job2_node;

            job2_node->machineId = job1_node->machineId;
        }
            //换位置
        else if (job1_node->preByMachine == job2_node) {
            job2_node->preByMachine->sucByMachine = job2_node->sucByMachine;
            job2_node->sucByMachine->preByMachine = job2_node->preByMachine;

            job2_node->sucByMachine = job1_node->sucByMachine;
            job2_node->preByMachine = job1_node;

            job1_node->sucByMachine->preByMachine = job2_node;
            job1_node->sucByMachine = job2_node;

            job2_node->machineId = job1_node->machineId;
        } else if (job1_node->sucByMachine->jobId != -1 && job2_node->sucByMachine->jobId != -1) {
            bool after = wyt_rand();
            int size = block_size;
            Node *p = job1_node;
            while (size != 1) {
                p = p->sucByMachine;
                size--;
            }
            size = block_size;
            Node *q = job2_node;
            while (size != 1) {
                q = q->sucByMachine;
                size--;
            }
            //2在1后面
            if (after) {
                job2_node->preByMachine->sucByMachine = q->sucByMachine;
                q->sucByMachine->preByMachine = job2_node->preByMachine;

                Node *t = p->sucByMachine;
                p->sucByMachine = job2_node;
                job2_node->preByMachine = p;
                q->sucByMachine = t;
                t->preByMachine = q;
                q = job2_node->sucByMachine;
                while (block_size != 1) {
                    block_size--;
                    q->machineId = job1_node->machineId;
                    q = q->sucByMachine;
                }
                job2_node->machineId = job1_node->machineId;
            } else {
                job1_node->preByMachine->sucByMachine = p->sucByMachine;
                p->sucByMachine->preByMachine = job1_node->preByMachine;

                Node *t = q->sucByMachine;
                q->sucByMachine = job1_node;
                job1_node->preByMachine = q;
                p->sucByMachine = t;
                t->preByMachine = p;
                p = job1_node->sucByMachine;
                while (block_size != 1) {
                    block_size--;
                    p->machineId = job2_node->machineId;
                    p = p->sucByMachine;
                }
                job1_node->machineId = job2_node->machineId;
            }
        }
    }
}

void Solution::disruption_node_at_one_stage(const std::vector<Node *> &critical_nodes) {
    int num_of_jobs = getProblem().getNumOfJobs();
    int num_of_stages = getProblem().getNumOfStages();

    Node *job1_node;
    Node *job2_node;

    int stage_id = wyt_rand(num_of_stages);

    int i = 0;

    while (i < critical_nodes.size()) {
        if (critical_nodes[i]->stageId == stage_id) {
            job1_node = critical_nodes[i];
            ++i;
            break;
        }
        ++i;
    }

    int job2_id;
    do {
        job2_id = wyt_rand(num_of_jobs);
        //job2_id = critical_nodes[wyt_rand(critical_nodes.size())]->jobId;
    } while (job2_id == job1_node->jobId);

    job2_node = operations[stage_id][job2_id];
//换位置
    if (job1_node->sucByMachine == job2_node) {
        job2_node->sucByMachine->preByMachine = job2_node->preByMachine;
        job2_node->preByMachine->sucByMachine = job2_node->sucByMachine;

        job2_node->preByMachine = job1_node->preByMachine;
        job2_node->sucByMachine = job1_node;

        job1_node->preByMachine->sucByMachine = job2_node;
        job1_node->preByMachine = job2_node;

        job2_node->machineId = job1_node->machineId;
    }
        //换位置
    else if (job1_node->preByMachine == job2_node) {
        job2_node->preByMachine->sucByMachine = job2_node->sucByMachine;
        job2_node->sucByMachine->preByMachine = job2_node->preByMachine;

        job2_node->sucByMachine = job1_node->sucByMachine;
        job2_node->preByMachine = job1_node;

        job1_node->sucByMachine->preByMachine = job2_node;
        job1_node->sucByMachine = job2_node;

        job2_node->machineId = job1_node->machineId;
    } else {
        bool after = wyt_rand();
        if (after) {
            job2_node->preByMachine->sucByMachine = job2_node->sucByMachine;
            job2_node->sucByMachine->preByMachine = job2_node->preByMachine;

            job2_node->sucByMachine = job1_node->sucByMachine;
            job2_node->preByMachine = job1_node;

            job1_node->sucByMachine->preByMachine = job2_node;
            job1_node->sucByMachine = job2_node;

            job2_node->machineId = job1_node->machineId;
        } else {
            job2_node->sucByMachine->preByMachine = job2_node->preByMachine;
            job2_node->preByMachine->sucByMachine = job2_node->sucByMachine;

            job2_node->preByMachine = job1_node->preByMachine;
            job2_node->sucByMachine = job1_node;

            job1_node->preByMachine->sucByMachine = job2_node;
            job1_node->preByMachine = job2_node;

            job2_node->machineId = job1_node->machineId;
        }
    }
}

void Solution::copy_one_stage(const Solution &solution, int stage_id) {
    int machineCount = problem.getNumOfMachinesInStage(stage_id);
    for (int m = 0; m < machineCount; ++m) {
        f_headOnMachinesInEachStage[stage_id][m]->f_startTime = solution.f_headOnMachinesInEachStage[stage_id][m]
                ->f_startTime;
        f_headOnMachinesInEachStage[stage_id][m]->b_startTime = solution.f_headOnMachinesInEachStage[stage_id][m]
                ->b_startTime;
        b_headOnMachinesInEachStage[stage_id][m]->f_startTime = solution.b_headOnMachinesInEachStage[stage_id][m]
                ->f_startTime;
        b_headOnMachinesInEachStage[stage_id][m]->b_startTime = solution.b_headOnMachinesInEachStage[stage_id][m]
                ->b_startTime;
    }

    for (int m = 0; m < machineCount; ++m) {
        Node *f_head_node_at_machine = solution.f_headOnMachinesInEachStage[stage_id][m];
        Node *node_at_machine = f_head_node_at_machine->sucByMachine;
        if (node_at_machine->jobId == -1) {
            f_headOnMachinesInEachStage[stage_id][m]->sucByMachine = b_headOnMachinesInEachStage[stage_id][m];
            b_headOnMachinesInEachStage[stage_id][m]->preByMachine = f_headOnMachinesInEachStage[stage_id][m];
            continue;
        }
        Node *pre_node_at_machine = f_headOnMachinesInEachStage[stage_id][m];
        while (node_at_machine->jobId != -1) {
            int job_id = node_at_machine->jobId;
            //Node* operation = new Node(*node_at_machine);
            Node *operation = operations[stage_id][job_id];
            operation->f_startTime = node_at_machine->f_startTime;
            operation->b_startTime = node_at_machine->b_startTime;
            operation->machineId = node_at_machine->machineId;

            pre_node_at_machine->sucByMachine = operation;
            operation->sucByMachine = b_headOnMachinesInEachStage[stage_id][m];

            b_headOnMachinesInEachStage[stage_id][m]->preByMachine = operation;
            operation->preByMachine = pre_node_at_machine;

            pre_node_at_machine = operation;
            node_at_machine = node_at_machine->sucByMachine;
        }
    }
}

void Solution::copy_stage_to_vector(int stage_id, std::vector<std::vector<int>> &vec) {
    int machineCount = problem.getNumOfMachinesInStage(stage_id);
    vec.resize(machineCount);
    for (int m = 0; m < machineCount; ++m) {
        Node *f_head_node_at_machine = f_headOnMachinesInEachStage[stage_id][m];
        Node *node_at_machine = f_head_node_at_machine->sucByMachine;
        if (node_at_machine->jobId == -1) {
            continue;
        }
        while (node_at_machine->jobId != -1) {
            int job_id = node_at_machine->jobId;
            vec[m].emplace_back(job_id);
            node_at_machine = node_at_machine->sucByMachine;
        }
    }
}

void Solution::copy_vector_to_stage(int stage_id, const std::vector<std::vector<int>> &vec) {
    int machineCount = problem.getNumOfMachinesInStage(stage_id);

    for (int m = 0; m < machineCount; ++m) {
        if (vec[m].empty()) {
            f_headOnMachinesInEachStage[stage_id][m]->sucByMachine = b_headOnMachinesInEachStage[stage_id][m];
            b_headOnMachinesInEachStage[stage_id][m]->preByMachine = f_headOnMachinesInEachStage[stage_id][m];
            continue;
        }
        Node *pre_node_at_machine = f_headOnMachinesInEachStage[stage_id][m];
        for (auto job_id: vec[m]) {
            Node *operation = operations[stage_id][job_id];
            operation->machineId = m;

            pre_node_at_machine->sucByMachine = operation;
            operation->sucByMachine = b_headOnMachinesInEachStage[stage_id][m];

            b_headOnMachinesInEachStage[stage_id][m]->preByMachine = operation;
            operation->preByMachine = pre_node_at_machine;
            pre_node_at_machine = operation;
        }
    }
}

std::pair<int, int> Solution::find_best_insert_position_f(int inserted_job_id) {
    int min_span = INT_MAX;
    int best_pos;
    for (int pos = 0; pos <= jobSequence.size(); ++pos) {
        jobSequence.insert(jobSequence.begin() + pos, inserted_job_id);
        int span = decode_forward();
        if (span < min_span) {
            min_span = span;
            best_pos = pos;
        }

        jobSequence.erase(jobSequence.begin() + pos);
    }
    //jobSequence.insert(jobSequence.begin() + best_pos, inserted_job_id);
    return {min_span, best_pos};
}

std::pair<int, int> Solution::find_best_insert_position_b(int inserted_job_id) {
    int min_span = INT_MAX;
    int best_pos;
    for (int pos = 0; pos <= jobSequence.size(); ++pos) {
        jobSequence.insert(jobSequence.begin() + pos, inserted_job_id);
        int span = decode_backward();
        if (span < min_span) {
            min_span = span;
            best_pos = pos;
        }

        jobSequence.erase(jobSequence.begin() + pos);
    }
    //jobSequence.insert(jobSequence.begin() + best_pos, inserted_job_id);
    return {min_span, best_pos};
}

void Solution::destruction(std::vector<int> &erased_jobs, int d) {
    int k = 0;
    while (k < d) {
        int pos = wyt_rand(jobSequence.size());
        erased_jobs.emplace_back(jobSequence[pos]);
        jobSequence.erase(jobSequence.begin() + pos);
        ++k;
    }
}

void Solution::destruction_block(std::vector<int> &erased_jobs, int block_size) {
    int pos = wyt_rand(jobSequence.size() - block_size + 1);
    erased_jobs.insert(erased_jobs.begin(), jobSequence.begin() + pos, jobSequence.begin() + pos + block_size);
    jobSequence.erase(jobSequence.begin() + pos, jobSequence.begin() + pos + block_size);
}

std::pair<int, int> Solution::find_best_position_block(const std::vector<int> &erased_jobs) {
    int block_size = erased_jobs.size();
    int min_span = INT_MAX;
    int best_pos;
    for (int pos = 0; pos <= jobSequence.size(); ++pos) {
        jobSequence.insert(jobSequence.begin() + pos, erased_jobs.begin(), erased_jobs.end());
        int span = evaluate();
        if (span < min_span) {
            min_span = span;
            best_pos = pos;
        }
        jobSequence.erase(jobSequence.begin() + pos, jobSequence.begin() + pos + block_size);
    }
    return {min_span, best_pos};
}

void Solution::print() const {
    int stageId = 0;
    for (const auto &headOnMachines: f_headOnMachinesInEachStage) {
        std::cout << stageId << std::endl;
        int machineId = 0;
        for (const auto &head: headOnMachines) {
            std::cout << "\t" << machineId << ":\t";
            Node *node = head->sucByMachine;
            while (node->jobId != -1) {
                std::cout << "(" << node->jobId;
                std::cout << "," << node->f_startTime;
                std::cout << "," << node->processTime << ")";
                node = node->sucByMachine;
            }
            ++machineId;
            std::cout << std::endl;
        }
        ++stageId;
    }

    int jobId = 0;
    for (const auto &head: f_headJobs) {
        std::cout << jobId;
        std::cout << "\t";
        Node *node = head->sucByStage;
        while (node->stageId != -1) {
            std::cout << "(" << node->stageId;
            std::cout << "," << node->machineId;
            std::cout << "," << node->f_startTime;
            std::cout << "," << node->processTime << ")";
            node = node->sucByStage;
        }
        std::cout << std::endl;
        ++jobId;
    }
}

std::pair<int, int> Solution::find_best_swap_f(int swapped_index) {
    int min_span = INT_MAX;
    int best_index;
    for (int index = 0; index < jobSequence.size(); ++index) {
        if (index == swapped_index) {
            continue;
        }
        std::swap(jobSequence[index], jobSequence[swapped_index]);
        int span = decode_forward();
        if (span < min_span) {
            min_span = span;
            best_index = index;
        }
        std::swap(jobSequence[index], jobSequence[swapped_index]);
    }
    return {min_span, best_index};
}











