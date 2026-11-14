#pragma once

#define JOB_PARAMS 0x25100000u
#define JOB_CODE 0x25101000u
#define SUP_END 0x25300000u

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef struct {
  uint32_t Command;
  uint32_t Done;
  uint32_t File;
  uint32_t Entry;
  uint32_t Result;
} JobParamsType;

enum { OK, NOT_OK };

enum {
  MAP_USER = 1,
  MAP_PRIV = 2,
  MAP_DEVICE = 4,
  MAP_UNCACHED = 8,
  MAP_RO = 16
};
#define MAP_ALL MAP

static const JobParams *Job = (JobParamsType *)JOB_PARAMS;

void SupLoadProgram();

void mmu_reset(void);
bool mmu_map(u32 va, u32 pa, u32 size, u32 flags);
void mmu_unmap(u32 va, u32 size);
u32 mmu_pa(u32 va);
bool mmu_user_write_ok(u32 va, u32 size);
bool mmu_user_ok(u32 va, u32 size);
void mmu_enter(void);
void mmu_leave(void);
bool mmu_base(void);
void mmu_tables_on(void);
