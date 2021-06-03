#include <grb/grb.hpp>

template <typename T>
struct plus {
	auto operator()(T a, T b) {
		return a + b;
	}

	T identity() {
		return T(0);
	}
};

// grb::plus, grb::times
// to get identity, then:
// grb::monoid_traits<grb::plus>::identity<int>()

// grb::plus<float>, grb::times<float, int>
// to get identity, then:
// grb::monoid_traits<grb::plus<float>>::identity()

int main(int argc, char** argv) {
	std::cout << std::plus<void>(1, 2.0f) << std::endl;
	std::cout << grb::monoid_traits<std::plus<int>>::identity() << std::endl;
	// std::string s = grb::monoid_traits<std::plus<void>>::identity<float>();
	std::cout << grb::monoid_traits<plus<int>>::identity<float>() << std::endl;
	return 0;
}