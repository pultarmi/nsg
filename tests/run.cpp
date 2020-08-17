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

template <typename Iter, typename Compare>
std::vector<int> argsort(Iter begin, Iter end, Compare comp)
{
    // Begin Iterator, End Iterator, Comp
    std::vector<std::pair<int, Iter> > pairList; // Pair Vector
    std::vector<int> ret; // Will hold the indices

    int i = 0;
    for (auto it = begin; it < end; it++)
    {
        std::pair<int, Iter> pair(i, it); // 0: Element1, 1:Element2...
        pairList.push_back(pair); // Add to list
        i++;
    }
    // Stable sort the pair vector
    std::stable_sort(pairList.begin(), pairList.end(),
                     [comp](std::pair<int, Iter> prev, std::pair<int, Iter> next) -> bool
                     {return comp(*prev.second, *next.second); } // This is the important part explained below
    );

    /*
        Comp is a templated function pointer that makes a basic comparison
        std::stable_sort takes a function pointer that takes
        (std::pair<int, Iter> prev, std::pair<int, Iter> next)
        and returns bool. We passed a corresponding lambda to stable sort
    and used our comp within brackets to capture it as an outer variable.
    We then applied this function to our iterators which are dereferenced.
    */
    for (auto i : pairList)
        ret.push_back(i.first); // Take indices

    return ret;
}

class Searcher{
public:
    unsigned query_dim;
    unsigned K,L;
    unsigned points_num, dim;
    efanna2e::IndexNSG* index;
    efanna2e::Parameters paras;

    Searcher(const char* filename, unsigned query_dim, const char *nsg_path, unsigned L, unsigned K){
        float *data_load = NULL;
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
    for(int i=0; i < 4; i++) {
        fs::path p1 = filename / std::to_string(i) / "embeds.fvecs";
        std::cout << p1 << std::endl;
        fs::path p2 = nsg_path / std::to_string(i) / "embeds.nsg";
        Searcher searcher(p1.string().c_str(), query_dim, p2.string().c_str(), L, K);
        searchers.push_back(searcher);
    }
    std::vector<std::future<std::pair<std::vector<unsigned>,std::vector<float>>>> futures(searchers.size());
    for(int i=0; i<searchers.size(); i++){
//        std::cout << i <<std::endl;
        searchers[0].search(queries);
        futures[i] = std::async(&Searcher::search, &searchers[i], queries);
    }
    for(int i=0; i<searchers.size(); i++){
        futures[i].wait();
    }

//    std::vector<unsigned> indices_(searchers.size() * K);
//    std::vector<unsigned> dists_(searchers.size() * K);
    std::vector<unsigned> indices_;
    std::vector<float> dists_;
    unsigned offset=0;
    for(int i=0; i<searchers.size(); i++){
        auto aux = futures[i].get();
        for(unsigned j=0; j<aux.first.size(); j++) {
            aux.first[j] += offset;
            std::cout << aux.second[j] << std::endl;
        }
//        std::cout << aux.first[0] << std::endl;
        indices_.insert(indices_.end(), aux.first.begin(), aux.first.end());
        dists_.insert(dists_.end(), aux.second.begin(), aux.second.end());
//        std::cout << aux.first[0] << std::endl;
//        std::cout << indices_[0] << std::endl;
        offset += searchers[i].points_num;
    }

    auto asort = argsort(dists_.begin(), dists_.end(), std::less<float>());

    std::vector<unsigned> indices(K);
    std::vector<float> dists(K);
    for(unsigned i=0; i<indices.size(); i++){
        indices[i] = ids[indices_[asort[i]]];
//        std::cout << indices[i] << std::endl;
        dists[i] = dists_[asort[i]];
    }

//    for(unsigned i=0; i<10; i++){
//    }
    return 0;
}
