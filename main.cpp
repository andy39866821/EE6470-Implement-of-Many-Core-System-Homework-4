#define SC_INCLUDE_FX // enable the fixed-point data types

#include <iostream>
#include <string>
#include <systemc>
#include <sys/time.h>
#include "filter.h"
#include "testbench.h"
#include "SimpleBus.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;

int sc_main(int argc, char *argv[]) {
    Testbench tb("tb");
    Filter filt0("filt0", 0);
    Filter filt1("filt1", 1);
    SimpleBus<1, 2> bus("bus");

    tb.initiator.i_skt(bus.t_skt[0]);
    bus.set_clock_period(sc_time(CLOCK_PERIOD, SC_NS));
    bus.setDecode(1, FILTER_MM_BASE, FILTER_MM_BASE + FILTER_MM_SIZE - 1);
    bus.i_skt[0](filt0.t_skt); 
    bus.i_skt[1](filt1.t_skt); 


    tb.read_bmp();
    sc_start();
    tb.write_bmp();  

    return 0;
}