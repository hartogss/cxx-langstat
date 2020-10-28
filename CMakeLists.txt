cmake_minimum_required(VERSION 3.18.3)

project(cxx-langstat) #referenced by ${PROJECT_NAME}

set(CMAKE_CXX_STANDARD 14) #resolved nullptr not found error to go away, but nullptr part of languge since c++11?


#build something
add_executable(${PROJECT_NAME} Langstat.cpp)

#
add_subdirectory(include/cxx-langstat)
# directory to search for include files
include_directories(include/cxx-langstat)
# not this one, want to have includes not only for target
# target_include_directories(${PROJECT_NAME} PRIVATE include/cxx-langstat)

#
add_subdirectory(lib)
# specify directory to look for libraries
target_link_directories(${PROJECT_NAME} PUBLIC lib/)
# link library to project, i.e. specific files
target_link_libraries(${PROJECT_NAME} ForStmtAnalysis)





# add_subdirectory(lib)
# add_subdirectory(include/cxx-langstat/)
#
# target_include_directories(${PROJECT_NAME} PRIVATE include/cxx-langstat)
#
# target_link_directories(${PROJECT_NAME} PRIVATE include/cxx-langstat)

# target_link_libraries(${PROJECT_NAME} Extraction)
