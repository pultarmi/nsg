#include "run.h"
#include <efanna2e/index_nsg.h>
#include <efanna2e/util.h>
#include <iostream>
#include <experimental/filesystem>
#include <future>

namespace fs = std::experimental::filesystem;


template<typename T>
void load_data(const char* filename, T*& data, unsigned& num, unsigned& dim) {  // load data with sift10K pattern
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
    data = new T[(size_t)num * (size_t)dim];

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
    efanna2e::Parameters paras;

    Searcher(const char* filename, unsigned query_dim, const char *nsg_path, unsigned L, unsigned K){
        float *data_load = NULL;
        unsigned points_num, dim;
        load_data(filename, data_load, points_num, dim);
//        auto query_dim = (unsigned) atoi(argv[2]);
//        float *query_load = NULL;

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

//        query_load = new float[(size_t) dim];

        paras.Set<unsigned>("L_search", L);
        paras.Set<unsigned>("P_search", L);
    }

    std::pair<std::vector<unsigned>, std::vector<float>> search(float* query_load){
        std::vector<unsigned> res(K);
        std::vector<float> dists(K);
//        load_query(query_load, query_dim);
        index->SearchWithOptGraph(query_load, K, paras, res.data(), dists.data());
//        write_result(res);
        return std::make_pair(res, dists);
    }
};
// path='/home/mpultar/Data/mix_fc:plus_pca:pca-P5-36_splits:36_1-1--1_nsg'
// ~/nsg/build/tests/run $path 128 $path 300 100 /home/mpultar/Data/ids.ivecs /home/mpultar/Data/query.fvecs
int main(int argc, char **argv) {
    if (argc != 8) {
        std::cout << argv[0] << " data_file query_dim nsg_path search_L search_K path_ids path_query" << std::endl;
        exit(-1);
    }
    fs::path filename = argv[1];
    auto query_dim = (unsigned) atoi(argv[2]);
    fs::path nsg_path = argv[3];
    unsigned L = (unsigned) atoi(argv[4]);
    unsigned K = (unsigned) atoi(argv[5]);

    fs::path path_ids = argv[6];
    fs::path path_query = argv[7];
//    std::ifstream in(path_query, std::ios::binary);
    float* queries = NULL;
    unsigned* ids = NULL;
    unsigned nquery, dim;
    load_data(path_query.string().c_str(), queries, nquery, dim);
    load_data(path_ids.string().c_str(), ids, nquery, dim);
//    std::cout << nquery << std::endl;
//    std::cout << dim << std::endl;
//    in.seekg(0, std::ios::end);
//    int nquery = in.tellg() / (4*query_dim+1);
//    std::vector<std::vector<float> > queries(nquery, std::vector<float>(query_dim));
//    for(int i=0; i < nquery; i++){
//        for(int j=0; j < query_dim; j++){
//            queries[i][j] =
//        }
//    };

    std::vector<Searcher> searchers;
    for(int i=0; i < 3; i++) {
        fs::path p1 = filename / std::to_string(i) / "embeds.fvecs";
        std::cout << p1 << std::endl;
        fs::path p2 = nsg_path / std::to_string(i) / "embeds.nsg";
        Searcher searcher(p1.string().c_str(), query_dim, p2.string().c_str(), L, K);
        searchers.push_back(searcher);
    }
    std::vector<std::future<std::pair<std::vector<unsigned>,std::vector<float>>>> futures(searchers.size());
    for(int i=0; i<searchers.size(); i++){
        std::cout << i <<std::endl;
        searchers[0].search(queries + i*dim);
        futures[i] = std::async(&Searcher::search, &searchers[i], queries + i*dim);
    }
    for(int i=0; i<searchers.size(); i++){
        futures[i].wait();
    }

    std::vector<unsigned> indices(searchers.size()*K);
    std::vector<unsigned> dists(searchers.size()*K);
    unsigned offset=0;
    for(int i=0; i<searchers.size(); i++){
        auto aux = futures[i].get();
        for(auto && mem : aux.first)
            mem += offset;
        std::cout << offset << std::endl;
        indices.insert(indices.end(), aux.first.begin(), aux.first.end());
        std::cout << aux.first[0] << std::endl;
        std::cout << aux.second[0] << std::endl;
        offset += aux.first.size();
    }
    return 0;
}
