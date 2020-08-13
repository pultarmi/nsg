#include "run.h"
#include <efanna2e/index_nsg.h>
#include <efanna2e/util.h>
#include <iostream>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;


void load_data(const char* filename, float*& data, unsigned& num, unsigned& dim) {  // load data with sift10K pattern
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) {
        std::cout << "open file error" << std::endl;
        exit(-1);
    }
    in.read((char*)&dim, 4);
    // std::cout<<"data dimension: "<<dim<<std::endl;
    in.seekg(0, std::ios::end);
    std::ios::pos_type ss = in.tellg();
    size_t fsize = (size_t)ss;
    num = (unsigned)(fsize / (dim + 1) / 4);
    data = new float[(size_t)num * (size_t)dim];

    in.seekg(0, std::ios::beg);
    for (size_t i = 0; i < num; i++) {
        in.seekg(4, std::ios::cur);
        in.read((char*)(data + i * dim), dim * 4);
    }
    in.close();
}

void load_query(float *&data, unsigned dim) {  // load data with sift10K pattern
    std::cin.read((char *) data, dim * 4);
}

void write_result( std::vector<unsigned> &results) {
    unsigned GK = (unsigned) results.size();
//    std::cout.write((char *) &GK, sizeof(unsigned));
    std::cout.write((char *) results.data(), GK * sizeof(unsigned));
    std::cout.flush();
}

class Searcher{
public:
    unsigned query_dim;
    unsigned K,L;
    efanna2e::IndexNSG* index;
    Searcher(const char* filename, unsigned query_dim, const char *nsg_path, unsigned L, unsigned K){
        float *data_load = NULL;
        unsigned points_num, dim;
        load_data(filename, data_load, points_num, dim);
//        auto query_dim = (unsigned) atoi(argv[2]);
        float *query_load = NULL;

//        unsigned L = (unsigned) atoi(argv[4]);
//        unsigned K = (unsigned) atoi(argv[5]);
        this->K = K;
        this->L = L;
        if (L < K) {
            std::cout << "search_L cannot be smaller than search_K!" << std::endl;
            exit(-1);
        }

        this->index = new efanna2e::IndexNSG(dim, points_num, efanna2e::FAST_L2, nullptr);
        index->Load(nsg_path);
        index->OptimizeGraph(data_load);
        this->query_dim = query_dim;
        assert(dim == query_dim);
        delete[] data_load;

        query_load = new float[(size_t) dim];
    }

    void search(float* query_load){
        efanna2e::Parameters paras;
        paras.Set<unsigned>("L_search", L);
        paras.Set<unsigned>("P_search", L);

        std::vector<unsigned> res(K);
        load_query(query_load, query_dim);
        index->SearchWithOptGraph(query_load, K, paras, res.data());
        write_result(res);
    }
};
// path='/home/mpultar/Data/mix_fc:plus_pca:pca-P5-36_splits:36_1-1--1_nsg'
// ~/nsg/build/tests/run $path 128 $path 300 100
int main(int argc, char **argv) {
//    std::cout << argv[0] << " data_file query_dim nsg_path search_L search_K" << std::endl;
    char* filename = argv[1];
    fs::path p1 = filename;
    std::cout << p1 << std::endl;
    auto query_dim = (unsigned) atoi(argv[2]);
    char* nsg_path = argv[3];
    fs::path p2 = nsg_path;
    unsigned L = (unsigned) atoi(argv[4]);
    unsigned K = (unsigned) atoi(argv[5]);
    std::vector<Searcher> searchers;
    for(int i=0; i < 36; i++) {
        p1 /= std::to_string(i);
        p1 /= "embeds.fvecs";
        p2 /= std::to_string(i);
        p2 /= "embeds.nsg";
        Searcher searcher(p1.string().c_str(), query_dim, p2.string().c_str(), L, K);
        searchers.push_back(searcher);
    }
    return 0;
}
