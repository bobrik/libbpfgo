//+build ignore
#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

#ifdef asm_inline
#undef asm_inline
#define asm_inline asm
#endif

struct {
    __uint(type, BPF_MAP_TYPE_RINGBUF);
    __uint(max_entries, 1 << 24);
} events SEC(".maps");

const volatile int foo SEC(".rodata") = 0;
const volatile int bar SEC(".data") = 0;
const volatile int baz SEC(".rodata.baz") = 0;
const volatile int qux SEC(".rodata.qux") = 0;
const volatile int quux SEC(".data.quux") = 0;
const volatile int quuz SEC(".data.quuz") = 0;

long ringbuffer_flags = 0;

SEC("kprobe/sys_mmap")
int kprobe__sys_mmap(struct pt_regs *ctx)
{
    int *process;

    // Reserve space on the ringbuffer for the sample
    process = bpf_ringbuf_reserve(&events, sizeof(int), ringbuffer_flags);
    if (!process) {
        return 1;
    }

    *process = foo + bar + baz + qux + quux + quuz;

    bpf_ringbuf_submit(process, ringbuffer_flags);
    return 1;
}

char LICENSE[] SEC("license") = "GPL";

