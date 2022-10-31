#include "test_bench.h"
#include "config.h"
tb::tb (sc_module_name n) : sc_module (n), mem_8 ("mem_8"), mem_16 ("mem_16"), 
mem_32("mem_32")
{
SC_THREAD (config_thread);
SC_METHOD (method_tc);
sensitive<<interrupt_tc;
dont_initialize ();
SC_METHOD (method_err);
sensitive<<interrupt_err;
dont_initialize ();
}
void tb::config_thread ()
{
uint32_t val = 0;
std::cout<<"tb\n";
//setting up channel 1.
master->write (0x008, 0x2);
master->write (0x010, 0x2);
master->write (0x120, src_addr_ch1);
master->write (0x124, dst_addr_ch1);
val = (DI_ch1 << 27 | SI_ch1 << 26 | D_ch1 << 25 | S_ch1 << 24 | dst_src_ch1 
<< 16 | transfer_size_ch1);
master->write (0x12C, val);
master->write (0x130,(ITC_ch1 << 15 | IE_ch1 << 14 | ch1_en));
//setting up channel 0.
master->write (0x008, 0x1);
master->write (0x010, 0x1);
master->write (0x100, src_addr_ch0);
master->write (0x104, dst_addr_ch0);
val = (DI_ch0 << 27 | SI_ch0 << 26 | D_ch0 << 25 | S_ch0 << 24 | dst_src_ch0 
<< 16 | transfer_size_ch0);
master->write (0x10C, val);
master->write (0x110,(ITC_ch0 << 15 | IE_ch0 << 14 | ch0_en));
master->write (0x030, EN);
//Uncomment these lines to check disabling of channel.
/*wait (10, SC_NS);
 master->write (0x110,(1 << 15 | 1 << 14 | 0));
 std::cout<<"Back in tb thread\n";*/
for (int i = 0; i < 2; i++) {
wait (done);
std::cout<<"\n32 bit memory status :- \n";
mem_32.print_data ();
std::cout<<"\n16 bit memory status :- \n";
mem_16.print_data ();
std::cout<<"\n8 bit memory status :- \n";
mem_8.print_data ();
}
}
void tb::method_tc ()
{
if (interrupt_tc->read() != 0) {
std::cout<<"TC interrupt line :- "<<interrupt_tc->read ()<<"\n";
uint32_t data;
master->read (0x000, data);
std::cout<<"Interrupt status register :- "<<std::hex<<data<<std::dec<<"\n";
master->read (0x004, data);
std::cout<<"Interrupt TC status register :- "<<std::hex<<data<<std::dec<<"\n";
master->read (0x00C, data);
std::cout<<"Interrupt Error status register :- "<<std::hex<<data<<std::dec<<"\n";
master->read (0x014, data);
std::cout<<"Interrupt Raw TC status register :- "<<std::hex<<data<<std::dec<<"\n";
master->read (0x018, data);
std::cout<<"Interrupt Raw Error status register :- "<<std::hex<<data<<std::dec<<"\n";
master->write (0x008, 0x3);
done.notify (SC_ZERO_TIME);
}
}
void tb::method_err ()
{
if (interrupt_err->read () != 0) {
std::cout<<"Err interrupt line :- "<<interrupt_err->read ()<<"\n";
std::cout<<"Error occured\n";
uint32_t data;
master->read (0x000, data);
std::cout<<"Interrupt status register :- "<<std::hex<<data<<std::dec<<"\n";
master->read (0x004, data);
std::cout<<"Interrupt TC status register :- "<<std::hex<<data<<std::dec<<"\n";
master->read (0x00C, data);
std::cout<<"Interrupt Error status register :- "<<std::hex<<data<<std::dec<<"\n";
master->read (0x014, data);
std::cout<<"Interrupt Raw TC status register :- "<<std::hex<<data<<std::dec<<"\n";
master->read (0x018, data);
std::cout<<"Interrupt Raw Error status register :- "<<std::hex<<data<<std::dec<<"\n";
master->write (0x010, 0x3);
done.notify (SC_ZERO_TIME);
}
}
