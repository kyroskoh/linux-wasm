# WebAssembly Wish List for Linux/Wasm

This document outlines desired WebAssembly features and improvements that would significantly benefit the Linux/Wasm project and the broader WebAssembly ecosystem.

## 🎯 Active Proposals (Actionable Now!)

**Good news**: There are WebAssembly proposals currently in progress that could dramatically improve Linux/Wasm! While they need some tweaking for OS use cases, they're much closer to reality than long-term wishes:

- **Stack Switching** - Better context switching, coroutines, and signal handling
- **Memory Control** - Fine-grained memory management for process isolation
- **Execution State Hibernation** - Boot once, instant startup thereafter (concept stage)

These proposals are actively being developed and offer the best opportunity for community engagement. See the [Active Proposals section](#active-proposals-nearly-ready) below for details.

**Your involvement matters!** Providing feedback and demonstrating OS use cases can help shape these proposals to work well for Linux/Wasm.

---

## Core Features (Long-term Wishes)

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

## Active Proposals (Nearly Ready)

There are WebAssembly proposals currently in progress that could significantly improve the Linux/Wasm experience. These proposals are not quite ready for Linux use cases yet, but with the right motivation and community feedback, they could be adapted:

### Stack Switching
**Status**: Proposal in progress  
**Relevance**: High  

Stack switching would enable:
- **Cooperative multitasking** with efficient context switching
- **Coroutines and fiber support** for lightweight concurrency
- **Better signal handling** through controlled stack manipulation
- **Efficient system call implementation** without blocking

**Current State**: The proposal exists but needs tweaks to make it fully compatible with Linux kernel requirements. With proper community engagement, this could be adapted for OS use cases.

**Resources**:
- [Stack Switching Proposal](https://github.com/WebAssembly/stack-switching)

### Memory Control
**Status**: Proposal in progress  
**Relevance**: High  

Enhanced memory control features would provide:
- **Fine-grained memory management** for process isolation
- **Custom allocation strategies** for kernel memory management
- **Better memory protection** mechanisms
- **Support for memory-mapped I/O** patterns

**Current State**: The proposal is advancing but needs adjustments for full OS-level memory management. Linux/Wasm could benefit greatly from participating in shaping this proposal.

**Resources**:
- [Memory Control Proposal](https://github.com/WebAssembly/memory-control) (if available)

### Execution State Hibernation
**Status**: Concept (not yet proposed)  
**Relevance**: High  
**Priority**: Future consideration  

True hibernation of execution state would enable:
- **Boot once, reuse forever** - Save a fully booted Linux system state
- **Instant startup** - Skip kernel initialization on subsequent loads
- **Snapshot and restore** - Save/restore system state at any point
- **Migration** - Move running systems between environments

**Current Implementation**: Already possible via emulation (similar to how `setjmp`/`longjmp` is implemented today), but would be more elegant and performant with native browser support.

**Benefits**:
- Dramatically faster startup times (milliseconds vs. seconds)
- Better user experience for repeated use
- Enable persistent system state across sessions
- Lower resource usage through state reuse

**What's Needed**: Native support from browsers for serializing and deserializing complete execution state, including:
- Memory state
- CPU registers and program counter
- Thread states
- WebAssembly instance state

This would be a game-changer for web-based operating systems, enabling desktop-like persistence and performance.

## Related Proposals

Track additional WebAssembly proposals that may address our needs:
- [WebAssembly Threads](https://github.com/WebAssembly/threads)
- [WebAssembly Exception Handling](https://github.com/WebAssembly/exception-handling)
- [WebAssembly SIMD](https://github.com/WebAssembly/simd)
- [WebAssembly Component Model](https://github.com/WebAssembly/component-model)

## Contributing

### Engage with Active Proposals

The **most impactful** way to help is by engaging with active proposals that are nearly ready:

1. **Stack Switching & Memory Control**: Provide feedback on these proposals
   - Test with Linux/Wasm use cases
   - Document what works and what needs tweaking
   - Share your findings with proposal authors
   - Participate in discussions on GitHub

2. **Execution State Hibernation**: Help create the proposal
   - Prototype hibernation using current setjmp/longjmp emulation
   - Measure performance benefits
   - Document use cases and requirements
   - Advocate for native browser support

### General Contributions

For broader WebAssembly improvements:
1. Participate in [WebAssembly Community Group](https://www.w3.org/community/webassembly/)
2. File issues and proposals in relevant WebAssembly GitHub repositories
3. Prototype and demonstrate use cases that would benefit from these features
4. Contribute to browser implementations and WebAssembly tooling

**Remember**: Real-world demonstrations of Linux/Wasm use cases are incredibly valuable for showing proposal authors why these features matter!

## Priority Assessment

### Core Wish List Items

| Feature | Priority | Difficulty | Impact on Linux/Wasm |
|---------|----------|------------|---------------------|
| MMU Support | High | Very High | Critical - enables full process model |
| Thread Suspension | High | High | Major - improves kernel capabilities |
| ELF Format | Medium | Very High | Significant - better tooling |
| Shared Instances | Medium | High | Moderate - resource optimization |
| Memory Breakpoints | Low-Medium | Medium | Minor - debugging improvement |

### Active Proposals (Actionable Now)

| Proposal | Priority | Adaptation Needed | Impact on Linux/Wasm |
|----------|----------|-------------------|---------------------|
| Stack Switching | High | Medium | Major - better context switching & signals |
| Memory Control | High | Medium | Major - improved memory management |
| Execution State Hibernation | High | High (needs proposal) | Critical - instant boot & persistence |

**Note**: Active proposals are closer to reality and offer the best opportunity for community engagement to shape them for OS use cases.

---

**Last Updated**: November 10, 2025  
**Maintainer**: Kyros Koh

For questions or suggestions, please open an issue on the project repository.

