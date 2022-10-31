#ifndef DMAC_H
#define DMA_H
#include "systemc.h"
#include "bus_if.h"
#include "DMA_control_logic.h"
#include "DMA_channel_logic.h"
class DMAC : public sc_channel, public bus_if {
SC_HAS_PROCESS (DMAC);
DMA_control_logic ctrl_regs;
sc_vector <DMA_channel_logic> channels;
sc_event do_mem_xfer;
int curr_chnl;
public :
sc_port <bus_if> master_port_1;
sc_port <bus_if> master_port_2;
sc_out <bool> DMACINTTC, DMACINTERR;
DMAC (sc_module_name n); 
private :
void set_num_den (int Swidth, int Dwidth, int &num, int &den);
void update_dest_address (int Dwidth, bool flag);
void update_src_address (int Swidth, bool flag);
void set_tc_interrupt (int transfer_size, int ITC);
void set_err_interrupt (int transfer_size, int IE);
void check_master_port (int D, int S);
public :
void DMAC_thread (void);
bool read (uint32_t addr, uint32_t &data);
bool write (uint32_t addr, uint32_t data);
};
#endif
