//
// Created by 付聪 on 2017/6/21.
//

#include <efanna2e/index_nsg.h>
#include <efanna2e/util.h>
#include <chrono>
#include <string>

void load_data(char* filename, float*& data, unsigned& num, unsigned& dim) {  // load data with sift10K pattern
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

void save_result(const char *filename, std::vector<std::vector<unsigned> > &results) {
    std::ofstream out(filename, std::ios::binary | std::ios::out);

    for (unsigned i = 0; i < results.size(); i++) {
        unsigned GK = (unsigned) results[i].size();
        out.write((char *) &GK, sizeof(unsigned));
        out.write((char *) results[i].data(), GK * sizeof(unsigned));
    }
    out.close();
}

void write_result( std::vector<unsigned> &results) {
    unsigned GK = (unsigned) results.size();
//    std::cout.write((char *) &GK, sizeof(unsigned));
    std::cout.write((char *) results.data(), GK * sizeof(unsigned));
    std::cout.flush();
}

int main(int argc, char **argv) {
    if (argc != 6) {
        std::cout << argv[0]
                  << " data_file query_dim nsg_path search_L search_K"
                  << std::endl;
        exit(-1);
    }
    float *data_load = NULL;
    unsigned points_num, dim;
    load_data(argv[1], data_load, points_num, dim);
    auto query_dim = (unsigned) atoi(argv[2]);
    float *query_load = NULL;

    unsigned L = (unsigned) atoi(argv[4]);
    unsigned K = (unsigned) atoi(argv[5]);

    if (L < K) {
        std::cout << "search_L cannot be smaller than search_K!" << std::endl;
        exit(-1);
    }

    efanna2e::IndexNSG index(dim, points_num, efanna2e::FAST_L2, nullptr);
    index.Load(argv[3]);
    index.OptimizeGraph(data_load);
    assert(dim == query_dim);
    delete[] data_load;

    efanna2e::Parameters paras;
    paras.Set<unsigned>("L_search", L);
    paras.Set<unsigned>("P_search", L);

    query_load = new float[(size_t) dim];
    std::vector<unsigned> res(K);

    while (!std::cin.eof()) {
        load_query(query_load, query_dim);
        std::cout  << 10 <<std::endl;
        return 0;
        write_result(res);
//        index.SearchWithOptGraph(query_load, K, paras, res.data());
//        write_result(res);
    }

//    auto s = std::chrono::high_resolution_clock::now();
//    for (unsigned i = 0; i < query_num; i++) {
//        index.SearchWithOptGraph(query_load + i * dim, K, paras, res[i].data());
//    }
//    auto e = std::chrono::high_resolution_clock::now();
//    std::chrono::duration<double> diff = e - s;
//    std::cout << "search time: " << diff.count() << "\n";
//
//    save_result(argv[6], res);

    return 0;
}
