#ifndef CPP17_VISITOR_HELPER
#define CPP17_VISITOR_HELPER

#include <iomanip>
#include <iostream>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

// licenced under creative commons per cppreference.com

template<class T> struct always_false : std::false_type {};
using var_t = std::variant<int, long, double, std::string>;
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

#endif
