/* define basic configuration for PML model */
#define NUM_OF_TASKS 1
#define MAX_PRIORITY 1
#define NUM_EVENTS 1
#define NUM_RESOURCE 1
#define MIN_TASK_ID 0
#define MIN_RESOURCE_ID 0
#define MAX_RESOURCE_ID 1
#define MIN_EVENT_ID 0
#define MAX_EVENT_ID 1
#define NUM_OF_ISRs 0
#define is_alloc(r) ( Resource_Table[r].t[1] )
#define has_resource(tid) ( Resource_Table[0].t[tid] )
#define is_idle ( task_state[0] != Running && task_state[1] != Running )
