#include <cstdlib>
#include <grb/grb.hpp>
#include <iostream>

#include "kernels.hpp"
#include "views_help.hpp"

int main(int argc, char** argv) {

  if (argc != 2) {
    std::cerr << "usage: ./reduce file.mtx" << std::endl;
    exit(1);
  }

  std::string fname(argv[1]);

  std::cout << "Reading matrix..." << std::endl;
  grb::matrix<float, std::size_t, grb::sparse> a(fname);

  std::cout << "Beginning experiments..." << std::endl;

  std::cout << "=== SUM REDUCE ===" << std::endl;

  std::cout << "Default GrB Matrix (CSR)" << std::endl;
  test_sumreduce(a);

  {
    auto edge_list = get_edge_list(a);
    auto el_view = grb::views::all(edge_list);
    std::cout << "NWGraph edge_list" << std::endl;
    test_sumreduce(el_view);
  }

  {
    auto edge_list = get_edge_list(a);
    std::cout << "NWGraph edge_list (RAW)" << std::endl;
    test_sumreduce(edge_list);
  }

  {
    auto adjacency_list = get_adjacency_list(a);
    auto al_view = grb::views::all(adjacency_list);
    std::cout << "NWGraph adjacency_list" << std::endl;
    test_sumreduce(al_view);
  }

  {
    auto adjacency_list = get_adjacency_list(a);
    std::cout << "NWGraph adjacency_list (RAW)" << std::endl;
    test_sumreduce(adjacency_list);
  }

  {
    auto csr_view = get_csr_view(a);
    std::cout << "CSR View" << std::endl;
    test_sumreduce(csr_view);
  }

  /*
  {
  grb::coo_matrix<float, std::size_t> coo(a.shape());
  coo.push_back(a.begin(), a.end());
  std::cout << "Default GrB Matrix (COO)" << std::endl;
  test_sumreduce(coo);
  }
  */

  std::cout << "=== SPARSE DENSE MULTIPLY ===" << std::endl;

  std::cout << "Default GrB Matrix (CSR)" << std::endl;
  test_spmm(a);

  {
    auto edge_list = get_edge_list(a);
    auto el_view = grb::views::all(edge_list);
    std::cout << "NWGraph edge_list" << std::endl;
    test_spmm(el_view);
  }

  {
    auto edge_list = get_edge_list(a);
    std::cout << "NWGraph edge_list (RAW)" << std::endl;
    test_spmm(edge_list);
  }

  {
    auto adjacency_list = get_adjacency_list(a);
    auto al_view = grb::views::all(adjacency_list);
    std::cout << "NWGraph adjacency_list" << std::endl;
    test_spmm(al_view);
  }

  {
    auto adjacency_list = get_adjacency_list(a);
    std::cout << "NWGraph adjacency_list (RAW)" << std::endl;
    test_spmm(adjacency_list);
  }

  {
    auto csr_view = get_csr_view(a);
    std::cout << "CSR View" << std::endl;
    test_spmm(csr_view);
  }

  /*
  {
  grb::coo_matrix<float, std::size_t> coo(a.shape());
  coo.push_back(a.begin(), a.end());
  std::cout << "Default GrB Matrix (COO)" << std::endl;
  test_spmm(coo);
  }
  */

  return 0;
}
