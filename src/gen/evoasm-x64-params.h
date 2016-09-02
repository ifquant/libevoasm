/* vim: set filetype=c: */
/* AUTOGENERATED FILE, DO NOT EDIT */

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "evoasm-x64-params.h"

typedef struct {
  uint64_t addr_size : 1;
  uint64_t disp_set : 1;
  uint64_t disp_size : 1;
  uint64_t disp_size_set : 1;
  uint64_t force_disp32 : 1;
  uint64_t force_long_vex : 1;
  uint64_t force_rex : 1;
  uint64_t force_sib : 1;
  uint64_t lock : 1;
  uint64_t reg0_high_byte : 1;
  uint64_t reg0_set : 1;
  uint64_t reg1_high_byte : 1;
  uint64_t reg1_set : 1;
  uint64_t reg_base_set : 1;
  uint64_t reg_index_set : 1;
  uint64_t rex_b : 1;
  uint64_t rex_r : 1;
  uint64_t rex_w : 1;
  uint64_t rex_x : 1;
  uint64_t vex_l : 1;
  uint64_t scale : 2;
  uint64_t legacy_prefix_order : 3;
  uint64_t modrm_reg : 3;
  uint64_t vex_v : 4;
  uint64_t reg0 : 7;
  uint64_t reg1 : 7;
  uint64_t reg2 : 7;
  uint64_t reg3 : 7;
  uint64_t reg_base : 7;
  uint64_t reg_index : 7;
  uint64_t disp : 32;
  uint64_t imm : 64;
} evoasm_x64_params_t;

typedef struct {
  uint64_t reg0_high_byte : 1;
  uint64_t reg1_high_byte : 1;
  uint64_t reg0 : 7;
  uint64_t reg1 : 7;
  uint64_t reg2 : 7;
  uint64_t reg3 : 7;
  uint64_t imm : 32;
} evoasm_x64_basic_params_t;

static inline void _evoasm_x64_params_set(evoasm_x64_params_t * params, evoasm_x64_param_id_t param, int64_t param_val) {
  switch(param) {
    case EVOASM_X64_PARAM_REG0:
      params->reg0 = ((uint64_t) param_val) & 0x7f;
      params->reg0_set = true;
      break;
    case EVOASM_X64_PARAM_REG1:
      params->reg1 = ((uint64_t) param_val) & 0x7f;
      params->reg1_set = true;
      break;
    case EVOASM_X64_PARAM_REG2:
      params->reg2 = ((uint64_t) param_val) & 0x7f;
      break;
    case EVOASM_X64_PARAM_REG3:
      params->reg3 = ((uint64_t) param_val) & 0x7f;
      break;
    case EVOASM_X64_PARAM_IMM:
      params->imm = ((uint64_t) param_val) & 0xffffffffffffffff;
      break;
    case EVOASM_X64_PARAM_FORCE_REX:
      params->force_rex = ((uint64_t) param_val) & 0x1;
      break;
    case EVOASM_X64_PARAM_REX_R:
      params->rex_r = ((uint64_t) param_val) & 0x1;
      break;
    case EVOASM_X64_PARAM_REX_X:
      params->rex_x = ((uint64_t) param_val) & 0x1;
      break;
    case EVOASM_X64_PARAM_REX_B:
      params->rex_b = ((uint64_t) param_val) & 0x1;
      break;
    case EVOASM_X64_PARAM_LEGACY_PREFIX_ORDER:
      params->legacy_prefix_order = ((uint64_t) param_val) & 0x7;
      break;
    case EVOASM_X64_PARAM_LOCK:
      params->lock = ((uint64_t) param_val) & 0x1;
      break;
    case EVOASM_X64_PARAM_ADDR_SIZE:
      params->addr_size = ((uint64_t) param_val) & 0x1;
      break;
    case EVOASM_X64_PARAM_REG_BASE:
      params->reg_base = ((uint64_t) param_val) & 0x7f;
      params->reg_base_set = true;
      break;
    case EVOASM_X64_PARAM_REG_INDEX:
      params->reg_index = ((uint64_t) param_val) & 0x7f;
      params->reg_index_set = true;
      break;
    case EVOASM_X64_PARAM_REX_W:
      params->rex_w = ((uint64_t) param_val) & 0x1;
      break;
    case EVOASM_X64_PARAM_DISP:
      params->disp = ((uint64_t) param_val) & 0xffffffff;
      params->disp_set = true;
      break;
    case EVOASM_X64_PARAM_SCALE:
      params->scale = ((uint64_t) param_val) & 0x3;
      break;
    case EVOASM_X64_PARAM_FORCE_SIB:
      params->force_sib = ((uint64_t) param_val) & 0x1;
      break;
    case EVOASM_X64_PARAM_DISP_SIZE:
      params->disp_size = ((uint64_t) param_val) & 0x1;
      params->disp_size_set = true;
      break;
    case EVOASM_X64_PARAM_FORCE_DISP32:
      params->force_disp32 = ((uint64_t) param_val) & 0x1;
      break;
    case EVOASM_X64_PARAM_REG0_HIGH_BYTE:
      params->reg0_high_byte = ((uint64_t) param_val) & 0x1;
      break;
    case EVOASM_X64_PARAM_REG1_HIGH_BYTE:
      params->reg1_high_byte = ((uint64_t) param_val) & 0x1;
      break;
    case EVOASM_X64_PARAM_FORCE_LONG_VEX:
      params->force_long_vex = ((uint64_t) param_val) & 0x1;
      break;
    case EVOASM_X64_PARAM_VEX_L:
      params->vex_l = ((uint64_t) param_val) & 0x1;
      break;
    case EVOASM_X64_PARAM_MODRM_REG:
      params->modrm_reg = ((uint64_t) param_val) & 0x7;
      break;
    case EVOASM_X64_PARAM_VEX_V:
      params->vex_v = ((uint64_t) param_val) & 0xf;
      break;
    default:
      evoasm_assert_not_reached();
  }
}

