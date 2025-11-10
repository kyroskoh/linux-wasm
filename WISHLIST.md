# WebAssembly Wish List for Linux/Wasm

This document outlines desired WebAssembly features and improvements that would significantly benefit the Linux/Wasm project and the broader WebAssembly ecosystem.

## Core Features

### 1. MMU Support for Memory Sharing and Protection
**Status**: Not available  
**Priority**: High  

An MMU (Memory Management Unit) would enable:
- **Memory sharing** between WebAssembly instances
- **Memory protection** to isolate processes
- **True multi-process support** similar to traditional operating systems
- **Security improvements** through hardware-level memory isolation

**Impact on Linux/Wasm**: Currently, Linux runs in NOMMU configuration, limiting process isolation and memory sharing capabilities. MMU support would enable full Linux process model.

### 2. Thread Suspension
**Status**: Not available  
**Priority**: High  

The ability to suspend and resume threads would enable:
- **Proper signal handling** (SIGSTOP, SIGCONT)
- **Debugging capabilities** (breakpoints, step-through)
- **Preemptive scheduling** for better task management
- **System call blocking** without busy-waiting

**Impact on Linux/Wasm**: Would significantly improve Linux kernel's ability to manage processes and handle signals properly.

### 3. ELF Binary Format Support
**Status**: Not available (Wasm uses custom binary format)  
**Priority**: Medium  

Moving to ELF (Executable and Linkable Format) would provide:
- **Tool compatibility** with existing development tools (objdump, readelf, gdb)
- **Standard debug information** formats (DWARF)
- **Easier integration** with existing build systems
- **Better interoperability** with native code toolchains

**Impact on Linux/Wasm**: Would simplify debugging, profiling, and integration with standard Linux development workflows. Currently, special tooling is needed to work with Wasm binaries.

**Community Action Required**: This would need broad ecosystem agreement and migration path.

### 4. Shared WebAssembly Instances Between Workers
**Status**: Not available  
**Priority**: Medium  

Being able to share Wasm instances across Web Workers would enable:
- **Reduced memory footprint** (shared code across workers)
- **Faster worker initialization** (no need to recompile/instantiate)
- **True shared libraries** between processes
- **Better resource utilization**

**Impact on Linux/Wasm**: Would enable more efficient multi-CPU support and reduce memory overhead for running multiple processes.

### 5. Memory Address Breakpoints in Debugger
**Status**: Unknown (may be possible already?)  
**Priority**: Low-Medium  

Setting breakpoints on memory addresses would enable:
- **Data watchpoints** (break when specific memory is read/written)
- **Memory corruption debugging** (catch buffer overflows)
- **Hardware-level debugging** similar to native applications
- **Better understanding of memory access patterns**

**Impact on Linux/Wasm**: Would significantly improve debugging capabilities, especially for kernel development and low-level system programming.

**Note**: This may already be possible with existing browser DevTools - needs investigation.

## Additional Considerations

### Performance Optimizations
- **SIMD improvements** for faster graphics and computation
- **Zero-copy memory operations** between Wasm and JavaScript
- **Faster exception handling** for system calls

### System Integration
- **Direct filesystem access** (with proper sandboxing)
- **Network socket support** in WebAssembly
- **Hardware acceleration** for graphics (WebGPU integration)

### Developer Experience
- **Better debugging tools** specific to Wasm
- **Performance profiling** with lower overhead
- **Source maps** for all Wasm languages

## Related Proposals

Track WebAssembly proposals that may address these needs:
- [WebAssembly Threads](https://github.com/WebAssembly/threads)
- [WebAssembly Exception Handling](https://github.com/WebAssembly/exception-handling)
- [WebAssembly SIMD](https://github.com/WebAssembly/simd)
- [WebAssembly Component Model](https://github.com/WebAssembly/component-model)

## Contributing

If you have additional wish list items or want to contribute to making these features a reality:
1. Participate in [WebAssembly Community Group](https://www.w3.org/community/webassembly/)
2. File issues and proposals in relevant WebAssembly GitHub repositories
3. Prototype and demonstrate use cases that would benefit from these features
4. Contribute to browser implementations and WebAssembly tooling

## Priority Assessment

| Feature | Priority | Difficulty | Impact on Linux/Wasm |
|---------|----------|------------|---------------------|
| MMU Support | High | Very High | Critical - enables full process model |
| Thread Suspension | High | High | Major - improves kernel capabilities |
| ELF Format | Medium | Very High | Significant - better tooling |
| Shared Instances | Medium | High | Moderate - resource optimization |
| Memory Breakpoints | Low-Medium | Medium | Minor - debugging improvement |

---

**Last Updated**: November 10, 2025  
**Maintainer**: Kyros Koh

For questions or suggestions, please open an issue on the project repository.

