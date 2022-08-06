#pragma once

#if _HAS_NODISCARD && !defined(_NODISCARD)
#define _NODISCARD [[nodiscard]]
#elif !defined(_NODISCARD)
#define _NODISCARD
#endif

constexpr auto forceSemicolon = 0;

#ifdef DEBUG
#define TGE_EXPECT(statement, message, rv)                                         \
  if (!(statement)) {                                                          \
    printf(message);                                                           \
    return rv;                                                                 \
  }                                                                            \
  forceSemicolon
#else
#define TGE_EXPECT(statement, message, rv) forceSemicolon
#endif // DEBUG

namespace tge::main {

enum class Error {
  NONE,
  ALREADY_RUNNING,
  ALREADY_INITIALIZED,
  NOT_INITIALIZED,
  NO_GRAPHIC_QUEUE_FOUND,
  SURFACECREATION_FAILED,
  COULD_NOT_CREATE_WINDOW,
  COULD_NOT_CREATE_WINDOW_CLASS,
  NO_MODULE_HANDLE,
  FORMAT_NOT_FOUND,
  SWAPCHAIN_EXT_NOT_FOUND,
  NO_SURFACE_SUPPORT,
  VULKAN_ERROR,
  GLTF_LOADER_ERROR,
  FORMAT_NOT_SUPPORTED
};

extern Error error;

} // namespace tge::main
