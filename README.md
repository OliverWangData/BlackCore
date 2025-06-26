# BlackCore

**BlackCore** is a modular C++ utility library designed for Unreal Engine. It provides reusable, performance-focused components and patterns that extend UE's core systems. This includes custom containers, resource managers, and systems built for high-performance runtime use.

## Contents

### `BlackCommon`

- **TStack** — Lightweight stack container tailored for Unreal’s memory and allocator model.  
- **TIndexPool** — Reusable index pool for efficient handle or ID management.  
- **TIndexPool2D** — 2D variant of `TIndexPool` for managing grid or matrix indices.  
- **TObjectPool** — Object pooling system to minimize allocations and improve cache locality.  
- **ArrayIndexing** — Helper functions to simplify and optimize multi-dimensional array indexing in C++.  
- **IntrusiveRefCountable** — Base class for intrusive reference counting patterns.  
- **RefCounter** — Utility for managing reference counts externally from objects.  
- **RefProvider** — Provider interface facilitating reference management and safe pointer access.
- *(More coming soon)*

### `BlackRuntimeResources`
Contains runtime systems designed for efficient memory and resource use, such as:

- **TextureAtlas** — Packs multiple smaller textures into a single atlas texture for optimized GPU usage.
- **LRUTextureAtlas** — Extends the `TextureAtlas` with a least-recently-used eviction policy to support dynamic streaming workloads.
- *(More coming soon)*
