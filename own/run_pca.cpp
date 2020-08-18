//
// Created by milan on 18/08/2020.
//

#include "run_pca.h"
#include "utils.h"
#include <efanna2e/index_nsg.h>
#include <efanna2e/util.h>
#include <iostream>
#include <experimental/filesystem>
#include <future>
#include <faiss/IndexIVFPQ.h>
#include <faiss/index_io.h>
#include <faiss/VectorTransform.h>

namespace fs = std::experimental::filesystem;


// path='/home/mpultar/Data/mix_fc:plus_pca:pca-P5-36_splits:36_1-1--1_nsg'
// ~/nsg/build/own/run_pca /home/mpultar/Data/pca-P5-36
int main(int argc, char **argv) {
    if (argc != 4) {
        std::cout << argv[0] << " path_pca idim odim" << std::endl;
        exit(-1);
    }
    fs::path path_pca = argv[1];
    unsigned idim = (unsigned) atoi(argv[2]);
    unsigned odim = (unsigned) atoi(argv[3]);
    faiss::VectorTransform* pca = faiss::read_VectorTransform(path_pca.string().c_str());
    float* aux = NULL;
    load_data(path_pca.string().c_str(), aux, idim);
}
