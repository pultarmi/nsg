set(CMAKE_CXX_STANDARD 17)

include_directories("Include")

add_executable(run run.cpp utils.cpp Include/aux.h)
target_link_libraries(run ${PROJECT_NAME} -ltcmalloc -lstdc++fs faiss)
#target_link_libraries(run ${PROJECT_NAME} -ltcmalloc -lstdc++fs /home/mpultar/nsg/libfaiss.a)

add_library(run_pca SHARED run_pca.cpp utils.cpp)
add_executable(run_pca run_pca.cpp utils.cpp)
#target_link_libraries(run_pca ${PROJECT_NAME} -ltcmalloc -lstdc++fs /home/mpultar/nsg/libfaiss.a /usr/lib/x86_64-linux-gnu/libopenblas.so)
target_link_libraries(run_pca ${PROJECT_NAME} -ltcmalloc -lstdc++fs faiss ${BLAS_LIBRARIES})