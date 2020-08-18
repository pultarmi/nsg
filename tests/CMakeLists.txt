set(CMAKE_CXX_STANDARD 17)

add_executable(test_nsg_index test_nsg_index.cpp)
target_link_libraries(test_nsg_index ${PROJECT_NAME} -ltcmalloc)

add_executable(run run.cpp)
target_link_libraries(run ${PROJECT_NAME} -ltcmalloc -lstdc++fs /home/mpultar/nsg/libfaiss.a)

add_executable(run2 run2.cpp)
target_link_libraries(run2 ${PROJECT_NAME} -ltcmalloc -lstdc++fs /home/mpultar/nsg/libfaiss.a)

add_executable(test_nsg_search test_nsg_search.cpp)
target_link_libraries(test_nsg_search ${PROJECT_NAME})

add_executable(test_nsg_optimized_search test_nsg_optimized_search.cpp)
target_link_libraries(test_nsg_optimized_search ${PROJECT_NAME} -ltcmalloc)

