//
// Created by milan on 18/08/2020.
//

#ifndef EFANNA2E_UTILS_H
#define EFANNA2E_UTILS_H

#include <efanna2e/index_nsg.h>
#include <efanna2e/util.h>
#include <iostream>
#include <experimental/filesystem>
#include <future>
#include <faiss/IndexIVFPQ.h>
#include <faiss/index_io.h>
#include <faiss/VectorTransform.h>

class utils {

};

template<typename T>
unsigned load_data(const char* filename, T*& data, unsigned query_dim);

template <typename Iter, typename Compare>
std::vector<int> argsort(Iter begin, Iter end, Compare comp);

#endif //EFANNA2E_UTILS_H
