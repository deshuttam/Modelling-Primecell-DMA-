#ifndef TEST_BENCH_H
#define TEST_BENCH_H
#include "systemc.h"
#include "memory.h"
#include "bus_if.h"
class tb : public sc_module {
SC_HAS_PROCESS (tb);
sc_event done;
public :
sc_port <bus_if> master;
sc_in <bool> interrupt_tc, interrupt_err;
memory_8 mem_8;
memory_16 mem_16;
memory_32 mem_32;
tb (sc_module_name n); 
void config_thread (void);
void method_tc (void);
void method_err (void);
};
#endif