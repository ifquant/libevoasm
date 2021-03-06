/*
 * Copyright (C) 2016 Julian Aron Prenner
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "evoasm-pop.h"
#include "evoasm-signal.h"
#include "evoasm-util.h"
#include "evoasm-program.h"
#include "evoasm-error.h"

#ifdef _OPENMP

#  include <omp.h>

#endif

#include <gen/evoasm-x64-params.h>

EVOASM_DEF_LOG_TAG("pop")

#define EVOASM_DEME_LOSS_OFF(deme, indiv_idx) (indiv_idx)

#define EVOASM_DEME_N_TOPOLOGY_EDGES_PER_PROGAM_(topology_size) ((size_t)(1 * topology_size))
#define EVOASM_DEME_N_TOPOLOGY_EDGES_PER_PROGAM(deme) EVOASM_DEME_N_TOPOLOGY_EDGES_PER_PROGAM_(deme->params->topology_size)

#define EVOASM_DEME_TOPOLOGY_EDGE_OFF_(topology_size, topology_idx, edge_idx) \
  (3u * (((topology_idx) * EVOASM_DEME_N_TOPOLOGY_EDGES_PER_PROGAM_(topology_size)) + (edge_idx)))

#define EVOASM_DEME_TOPOLOGY_DEFAULT_EDGE_OFF_(topology_size, topology_idx, kernel_idx) \
  (((topology_idx) * EVOASM_PROGRAM_TOPOLOGY_MAX_SIZE) + (kernel_idx))

#define EVOASM_DEME_TOPOLOGY_KERNEL_IDX_OFF_(topology_size, topology_idx, pos) \
  (((topology_idx) * (topology_size)) + (pos))

#define EVOASM_DEME_TOPOLOGY_KERNEL_IDX_OFF(deme, topology_idx, pos) \
  EVOASM_DEME_TOPOLOGY_KERNEL_IDX_OFF_(deme->params->topology_size, topology_idx, pos)

#define EVOASM_DEME_TOPOLOGY_EDGE_OFF(deme, topology_idx, edge_idx) \
  EVOASM_DEME_TOPOLOGY_EDGE_OFF_((deme)->pop->params->topology_size, topology_idx, edge_idx)

#define EVOASM_DEME_TOPOLOGY_DEFAULT_EDGE_OFF(deme, topology_idx, kernel_idx) \
  EVOASM_DEME_TOPOLOGY_DEFAULT_EDGE_OFF_((deme)->pop->params->topology_size, topology_idx, kernel_idx)

#define EVOASM_DEME_KERNEL_INST_OFF_(kernel_size, kernel_idx, inst_idx) \
  (size_t)((kernel_idx) * (kernel_size) + (inst_idx))

#define EVOASM_DEME_KERNEL_INST_OFF(deme, kernel_idx, inst_idx) \
  EVOASM_DEME_KERNEL_INST_OFF_((deme)->pop->params->kernel_size, kernel_idx, inst_idx)

#define EVOASM_DEME_N_KERNELS(deme) ((deme)->pop->params->topology_size * (deme)->pop->params->deme_size)

static evoasm_success_t
evoasm_deme_loss_data_init(evoasm_deme_loss_data_t *loss_data, size_t n_indivs) {
  EVOASM_TRY_ALLOC(error, aligned_calloc, loss_data->samples, EVOASM_CACHE_LINE_SIZE,
                   n_indivs,
                   sizeof(evoasm_loss_t));
  return true;
error:
  return false;
}

static void
evoasm_deme_loss_data_destroy(evoasm_deme_loss_data_t *loss_data) {
  evoasm_free(loss_data->samples);
}


static evoasm_success_t
evoasm_deme_topology_data_init(evoasm_deme_topology_data_t *topology_data, size_t n_topologies, size_t topology_size) {

  EVOASM_TRY_ALLOC(error, aligned_calloc, topology_data->default_succs, EVOASM_CACHE_LINE_SIZE,
                   n_topologies * EVOASM_PROGRAM_TOPOLOGY_MAX_SIZE,
                   sizeof(topology_data->default_succs[0]));

  EVOASM_TRY_ALLOC(error, aligned_calloc, topology_data->edges, EVOASM_CACHE_LINE_SIZE,
                   n_topologies * EVOASM_DEME_N_TOPOLOGY_EDGES_PER_PROGAM_(topology_size) * 3,
                   sizeof(topology_data->edges[0]));
  return true;

error:
  return false;
}

static void
evoasm_deme_topology_data_destroy(evoasm_deme_topology_data_t *topology_data) {
  evoasm_free(topology_data->default_succs);
  evoasm_free(topology_data->edges);
}

static evoasm_success_t
evoasm_deme_kernel_data_init(evoasm_deme_kernel_data_t *kernel_data,
                             evoasm_arch_id_t arch_id,
                             size_t n_kernels,
                             size_t kernel_size) {


  size_t n_insts = n_kernels * kernel_size;

  EVOASM_TRY_ALLOC(error, aligned_calloc, kernel_data->insts, EVOASM_CACHE_LINE_SIZE,
                   n_insts,
                   sizeof(evoasm_inst_id_t));

  switch(arch_id) {
    case EVOASM_ARCH_X64:
      EVOASM_TRY_ALLOC(error, aligned_calloc, kernel_data->params.x64, EVOASM_CACHE_LINE_SIZE,
                       n_insts,
                       sizeof(evoasm_x64_basic_params_t));
      break;
    default:
      evoasm_assert_not_reached();
  }
  return true;

error:
  return false;
}

static void
evoasm_deme_kernel_data_destroy(evoasm_deme_kernel_data_t *kernel_data) {
  evoasm_free(kernel_data->insts);
  evoasm_free(kernel_data->params.data);
}

static evoasm_success_t
evoasm_pop_module_data_init(evoasm_pop_module_data_t *module_data, size_t n) {
//  EVOASM_TRY(error, evoasm_deme_topology_data_init, &module_data->topology_data, n);
//
//  EVOASM_TRY_ALLOC(error, aligned_calloc, module_data->sizes, EVOASM_CACHE_LINE_SIZE,
//                   n,
//                   sizeof(uint16_t));
//  EVOASM_TRY_ALLOC(error, aligned_calloc, module_data->pheromones, EVOASM_CACHE_LINE_SIZE,
//                   n,
//                   sizeof(float));
  return true;
error:
  return false;
}

static void
evoasm_pop_module_data_destroy(evoasm_pop_module_data_t *module_data) {
  evoasm_deme_topology_data_destroy(&module_data->topology_data);
  evoasm_free(module_data->pheromones);
  evoasm_free(module_data->sizes);
}

static void
evoasm_deme_destroy(evoasm_deme_t *deme) {
  EVOASM_TRY_WARN(evoasm_program_destroy, &deme->program);
  evoasm_prng_destroy(&deme->prng);
  evoasm_free(deme->surviv_counters);
  evoasm_free(deme->immig_idxs);
  evoasm_free(deme->error_counters);

  evoasm_deme_kernel_data_destroy(&deme->kernel_data);
  evoasm_deme_loss_data_destroy(&deme->loss_data);

  evoasm_deme_kernel_data_destroy(&deme->best_kernel_data);

  if(deme->params->topology_size > 1) {
    evoasm_deme_topology_data_destroy(&deme->best_topology_data);
    evoasm_deme_topology_data_destroy(&deme->topology_data);
  }
}

void
evoasm_pop_destroy(evoasm_pop_t *pop) {
  evoasm_free(pop->domains);

  for(size_t i = 0; i < pop->n_demes; i++) {
    evoasm_deme_destroy(&pop->demes[i]);
  }
  evoasm_free(pop->demes);
  evoasm_free(pop->summary_losses);

  evoasm_pop_module_data_destroy(&pop->module_data);
}

#define EVOASM_DEME_MIN_MUT_RATE 0.008f
#define EVOASM_DEME_MAX_MUT_RATE 0.15f

static evoasm_success_t
evoasm_deme_init(evoasm_deme_t *deme,
                 evoasm_pop_t *pop,
                 size_t deme_idx,
                 evoasm_arch_id_t arch_id,
                 const evoasm_prng_state_t *seed) {

  static evoasm_deme_t zero_deme = {0};
  const evoasm_pop_params_t *params = pop->params;
  size_t n_examples = evoasm_program_io_get_n_tuples(params->program_input);

  *deme = zero_deme;
  deme->idx = (uint16_t) deme_idx;

  deme->pop = pop;
  deme->params = pop->params;
  deme->domains = pop->domains;
  deme->arch_id = arch_id;
  deme->mut_rate = EVOASM_DEME_MIN_MUT_RATE;

  if(n_examples > deme->params->example_win_size) {
    deme->example_win_off = (uint16_t) (n_examples / params->n_demes * deme_idx);
  }

  evoasm_prng_init(&deme->prng, seed);

  EVOASM_TRY_ALLOC_N(error, aligned_calloc, deme->surviv_counters, EVOASM_CACHE_LINE_SIZE, params->deme_size);
  EVOASM_TRY_ALLOC_N(error, aligned_calloc, deme->immig_idxs, EVOASM_CACHE_LINE_SIZE, params->deme_size);

  EVOASM_TRY(error, evoasm_program_init, &deme->program,
             evoasm_get_arch_info(arch_id),
             params->topology_size,
             params->kernel_size,
             n_examples,
             params->recur_limit,
             true);

  if(params->topology_size > 1) {
    EVOASM_TRY(error, evoasm_deme_topology_data_init, &deme->topology_data,
               params->deme_size, params->kernel_size);

    EVOASM_TRY(error, evoasm_deme_topology_data_init, &deme->best_topology_data, 1u, params->topology_size);
  }

  EVOASM_TRY(error, evoasm_deme_kernel_data_init, &deme->kernel_data, arch_id,
             (size_t) params->topology_size * params->deme_size, params->kernel_size);

  EVOASM_TRY(error, evoasm_deme_loss_data_init, &deme->loss_data, deme->params->deme_size);

  EVOASM_TRY(error, evoasm_deme_kernel_data_init, &deme->best_kernel_data, arch_id,
             params->topology_size, params->kernel_size);

  deme->best_loss = INFINITY;
  deme->top_loss = INFINITY;

  EVOASM_TRY_ALLOC(error, aligned_calloc, deme->error_counters, (size_t) n_examples, EVOASM_CACHE_LINE_SIZE,
                   sizeof(uint64_t));

  return true;

error:
  return false;
}

evoasm_success_t
evoasm_pop_init_domains(evoasm_pop_t *pop) {
  size_t i, j, k;
  evoasm_domain_t cloned_domain;

  const evoasm_pop_params_t *params = pop->params;

  size_t domains_len = (size_t) (params->n_insts * params->n_params);
  pop->domains = evoasm_calloc(domains_len,
                               sizeof(evoasm_domain_t));

  if(!pop->domains) goto fail;

  for(i = 0; i < params->n_insts; i++) {
    evoasm_x64_inst_t *inst = evoasm_x64_inst_((evoasm_x64_inst_id_t) params->inst_ids[i]);
    for(j = 0; j < params->n_params; j++) {
      evoasm_domain_t *inst_domain = &pop->domains[i * params->n_params + j];
      evoasm_param_id_t param_id = params->param_ids[j];
      for(k = 0; k < inst->n_params; k++) {
        evoasm_param_t *param = &inst->params[k];
        if(param->id == param_id) {
          evoasm_domain_t *user_domain = params->domains[param_id];
          if(user_domain != NULL) {
            if(evoasm_domain_is_empty(user_domain)) goto empty_domain;

            evoasm_domain_clone(user_domain, &cloned_domain);
            evoasm_domain_intersect(&cloned_domain, param->domain, inst_domain);
            if(evoasm_domain_is_empty(inst_domain)) goto empty_domain;
          } else {
            evoasm_domain_clone(param->domain, inst_domain);
          }
          goto found;
        }
      }
      /* not found */
      inst_domain->type = EVOASM_DOMAIN_TYPE_NONE;
