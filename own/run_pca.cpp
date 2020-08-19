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


// ~/nsg/build/own/run_pca /home/mpultar/Data/pca-P5-36 /home/mpultar/Providers/P2/embeds0.fvecs 512 128
int main(int argc, char **argv) {
    if (argc != 5) {
        std::cout << argv[0] << " path_pca path_data idim odim" << std::endl;
        exit(-1);
    }
    fs::path path_pca = argv[1];
    fs::path path_data = argv[2];
    unsigned idim = (unsigned) atoi(argv[3]);
    unsigned odim = (unsigned) atoi(argv[4]);
    faiss::VectorTransform* pca = faiss::read_VectorTransform(path_pca.string().c_str());
//    float* aux = NULL;
//    faiss::idx_t n;
    float* data = NULL;
    load_data(path_data.string().c_str(), data, idim);
    float* aux = pca->apply(1, data);
    for(unsigned i=0; i<50;i++){
        std::cout << aux[i] << std::endl;
    }
//    load_data(path_pca.string().c_str(), aux, idim);
}
