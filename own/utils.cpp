#include "utils.h"
#include <efanna2e/index_nsg.h>
#include <efanna2e/util.h>
#include <iostream>
#include <experimental/filesystem>
#include <future>
#include <faiss/IndexIVFPQ.h>
#include <faiss/index_io.h>
#include <faiss/VectorTransform.h>


template<typename T>
unsigned load_data(const char* filename, T*& data, unsigned query_dim) {  // load data with sift10K pattern
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) {
        std::cout << "open file error" << std::endl;
        exit(-1);
    }
    unsigned dim;
    in.read((char*)&dim, 4);
    assert(dim == query_dim);
    in.seekg(0, std::ios::end);
    std::ios::pos_type ss = in.tellg();
    size_t fsize = (size_t)ss;
    auto num_vecs = (unsigned)(fsize / (dim + 1) / 4);
    data = new T[(size_t)num_vecs * (size_t)dim];

    in.seekg(0, std::ios::beg);
    for (size_t i = 0; i < num_vecs; i++) {
        in.seekg(4, std::ios::cur);
        in.read((char*)(data + i * dim), dim * 4);
    }
    in.close();
    return num_vecs;
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
std::vector<int> argsort(Iter begin, Iter end, Compare comp){
    std::vector<std::pair<int, Iter> > pairList; // Pair Vector
    std::vector<int> ret; // Will hold the indices

    int i = 0;
    for (auto it = begin; it < end; it++){
        std::pair<int, Iter> pair(i, it); // 0: Element1, 1:Element2...
        pairList.push_back(pair); // Add to list
        i++;
    }
    std::stable_sort(pairList.begin(), pairList.end(),
                     [comp](std::pair<int, Iter> prev, std::pair<int, Iter> next) -> bool
                     {return comp(*prev.second, *next.second); } // This is the important part explained below
    );
    /* Comp is a templated function pointer that makes a basic comparison
        std::stable_sort takes a function pointer that takes
        (std::pair<int, Iter> prev, std::pair<int, Iter> next)
        and returns bool. We passed a corresponding lambda to stable sort
    and used our comp within brackets to capture it as an outer variable.
    We then applied this function to our iterators which are dereferenced.*/
    for (auto i : pairList)
        ret.push_back(i.first); // Take indices
    return ret;
}