found:;
    }
  }

  /*
  for(i = 0; i < domains_len; i++) {
    evoasm_domain_log(&pop->domains[i], EVOASM_LOG_LEVEL_WARN);
  }*/

  return true;

fail:
  return false;

empty_domain:
  evoasm_error(EVOASM_ERROR_TYPE_POP, EVOASM_ERROR_CODE_NONE,
               "Empty domain");
  return false;
}

evoasm_success_t
evoasm_pop_init(evoasm_pop_t *pop,
                evoasm_arch_id_t arch_id,
                const evoasm_pop_params_t *params) {
  static evoasm_pop_t zero_pop = {0};
  evoasm_prng_t seed_prng;

  *pop = zero_pop;

  if(!evoasm_pop_params_validate((evoasm_pop_params_t *) params)) goto error;

  pop->params = params;
  pop->n_demes = params->n_demes;

#ifdef _OPENMP
  {
    int max_threads;
    max_threads = omp_get_max_threads();
    omp_set_dynamic(0);
    int n_threads = EVOASM_MIN(max_threads, pop->n_demes);
    omp_set_num_threads(n_threads);
    evoasm_log_info("Using OpenMP with %d threads", n_threads);
  }
#endif

  evoasm_prng_init(&seed_prng, &params->seed);

  EVOASM_TRY(error, evoasm_pop_init_domains, pop);
  EVOASM_TRY(error, evoasm_pop_module_data_init, &pop->module_data, params->library_size);

  EVOASM_TRY_ALLOC(error, aligned_calloc, pop->demes, EVOASM_CACHE_LINE_SIZE, (size_t) pop->n_demes,
                   sizeof(evoasm_deme_t));

  for(size_t i = 0; i < pop->n_demes; i++) {
    evoasm_prng_state_t seed;

    for(size_t j = 0; j < EVOASM_PRNG_SEED_LEN; j++) {
      seed.data[j] = evoasm_prng_rand64_(&seed_prng);
    }

    EVOASM_TRY(error, evoasm_deme_init,
               &pop->demes[i],
               pop,
               i,
               arch_id,
               &seed);
  }

  return true;

error:
  evoasm_pop_destroy(pop);
  return false;
}

