#include "Sup.h"

void job_main(void) {
  if (Job->Command == JOB_LOAD_ELF) {
    SupLoadProgram();
  }

  Job->Done = true;
}
