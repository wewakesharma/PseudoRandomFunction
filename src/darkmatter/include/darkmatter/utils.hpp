#ifndef DARKMATTER_UTILS_H
#define DARKMATTER_UTILS_H
// Copyright (C) 2020 Tzipora Halevi, MIT license - see LICENSE file
/** utils.hpp
 *  - randomness, communications, and I/O
 */
#include <vector>
#include <iostream>
#include <iterator>

template <typename T>
std::ostream& operator<<(std::ostream& s, const std::vector<T>& v) {
  s << '[';
  if (!v.empty()) {
    std::copy(v.begin(), v.end(), std::ostream_iterator<T>(s," "));
  }
  s << ']';
  return s;
}

template <typename T>
std::istream& operator>>(std::istream& s, std::vector<T>& v) {
  // FIXME: not implemented yet
  return s;
}


// FIXME: This implementation of randomWord is buggy and insecure,
// for production it must replaced, e.g., using libsodium. See
// https://github.com/jedisct1/libsodium
#include <random>
inline uint64_t randomWord(uint64_t seed=0) 
{
    static bool initialized = false;
    static std::mt19937 gen; // mt19937 is a standard mersenne_twister_engine
    if (!initialized) {
        initialized = true;
        if (seed!=0) gen.seed(seed);
    }
    // for some reason gen() seems to return only 32 bits each time
    return (uint64_t(gen())<<32) ^ gen();
}


#endif // DARKMATTER_UTILS_H
