//
// Created by milan on 18/08/2020.
//

#include "run_pca.h"
#include <efanna2e/index_nsg.h>
#include <efanna2e/util.h>
#include <iostream>
#include <experimental/filesystem>
#include <future>
#include <faiss/IndexIVFPQ.h>
#include <faiss/index_io.h>
#include <faiss/VectorTransform.h>

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

// path='/home/mpultar/Data/mix_fc:plus_pca:pca-P5-36_splits:36_1-1--1_nsg'
// ~/nsg/build/own/run_pca /home/mpultar/Data/pca-P5-36
int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << argv[0] << " path_pca" << std::endl;
        exit(-1);
    }
    fs::path path_pca = argv[1];
    faiss::VectorTransform* pca = faiss::read_VectorTransform(path_pca.string().c_str());
}