static inline void _evoasm_x64_basic_params_set(evoasm_x64_basic_params_t * params, evoasm_x64_param_id_t param, int64_t param_val) {
  switch(param) {
    case EVOASM_X64_PARAM_REG0:
      params->reg0 = ((uint64_t) param_val) & 0x7f;
      break;
    case EVOASM_X64_PARAM_REG1:
      params->reg1 = ((uint64_t) param_val) & 0x7f;
      break;
    case EVOASM_X64_PARAM_REG2:
      params->reg2 = ((uint64_t) param_val) & 0x7f;
      break;
    case EVOASM_X64_PARAM_REG3:
      params->reg3 = ((uint64_t) param_val) & 0x7f;
      break;
    case EVOASM_X64_PARAM_IMM:
      params->imm = ((uint64_t) param_val) & 0xffffffff;
      break;
    case EVOASM_X64_PARAM_REG0_HIGH_BYTE:
      params->reg0_high_byte = ((uint64_t) param_val) & 0x1;
      break;
    case EVOASM_X64_PARAM_REG1_HIGH_BYTE:
      params->reg1_high_byte = ((uint64_t) param_val) & 0x1;
      break;
    default:
      evoasm_assert_not_reached();
  }
}
static inline int64_t _evoasm_x64_params_get(evoasm_x64_params_t * params, evoasm_x64_param_id_t param) {
  switch(param) {
    case EVOASM_X64_PARAM_REG0:
      return (int64_t) params->reg0;
    case EVOASM_X64_PARAM_REG1:
      return (int64_t) params->reg1;
    case EVOASM_X64_PARAM_REG2:
      return (int64_t) params->reg2;
    case EVOASM_X64_PARAM_REG3:
      return (int64_t) params->reg3;
    case EVOASM_X64_PARAM_IMM:
      return (int64_t) params->imm;
    case EVOASM_X64_PARAM_FORCE_REX:
      return (int64_t) params->force_rex;
    case EVOASM_X64_PARAM_REX_R:
      return (int64_t) params->rex_r;
    case EVOASM_X64_PARAM_REX_X:
      return (int64_t) params->rex_x;
    case EVOASM_X64_PARAM_REX_B:
      return (int64_t) params->rex_b;
    case EVOASM_X64_PARAM_LEGACY_PREFIX_ORDER:
      return (int64_t) params->legacy_prefix_order;
    case EVOASM_X64_PARAM_LOCK:
      return (int64_t) params->lock;
    case EVOASM_X64_PARAM_ADDR_SIZE:
      return (int64_t) params->addr_size;
    case EVOASM_X64_PARAM_REG_BASE:
      return (int64_t) params->reg_base;
    case EVOASM_X64_PARAM_REG_INDEX:
      return (int64_t) params->reg_index;
    case EVOASM_X64_PARAM_REX_W:
      return (int64_t) params->rex_w;
    case EVOASM_X64_PARAM_DISP:
      return (int64_t) params->disp;
    case EVOASM_X64_PARAM_SCALE:
      return (int64_t) params->scale;
    case EVOASM_X64_PARAM_FORCE_SIB:
      return (int64_t) params->force_sib;
    case EVOASM_X64_PARAM_DISP_SIZE:
      return (int64_t) params->disp_size;
    case EVOASM_X64_PARAM_FORCE_DISP32:
      return (int64_t) params->force_disp32;
    case EVOASM_X64_PARAM_REG0_HIGH_BYTE:
      return (int64_t) params->reg0_high_byte;
    case EVOASM_X64_PARAM_REG1_HIGH_BYTE:
      return (int64_t) params->reg1_high_byte;
    case EVOASM_X64_PARAM_FORCE_LONG_VEX:
      return (int64_t) params->force_long_vex;
    case EVOASM_X64_PARAM_VEX_L:
      return (int64_t) params->vex_l;
    case EVOASM_X64_PARAM_MODRM_REG:
      return (int64_t) params->modrm_reg;
    case EVOASM_X64_PARAM_VEX_V:
      return (int64_t) params->vex_v;
    default:
      evoasm_assert_not_reached();
  }
}

