cmake_minimum_required(VERSION 3.19)
project(HFSP_Critical_Path)

set(CMAKE_CXX_STANDARD 17)

add_executable(HFSP_Critical_Path main.cpp HFS_Problem.cpp HFS_Problem.h Job.h Solution.cpp Solution.h Job.cpp PyGantt.cpp PyGantt.h rand.h rand.cpp
        utils.cpp
        utils.h
        IGWS.cpp
        IGWS.h
        PaperAlgorithm.cpp
        PaperAlgorithm.h
        IGWYT.cpp
        IGWYT.h
        CSA.cpp
        CSA.h
        TCSNSA.cpp
        TCSNSA.h
        MetaheuristicAlgorithms.cpp
        MetaheuristicAlgorithms.h
        IGLCX.h
        IGLCX.cpp
        matplotlibcpp.h)

#find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
#if (Python3_FOUND)
#    message("Python include directory: " ${Python3_INCLUDE_DIRS})
#    message("Python version is: " ${Python3_VERSION})
#    message("Python libs is: " ${Python3_LIBRARIES})
#    include_directories(${Python3_INCLUDE_DIRS})
#    target_link_libraries(HFSP_Critical_Path ${Python3_LIBRARIES})
#endif (Python3_FOUND)
#
##include_directories("D:/coding_net/matplotlib-cpp")
#include_directories("D:/python/Lib/site-packages/numpy/core/include")