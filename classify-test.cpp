/**
 * @file classify-test.cpp
 */

#include <vector>
#include <string>
#include <iostream>

#include "index/forward_index.h"
#include "index/ranker/all.h"
#include "util/common.h"
#include "util/invertible_map.h"
#include "classify/classifier/all.h"

using std::cout;
using std::cerr;
using std::endl;
using namespace meta;

template <class Index>
classify::confusion_matrix cv(Index & idx, classify::classifier<Index> & c)
{
    std::vector<doc_id> docs = idx.docs();
    classify::confusion_matrix matrix = c.cross_validate(docs, 5);
    matrix.print();
    matrix.print_stats();
    return matrix;
}

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        cerr << "Usage:\t" << argv[0] << " config.toml" << endl;
        return 1;
    }

    auto config = cpptoml::parse_file(argv[1]);
    auto f_idx = index::make_index<index::forward_index>(argv[1]);
    auto i_idx = index::make_index<index::inverted_index>(argv[1]);

    classify::svm_wrapper svm{f_idx,
                              *config.get_as<std::string>("liblinear"),
                              classify::svm_wrapper::kernel::None };
    auto m1 = cv(f_idx, svm);

 // classify::linear_svm l2svm{idx};
 // auto m2 = cv(idx, l2svm);
 // std::cout << "(liblinear vs l2svm) Significant? " << std::boolalpha
 //           << classify::confusion_matrix::mcnemar_significant( m1, m2 )
 //           << std::endl;

    classify::knn<index::okapi_bm25> k{i_idx, 10, 1.5, 0.75, 500.0};
    auto m2 = cv(i_idx, k);
    std::cout << "(liblinear vs knn) Significant? " << std::boolalpha
              << classify::confusion_matrix::mcnemar_significant( m1, m2 )
              << std::endl;

 // classify::linear_svm l1svm{ idx, classify::linear_svm::loss_function::L1 };
 // auto m3 = cv(idx, l1svm);
 // std::cout << "(liblinear vs l1svm) Significant? " << std::boolalpha
 //           << classify::confusion_matrix::mcnemar_significant( m1, m3 )
 //           << std::endl;

 // classify::perceptron p{idx};
 // auto m4 = cv(idx, p);
 // std::cout << "(liblinear vs perceptron) Significant? " << std::boolalpha
 //           << classify::confusion_matrix::mcnemar_significant( m2, m4 )
 //           << std::endl;

 // classify::naive_bayes nb{idx};
 // auto m5 = cv(idx, nb);
 // std::cout << "(liblinear vs naive bayes) Significant? " << std::boolalpha
 //           << classify::confusion_matrix::mcnemar_significant( m2, m5 )
 //           << std::endl;

    return 0;
}