static inline int64_t _evoasm_x64_basic_params_get(evoasm_x64_basic_params_t * params, evoasm_x64_param_id_t param) {
  switch(param) {
    case EVOASM_X64_PARAM_REG0:
      return (int64_t) params->reg0;
    case EVOASM_X64_PARAM_REG1:
      return (int64_t) params->reg1;
    case EVOASM_X64_PARAM_REG2:
      return (int64_t) params->reg2;
    case EVOASM_X64_PARAM_REG3:
      return (int64_t) params->reg3;
    case EVOASM_X64_PARAM_IMM:
      return (int64_t) params->imm;
    case EVOASM_X64_PARAM_REG0_HIGH_BYTE:
      return (int64_t) params->reg0_high_byte;
    case EVOASM_X64_PARAM_REG1_HIGH_BYTE:
      return (int64_t) params->reg1_high_byte;
    default:
      evoasm_assert_not_reached();
  }
}
static inline void _evoasm_x64_params_unset(evoasm_x64_params_t * params, evoasm_x64_param_id_t param) {
  switch(param) {
    case EVOASM_X64_PARAM_REG0:
      params->reg0 = 0;
      params->reg0_set = false;
      break;
    case EVOASM_X64_PARAM_REG1:
      params->reg1 = 0;
      params->reg1_set = false;
      break;
    case EVOASM_X64_PARAM_REG2:
      params->reg2 = 0;
      break;
    case EVOASM_X64_PARAM_REG3:
      params->reg3 = 0;
      break;
    case EVOASM_X64_PARAM_IMM:
      params->imm = 0;
      break;
    case EVOASM_X64_PARAM_FORCE_REX:
      params->force_rex = 0;
      break;
    case EVOASM_X64_PARAM_REX_R:
      params->rex_r = 0;
      break;
    case EVOASM_X64_PARAM_REX_X:
      params->rex_x = 0;
      break;
    case EVOASM_X64_PARAM_REX_B:
      params->rex_b = 0;
      break;
    case EVOASM_X64_PARAM_LEGACY_PREFIX_ORDER:
      params->legacy_prefix_order = 0;
      break;
    case EVOASM_X64_PARAM_LOCK:
      params->lock = 0;
      break;
    case EVOASM_X64_PARAM_ADDR_SIZE:
      params->addr_size = 0;
      break;
    case EVOASM_X64_PARAM_REG_BASE:
      params->reg_base = 0;
      params->reg_base_set = false;
      break;
    case EVOASM_X64_PARAM_REG_INDEX:
      params->reg_index = 0;
      params->reg_index_set = false;
      break;
    case EVOASM_X64_PARAM_REX_W:
      params->rex_w = 0;
      break;
    case EVOASM_X64_PARAM_DISP:
      params->disp = 0;
      params->disp_set = false;
      break;
    case EVOASM_X64_PARAM_SCALE:
      params->scale = 0;
      break;
    case EVOASM_X64_PARAM_FORCE_SIB:
      params->force_sib = 0;
      break;
    case EVOASM_X64_PARAM_DISP_SIZE:
      params->disp_size = 0;
      params->disp_size_set = false;
      break;
    case EVOASM_X64_PARAM_FORCE_DISP32:
      params->force_disp32 = 0;
      break;
    case EVOASM_X64_PARAM_REG0_HIGH_BYTE:
      params->reg0_high_byte = 0;
      break;
    case EVOASM_X64_PARAM_REG1_HIGH_BYTE:
      params->reg1_high_byte = 0;
      break;
    case EVOASM_X64_PARAM_FORCE_LONG_VEX:
      params->force_long_vex = 0;
      break;
    case EVOASM_X64_PARAM_VEX_L:
      params->vex_l = 0;
      break;
    case EVOASM_X64_PARAM_MODRM_REG:
      params->modrm_reg = 0;
      break;
    case EVOASM_X64_PARAM_VEX_V:
      params->vex_v = 0;
      break;
    default:
      evoasm_assert_not_reached();
  }
}

static inline void _evoasm_x64_basic_params_unset(evoasm_x64_basic_params_t * params, evoasm_x64_param_id_t param) {
  switch(param) {
    case EVOASM_X64_PARAM_REG0:
      params->reg0 = 0;
      break;
    case EVOASM_X64_PARAM_REG1:
      params->reg1 = 0;
      break;
    case EVOASM_X64_PARAM_REG2:
      params->reg2 = 0;
      break;
    case EVOASM_X64_PARAM_REG3:
      params->reg3 = 0;
      break;
    case EVOASM_X64_PARAM_IMM:
      params->imm = 0;
      break;
    case EVOASM_X64_PARAM_REG0_HIGH_BYTE:
      params->reg0_high_byte = 0;
      break;
    case EVOASM_X64_PARAM_REG1_HIGH_BYTE:
      params->reg1_high_byte = 0;
      break;
    default:
      evoasm_assert_not_reached();
  }
}

