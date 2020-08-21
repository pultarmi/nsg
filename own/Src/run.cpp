#include "run.h"
#include "utils.h"
#include "aux.h"
#include <efanna2e/index_nsg.h>
#include <efanna2e/util.h>
#include <iostream>
#include <experimental/filesystem>
#include <future>
#include <faiss/IndexIVFPQ.h>
#include <faiss/index_io.h>
#include <faiss/VectorTransform.h>


namespace fs = std::experimental::filesystem;


class Searcher{
public:
    unsigned K;
    unsigned num_vecs;
    efanna2e::IndexNSG* index;
    efanna2e::Parameters paras;

    Searcher(const char* filename, unsigned query_dim, const char *nsg_path, unsigned L, unsigned K){
        float *data_load = NULL;
        num_vecs = load_data(filename, data_load, query_dim);

        this->K = K;
        if (L < K) {
            std::cout << "search_L cannot be smaller than search_K!" << std::endl;
            exit(-1);
        }

        this->index = new efanna2e::IndexNSG(query_dim, num_vecs, efanna2e::FAST_L2, nullptr);
        index->Load(nsg_path);
        index->OptimizeGraph(data_load);
        delete[] data_load;

        paras.Set<unsigned>("L_search", L);
        paras.Set<unsigned>("P_search", L);
    }

    std::pair<std::vector<unsigned>, std::vector<float>> search(float* query_load){
        std::vector<unsigned> res(K);
        std::vector<float> dists(K);
        index->SearchWithOptGraph(query_load, K, paras, res.data(), dists.data());
//        write_result(res);
        return std::make_pair(res, dists);
    }
};

// path='/home/mpultar/Data/mix_fc:plus_pca:pca-P5-36_splits:36_1-1--1_nsg'
// ~/nsg/build/own/run $path 128 $path 300 100 /home/mpultar/Data/ids.ivecs /home/mpultar/Data/query.fvecs
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
    float* queries = NULL;
    unsigned* ids = NULL;
    load_data(path_query.string().c_str(), queries, query_dim);
    load_data(path_ids.string().c_str(), ids, 1);

    std::vector<Searcher> searchers;
    for(int i=0; i < 36; i++) {
        fs::path p1 = filename / std::to_string(i) / "embeds.fvecs";
        std::cout << p1 << std::endl;
        fs::path p2 = nsg_path / std::to_string(i) / "embeds.nsg";
        Searcher searcher(p1.string().c_str(), query_dim, p2.string().c_str(), L, K);
        searchers.push_back(searcher);
    }
    std::vector<std::future<std::pair<std::vector<unsigned>,std::vector<float>>>> futures(searchers.size());
    for(unsigned i=0; i<searchers.size(); i++){
        futures[i] = std::async(&Searcher::search, &searchers[i], queries);
    }
    for(unsigned i=0; i<searchers.size(); i++){
        futures[i].wait();
    }

    std::vector<unsigned> indices_;
    std::vector<float> dists_;
    unsigned offset=0;
    for(unsigned i=0; i<searchers.size(); i++){
        auto aux = futures[i].get();
        for(unsigned j=0; j<aux.first.size(); j++) {
            aux.first[j] += offset;
        }
        std::cout << aux.first[0] << std::endl;
        indices_.insert(indices_.end(), aux.first.begin(), aux.first.end());
        dists_.insert(dists_.end(), aux.second.begin(), aux.second.end());
        offset += searchers[i].num_vecs;
    }

    auto asort = argsort(dists_.begin(), dists_.end(), std::less<float>());

    std::vector<unsigned> indices(K);
    std::vector<float> dists(K);
    for(unsigned i=0; i<indices.size(); i++){
        indices[i] = ids[indices_[asort[i]]];
        std::cout << i << std::endl;
        std::cout << indices[i] << std::endl;
        dists[i] = dists_[asort[i]];
//        std::cout << dists[i] << std::endl;
    }

    return 0;
}
