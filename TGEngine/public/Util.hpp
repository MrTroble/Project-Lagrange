#pragma once

#include <filesystem>
#include <stdint.h>
#include <type_traits>
#include <vector>

namespace fs = std::filesystem;

namespace tge::util {

template <class T> concept Callable = std::is_invocable_v<T>;
template <Callable C> class OnExit {

  const C call;

public:
  constexpr OnExit(const C cin) : call(cin) {}
  constexpr ~OnExit() { call(); }
};

std::vector<char> wholeFile(const fs::path &path);

extern bool exitRequest;

void requestExit();

} // namespace tge::util