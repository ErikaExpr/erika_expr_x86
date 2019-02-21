c_decl{
#define TRUE 1
#define EE_TRUE (1)
#define EE_ASSERT_NIL -1
#define EE_ASSERT_YES 1
typedef int EE_ASSERTIONS;
const int EE_ASSERT_FIN = 0;
const int EE_ASSERT_INIT = 1;
const int EE_ASSERT_ISR_FIRED = 2;
const int EE_ASSERT_TASK_FIRED = 3;
const int EE_ASSERT_TASK_END = 4;
const int EE_ASSERT_DIM = 5;
EE_TYPEASSERTVALUE EE_assertions[5];
volatile EE_TYPEASSERTVALUE result;
volatile int task1_fired = 0;
volatile int isr1_fired = 0;
volatile int counter = 0;
};
