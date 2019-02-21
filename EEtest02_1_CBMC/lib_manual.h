#define EE_TYPEASSERTVALUE short  
#define EE_TYPEASSERT int
#define EE_ASSERT_INITVALUE 0
#define EE_ASSERT_NIL -1
#define EE_ASSERT_YES 1
#define EE_ASSERT_NO 2
#define EE_ASSERT_ALREADYUSED 3

unsigned short enable_interrupt = 0; 
unsigned short shutdown = 0;

void EE_hal_enableIRQ(){
     enable_interrupt =  1;
}


EE_TYPEASSERTVALUE EE_assert(EE_TYPEASSERT id,
                             int test,
                             EE_TYPEASSERT prev)
{
  /* I can use id into an assertion only once */
  if (EE_assertions[id] != EE_ASSERT_INITVALUE) {
    EE_assertions[id] = EE_ASSERT_ALREADYUSED;
    return EE_ASSERT_ALREADYUSED;
  }

  if (test) {
    if (prev != EE_ASSERT_NIL) {
      if (EE_assertions[prev] == EE_ASSERT_YES) {
        /* test is true and the prev assertion is YES */
        EE_assertions[id] = EE_ASSERT_YES;
        return EE_ASSERT_YES;
      }
      else {
        /* test is true but the prev assertion is != YES */
        EE_assertions[id] = EE_ASSERT_NO;
        return EE_ASSERT_NO;
      }
    } else {
      /* test is true and prev is EE_ASSERT_NIL */
      EE_assertions[id] = EE_ASSERT_YES;
      return EE_ASSERT_YES;
    }
  } else {
    /* test is false */
    EE_assertions[id] = EE_ASSERT_NO;
    return EE_ASSERT_NO;
  }
}

EE_TYPEASSERTVALUE EE_assert_range(EE_TYPEASSERT id,
                                   EE_TYPEASSERT begin,
                                   EE_TYPEASSERT end)
{
  EE_TYPEASSERT i;

  for (i=begin; i<=end; i++) {
    if (EE_assertions[i] != EE_ASSERT_YES) {
      EE_assertions[id] = EE_ASSERT_NO;
      return EE_ASSERT_NO;
    }
  }

  EE_assertions[id] = EE_ASSERT_YES;
  return EE_ASSERT_YES;
}

EE_TYPEASSERTVALUE EE_assert_last(void)
{
  return EE_assertions[0];
}

void EE_hal_shutdown_system(){
    shutdown = 1;
}