static void
evoasm_deme_seed_kernel_param_x64(evoasm_deme_t *deme, size_t kernel_idx, evoasm_inst_id_t *inst_id_ptr,
                                  evoasm_x64_basic_params_t *params_ptr) {
  const evoasm_pop_params_t *params = deme->params;
  size_t n_params = params->n_params;
  evoasm_prng_t *prng = &deme->prng;
  size_t inst_idx;

  if(deme->params->kernel_size > 1) {
    inst_idx = (size_t) evoasm_prng_rand_between_(prng, 0, params->n_insts);
  } else {
    inst_idx = ((size_t) deme->idx * deme->params->deme_size + kernel_idx) % params->n_insts;
  }

  evoasm_inst_id_t inst_id = params->inst_ids[inst_idx];
  *inst_id_ptr = inst_id;

  /* set parameters */
  for(size_t i = 0; i < n_params; i++) {
    evoasm_domain_t *domain = &deme->domains[inst_idx * n_params + i];
    if(domain->type < EVOASM_DOMAIN_TYPE_NONE) {
      evoasm_x64_basic_param_id_t param_id = (evoasm_x64_basic_param_id_t) deme->params->param_ids[i];
      evoasm_param_val_t param_val;

      param_val = (int64_t) evoasm_domain_rand_(domain, prng);
      evoasm_x64_basic_params_set_(params_ptr, param_id, param_val);
    }
  }
}

static void
evoasm_deme_seed_kernel_inst(evoasm_deme_t *deme,
                             size_t kernel_idx,
                             size_t inst_idx) {

  size_t kernel_inst_off = EVOASM_DEME_KERNEL_INST_OFF(deme, kernel_idx, inst_idx);

  evoasm_deme_kernel_data_t *kernel_data = &deme->kernel_data;
  evoasm_inst_id_t *insts_ptr = &kernel_data->insts[kernel_inst_off];
//  fprintf(stderr, "OFF: %zu (K:%zu, I:%zu)\n", kernel_inst_off, kernel_idx, inst_idx);

  switch(deme->arch_id) {
    case EVOASM_ARCH_X64: {
      evoasm_x64_basic_params_t *params_ptr = &kernel_data->params.x64[kernel_inst_off];
      evoasm_deme_seed_kernel_param_x64(deme, kernel_idx, insts_ptr, params_ptr);
      break;
    }
    default:
      evoasm_assert_not_reached();
  }
}


static void
evoasm_deme_mutate_kernel_inst(evoasm_deme_t *deme,
                               size_t kernel_idx,
                               size_t inst_idx) {
  evoasm_deme_seed_kernel_inst(deme, kernel_idx, inst_idx);
}

static void
evoasm_deme_seed_kernel(evoasm_deme_t *deme, size_t kernel_idx) {
  for(size_t i = 0; i < deme->params->kernel_size; i++) {
    evoasm_deme_seed_kernel_inst(deme, kernel_idx, i);
  }
}

static void
evoasm_deme_seed_topology_edge(evoasm_deme_t *deme,
                               size_t topology_idx, size_t edge_idx) {


  size_t topology_size = deme->params->topology_size;
  evoasm_prng_t *prng = &deme->prng;
  size_t edge_off = EVOASM_DEME_TOPOLOGY_EDGE_OFF(deme, topology_idx, edge_idx);
  evoasm_deme_topology_data_t *topology_data = &deme->topology_data;

  uint8_t kernel_idx = (uint8_t) evoasm_prng_rand_between_(prng, 0, (int64_t) topology_size);
  uint8_t succ_kernel_idx = (uint8_t) evoasm_prng_rand_between_(prng, 0, (int64_t) topology_size);
  uint8_t cond = (uint8_t) evoasm_prng_rand_between_(prng, 0, UINT8_MAX);

  topology_data->edges[edge_off + 0] = (uint8_t) kernel_idx;
  topology_data->edges[edge_off + 1] = (uint8_t) succ_kernel_idx;
  topology_data->edges[edge_off + 2] = cond;
}

