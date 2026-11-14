#pragma once

#define JOB_PARAMS 0x25100000u
#define JOB_CODE 0x25101000u
#define SUP_END 0x25300000u

enum {
  OK,
  NOT_OK
}

typedef struct {
  uint32_t Command;
  uint32_t Done;
  uint32_t File;
  uint32_t Entry;
  uint32_t Result;
} JobParamsType;

static const JobParams *Job = (JobParamsType *)JOB_PARAMS;
