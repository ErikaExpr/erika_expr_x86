/* creation : 2017.7.31. Monday 
   author : Yunja Choi

   2017.11.22. Wednesday
   additions & changes
   1. the size of api_ch is increased from 1 to 5 
   2. run_tid variable is added to keep track of currently running Task

   2018.9.20. Thursday
   1. handling of ShutdownOS has changed to call terminate_task before actually shutting it down

   2018.10.8
   increased the size of task_state by 1 for alarm
*/

#include "config.h"
#include "error.h"

/*----------declaring channels for passing API calls from application task to internal task-------*/
/*          it consists of the id of calling task,  API name, the first parameter, and the second parameter             */
  chan api_ch = [5] of {byte, mtype, byte, byte};
/*---------- enumerating possible API function names ----------------*/
  mtype = {StartOS, ShutDownOS, SetEvent, ReleaseResource, GetResource, ActivateTask, Schedule, TerminateTask, ChainTask, WaitEvent, ClearEvent, SetRelAlarm, SetAbsAlarm, CancelAlarm, ShutdownOS }; 
  mtype = {Suspended, Ready, Running, Waiting, RT, ISR};



/*----------- keep track of the state of each task -------------------*/
  //int task_state[NUM_OF_TASKS+NUM_OF_ISRs+1];
  int task_state[NUM_OF_TASKS+NUM_OF_ISRs+2];

/*---------- define constants   ---*/
#define Suspended 0
#define Ready 1
#define Running 2
#define Waiting 3

#define MAX_QUEUE_LENGTH 12    // sum of all max_activation_count + 1

/* variable for keeping error status */
unsigned e_code:5 = 0;

#include "app.h"
#include "sysds.h"
#include "APIs.h"

unsigned run_tid:3=0;
hidden int ret = 0; // for return values from user-defined functions

proctype OSEK_OS(){
    bit goes_ready = 0; 
    // local variables for api call
    mtype api;
    unsigned param0:3 = 0;  // calling tid 
    unsigned param1:3 = 0;
    unsigned param2:3 = 0; 
    // local variable for ceiling priority
    unsigned prio:4 = 0;
    unsigned tid:3 =0;
    // for counting 
    unsigned i:3 = 0; 

    Off:
        api_ch?[_,eval(StartOS),_,_]; api_ch?_,_,_,_;
        goto On; 

    On: 

        if
        :: atomic{ api_ch?[_,eval(ActivateTask),_,_] -> api_ch?param0,_, param1,_ ;     
             printf("task %d is activating task %d \n", param0, param1);
             activate_task(param0, param1); }
        :: atomic{ api_ch?[_,eval(Schedule),_,_]  -> api_ch?param0,_,_,_;  
             printf("task %d is scheduling \n", param0);
             schedule(param0);  }
        :: atomic{ api_ch?[_,eval(SetEvent),_,_] ->  api_ch?param0,_,param1, param2;   
             printf("task %d is setting event %d for task %d\n", tid, param2, param1);                     
             printf("current value of event_table[%d][%d] = %d \n", param2, param1, Event_Table[param2].t[param1]); }
             set_event(param0, param1, param2);
        :: atomic{api_ch?[_,eval(GetResource),_,_] -> api_ch?param0,_,param1,_;
               printf("task %d is getting resource %d\n", tid, param1);
               get_resource(param0, param1); }
        :: atomic{api_ch?[_,eval(ReleaseResource),_,_] -> api_ch?param0,_,param1,_ ;  //param1: resource id
               release_resource(param0, param1); }
        :: atomic{ api_ch?[_,eval(ChainTask),_,_] -> api_ch?param0,_,param1,param2;   // param1 : task_id 
               chain_task(param0, param1); }
        :: atomic{api_ch?[_,eval(TerminateTask),_,_] -> api_ch?param0,_,param1,param2;
               terminate_task(param0); }
        :: atomic{api_ch?[_,eval(WaitEvent),_,_] -> api_ch?param0,_,param1,_;   // param1: event id
               wait_event(param0, param1); }
	:: atomic{ api_ch?[_,eval(ShutdownOS),_,_] -> api_ch?param0,_,_,_;
                    terminate_task(param0);
                 }
		goto Off;
         :: atomic{ api_ch?[_,eval(ISR),_,_] -> api_ch?param0,_,param1,param2; 
               push_task_into_readyQ(param1, param2,0);
               schedule(param0); }
        // if no api call is comming, check whether there is a running task
        ::atomic{ is_idle && !emptyQ -> get_task_from_readyQ(tid, prio);
			        task_state[tid] = Running;  run_tid = tid; }
        :: else -> skip;
        fi;
   end:     goto On;
 }     
               
proctype Alarm(){
   int i =0;
   unsigned default_tid:3 = 0;   unsigned ctid:3 = 0;
   int cycle = 0; 
init_state: 
      api_ch?[_, eval(SetRelAlarm),_,_]; 
   api_ch?ctid,_,_,cycle; 
   api_ch!default_tid, RT, ctid, 0; 
timer:  
   // task_state[1] != Running;
   do
   :: i < cycle -> i = i+1; 
   :: else -> i =0; break; 
   od;
   EE_pit_handler(2);
   if
   ::api_ch?[_, eval(CancelAlarm),_,_] -> api_ch?ctid,_,_,_; 
         api_ch!default_tid, RT, ctid, 0;
         goto end;
   ::else -> skip; 
   fi; 
   task_state[0] == Running || task_state[1] == Running;
   goto timer; 
end:
}
proctype App(byte tid){
 main :
      if
      :: tid == 0 -> goto task0;
      :: tid == 1 -> goto task1;
      fi;
 task0: 
      task_dyn_info[tid].act_cnt > 0; 
      TASK_Task0(tid); 
      goto task0;

 task1: 
      task_dyn_info[tid].act_cnt > 0; 
      TASK_Task1(tid); 
      goto task1;

}
init{
   unsigned i:3=0;
   unsigned tid:3=0;
   unsigned prio:3 =0; 

   initialize();
atomic{
   task_state[0] = Running;
   task_dyn_info[0].act_cnt = 1;
   run_tid = 0; 
   run OSEK_OS();
   api_ch!0,StartOS,0,0;
   run App(0);
   run App(1);
   run Alarm(); 
   startup_process(1,1);
} }

//ltl p1 {[] !(task_state[1] == Running && task_state[2] == Running) }
ltl p2 {[] (task_state[1] == Running -> <> (task_state[1] == Suspended))}