static void
evoasm_deme_mutate_topology_edge(evoasm_deme_t *deme,
                                 size_t topology_idx,
                                 size_t edge_idx) {

  /* conditional edges - can be mutated freely */
  /* FIXME: should this be softer, like only change one side of the edge ? */
  evoasm_deme_seed_topology_edge(deme, topology_idx, edge_idx);
}

static void
evoasm_deme_seed_default_topology_edge(evoasm_deme_t *deme, size_t topology_idx, size_t kernel_idx) {
  evoasm_deme_topology_data_t *topology_data = &deme->topology_data;
  evoasm_prng_t *prng = &deme->prng;
  size_t topology_size = deme->params->topology_size;

  size_t default_succ_off = EVOASM_DEME_TOPOLOGY_DEFAULT_EDGE_OFF(deme, topology_idx, kernel_idx);
  uint8_t default_succ_kernel_idx = (uint8_t) evoasm_prng_rand_between_(prng, 0, (int64_t) topology_size + 1u);

  topology_data->default_succs[default_succ_off] = default_succ_kernel_idx;
}

static void
evoasm_deme_seed_topology(evoasm_deme_t *deme,
                          size_t topology_idx) {

  size_t topology_size = deme->params->topology_size;
  size_t n_edges = EVOASM_DEME_N_TOPOLOGY_EDGES_PER_PROGAM_(topology_size);

  for(size_t i = 0; i < n_edges; i++) {
    evoasm_deme_seed_topology_edge(deme, topology_idx, i);
  }

  for(size_t i = 0; i < topology_size; i++) {
    evoasm_deme_seed_default_topology_edge(deme, topology_idx, i);
  }

}

static void
evoasm_deme_seed(evoasm_deme_t *deme) {
  size_t n_kernels = (size_t) EVOASM_DEME_N_KERNELS(deme);
  for(size_t i = 0; i < n_kernels; i++) {
    evoasm_deme_seed_kernel(deme, i);
  }

  if(deme->params->topology_size > 1) {
    for(size_t i = 0; i < deme->params->deme_size; i++) {
      evoasm_deme_seed_topology(deme, i);
    }
  }
}

evoasm_success_t
evoasm_pop_seed(evoasm_pop_t *pop) {

#pragma omp parallel for
  for(size_t i = 0; i < pop->n_demes; i++) {
    evoasm_deme_seed(&pop->demes[i]);
  }
  pop->seeded = true;
  return true;
}


static void
evoasm_deme_load_program_(evoasm_deme_t *deme,
                          evoasm_program_t *program,
                          evoasm_deme_topology_data_t *topology_data,
                          evoasm_deme_kernel_data_t *kernel_data,
                          size_t topology_idx) {

  size_t topology_size = deme->params->topology_size;
  size_t kernel_size = deme->params->kernel_size;

  for(size_t i = 0; i < topology_size; i++) {
    size_t kernel_idx = EVOASM_DEME_TOPOLOGY_KERNEL_IDX_OFF(deme, topology_idx, i);
    size_t inst0_off = EVOASM_DEME_KERNEL_INST_OFF_(kernel_size, kernel_idx, 0);
    evoasm_kernel_t *kernel = &program->kernels[i];

    if(program->shallow) {
      kernel->insts = &kernel_data->insts[inst0_off];
    } else {
      EVOASM_MEMCPY_N(kernel->insts,
                      &kernel_data->insts[inst0_off], kernel->size);
    }

    switch(deme->arch_id) {
      case EVOASM_ARCH_X64:
        if(program->shallow) {
          kernel->x64.params = &kernel_data->params.x64[inst0_off];
        } else {
          EVOASM_MEMCPY_N(kernel->x64.params,
                          &kernel_data->params.x64[inst0_off], kernel->size);
        }
        break;
      default:
        evoasm_assert_not_reached();
    }
  }

  if(topology_size > 1) {
    size_t edge0_off = EVOASM_DEME_TOPOLOGY_EDGE_OFF(deme, topology_idx, 0);
    size_t n_edges = EVOASM_DEME_N_TOPOLOGY_EDGES_PER_PROGAM(deme);

    size_t default_succ0_off = EVOASM_DEME_TOPOLOGY_DEFAULT_EDGE_OFF(deme, topology_idx, 0);
    evoasm_program_update_topology(program, &topology_data->edges[edge0_off], n_edges,
                                   &topology_data->default_succs[default_succ0_off]);
  }
}

static inline void
evoasm_deme_load_program(evoasm_deme_t *deme,
                         size_t topology_idx) {

  evoasm_program_t *program = &deme->program;
  evoasm_deme_kernel_data_t *kernel_data = &deme->kernel_data;
  evoasm_deme_topology_data_t *topology_data = &deme->topology_data;

  evoasm_deme_load_program_(deme, program, topology_data,
                            kernel_data, topology_idx);
}


static evoasm_success_t
evoasm_deme_eval_program(evoasm_deme_t *deme, bool major, evoasm_loss_t *ret_loss) {
  const evoasm_pop_params_t *params = deme->params;
  evoasm_program_t *program = &deme->program;

  //bool prepare, bool emit_kernels, bool emit_io_load_store, bool set_io_mapping
  evoasm_program_emit_flags_t emit_flags = EVOASM_PROGRAM_EMIT_FLAG_SET_IO_MAPPING;

  size_t win_off = 0;
  size_t win_size = SIZE_MAX;

  if(!major) {
    win_off = deme->example_win_off;
    win_size = deme->params->example_win_size;
  }

  if(!evoasm_program_emit(program, params->program_input, win_off, win_size,
                          emit_flags)) {
    *ret_loss = INFINITY;

    if(evoasm_last_error.code == EVOASM_PROGRAM_ERROR_CODE_NO_OUTPUT) {
      /* do not abort on this error, instead just let loss be infinity */
      return true;
    }
    return false;
  }

  *ret_loss = evoasm_program_eval(program, params->program_output, win_off, win_size);

  return true;
}

static evoasm_deme_t *
evoasm_pop_find_best_deme(evoasm_pop_t *pop) {
  evoasm_deme_t *best_deme = &pop->demes[0];
  evoasm_loss_t best_loss = best_deme->best_loss;

  for(size_t i = 1; i < pop->n_demes; i++) {
    evoasm_deme_t *deme = &pop->demes[i];
    if(deme->best_loss < best_loss) {
      best_loss = deme->best_loss;
      best_deme = deme;
    }
  }
  return best_deme;
}

