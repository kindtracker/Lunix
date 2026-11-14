#include "Sup.h"

static inline void memory_barrier(void) {
  uint32_t Zero = 0;
  __asm__ volatile("mcr p15, 0, %0, c7, c10, 5" : : "r"(Zero) : "memory");
}

void job_main(void *Argument) {
  volatile JobParamsType *Job = (volatile JobParamsType *)Argument;

  while (1) {
    if (Job->Command == JOB_IDLE) {
      memory_barrier();
      continue;
    }

    if (Job->Command == JOB_LOAD_ELF) {
      Job->Done = false;
      SupLoadProgram();
    }

    memory_barrier();
    Job->Done = true;
    memory_barrier();
    Job->Command = JOB_IDLE;
  }
}
