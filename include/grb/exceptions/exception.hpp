
#pragma once

#include <stdexcept>
#include <string>

namespace grb {

class exception : public std::exception {
public:
  exception(const std::string& what_arg) : what_arg_(what_arg) {}
  exception() {}

  const char* what() const throw() {
    return what_arg_.c_str();
  }

private:
  std::string what_arg_;
};

class out_of_range final : public grb::exception {
public:
  out_of_range(const std::string& what_arg) : what_arg_(what_arg) {}

  const char* what() const throw() {
    return what_arg_.c_str();
  }

private:
  std::string what_arg_;
};

class invalid_argument final : public grb::exception {
public:
  invalid_argument(const std::string& what_arg) : what_arg_(what_arg) {}

  const char* what() const throw() {
    return what_arg_.c_str();
  }

private:
  std::string what_arg_;
};

} // namespace grb