evoasm_success_t
evoasm_pop_load_best_program(evoasm_pop_t *pop, evoasm_program_t *program) {

  evoasm_deme_t *best_deme = evoasm_pop_find_best_deme(pop);
  const evoasm_pop_params_t *params = best_deme->params;
  size_t n_examples = evoasm_program_io_get_n_tuples(params->program_input);

  EVOASM_TRY(error, evoasm_program_init, program,
             evoasm_get_arch_info(best_deme->arch_id),
             params->topology_size,
             params->kernel_size,
             n_examples,
             params->recur_limit,
             false);

  size_t topology_idx = 0;
  evoasm_deme_load_program_(best_deme,
                            program,
                            &best_deme->best_topology_data,
                            &best_deme->best_kernel_data,
                            topology_idx);


  program->_input = *params->program_input;
  program->_output = *params->program_output;
  program->_input.len = 0;
  program->_output.len = 0;

  evoasm_program_emit_flags_t emit_flags = EVOASM_PROGRAM_EMIT_FLAG_SET_IO_MAPPING;

  EVOASM_TRY(error, evoasm_program_emit, program, params->program_input, 0, SIZE_MAX, emit_flags);

  evoasm_signal_set_exception_mask(program->exception_mask);
  evoasm_loss_t loss = evoasm_program_eval(program, params->program_output, 0, SIZE_MAX);
  (void) loss;
  assert(loss == best_deme->best_loss);
  evoasm_signal_clear_exception_mask();

  return true;

error:
  return false;
}

static evoasm_success_t
evoasm_deme_test_indiv(evoasm_deme_t *deme, bool major, size_t topology_idx, evoasm_loss_t *ret_loss) {

  evoasm_loss_t loss;
  evoasm_deme_loss_data_t *loss_data = &deme->loss_data;

  evoasm_deme_load_program(deme, topology_idx);

  EVOASM_TRY(error, evoasm_deme_eval_program, deme, major, &loss);

  loss_data->samples[EVOASM_DEME_LOSS_OFF(deme, topology_idx)] = loss;
  *ret_loss = loss;
  return true;
error:
  return false;
}


static void
evoasm_deme_topology_data_copy_edges(evoasm_deme_topology_data_t *topology_data,
                                     size_t off,
                                     evoasm_deme_topology_data_t *dst,
                                     size_t dst_off,
                                     size_t len) {

  memcpy(dst->edges + dst_off, topology_data->edges + off, sizeof(*dst->edges) * 3 * len);
}

static void
evoasm_deme_topology_data_copy_default_succs(evoasm_deme_topology_data_t *topology_data,
                                             size_t off,
                                             evoasm_deme_topology_data_t *dst,
                                             size_t dst_off,
                                             size_t len) {
  memcpy(dst->default_succs + dst_off, topology_data->default_succs + off, sizeof(*dst->default_succs) * len);
}

static void
evoasm_deme_kernel_data_copy_insts(evoasm_deme_kernel_data_t *kernel_data,
                                   evoasm_arch_id_t arch_id,
                                   size_t off,
                                   evoasm_deme_kernel_data_t *dst,
                                   size_t dst_off,
                                   size_t len) {

  memcpy(dst->insts + dst_off, kernel_data->insts + off,
         sizeof(evoasm_inst_id_t) * len);

  switch(arch_id) {
    case EVOASM_ARCH_X64:
      memcpy(dst->params.x64 + dst_off, kernel_data->params.x64 + off,
             sizeof(evoasm_x64_basic_params_t) * len);
      break;
    default:
      evoasm_assert_not_reached();
  }
}

static evoasm_used void
evoasm_deme_kernel_data_move(evoasm_deme_kernel_data_t *kernel_data,
                             evoasm_arch_id_t arch_id,
                             size_t src_off,
                             size_t dst_off,
                             size_t len) {

  memmove(kernel_data->insts + dst_off, kernel_data->insts + src_off,
          sizeof(evoasm_inst_id_t) * len);

  switch(arch_id) {
    case EVOASM_ARCH_X64:
      memmove(kernel_data->params.x64 + dst_off, kernel_data->params.x64 + src_off,
              sizeof(evoasm_x64_basic_params_t) * len);
      break;
    default:
      evoasm_assert_not_reached();
  }
}

static inline void
evoasm_deme_update_best(evoasm_deme_t *deme, evoasm_loss_t loss, size_t topology_idx) {


  size_t topology_size = deme->params->topology_size;
  size_t kernel_size = deme->params->kernel_size;

  evoasm_log_info("new best program loss: %g", loss);

  evoasm_buf_log(deme->program.buf, EVOASM_LOG_LEVEL_DEBUG);

//    for(size_t i = 0; i < deme->program.output_regs_mapping)

  deme->best_loss = loss;

  if(topology_size > 1) {
    {
      size_t src_edge0_off = EVOASM_DEME_TOPOLOGY_EDGE_OFF(deme, topology_idx, 0);
      size_t dst_edge0_off = EVOASM_DEME_TOPOLOGY_EDGE_OFF_(deme->params->topology_size, 0, 0);

      evoasm_deme_topology_data_copy_edges(&deme->topology_data, src_edge0_off,
                                           &deme->best_topology_data, dst_edge0_off,
                                           EVOASM_DEME_N_TOPOLOGY_EDGES_PER_PROGAM(deme));
    }

    {
      size_t src_default_succ0_off = EVOASM_DEME_TOPOLOGY_DEFAULT_EDGE_OFF(deme, topology_idx, 0);
      size_t dst_default_succ0_off = EVOASM_DEME_TOPOLOGY_DEFAULT_EDGE_OFF_(deme->params->topology_size, 0, 0);

      evoasm_deme_topology_data_copy_default_succs(&deme->topology_data, src_default_succ0_off,
                                                   &deme->best_topology_data, dst_default_succ0_off,
                                                   deme->params->topology_size);
    }
  }

  for(size_t i = 0; i < topology_size; i++) {
    size_t kernel_idx = EVOASM_DEME_TOPOLOGY_KERNEL_IDX_OFF(deme, topology_idx, i);
    size_t src_kernel_inst0_off = EVOASM_DEME_KERNEL_INST_OFF(deme, kernel_idx, 0);
    size_t dst_kernel_inst0_off = EVOASM_DEME_KERNEL_INST_OFF_(deme->params->kernel_size, i, 0);

    evoasm_deme_kernel_data_copy_insts(&deme->kernel_data, deme->arch_id, src_kernel_inst0_off,
                                       &deme->best_kernel_data, dst_kernel_inst0_off, kernel_size);

  }
}

