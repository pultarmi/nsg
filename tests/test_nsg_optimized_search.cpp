//
// Created by 付聪 on 2017/6/21.
//

#include <efanna2e/index_nsg.h>
#include <efanna2e/util.h>
#include <chrono>
#include <string>

void load_data(float *&data, unsigned dim) {  // load data with sift10K pattern
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

int main(int argc, char **argv) {
    if (argc != 6) {
        std::cout << argv[0]
                  << " data_file query_dim nsg_path search_L search_K result_path"
                  << std::endl;
        exit(-1);
    }
    float *data_load = NULL;
    unsigned points_num, dim;
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

    efanna2e::Parameters paras;
    paras.Set<unsigned>("L_search", L);
    paras.Set<unsigned>("P_search", L);

    query_load = new float[(size_t) (size_t) dim];

    while (1) {
        load_data(query_load, query_dim);
        assert(dim == query_dim);
        std::vector<std::vector<unsigned> > res(query_num);
        for (unsigned i = 0; i < query_num; i++) res[i].resize(K);

        for (unsigned i = 0; i < query_num; i++) {
            index.SearchWithOptGraph(query_load + i * dim, K, paras, res[i].data());
        }
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
