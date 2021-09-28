#include <grb/grb.hpp>
#include <iostream>

template <typename T = void, typename U = T, typename V = void>
class plus;

template <typename T, typename U, typename V>
class plus {
public:
  V operator()(const T& left, const U& right) {
    return left + right;
  }
};

template <typename T, typename U>
class plus<T, U, void> {
public:
  auto operator()(const T& left, const U& right) {
    return left + right;
  }
};

template <>
class plus<void, void, void>
{
public:
  template <typename T, typename U>
  auto operator()(const T& left, const U& right) {
    return left + right;
  }
};

void foo(std::string) {}

int main(int argc, char** argv) {
	grb::matrix<float, int, grb::sparse> matrix({1024, 1024});

  plus<int> p;

  std::cout << 12 << " " << 13 << " " << p(12, 13) << std::endl;

  plus<> q;

  std::cout << 12 << " " << 13 << " " << q(12, 13) << std::endl;

  plus f;

  std::cout << 12 << " " << 13 << " " << f(12, 13) << std::endl;

  plus <int, float> f2;

  plus <int, float, int> f3;

  auto lel = f3(12, 13);

  foo(lel);

  return 0;
}