static evoasm_success_t
evoasm_deme_test(evoasm_deme_t *deme, bool major) {

  for(size_t i = 0; i < deme->params->deme_size; i++) {
    evoasm_loss_t loss;

    EVOASM_TRY(error, evoasm_deme_test_indiv, deme, major, i, &loss);
  }

  return true;

error:
  return false;
}

evoasm_loss_t
evoasm_pop_get_best_loss(evoasm_pop_t *pop) {
  evoasm_deme_t *best_deme = evoasm_pop_find_best_deme(pop);
  return best_deme->best_loss;
}

size_t
evoasm_pop_get_gen_counter(evoasm_pop_t *pop) {
  return pop->gen_counter;
}

static void
evoasm_deme_eval_update(evoasm_deme_t *deme, bool major) {

  evoasm_deme_loss_data_t *loss_data = &deme->loss_data;

  evoasm_loss_t top_loss = INFINITY;
//  evoasm_loss_t avg_loss = 0.0;
  size_t top_indiv_idx = SIZE_MAX;

  {
//    size_t n = 1;

    for(size_t i = 0; i < deme->params->deme_size; i++) {
      size_t loss_off = EVOASM_DEME_LOSS_OFF(deme, i);

      evoasm_loss_t indiv_loss = loss_data->samples[loss_off];

      if(indiv_loss < top_loss) {
        evoasm_log_info("new top loss: %f -> %f", top_loss, indiv_loss);
        top_loss = indiv_loss;
        top_indiv_idx = i;
      }

//      if(!isinf(indiv_loss)) {
//        avg_loss += (indiv_loss - avg_loss) / (evoasm_loss_t) n;
//        n++;
//      }
    }
  }

  if(major) {

    if(top_loss < deme->best_loss) {
      evoasm_deme_update_best(deme, top_loss, top_indiv_idx);
    }

    double top_loss_diff = fabs(deme->top_loss - top_loss);

    if(top_loss_diff < 0.05) {
      deme->mut_rate = EVOASM_MIN(EVOASM_DEME_MAX_MUT_RATE, deme->mut_rate * 1.04f);
      deme->stagn_counter++;
    } else {
      deme->mut_rate = EVOASM_MAX(EVOASM_DEME_MIN_MUT_RATE, deme->mut_rate / 1.02f);
      deme->stagn_counter = 0;
    }

    deme->top_loss = top_loss;
  }

}

static evoasm_success_t
evoasm_deme_eval(evoasm_deme_t *deme, bool major) {
  bool retval = true;

  if(!evoasm_deme_test(deme, major)) {
    retval = false;
    goto done;
  }

  evoasm_deme_eval_update(deme, major);

done:
  return retval;
}

static evoasm_success_t
evoasm_pop_eval_(evoasm_pop_t *pop, bool major) {
  bool retval = true;
  size_t n_demes = pop->n_demes;

  if(!pop->seeded) {
    retval = false;
    evoasm_error(EVOASM_ERROR_TYPE_POP, EVOASM_ERROR_CODE_NONE,
                 "not seeded");
    goto done;
  }

  bool *retvals = evoasm_alloca(sizeof(bool) * n_demes);
  evoasm_error_t *errors = evoasm_alloca(sizeof(evoasm_error_t) * n_demes);

  if(!major &&
     pop->gen_counter > 0 &&
     evoasm_program_input_get_n_tuples(pop->params->program_input) > pop->params->example_win_size) {
    for(size_t i = 0; i < n_demes; i++) {
      pop->demes[i].example_win_off++;
    }
  }

#pragma omp parallel for
  for(size_t i = 0; i < n_demes; i++) {
    retvals[i] = evoasm_deme_eval(&pop->demes[i], major);
    if(!retvals[i]) {
      errors[i] = *evoasm_get_last_error();
    }
  }

  for(size_t i = 0; i < n_demes; i++) {
    if(!retvals[i]) {
      evoasm_set_last_error(&errors[i]);
      retval = false;
      break;
    }
  }

done:
  return retval;
}

#define EVOASM_DEME_TOURN_SIZE 5

static inline void
evoasm_deme_select(evoasm_deme_t *deme) {
  evoasm_deme_loss_data_t *loss_data = &deme->loss_data;
  size_t deme_size = deme->params->deme_size;

  EVOASM_MEMSET_N(deme->surviv_counters, 0, deme_size);

  size_t n_selected = 0;
  size_t n_to_select = deme_size - deme->params->n_demes;

  while(n_selected < n_to_select) {
    size_t min_idx = SIZE_MAX;
    evoasm_loss_t min_loss = INFINITY;

    for(size_t i = 0; i < EVOASM_DEME_TOURN_SIZE; i++) {
      size_t idx = (size_t) evoasm_prng_rand_between_(&deme->prng, 0, (int64_t) deme_size);
      evoasm_loss_t loss = loss_data->samples[EVOASM_DEME_LOSS_OFF(deme, idx)];
      if(loss < min_loss) {
        min_loss = loss;
        min_idx = idx;
      }
    }

    if(!isinf(min_loss)) {
      size_t sum = 0;
      for(size_t i = 0; i < deme_size; i++) {
        sum += deme->surviv_counters[i];
      }

      deme->surviv_counters[min_idx]++;
      n_selected++;
    }
  }
}

