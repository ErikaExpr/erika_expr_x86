#include "lib_manual.h" 
#include "lib.h" 
inline EE_pit_handler(tid){
   c_code{
   isr1_fired++;
         printf ("Timer: (cnt: %d) - Activating Task1\n", isr1_fired);
   if (isr1_fired == 1)
     {
       EE_assert(EE_ASSERT_ISR_FIRED, isr1_fired == 1, EE_ASSERT_INIT);
     }
      }; 
      api_ch!tid,ActivateTask,1,0; api_ch?[_, eval(RT), eval(tid),_]; api_ch?_,_,_,_;
   c_code{
         printf ("Timer - EOI\n");
};
}
inline TASK_Task1(tid){ 
      task_state[tid]==Running; 
   c_code{
   task1_fired++;
         printf("\tTask1 activated. cnt: %d\n", task1_fired);
   if (task1_fired == 1) {
       EE_assert(EE_ASSERT_TASK_FIRED, task1_fired == 1, EE_ASSERT_ISR_FIRED);
     }
      }; 
      task_state[tid]==Running; 
      api_ch!tid,TerminateTask,0,0; api_ch?[_, eval(RT), eval(tid),_]; api_ch?_,_,_,_;
}
inline TASK_Task0(tid){
      task_state[tid]==Running; 
   c_code{
         printf("Starting ERIKA\n");
      }; 
      task_state[tid]==Running; 
      api_ch!tid,SetRelAlarm,20,0; api_ch?[_, eval(RT), eval(tid),_]; api_ch?_,_,_,_;
      task_state[tid]==Running; 
   c_code{
         EE_assert(EE_ASSERT_INIT, EE_TRUE, EE_ASSERT_NIL);
         EE_hal_enableIRQ();
   };
   do
   :: c_expr{task1_fired == 0} -> task_state[tid] == Running; 
      
      :: else -> skip; 
      od;
   
      task_state[tid]==Running; 
   c_code{
         EE_assert(EE_ASSERT_TASK_END, task1_fired==1, EE_ASSERT_TASK_FIRED);
         EE_assert_range(EE_ASSERT_FIN, EE_ASSERT_INIT, EE_ASSERT_TASK_END);
   result = EE_assert_last();
   if(result == EE_ASSERT_YES) {
       printf("TEST SUCCESSFUL FINISHED!\n");
     } else {
       int i =0;
       for(i = 0; i < EE_ASSERT_DIM; i++) {
         printf("ASSERT %d : %d\n", i, EE_assertions[i]);
       }
       printf("TEST FAILED!\n");
     }
      }; 
      task_state[tid]==Running; 
      api_ch!tid,ShutdownOS,0,0; api_ch?[_, eval(RT), eval(tid),_]; api_ch?_,_,_,_;
      task_state[tid]==Running; 
      api_ch!tid,ShutdownOS,0,0; api_ch?[_, eval(RT), eval(tid),_]; api_ch?_,_,_,_;
      task_state[tid]==Running; 
   c_code{
         __builtin_unreachable();
};

}
