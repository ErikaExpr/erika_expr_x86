proctype Alarm(){
   int i =0;
   unsigned default_tid:3 = 0;   unsigned ctid:3 = 0;
   int cycle = 0; 
init_state: 
      api_ch?[_, eval(SetRelAlarm),_,_]; 
   api_ch?ctid,_,_,cycle; 
   api_ch!default_tid, RT, ctid, 0; 
timer:  
    task_state[1] != Running;
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
