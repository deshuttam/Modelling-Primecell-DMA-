#include "DMAC.h"
DMAC::DMAC (sc_module_name n) :
sc_module (n), ctrl_regs ("ctrl_regs"), channels ("chanels", 2)
{
curr_chnl = 0;
SC_THREAD (DMAC_thread);
}
//This function will be used in packing and unpacking of data.
void DMAC::set_num_den (int Swidth, int Dwidth, int &num, int &den)
{
if (Swidth == 2) {
std::cout<<"Swidth is 32 bits/4 bytes\n";
if ((channels[curr_chnl].DMACCSrcAddr & 0x3) != 0) {
std::cout<<"Warning :- Data sheet dictates that source";
std::cout<<" address must be aligned with source widths\n";
}
num = 32;
}
if (Swidth == 1) {
std::cout<<"Swidth is 16 bits/2 bytes\n";
if ((channels[curr_chnl].DMACCSrcAddr & 0x1) != 0) {
std::cout<<"Warning :- Data sheet dictates that source";
std::cout<<" address must be aligned with source widths\n";
}
num = 16;
}
if (Swidth == 0) {
std::cout<<"Swidth is 8 bits/1 byte\n";
num = 8;
}
if (Dwidth == 2) {
std::cout<<"Dwidth is 32 bits/4 bytes\n";
if ((channels[curr_chnl].DMACCDestAddr & 0x3) != 0) {
std::cout<<"Warning :- Data sheet dictates that destination";
std::cout<<" address must be aligned with destination widths\n";
}
den = 32;
}
if (Dwidth == 1) {
std::cout<<"Dwidth is 16 bits/2 bytes\n";
if ((channels[curr_chnl].DMACCDestAddr & 0x1) != 0) {
std::cout<<"Warning :- Data sheet dictates that Destination";
std::cout<<" address must be aligned with Destination widths\n";
}
den = 16;
}
if (Dwidth == 0) {
std::cout<<"Dwidth is 8 bits/1 byte\n";
den = 8;
}
}

void DMAC::update_dest_address (int Dwidth, bool flag) 
{
if (((channels[curr_chnl].DMACCControl >> 27) & 0x1) || flag == true) {
switch (Dwidth) {
case 0:
channels[curr_chnl].DMACCDestAddr += 1;
break;
case 1:
channels[curr_chnl].DMACCDestAddr += 2;
break;
case 2:
channels[curr_chnl].DMACCDestAddr += 4;
break;
default:
std::cout<<"Invalid Dwidth specified\n";
break;
}
}
}

void DMAC::update_src_address (int Swidth, bool flag)
{
if (((channels[curr_chnl].DMACCControl >> 26) & 0x1) || flag == true) {
switch (Swidth) {
case 0:
channels[curr_chnl].DMACCSrcAddr += 1;
break;
case 1:
channels[curr_chnl].DMACCSrcAddr += 2;
break;
case 2:
channels[curr_chnl].DMACCSrcAddr += 4;
break;
default:
std::cout<<"Invalid Swidth specified\n";
break;
}
}
}

