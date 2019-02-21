
//#include "ee.h"
//#include "test/assert/inc/ee_assert.h"

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#define TRUE 1

/* Assertions */
enum EE_ASSERTIONS {
  EE_ASSERT_FIN = 0,
  EE_ASSERT_INIT,
  EE_ASSERT_ISR_FIRED,
  EE_ASSERT_TASK_FIRED,
  EE_ASSERT_TASK_END,
  EE_ASSERT_DIM
};
short EE_assertions[EE_ASSERT_DIM];
#define EE_TRUE  1
//#define EE_ASSERT_NIL ((EE_TYPEASSERT)(-1))
#define EE_ASSERT_NIL -1
#define EE_ASSERT_YES 1

#include "lib_manual.h"

/* Final result */
volatile EE_TYPEASSERTVALUE result;

/* Counters */
volatile int task1_fired = 0;
volatile int task1_ended = 0;
volatile int isr1_fired = 0;
volatile int counter = 0;

void *TASK1(void *arg);
void *EE_pit_handler(void *arg) {

  pthread_t task;
  int thr_id;
  int status;
  int a = 1;
  int result = 0;

  while(1){
  isr1_fired++;
  printf ("Timer: (cnt: %d) - Activating Task1\n", isr1_fired);

  if (isr1_fired == 1)
  {
    EE_assert(EE_ASSERT_ISR_FIRED, isr1_fired == 1, EE_ASSERT_INIT);
  }

  assert(task1_fired == task1_ended);
  //ActivateTask(Task1);
  thr_id = pthread_create(&task, NULL, TASK1, (void *)&a);
    if(thr_id < 0){
            perror("Error!");
            exit(0);
    }
     pthread_join(task, (void **)&status);
  printf ("Timer - EOI\n");
  sleep(5);
  }
}

void *TASK1(void *arg)
{
  task1_fired++;

  printf("\tTask1 activated. cnt: %d\n", task1_fired);

  if (task1_fired == 1) {
    EE_assert(EE_ASSERT_TASK_FIRED, task1_fired == 1, EE_ASSERT_ISR_FIRED);
  }
  
 task1_ended++; 
}

/*
 * MAIN TASK
*/
int main(void)
{
	 pthread_t task[2];
    int thr_id;
    int status;
    int a = 1;
    int result = 0;

  printf("Starting ERIKA\n");

  //EE_pit_init();
  //EE_pit_set_periodic(20); // slow down to 20 Hz

     thr_id = pthread_create(&task[0], NULL, EE_pit_handler, (void *)&a);
    if(thr_id < 0){
            perror("Error!");
            exit(0);
    }

  EE_assert(EE_ASSERT_INIT, EE_TRUE, EE_ASSERT_NIL);

  EE_hal_enableIRQ(); // enable interrupt

  while (task1_fired == 0);
  EE_assert(EE_ASSERT_TASK_END, task1_fired==1, EE_ASSERT_TASK_FIRED);
  EE_assert_range(EE_ASSERT_FIN, EE_ASSERT_INIT, EE_ASSERT_TASK_END);
  result = EE_assert_last();

  if(result == EE_ASSERT_YES) {
    printf("TEST SUCCESSFUL FINISHED!\n");
  } else {
	  int i ;
    for( i = 0; i < EE_ASSERT_DIM; i++) {
      printf("ASSERT %d : %d\n", i, EE_assertions[i]);
    }
    printf("TEST FAILED!\n");
  }

     pthread_join(task[0], (void **)&status);
     assert(0);
  EE_hal_shutdown_system();

  __builtin_unreachable();
}

