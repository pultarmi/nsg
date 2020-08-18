set(CMAKE_CXX_STANDARD 17)

add_executable(run run.cpp utils.cpp utils.h)
target_link_libraries(run ${PROJECT_NAME} -ltcmalloc -lstdc++fs /home/mpultar/nsg/libfaiss.a)

add_executable(run_pca run_pca.cpp)
target_link_libraries(run_pca ${PROJECT_NAME} -ltcmalloc -lstdc++fs /home/mpultar/nsg/libfaiss.a /usr/lib/x86_64-linux-gnu/libopenblas.so)