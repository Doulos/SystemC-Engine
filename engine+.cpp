// BEGIN engine.cpp ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
// $Copyright: (c)2003-2006 Eklectically, Inc. $
// $License: May be used under the same terms as SystemC library. $

#include <systemc.h>
#include <iomanip>

//--------------------------------------------------------------------//
// This SystemC code is designed to illustrate the operation of the   //
// SystemC simulation kernel with added instrumentation. In order to  //
// output messages that indicate operation, instrumentation has been  //
// added to the basic code:                                           //
//                                                                    //
// 1. A special "Did_update" channel is present to monitor when       //
// update state occurs. It sets a flag used by other routines.        //
//                                                                    //
// 2. Two special dumping methods were added to the main module,      //
//    Eval() and Changes(). In addition, several "prev_" prefixed     //
//    variables hold the "old" values of variables. This allows       //
//    Eval() and Changes() to print differences on-the-fly. Eval()    //
//    is strategically located anywhere we expect evaluations begin.  //
//    Changes() monitors variable changes that may have occurred just //
//    prior to its call. Changes() is setup to occur anywhere         //
//    evaluation of a thread may be suspended or exit.                //
//                                                                    //
// 3. A monitor SC_METHOD, mon(), was added to observe the clock.     //
//--------------------------------------------------------------------//

struct Did_update : public sc_prim_channel {
  // This is special primitive channel to notify entry by the simulator
  // into the update state. In other words, end of a delta-cycle.
  unsigned int *counter;
  bool updated;
  void watch(unsigned int * count) {
    counter = count;
    request_update();
  }
  void clear(void) { updated = false; }
  void update(void) {
    (*counter)++;
    updated = true;
    cout << "T" << std::setfill('_') << std::setw(2) << *counter
         //<< std::setfill('~') << std::setw(60) << std::left 
         << ":---- UPDT" << std::endl;
  }
};//endstruct

SC_MODULE(M) {
  sc_in<bool> ckp;
  sc_out<int> s1p;
  sc_signal<int> s2;
  sc_event e1, e2;
  void P1_meth(void);
  void P3_thrd(void);
  void P2_thrd(void);
  SC_CTOR(M):count(0),temp(9)
    ,prev_time(99),prev_ck(true),prev_s1(99),prev_s2(99),prev_temp(99)
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
    SC_METHOD(mon);
    sensitive << ckp;
    dont_initialize();
  }//end SC_CTOR
  void mon(void){Eval("ck",1);}// Monitor the clock
  unsigned int count;
private:
  int temp;
  // Special variables and routines to monitor execution follow
  Did_update me;
  double prev_time;
  int prev_temp, prev_s1, prev_s2;
  bool prev_ck;
  void Eval(const char* message,int type=0){ // Used to trace simulation
    if (me.updated) {
      Changes();
      me.clear();
    }//endif
    me.watch(&count);
    count++;
    cout << "T" << std::setfill('_') << std::setw(2) << count;
    cout << std::setfill(' ');
    if (prev_time == sc_time_stamp()/sc_time(1,SC_NS)) {
      cout << ":    ";
    } else {
      cout << ":" << std::setw(2) << int(sc_time_stamp()/sc_time(1,SC_NS))  << "ns";
    }//endif
    switch (type) {
      case  1: cout << " MNTR "; break;
      default: cout << " EVAL "; break;
    }//endswitch
    if (ckp->read() == prev_ck) cout << "     ";
    else                        cout << " ck=" << ckp->read();
    if (s1p->read() == prev_s1) cout << "     ";
    else                        cout << " s1=" << s1p->read();
    if (s2.read() == prev_s2)   cout << "     ";
    else                        cout << " s2=" << s2.read();
    if (temp == prev_temp)      cout << "       ";
    else                        cout << " temp=" << temp;
    cout << ": " << message 
         << std::endl;
    prev_time = sc_time_stamp()/sc_time(1,SC_NS);
    prev_ck = ckp->read();
    prev_s1 = s1p->read();
    prev_s2 = s2.read();
    prev_temp = temp;
  }//end Eval()
  void Changes(void) { // Display variable changes if any
    if (ckp->read() != prev_ck || s1p->read() != prev_s1 || s2.read() != prev_s2 || temp != prev_temp) {
      cout << "         [  ] ";
      if (ckp->read() == prev_ck) cout << "     ";
      else                        cout << " ck=" << ckp->read();
      if (s1p->read() == prev_s1) cout << "     ";
      else                        cout << " s1=" << s1p->read();
      if (s2.read() == prev_s2)   cout << "     ";
      else                        cout << " s2=" << s2.read();
      if (temp == prev_temp)      cout << "       ";
      else                        cout << " temp=" << temp;
      cout << std::endl;
      prev_ck   = ckp->read();
      prev_s1   = s1p->read();
      prev_s2   = s2.read();
      prev_temp = temp;
    }//endif
  }//end Changes()
};//end SC_MODULE

void M::P1_meth(void) {
  Eval("P1_meth");
  temp = s2.read();
  s1p->write(temp+1);
  e2.notify(2,SC_NS);
  Changes();
}
void M::P2_thrd(void) {
  Eval("P2_thrd.A");
  A: s2.write(5);
  e1.notify();//immediate
  Changes();
  wait(); Eval("P2_thrd.B");
  B: for (int i=7;i<9;i++){
    s2.write(i);
    Changes();
    wait(1,SC_NS); Eval("P2_thrd.C");
    C: e1.notify(SC_ZERO_TIME);//delta cycle
    Changes();
    wait(); Eval("P2_thrd.B");//static sensitive
  }//endfor
  Eval("P2_thrd.EXITED");
}
void M::P3_thrd(void) {
  Eval("P3_thrd.D");
  D: while(true) { 
    Changes();
    wait(e1|e2); Eval("P3_thrd.E");
    E: cout << "NOTE " << sc_time_stamp() 
            << std::endl;
  }//endwhile
}

// The purpose of sc_main() is to start up the simulation.  This is
// where the design is constructed during the "elaboration phase",
// before simulation starts.
int sc_main(int argc __attribute__((unused)),char *argv[] __attribute__((unused))) {
  cout << "T-6:---- MAIN Elaborating (constructing/binding)" << std::endl;
  sc_clock ck("ck",sc_time(6,SC_NS),0.5,sc_time(3,SC_NS));
  sc_signal<int> s1;
  M m("m");
  cout << "T-2:---- MAIN Elaboration (Connecting ck)" << std::endl;
  m.ckp(ck);
  cout << "T-1:---- MAIN Elaborating (Connecting s1)" << std::endl;
  m.s1p(s1);
  cout << "T_0:---- MAIN Elaboration finished" << std::endl;
  cout << "T_0:---- MAIN Starting simulation" << std::endl;
  // Elaboration complete!
  // Begin simulation...
  sc_start(30,SC_NS); // Simulate for 30 nano-seconds
  // Simulation complete!
  cout << "T" << ++m.count << ":" 
       << int(sc_time_stamp()/sc_time(1,SC_NS)) << "ns" 
       << " MAIN Simulation stopped" << std::endl;
  return 0;
}//end sc_main()

// END engine.cpp ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
