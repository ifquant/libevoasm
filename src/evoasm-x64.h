#pragma once

#include <stdint.h>
#include "evoasm-error.h"
#include "evoasm-alloc.h"
#include "evoasm-arch.h"

#include "gen/evoasm-x64-enums.h"

typedef struct {
  evoasm_arch_t base;
  uint64_t features;
} evoasm_x64_t;

typedef struct {
  unsigned acc_r: 1;
  unsigned acc_w: 1;
  unsigned acc_u: 1;
  unsigned acc_c: 1;
  unsigned implicit: 1;
  unsigned param_idx: 5;
  unsigned type: EVOASM_X64_OPERAND_TYPE_BITSIZE;
  unsigned size: EVOASM_OPERAND_SIZE_BITSIZE_WITH_N;
  unsigned reg_id: EVOASM_X64_REG_BITSIZE_WITH_N;
  unsigned reg_type: EVOASM_X64_REG_TYPE_BITSIZE_WITH_N;
  unsigned acc_w_mask: EVOASM_X64_BIT_MASK_BITSIZE;
} evoasm_x64_operand_t;

typedef bool (*evoasm_x64_inst_enc_func_t)(evoasm_x64_t *x64, evoasm_arch_param_val_t *param_vals,
                                           evoasm_bitmap_t *set_params);

typedef struct {
  EVOASM_INST_HEADER
  evoasm_x64_inst_enc_func_t enc_func;
  uint64_t features;
  evoasm_x64_operand_t *operands;
  uint8_t n_operands;
  uint32_t exceptions;
  uint32_t flags;
} evoasm_x64_inst_t;

#define EVOASM_X64_ENC(inst) \
  EVOASM_TRY(enc_failed, evoasm_x64_##inst, x64, params.vals, (evoasm_bitmap_t *) &params.set)

#define EVOASM_X64_SET(param, val) \
  evoasm_arch_params_set(params.vals, (evoasm_bitmap_t *) &params.set, param, val)

#define EVOASM_X64_UNSET(param) \
  evoasm_arch_params_unset(params.vals, (evoasm_bitmap_t *) &params.set, param)

typedef enum {
  EVOASM_X64_ABI_SYSV
} evoasm_x64_abi_t;


static inline evoasm_success_t
evoasm_x64_inst_enc(const evoasm_x64_inst_t *inst, evoasm_x64_t *x64,
                    evoasm_arch_param_val_t *param_vals, evoasm_bitmap_t *set_params) {
  return inst->enc_func(x64, param_vals, set_params);
}

extern const evoasm_x64_inst_t *_EVOASM_X64_INSTS_VAR_NAME;

static inline const evoasm_x64_inst_t *
evoasm_x64_inst(unsigned index) {
  return &_EVOASM_X64_INSTS_VAR_NAME[index];
}

static inline evoasm_success_t
evoasm_x64_enc(evoasm_x64_t *x64, evoasm_inst_id_t inst_id, evoasm_arch_param_val_t *param_vals, evoasm_bitmap_t *set_params) {
  const evoasm_x64_inst_t *inst = &_EVOASM_X64_INSTS_VAR_NAME[inst_id];
  return evoasm_x64_inst_enc(inst, x64, param_vals, set_params);
}

#include "gen/evoasm-x64.h"

typedef struct {
  _EVOASM_ARCH_PARAMS_HEADER
  evoasm_arch_param_val_t vals[EVOASM_X64_N_PARAMS];
} evoasm_x64_params_t;

_Static_assert(EVOASM_X64_N_PARAMS <= EVOASM_ARCH_MAX_PARAMS,
                "Too much parameters. Redeclar EVOASM_ARCH_MAX_PARAMS and evoasm_arch_params_bitmap.");



void
evoasm_x64_destroy(evoasm_x64_t *x64);

evoasm_success_t
evoasm_x64_init(evoasm_x64_t *x64);

evoasm_success_t
evoasm_x64_func_prolog(evoasm_x64_t *x64, evoasm_buf_t *buf, evoasm_x64_abi_t abi);

evoasm_success_t
evoasm_x64_func_epilog(evoasm_x64_t *x64, evoasm_buf_t *buf, evoasm_x64_abi_t abi);
