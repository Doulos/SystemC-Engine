// BEGIN engine.cpp ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
// $Copyright: (c)2003-2006 Eklectically, Inc. $
// $License: May be used under the same terms as SystemC library. $

#include <systemc.h>
#include <iomanip>

//--------------------------------------------------------------------
// This SystemC code is designed to illustrate the operation of
// the SystemC simulation kernel. This is the BASIC uninstrumented
// code shown in the presentation. See engine+.cpp for an example
// that has hooks to show activity as the design simulates.
//--------------------------------------------------------------------

SC_MODULE(M) {
  sc_in<bool> ckp;
  sc_out<int> s1p;
  sc_signal<int> s2;
  sc_event e1, e2;
  void P1_meth(void);
  void P3_thrd(void);
  void P2_thrd(void);
  SC_CTOR(M):count(0),temp(9)
  {
    cout << "T-5:---- Elaborating (CTOR Registering P3_thrd)" << std::endl;
    SC_THREAD(P3_thrd);
    cout << "T-4:---- Elaborating (CTOR Registering P2_thrd)" << std::endl;
    SC_THREAD(P2_thrd);
    sensitive << ckp.pos();
    cout << "T-3:---- Elaborating (CTOR Registering P1_meth)" << std::endl;
    SC_METHOD(P1_meth);
    dont_initialize();
    sensitive << s2;
  }//end SC_CTOR
  unsigned int count;
private:
  int temp;
};//end SC_MODULE

void M::P1_meth(void) {
  temp = s2.read();
  s1p->write(temp+1);
  e2.notify(2,SC_NS);
}
void M::P2_thrd(void) {
  A: s2.write(5);
  e1.notify();//immediate
  wait();
  B: for (int i=7;i<9;i++){
    s2.write(i);
    wait(1,SC_NS);
    C: e1.notify(SC_ZERO_TIME);//delta cycle
    wait();//static sensitive
  }//endfor
}
void M::P3_thrd(void) {
  D: while(true) { 
    wait(e1|e2);
    E: cout << "NOTE " << sc_time_stamp() 
            << std::endl;
  }//endwhile
}

// The purpose of sc_main() is to start up the simulation.  This is
// where the design is constructed during the "elaboration phase",
// before simulation starts.
int sc_main(int argc __attribute__((unused)),char *argv[] __attribute__((unused))) {
  cout << "T-6:---- MAIN Elaboration begins" << std::endl;
  cout << "T-6:---- MAIN Elaborating (constructing/binding)" << std::endl;
  sc_clock ck("ck",sc_time(6,SC_NS),0.5,sc_time(3,SC_NS));
  sc_signal<int> s1;
  M m("m");
  cout << "T-2:---- MAIN Elaborating (Connecting ck)" << std::endl;
  m.ckp(ck);
  cout << "T-1:---- MAIN Elaborating (Connecting s1)" << std::endl;
  m.s1p(s1);
  cout << "T_0:---- MAIN Elaboration finished" << std::endl;
  cout << "T_0:---- MAIN Starting simulation" << std::endl;
  // Begin simulation...
  sc_start(30,SC_NS); // Simulate for 30 nano-seconds
  // Simulation complete!
  cout << "T" << ++m.count << ":" 
       << int(sc_time_stamp()/sc_time(1,SC_NS)) << "ns" 
       << " MAIN Simulation stopped" << std::endl;
  return 0;
}//end sc_main()

// END engine.cpp ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
