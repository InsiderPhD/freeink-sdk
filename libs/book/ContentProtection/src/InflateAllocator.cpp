#include "InflateAllocator.h"

namespace freeink {
namespace content {
namespace {
InflateAllocFn gAlloc = nullptr;
InflateFreeFn gFree = nullptr;
void* gOpaque = nullptr;
}  // namespace

void setInflateAllocator(InflateAllocFn alloc, InflateFreeFn release, void* opaque) {
  // Both or neither: a half-installed pair would hand miniz a buffer it then
  // freed with the wrong function.
  if (alloc == nullptr || release == nullptr) {
    gAlloc = nullptr;
    gFree = nullptr;
    gOpaque = nullptr;
    return;
  }
  gAlloc = alloc;
  gFree = release;
  gOpaque = opaque;
}

MzStreamAllocView currentInflateAllocator() {
  return MzStreamAllocView{reinterpret_cast<void*>(gAlloc), reinterpret_cast<void*>(gFree), gOpaque};
}

}  // namespace content
}  // namespace freeink
