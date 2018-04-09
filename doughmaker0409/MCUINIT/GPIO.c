#include "MCUInit.h"

void GPIO(void)
{
  P0_1_set_mode(OUTPUT_PP_GP);
  P0_2_set_mode(OUTPUT_PP_GP);
  P0_2_set();
}