static void
evoasm_deme_copy_indiv(evoasm_deme_t *deme, size_t parent_idx, size_t child_idx) {

  evoasm_deme_kernel_data_t *kernel_data = &deme->kernel_data;
  evoasm_deme_topology_data_t *topology_data = &deme->topology_data;


  if(deme->params->topology_size > 1) {
    {
      size_t parent_edge0_off = EVOASM_DEME_TOPOLOGY_EDGE_OFF(deme, parent_idx, 0);
      size_t child_edge0_off = EVOASM_DEME_TOPOLOGY_EDGE_OFF(deme, child_idx, 0);

      evoasm_deme_topology_data_copy_edges(topology_data, parent_edge0_off,
                                           topology_data, child_edge0_off,
                                           EVOASM_DEME_N_TOPOLOGY_EDGES_PER_PROGAM(deme));
    }

    {
      size_t parent_default_succ0_off = EVOASM_DEME_TOPOLOGY_DEFAULT_EDGE_OFF(deme, parent_idx, 0);
      size_t child_default_succ0_off = EVOASM_DEME_TOPOLOGY_DEFAULT_EDGE_OFF(deme, child_idx, 0);

      evoasm_deme_topology_data_copy_default_succs(topology_data, parent_default_succ0_off,
                                                   topology_data, child_default_succ0_off,
                                                   deme->params->topology_size);
    }
  }

  {
    size_t n_insts = (size_t) (deme->params->topology_size * deme->params->kernel_size);
    size_t parent_kernel0_off = EVOASM_DEME_TOPOLOGY_KERNEL_IDX_OFF(deme, parent_idx, 0);
    size_t parent_inst0_off = EVOASM_DEME_KERNEL_INST_OFF(deme, parent_kernel0_off, 0);

    size_t child_kernel0_off = EVOASM_DEME_TOPOLOGY_KERNEL_IDX_OFF(deme, child_idx, 0);
    size_t child_inst0_off = EVOASM_DEME_KERNEL_INST_OFF(deme, child_kernel0_off, 0);


    evoasm_deme_kernel_data_copy_insts(kernel_data, deme->arch_id, parent_inst0_off,
                                       kernel_data, child_inst0_off, n_insts);
  }
}

static void
evoasm_deme_combine(evoasm_deme_t *deme) {
  size_t deme_size = deme->params->deme_size;
  size_t surviv_idx = 0;
  size_t dead_idx = 0;

  while(true) {
    while(surviv_idx < deme_size && deme->surviv_counters[surviv_idx] <= 1) surviv_idx++;
    if(surviv_idx >= deme_size) break;
    while(dead_idx < deme_size && deme->surviv_counters[dead_idx] != 0) dead_idx++;

    evoasm_deme_copy_indiv(deme, surviv_idx, dead_idx);

    deme->surviv_counters[surviv_idx]--;
    dead_idx++;
  }

  // store immigration target indexes
  {
    size_t j = 0;
    for(size_t i = dead_idx; i < deme_size; i++) {
      if(deme->surviv_counters[i] == 0) {
        deme->immig_idxs[j++] = (uint16_t) i;
      }
    }
    assert(j == deme->params->n_demes);
  }

}

//static void evoasm_used
//evoasm_deme_crossover(evoasm_deme_t *deme) {
//  size_t n_doomed = EVOASM_ALIGN_DOWN(deme->n_doomed_indivs, 2u);
//  for(size_t i = 0; i < n_doomed; i += 2) {
//    size_t parent_indiv1_idx = deme->blessed_indiv_idxs[i % deme->n_blessed_indivs];
//    size_t parent_indiv2_idx = deme->blessed_indiv_idxs[(i + 1) % deme->n_blessed_indivs];
//
//    size_t child_indiv1_idx = deme->doomed_indiv_idxs[i];
//    size_t child_indiv2_idx = deme->doomed_indiv_idxs[i + 1];
//
//    evoasm_deme_crossover_indivs(deme, parent_indiv1_idx, parent_indiv2_idx, child_indiv1_idx, child_indiv2_idx);
//  }
//}

static int evoasm_pop_loss_cmp_func(const void *a, const void *b) {
  evoasm_loss_t loss_a = *(const evoasm_loss_t *) a;
  evoasm_loss_t loss_b = *(const evoasm_loss_t *) b;
  return (loss_a > loss_b) - (loss_a < loss_b);
}

#define EVOASM_POP_SUMMARY_LEN 5u

static inline void
evoasm_deme_calc_summary(evoasm_deme_t *deme, evoasm_loss_t *summary_losses, evoasm_loss_t *summary) {
  size_t deme_size = deme->params->deme_size;
  evoasm_deme_loss_data_t *loss_data = &deme->loss_data;

  for(size_t j = 0; j < deme_size; j++) {
    size_t loss_off = EVOASM_DEME_LOSS_OFF(deme, j);
    evoasm_loss_t loss = loss_data->samples[loss_off];
    summary_losses[j] = loss;
  }

  qsort(summary_losses, deme_size, sizeof(evoasm_loss_t), evoasm_pop_loss_cmp_func);

  for(size_t j = 0; j < EVOASM_POP_SUMMARY_LEN; j++) {
    summary[j] = summary_losses[j * (deme_size - 1) / 4];
  }
}

size_t
evoasm_pop_summary_len(evoasm_pop_t *pop) {
  return pop->n_demes * EVOASM_POP_SUMMARY_LEN;
}

evoasm_success_t
evoasm_pop_calc_summary(evoasm_pop_t *pop, evoasm_loss_t *summary) {
  if(pop->summary_losses == NULL) {
    pop->summary_losses = evoasm_calloc(pop->params->deme_size, sizeof(evoasm_loss_t));
    if(!pop->summary_losses) {
      return false;
    }
  }

  for(size_t i = 0; i < pop->n_demes; i++) {
    evoasm_deme_calc_summary(&pop->demes[i], pop->summary_losses, &summary[i * EVOASM_POP_SUMMARY_LEN]);
  }

  return true;
}

