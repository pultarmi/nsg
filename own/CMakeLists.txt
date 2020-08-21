set(CMAKE_CXX_STANDARD 17)

include_directories("Include")

add_executable(run Src/run.cpp Src/utils.cpp Include/aux.h)
target_link_libraries(run ${PROJECT_NAME} -ltcmalloc -lstdc++fs faiss)

add_executable(run_build Src/run_build.cpp Src/utils.cpp Include/aux.h)
target_link_libraries(run_build ${PROJECT_NAME} -ltcmalloc -lstdc++fs faiss)

#add_library(run_pca SHARED run_pca.cpp utils.cpp)
add_executable(run_pca Src/run_pca.cpp Src/utils.cpp)
target_link_libraries(run_pca ${PROJECT_NAME} -ltcmalloc -lstdc++fs faiss ${BLAS_LIBRARIES})