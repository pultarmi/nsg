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



std::unique_ptr<float[]> transform(faiss::VectorTransform* pca, unsigned num_vecs, float* embeds){
    std::unique_ptr<float[]> aux(pca->apply(num_vecs, embeds));
    return aux;
}

std::unique_ptr<float[]> combine(std::vector<float*> embeds, std::vector<float> coefs, unsigned num_vecs, unsigned dims){
    std::unique_ptr<float[]> aux(new float[num_vecs]);
    for(unsigned i=0; i<num_vecs*dims; i++){
        aux[i] = 0;
        for(unsigned j=0; j<embeds.size(); j++){
            aux[i] += coefs[j] * embeds[j][i];
        }
    }
    return aux;
}

std::unique_ptr<faiss::VectorTransform> get_pca(const fs::path* path_pca){
    std::unique_ptr<faiss::VectorTransform> aux(faiss::read_VectorTransform(path_pca->string().c_str()));
    return aux;
}

class Input_data{
public:
    fs::path path_pca;
    fs::path path_data;
    unsigned idim;
    unsigned odim;
    Input_data(int argc, char **argv){
        if (argc != 5) {
            std::cout << argv[0] << " path_pca path_data idim odim" << std::endl;
            exit(-1);
        }
        path_pca = argv[1];
        path_data = argv[2];
        idim = (unsigned) atoi(argv[3]);
        odim = (unsigned) atoi(argv[4]);
    }
};

// ~/nsg/build/own/run_pca /home/mpultar/Data/pca-P5-36 /home/mpultar/Providers/P2/embeds0.fvecs 512 128
int main(int argc, char **argv) {
    Input_data I(argc, argv);
    auto pca = get_pca(&I.path_pca);
    float* embeds = nullptr;
    auto num_vecs = load_data(I.path_data.string().c_str(), embeds, I.idim);
//    float* aux = pca->apply(2, data);
    auto aux = transform(pca.get(), num_vecs, embeds);
//    load_data(path_pca.string().c_str(), aux, idim);
}
