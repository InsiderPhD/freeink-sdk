#pragma once

// FreeInk — injectable allocator for the content module's inflate.
//
// miniz makes ONE allocation per inflate stream (miniz.c: mz_inflateInit2 ->
// zalloc(opaque, 1, sizeof(inflate_state))), and that struct carries the
// format-mandated 32KB LZ dictionary plus its Huffman tables: about 44KB, all
// contiguous. On a small device with a fragmented heap that single request is
// the thing that fails -- free heap can be comfortable while the largest block
// is half what is needed.
//
// So the embedder gets to say where it comes from. CrossPoint hands it the
// e-ink framebuffer, which is allocated once at boot, never fragments, and is
// idle while a book is being opened (the panel is bistable and holds its image
// without it). Left unset, miniz uses its own malloc as before -- which is what
// host tests and PSRAM boards want.
//
// Freestanding C++17.

#include <stddef.h>

namespace freeink {
namespace content {

// Signatures match miniz's mz_alloc_func / mz_free_func exactly.
using InflateAllocFn = void* (*)(void* opaque, size_t items, size_t size);
using InflateFreeFn = void (*)(void* opaque, void* address);

// Install (or clear, by passing nullptrs) the allocator every inflate stream in
// this module will use. Not thread-safe; set it once during start-up, or around
// a known-quiet operation.
void setInflateAllocator(InflateAllocFn alloc, InflateFreeFn release, void* opaque);

// Applies the installed allocator to a stream. Internal to the module; declared
// here so both inflate call sites share one definition.
struct MzStreamAllocView {
  void* alloc;
  void* release;
  void* opaque;
};
MzStreamAllocView currentInflateAllocator();

}  // namespace content
}  // namespace freeink