void DMAC::set_tc_interrupt (int transfer_size, int ITC)
{
if (transfer_size == 0) {
if (ITC) {
std::cout<<"\nTransfer complete!\n";
std::cout<<"Setting TC interrupt\n\n\n";
DMACINTTC->write (1);
ctrl_regs.DMACIntStatus |= (1 << curr_chnl);
ctrl_regs.DMACIntTCStatus |= (1 << curr_chnl);
}
ctrl_regs.DMACRawIntTCStatus |= (1 << curr_chnl);
}
}
/*Error interrupt is generated whenever some error occurs. Provided the interrupt is not 
masked out*/
void DMAC::set_err_interrupt (int transfer_size, int IE)
{
if ((transfer_size != 0 && (channels[curr_chnl].DMACCConfiguration & 0x1) == 1)) {
if (IE) {
std::cout<<"setting error interrupt\n";
DMACINTERR->write (1);
ctrl_regs.DMACIntStatus |= (1 << curr_chnl);
ctrl_regs.DMACIntErrorStatus = (1 << curr_chnl);
}
ctrl_regs.DMACRawIntErrorStatus = (1 << curr_chnl);
}
}
//shows which master port has been selected.
void DMAC::check_master_port (int D, int S)
{
if (D == 1) {
std::cout<<"Master port 2 selected for destination xfer\n";
} else {
std::cout<<"Master port 1 selected for destination xfer\n";
}
if (S == 1) {
std::cout<<"Master port 2 selected for source xfer\n";
} else {
std::cout<<"Master port 1 selected for source xfer\n";
}
}
//Thread which takes care of the transfer.
void DMAC::DMAC_thread ()
{
wait (SC_ZERO_TIME);
std::cout<<"dmac\n";
int i;
int val; 
for (i = 0; i < 2; i++) {
std::cout<<"\n\nCurrent Channel :- "<<i<<"\n\n";
//Check if the channel is enable and it is configured for mem to mem 
if (((ctrl_regs.DMACEnbldChns >> i) & 0x1) == 1) {
//For mem to mem xfer val should be zero.
val = channels[i].DMACCConfiguration & 0x00003800;
curr_chnl = i;
} else {
val = -1;
}
if (val == 0) {
int D, S, transfer_size = 0;
uint32_t src_addr, dst_addr;
std::cout<<"doing mem transfer\n";
D = (channels[curr_chnl].DMACCControl >> 25) & 0x1;
S = (channels[curr_chnl].DMACCControl >> 24) & 0x1;
check_master_port (D, S);
transfer_size = channels[curr_chnl].DMACCControl & 0xFFF;
std::cout<<"\n\nTransfer size :- "<<transfer_size<<"\n"; 
//Do the transfer only if the DMAC has been enabled.
if ((ctrl_regs.DMACConfiguration & 0x1) == 1) {
int Dwidth = (channels[curr_chnl].DMACCControl >> 21) & 0x7;
int Swidth = (channels[curr_chnl].DMACCControl >> 18) & 0x7;
uint32_t mask = 0xFFFFFFFF;
int loop_dst = 1, loop_src = 1;
int num = 32, den = 32;
bool check = false;
/*Used in packing and unpacking of data. num and den indicates the source and destination 
widths respectively in bits.*/ 
set_num_den (Swidth, Dwidth, num, den);
std::cout<<"\nDMAC enable\n";
uint32_t data = 0, temp = 0;
int ITC, IE;
ITC = (channels[curr_chnl].DMACCConfiguration >> 15) & 0x1;
IE = (channels[curr_chnl].DMACCConfiguration >> 14) & 0x1;
int temp_den = 0;
while (transfer_size != 0) {
/*If same memory is used and source and destinaiton address are same, display a 
warning.*/
if ((Swidth == Dwidth) &&(channels[curr_chnl].DMACCSrcAddr == 
channels[curr_chnl].DMACCDestAddr)) {
std::cout<<"Warning :- source and destination addresses are same\n";
}
temp_den = den;
if (Swidth > Dwidth) {
loop_dst = num / den;
loop_src = 1;
if (loop_dst == 4 && Swidth == 2) {
mask = 0xFF;
}
if (loop_dst == 2) {
if (Swidth == 2) {
mask = 0xFFFF;
}
if (Swidth == 1) {
mask = 0xFF;

}
} else {
loop_dst = 1;
loop_src = den / num;
}
std::cout<<"Transfering...\n";
while (loop_src != 0) {
src_addr = channels[curr_chnl].DMACCSrcAddr;
std::cout<<"src address :-"<<std::hex<<src_addr<<std::dec<<"\n";
if (S == 1) {
check = master_port_2->read (src_addr, temp);
} else {
check = master_port_1->read (src_addr, temp);
}
if (Swidth < Dwidth) {
temp_den = temp_den - num;
data |= temp << temp_den;
} else {
data = temp;
}
std::cout<<"Data :- "<<std::hex<<data<<std::dec<<"\n";
update_src_address (Swidth, loop_src > 1);
loop_src--;
}
if (check == false) {
break;
}
channels[curr_chnl].fifo.push_back (data);
//uncomment these two lines to check channel disabling.
/*std::cout<<"waiting\n";
 wait (5, SC_NS);*/
if ((channels[curr_chnl].DMACCConfiguration & 0x1) == 0) {
std::cout<<"Channel disabled by the user\n";
std::cout<<"Data transmission will be aborted\n";
break;
}
data = channels[curr_chnl].fifo.back ();
channels[curr_chnl].fifo.pop_back ();
while (loop_dst != 0) {
dst_addr = channels[curr_chnl].DMACCDestAddr;
std::cout<<"dst address :- "<<std::hex<<dst_addr<<std::dec<<"\n";

if (D == 1) {
check = master_port_2->write (dst_addr, data & mask);
} else { 
check = master_port_1->write (dst_addr, data & mask);
}
data = data >> den;
update_dest_address (Dwidth, loop_dst > 1);
loop_dst--;
}
if (check == false) {
break;
}
transfer_size--;
}
set_tc_interrupt (transfer_size, ITC);
wait (1, SC_NS);
set_err_interrupt (transfer_size, IE);
} else {
std::cout<<"DMAC is not enabled\n";
}
ctrl_regs.DMACEnbldChns &= ~(1 << curr_chnl);
} else {
std::cout<<"Channel not enabled\n\n";
}
}
}
bool DMAC::read (uint32_t addr, uint32_t &data)
{
bool ret = true;
switch (addr) {
case 0x000:
data = ctrl_regs.DMACIntStatus;
break;
case 0x004:
data = ctrl_regs.DMACIntTCStatus;
break;
case 0x008:
std::cout<<"Warning:- DMACIntTCClear is a write only register\n";
ret = false;
break;
case 0x00C:
data = ctrl_regs.DMACIntErrorStatus;
break;
case 0x010:
std::cout<<"Warning:- DMACIntErrorClear is a write only register\n";
ret = false;
break;
case 0x014:
data = ctrl_regs.DMACRawIntTCStatus;
break;
case 0x018:
data = ctrl_regs.DMACRawIntErrorStatus;
break;
case 0x01C:
data = ctrl_regs.DMACEnbldChns;
break;
case 0x020:
data = ctrl_regs.DMACSoftBReq;
break;
case 0x024:
data = ctrl_regs.DMACSoftSReq;
break;
case 0x028:
data = ctrl_regs.DMACSoftLBReq;
break;
case 0x02C:
data = ctrl_regs.DMACSoftLSReq;
break;
case 0x030:
data = ctrl_regs.DMACConfiguration;
break;
case 0x34:
data = ctrl_regs.DMACSync;
break;
case 0x100:
data = channels[0].DMACCSrcAddr;
break;
case 0x104:
data = channels[0].DMACCDestAddr;
break;
case 0x108:
data = channels[0].DMACCLLI;
break;
case 0x10C:
data = channels[0].DMACCControl;
break;
case 0x110:
data = channels[0].DMACCConfiguration;
break;
case 0x120:
data = channels[1].DMACCSrcAddr;
break;
case 0x124:
data = channels[1].DMACCDestAddr;
break;
case 0x128:
data = channels[1].DMACCLLI;
break;
case 0x12C:
data = channels[1].DMACCControl;
break;
case 0x130:
data = channels[1].DMACCConfiguration;
break;
}
return ret;
}
bool DMAC::write (uint32_t addr, uint32_t data)
{
bool ret = true;
switch (addr) {
case 0x000:
std::cout<<"Warning:- DMACIntStatus is a read only register\n";
ret = false;
break;
case 0x004:
std::cout<<"Warning:- DMACIntTCStatus is a read only register\n";
ret = false;
break;
case 0x008:
if ((data >> 7) != 0) {
std::cout<<"Warning:- writing to reseerved fields\n";
ret = false;
}
ctrl_regs.DMACIntTCClear = data;
ctrl_regs.DMACIntTCStatus = ctrl_regs.DMACIntTCStatus 
& (~(ctrl_regs.DMACIntTCClear));
ctrl_regs.DMACRawIntTCStatus = 
ctrl_regs.DMACRawIntTCStatus & (~(ctrl_regs.DMACIntTCClear));
if (!(ctrl_regs.DMACIntErrorStatus & data)) {
ctrl_regs.DMACIntStatus &= ~(data);
}
if (!ctrl_regs.DMACIntTCStatus) {
DMACINTTC->write (0);
}
break;
case 0x00C:
std::cout<<"Warning:- DMACIntErrorStatus is a read only register\n";
ret = false;
break;
case 0x010:
if ((data >> 7) != 0) {
std::cout<<"Warning:- writing to reseerved fields\n";
ret = false;
}
ctrl_regs.DMACIntErrClr = data;
ctrl_regs.DMACIntErrorStatus = 
ctrl_regs.DMACIntErrorStatus & (~(ctrl_regs.DMACIntErrClr));
ctrl_regs.DMACRawIntErrorStatus = 
ctrl_regs.DMACRawIntErrorStatus & (~(ctrl_regs.DMACIntErrClr));
if (!(ctrl_regs.DMACIntTCStatus & data)) {
ctrl_regs.DMACIntStatus &= ~(data);
}
if (!ctrl_regs.DMACIntErrorStatus) {
DMACINTERR->write (0);
}
break;
case 0x014:
std::cout<<"Warning:- DMACRawIntTCStatus is a read only register\n";
ret = false;
break;
case 0x018:
std::cout<<"Warning:- DMACRawIntErrorStatus is a read only register\n";
ret = false;
break;
case 0x01C:
std::cout<<"Warning:- DMACEnbldChns is a read only register\n";
ret = false;
break;
case 0x020:
ctrl_regs.DMACSoftBReq = data;
break;
case 0x024:
ctrl_regs.DMACSoftSReq = data;
break;
case 0x028:
ctrl_regs.DMACSoftLBReq = data;
break;
case 0x02C:
ctrl_regs.DMACSoftLSReq = data;
break;
case 0x030:
if ((data >> 3) != 0) {
std::cout<<"Warning:- writing to reseerved fields\n";
ret = false;
}
ctrl_regs.DMACConfiguration = data;
break;
case 0x34:
ctrl_regs.DMACSync = data;
break;
case 0x100:
channels[0].DMACCSrcAddr = data;
break;
case 0x104:
channels[0].DMACCDestAddr = data;
break;
case 0x108:
channels[0].DMACCLLI = data;
break;
case 0x10C:
channels[0].DMACCControl = data;
break;
case 0x110:
channels[0].DMACCConfiguration = data;
ctrl_regs.DMACEnbldChns |= (data & 0x1) << 0;
break;
case 0x120:
channels[1].DMACCSrcAddr = data;
break;
case 0x124:
channels[1].DMACCDestAddr = data;
break;
case 0x128:
channels[1].DMACCLLI = data;
break;
case 0x12C:
channels[1].DMACCControl = data;
break;
case 0x130:
channels[1].DMACCConfiguration = data;
ctrl_regs.DMACEnbldChns |= (data & 0x1) << 1;
break;
}
return ret;
}