static inline void
evoasm_deme_mutate_kernel(evoasm_deme_t *deme, size_t kernel_idx) {
  evoasm_prng_t *prng = &deme->prng;
  size_t kernel_size = deme->params->kernel_size;
  float r1 = evoasm_prng_randf_(prng);
  float kernel_mut_rate = (float) kernel_size * deme->mut_rate;

  if(r1 < kernel_mut_rate) {
    for(size_t i = 0; i < kernel_size; i++) {
      float r2 = evoasm_prng_randf_(prng);
      if(r2 < deme->mut_rate) {
        evoasm_deme_mutate_kernel_inst(deme, kernel_idx, i);
      }
    }
  }
}

static inline void
evoasm_deme_mutate_topology(evoasm_deme_t *deme, size_t topology_idx) {
  evoasm_prng_t *prng = &deme->prng;

  size_t topology_size = deme->params->topology_size;
//  size_t n_edges = EVOASM_DEME_N_TOPOLOGY_EDGES_PER_PROGAM(deme);
  size_t n_edges = topology_size;

  float r1 = evoasm_prng_randf_(prng);
  float topology_mut_rate = (float) topology_size * deme->mut_rate;

  float edge_mut_rate = (1.0f / 10.0f) * deme->mut_rate;

  if(r1 < topology_mut_rate) {
    for(size_t i = topology_size - 1; i < n_edges; i++) {
      float r2 = evoasm_prng_randf_(prng);
      /* FIXME: it seems a bad idea to
       * mutate this too strongly, as it hurts
       * kernel specialization.
       */
      if(r2 < edge_mut_rate) {
        evoasm_deme_mutate_topology_edge(deme, topology_idx, i);
      }
    }
  }
}

static void
evoasm_deme_mutate_topologies(evoasm_deme_t *deme) {
  size_t n_topologies = deme->params->deme_size;
  for(size_t i = 0; i < n_topologies; i++) {
    evoasm_deme_mutate_topology(deme, i);
  }
}

static void
evoasm_deme_mutate_kernels(evoasm_deme_t *deme) {
  size_t n_kernels = (size_t) EVOASM_DEME_N_KERNELS(deme);

  for(size_t i = 0; i < n_kernels; i++) {
    evoasm_deme_mutate_kernel(deme, i);
  }
}

static void
evoasm_deme_mutate(evoasm_deme_t *deme) {
  if(deme->params->topology_size > 1) {
    evoasm_deme_mutate_topologies(deme);
  }
  evoasm_deme_mutate_kernels(deme);
}

static void
evoasm_deme_inject_best(evoasm_deme_t *deme, evoasm_deme_t *src_deme) {

  size_t topology_size = deme->params->topology_size;
  size_t dead_topology_idx = deme->immig_idxs[src_deme->idx];

  if(topology_size > 1) {
    {
      size_t src_edge0_off = EVOASM_DEME_TOPOLOGY_EDGE_OFF_(deme->params->topology_size, 0, 0);
      size_t dst_edge0_off = EVOASM_DEME_TOPOLOGY_EDGE_OFF(deme, dead_topology_idx, 0);

      evoasm_deme_topology_data_copy_edges(&src_deme->best_topology_data, src_edge0_off, &deme->topology_data,
                                           dst_edge0_off, EVOASM_DEME_N_TOPOLOGY_EDGES_PER_PROGAM(deme));
    }

    {
      size_t src_default_succ0_off = EVOASM_DEME_TOPOLOGY_DEFAULT_EDGE_OFF_(deme->params->topology_size, 0, 0);
      size_t dst_default_succ0_off = EVOASM_DEME_TOPOLOGY_DEFAULT_EDGE_OFF(deme, dead_topology_idx, 0);

      evoasm_deme_topology_data_copy_default_succs(&src_deme->best_topology_data, src_default_succ0_off,
                                                   &deme->topology_data,
                                                   dst_default_succ0_off, deme->params->topology_size);
    }
  }

  {
    size_t src_inst0_off = EVOASM_DEME_KERNEL_INST_OFF_(deme->params->kernel_size, 0, 0);
    size_t kernel0_off = EVOASM_DEME_TOPOLOGY_KERNEL_IDX_OFF(deme, dead_topology_idx, 0);
    size_t dst_inst0_off = EVOASM_DEME_KERNEL_INST_OFF(deme, kernel0_off, 0);
    evoasm_deme_kernel_data_copy_insts(&src_deme->best_kernel_data, deme->arch_id, src_inst0_off, &deme->kernel_data,
                                       dst_inst0_off, deme->params->topology_size);
  }
}

static void
evoasm_deme_save_elite(evoasm_deme_t *deme) {
  evoasm_deme_inject_best(deme, deme);
}

static void
evoasm_deme_immigrate_elite(evoasm_deme_t *deme) {
  evoasm_deme_t *demes = deme->pop->demes;
  size_t demes_len = deme->params->n_demes;

  for(size_t i = 0; i < demes_len; i++) {
    evoasm_deme_t *immigration_deme = &demes[i];

    if(deme != immigration_deme) {
      evoasm_deme_inject_best(deme, immigration_deme);
    }

  }
}

static void
evoasm_deme_next_gen(evoasm_deme_t *deme, bool major) {
  evoasm_deme_select(deme);

  if(major) {
    evoasm_deme_save_elite(deme);
    evoasm_deme_immigrate_elite(deme);
  }
  evoasm_deme_combine(deme);
  evoasm_deme_mutate(deme);
}

static void
evoasm_pop_next_gen_(evoasm_pop_t *pop, bool major) {
#pragma omp parallel for
  for(size_t i = 0; i < pop->n_demes; i++) {
    evoasm_deme_next_gen(&pop->demes[i], major);
  }

  pop->gen_counter++;
}

void
evoasm_pop_next_gen(evoasm_pop_t *pop) {
  evoasm_pop_next_gen_(pop, true);
}

evoasm_success_t
evoasm_pop_eval(evoasm_pop_t *pop, size_t n_minor_gens) {
  for(size_t i = 0; i < n_minor_gens; i++) {
    EVOASM_TRY(error, evoasm_pop_eval_, pop, false);
    evoasm_pop_next_gen_(pop, false);
  }
  EVOASM_TRY(error, evoasm_pop_eval_, pop, true);
  return true;

error:
  return false;
}

EVOASM_DEF_ALLOC_FREE_FUNCS(pop)
