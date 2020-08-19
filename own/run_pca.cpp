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


//class Transformer{
//public:
//    faiss::VectorTransform* pca;
//    Transformer(faiss::VectorTransform* pca){
//
//    }
//}

float* transform(faiss::VectorTransform* pca, unsigned num_vecs, float* embeds){
    auto aux = pca->apply(num_vecs, embeds);
    return aux;
}

float* combine(std::vector<float*> embeds, std::vector<float> coefs, unsigned num_vecs, unsigned dims){
    auto aux = new float[num_vecs*dims];
    for(unsigned i=0; i<num_vecs*dims; i++){
        aux[i] = 0;
        for(unsigned j=0; j<embeds.size(); j++){
            aux[i] += coefs[j] * embeds[j][i];
        }
    }
    return aux;
}

faiss::VectorTransform* get_pca(const fs::path* path_pca){
    auto aux = faiss::read_VectorTransform(path_pca->string().c_str());
    return aux;
}

faiss::PCAMatrix* fit_pca(float* embeds, unsigned num_vecs, unsigned idims, unsigned odims){
    auto pca = new faiss::PCAMatrix(idims, odims, 0, false);
    pca->train(num_vecs, embeds);
    return pca;
//    void write_VectorTransform (const VectorTransform *vt, const char *fname);
}

class Input_data{
public:
    fs::path path_pca;
    fs::path path_data;
    unsigned idims;
    unsigned odims;
    unsigned num_providers=3;
    Input_data(int argc, char **argv){
        if (argc != 5) {
            std::cout << argv[0] << " path_pca path_data idims odims" << std::endl;
            exit(-1);
        }
        path_pca = argv[1];
        path_data = argv[2];
        idims = (unsigned) atoi(argv[3]);
        odims = (unsigned) atoi(argv[4]);
    }
};

// ~/nsg/build/own/run_pca /home/mpultar/Data/pca-P5-36 /home/mpultar/Providers/P2/embeds0.fvecs 512 128
int main(int argc, char **argv) {
    Input_data I(argc, argv);

    std::vector<float*> embeds(I.num_providers);
    auto num_vecs = load_data(I.path_data.string().c_str(), embeds[0], I.idims);
    load_data(I.path_data.string().c_str(), embeds[1], I.idims);
    load_data(I.path_data.string().c_str(), embeds[2], I.idims);

//    auto pca = get_pca(&I.path_pca);
    auto pca = fit_pca(embeds[2], num_vecs, I.idims, I.odims);
    return 0;

    std::vector<float*> embeds_t(I.num_providers);
    embeds_t[0] = transform(pca, num_vecs, embeds[0]);
    embeds_t[1] = transform(pca, num_vecs, embeds[1]);
    embeds_t[2] = transform(pca, num_vecs, embeds[2]);

    auto coefs = std::vector<float>{1.0, 1.0, -1.0};

    auto aux = combine(embeds_t, coefs, num_vecs, I.odims);
    for(unsigned i=0;i<50;i++){
        std::cout << aux[i]<<std::endl;
    }


    delete pca;
    for(unsigned i=0;i<embeds.size();i++)
        delete embeds[i];
}
