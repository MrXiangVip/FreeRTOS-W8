
#if RTVISION_BOARD

#include "platform_disp.h"

typedef enum{
  COMMAND = 1,
  DATA = 2
} DC;

void ILI_init (Gpu_Hal_Context_t *host);

#endif
