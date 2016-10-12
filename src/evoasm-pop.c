//
// Created by jap on 9/16/16.
//

#include "evoasm-pop.h"
#include "evoasm-pop-params.h"
#include "evoasm-rand.h"
#include "evoasm-signal.h"
#include "evoasm-program.h"
#include "evoasm.h"

//#ifdef _OPENMP
#  include <omp.h>
//#endif

EVOASM_DEF_LOG_TAG("pop")


static evoasm_success_t
evoasm_pop_indiv_data_init(evoasm_pop_indiv_data_t *indiv_data, unsigned deme_count, unsigned deme_size) {

  EVOASM_TRY_ALLOC(error, aligned_calloc, indiv_data->sizes, EVOASM_CACHE_LINE_SIZE,
                   deme_count * deme_size,
                   sizeof(uint16_t));
  EVOASM_TRY_ALLOC(error, aligned_calloc, indiv_data->losses, EVOASM_CACHE_LINE_SIZE,
                   deme_count * deme_size,
                   sizeof(evoasm_loss_t));
  EVOASM_TRY_ALLOC(error, aligned_calloc, indiv_data->best_losses, EVOASM_CACHE_LINE_SIZE,
                   deme_count,
                   sizeof(evoasm_loss_t));

  for(unsigned i = 0; i < deme_count; i++) {
    indiv_data->best_losses[i] = INFINITY;
  }

  return true;
error:
  return false;
}

static void
evoasm_pop_indiv_data_destroy(evoasm_pop_indiv_data_t *indiv_data) {
  evoasm_free(indiv_data->losses);
  evoasm_free(indiv_data->best_losses);
  evoasm_free(indiv_data->sizes);
}

static evoasm_success_t
evoasm_pop_basic_program_data_init(evoasm_pop_basic_program_data_t *basic_program_data, unsigned count) {

  EVOASM_TRY_ALLOC(error, aligned_calloc, basic_program_data->kernel_idxs, EVOASM_CACHE_LINE_SIZE,
                   count,
                   sizeof(uint16_t));
  EVOASM_TRY_ALLOC(error, aligned_calloc, basic_program_data->kernel_deme_idxs, EVOASM_CACHE_LINE_SIZE,
                   count,
                   sizeof(uint16_t));
  EVOASM_TRY_ALLOC(error, aligned_calloc, basic_program_data->jmp_offs, EVOASM_CACHE_LINE_SIZE,
                   count,
                   sizeof(uint16_t));
  EVOASM_TRY_ALLOC(error, aligned_calloc, basic_program_data->jmp_selectors, EVOASM_CACHE_LINE_SIZE,
                   count,
                   sizeof(uint8_t));

  return true;

error:
  return false;
}

static void
evoasm_pop_basic_program_data_destroy(evoasm_pop_basic_program_data_t *basic_program_data) {
  evoasm_free(basic_program_data->kernel_idxs);
  evoasm_free(basic_program_data->kernel_deme_idxs);
  evoasm_free(basic_program_data->jmp_offs);
  evoasm_free(basic_program_data->jmp_selectors);
}

static evoasm_success_t
evoasm_pop_kernel_data_init(evoasm_pop_kernel_data_t *inst_data, evoasm_arch_id_t arch_id,
                            unsigned kernel_count, unsigned kernel_size) {

  unsigned inst_count = kernel_count * kernel_size;
  EVOASM_TRY_ALLOC(error, aligned_calloc, inst_data->insts, EVOASM_CACHE_LINE_SIZE,
                   inst_count,
                   sizeof(evoasm_inst_id_t));

  switch(arch_id) {
    case EVOASM_ARCH_X64:
      EVOASM_TRY_ALLOC(error, aligned_calloc, inst_data->params.x64, EVOASM_CACHE_LINE_SIZE,
                       inst_count,
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
evoasm_pop_kernel_data_destroy(evoasm_pop_kernel_data_t *inst_data) {
  evoasm_free(inst_data->insts);
  evoasm_free(inst_data->params.data);
}

static evoasm_success_t
evoasm_pop_program_data_init(evoasm_pop_program_data_t *program_data, unsigned deme_count, unsigned deme_size) {
  EVOASM_TRY(error, evoasm_pop_basic_program_data_init, &program_data->basic_program_data, deme_count * deme_size);
  EVOASM_TRY(error, evoasm_pop_indiv_data_init, &program_data->indiv_data, deme_count, deme_size);

  return true;
error:
  return false;
}

static void
evoasm_pop_program_data_destroy(evoasm_pop_program_data_t *program_data) {
  evoasm_pop_basic_program_data_destroy(&program_data->basic_program_data);
  evoasm_pop_indiv_data_destroy(&program_data->indiv_data);
}

static evoasm_success_t
evoasm_pop_module_data_init(evoasm_pop_module_data_t *module_data, unsigned count) {
  EVOASM_TRY(error, evoasm_pop_basic_program_data_init, &module_data->basic_program_data, count);

  EVOASM_TRY_ALLOC(error, aligned_calloc, module_data->sizes, EVOASM_CACHE_LINE_SIZE,
                   count,
                   sizeof(uint16_t));
  EVOASM_TRY_ALLOC(error, aligned_calloc, module_data->pheromones, EVOASM_CACHE_LINE_SIZE,
                   count,
                   sizeof(float));
  return true;
error:
  return false;
}

static void
evoasm_pop_module_data_destroy(evoasm_pop_module_data_t *module_data) {
  evoasm_pop_basic_program_data_destroy(&module_data->basic_program_data);
  evoasm_free(module_data->pheromones);
  evoasm_free(module_data->sizes);
}

static evoasm_success_t
evoasm_pop_thread_data_destroy(evoasm_pop_thread_data_t *thread_data) {
  bool retval = true;

  if(!evoasm_program_destroy(&thread_data->program)) retval = false;
  evoasm_prng_destroy(&thread_data->prng);
  evoasm_free(thread_data->parent_idxs);

  evoasm_pop_basic_program_data_destroy(&thread_data->parent_basic_program_data);
  evoasm_pop_kernel_data_destroy(&thread_data->parent_kernel_data);

  return retval;
}

evoasm_success_t
evoasm_pop_destroy(evoasm_pop_t *pop) {
  bool retval = true;

  evoasm_free(pop->error_counters);
  evoasm_free(pop->domains);

  for(int i = 0; i < pop->max_threads; i++) {
    retval &= evoasm_pop_thread_data_destroy(&pop->thread_data[i]);
  }
  evoasm_free(pop->thread_data);

  evoasm_pop_program_data_destroy(&pop->program_data);
  evoasm_pop_kernel_data_destroy(&pop->kernel_data);
  evoasm_pop_module_data_destroy(&pop->module_data);

  return retval;
}

static evoasm_success_t bool
evoasm_pop_thread_data_init(evoasm_pop_thread_data_t *thread_data,
                            evoasm_pop_params_t *params,
                            evoasm_prng_state_t *seed,
                            evoasm_arch_id_t arch_id) {

  evoasm_prng_init(&thread_data->prng, seed);
  EVOASM_TRY(error, evoasm_program_init, &thread_data->program,
             arch_id,
             params->program_input,
             params->max_program_size,
             params->max_kernel_size,
             params->recur_limit);


  unsigned max_deme_size = EVOASM_MAX(params->kernel_deme_size, params->program_deme_size);
  EVOASM_TRY_ALLOC(error, calloc, thread_data->parent_idxs, max_deme_size, sizeof(uint16_t));

  EVOASM_TRY(error, evoasm_pop_basic_program_data_init, &thread_data->parent_basic_program_data,
             2u * params->max_program_size);
  EVOASM_TRY(error, evoasm_pop_kernel_data_init, &thread_data->parent_kernel_data, arch_id, 2, params->max_kernel_size);

  return true;

error:
  return false;
}

evoasm_success_t
evoasm_pop_init_domains(evoasm_pop_t *pop) {
  unsigned i, j, k;
  evoasm_domain_t cloned_domain;

  evoasm_pop_params_t *params = pop->params;

  size_t domains_len = (size_t) (params->inst_count * params->param_count);
  pop->domains = evoasm_calloc(domains_len,
                               sizeof(evoasm_domain_t));

  if(!pop->domains) goto fail;

  for(i = 0; i < params->inst_count; i++) {
    evoasm_x64_inst_t *inst = _evoasm_x64_inst(params->inst_ids[i]);
    for(j = 0; j < params->param_count; j++) {
      evoasm_domain_t *inst_domain = &pop->domains[i * params->param_count + j];
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
  evoasm_error(EVOASM_ERROR_TYPE_ARG, EVOASM_ERROR_CODE_NONE,
               NULL, "Empty domain");
  return false;
}

evoasm_success_t
evoasm_pop_init(evoasm_pop_t *pop,
                evoasm_arch_id_t arch_id,
                evoasm_pop_params_t *params) {
  int max_threads;
  static evoasm_pop_t zero_pop = {0};
  uint16_t program_kernel_count;
  evoasm_prng_t seed_prng;
  unsigned example_count = EVOASM_PROGRAM_INPUT_EXAMPLE_COUNT(params->program_input);
  unsigned total_kernel_count = params->max_program_size * params->kernel_deme_count * params->kernel_deme_size;

#ifdef _OPENMP
  max_threads = omp_get_max_threads();
#else
  max_threads = 1;
#endif

  *pop = zero_pop;
  pop->params = params;
  pop->example_count = example_count;
  pop->arch_id = arch_id;
  pop->max_threads = max_threads;

  evoasm_prng_init(&seed_prng, &params->seed);

  EVOASM_TRY(error, evoasm_pop_init_domains, pop);
  EVOASM_TRY(error, evoasm_pop_kernel_data_init, &pop->kernel_data, arch_id, total_kernel_count,
             params->max_kernel_size);
  EVOASM_TRY(error, evoasm_pop_program_data_init, &pop->program_data, params->program_deme_count,
             params->program_deme_size);
  EVOASM_TRY(error, evoasm_pop_module_data_init, &pop->module_data, params->library_size);

  EVOASM_TRY_ALLOC(error, aligned_calloc, pop->thread_data, EVOASM_CACHE_LINE_SIZE, (size_t) max_threads,
                   sizeof(evoasm_pop_thread_data_t));

  for(int i = 0; i < max_threads; i++) {
    evoasm_prng_state_t seed;

    for(int j = 0; j < EVOASM_PRNG_SEED_LEN; j++) {
      seed.data[j] = _evoasm_prng_rand64(&seed_prng);
    }

    EVOASM_TRY(error, evoasm_pop_thread_data_init,
               &pop->thread_data[i],
               params,
               &seed,
               arch_id);
  }

  EVOASM_TRY_ALLOC(error, calloc, pop->error_counters, params->program_deme_count * example_count, sizeof(uint64_t));
  pop->error_counter = 0;

  return true;

error:
  evoasm_pop_destroy(pop);
  return false;
}

#if 0
static evoasm_indiv_t *
evoasm_pop_indiv_(evoasm_pop_t *pop, uint32_t idx, unsigned char *ptr) {
  return (evoasm_indiv_t *) (ptr + idx * pop->kernel_size);
}

evoasm_indiv_t *
evoasm_pop_get_indiv(evoasm_pop_t *pop, uint32_t idx) {
  return evoasm_pop_indiv_(pop, idx, pop->main_indivs);
}

evoasm_loss_t
evoasm_pop_get_indiv_loss(evoasm_pop_t *pop, uint32_t idx) {
  return pop->losses[idx];
}

size_t
evoasm_pop_get_indiv_size(evoasm_pop_t *pop) {
  return pop->kernel_size;
}

void
evoasm_pop_inject(evoasm_pop_t *pop, evoasm_indiv_t *indiv, size_t indiv_size, evoasm_loss_t loss) {
  unsigned i;

  while(true) {
    for(i = 0; i < pop->params->size; i++) {
      uint32_t r = _evoasm_prng_rand32(&pop->prng);
      if(r > UINT32_MAX * ((pop->best_loss + 1.0) / (pop->losses[i] + 1.0))) {
        goto done;
      }
    }
  }
done:;
  assert(indiv_size <= pop->kernel_size);
  memcpy(evoasm_pop_get_indiv(pop, i), indiv, indiv_size);
  pop->losses[i] = loss;
}
#endif

#define EVOASM_POP_PROGRAM_OFF(pop, deme_idx, program_idx) \
  ((deme_idx) * (pop)->params->program_deme_size + (program_idx));

#define EVOASM_POP_PROGRAM_RANK_OFF(pop, program_off, rank) \
  ((program_off) * (pop)->params->max_program_size + (rank));

#define EVOASM_POP_KERNEL_OFF(pop, rank, deme_idx, kernel_idx) \
  ((rank) * ((pop)->params->kernel_deme_count * (pop)->params->kernel_deme_size) \
                        + (deme_idx) * (pop)->params->kernel_deme_size \
                        + (kernel_idx));

#define EVOASM_POP_KERNEL_INST_OFF(pop, kernel_off, inst_idx) \
  (kernel_off) * (pop)->params->max_kernel_size + inst_idx;
_

static void
evoasm_pop_seed_kernel_param_x64(evoasm_pop_t *pop, evoasm_inst_id_t *inst_id_ptr,
                                 evoasm_x64_basic_params_t *params_ptr,
                                 evoasm_prng_t *prng) {
  unsigned i;
  evoasm_pop_params_t *params = pop->params;
  unsigned param_count = params->param_count;

  int64_t inst_idx = _evoasm_prng_rand_between(prng, 0, params->inst_count - 1);
  evoasm_inst_id_t inst_id = params->inst_ids[inst_idx];

  *inst_id_ptr = inst_id;

  /* set parameters */
  for(i = 0; i < param_count; i++) {
    evoasm_domain_t *domain = &pop->domains[inst_idx * param_count + i];
    if(domain->type < EVOASM_DOMAIN_TYPE_NONE) {
      evoasm_x64_param_id_t param_id = (evoasm_x64_param_id_t) pop->params->param_ids[i];
      evoasm_param_val_t param_val;

      param_val = (evoasm_param_val_t) evoasm_domain_rand(domain, prng);
      _evoasm_x64_basic_params_set(params_ptr, param_id, param_val);
    }
  }
}

static void
evoasm_pop_seed_kernel(evoasm_pop_t *pop, unsigned rank, unsigned deme_idx, unsigned kernel_idx, int tid) {
  unsigned i;

  evoasm_prng_t *prng = &pop->thread_data[tid].prng;
  evoasm_pop_params_t *params = pop->params;
  evoasm_pop_kernel_data_t *kernel_data = &pop->kernel_data;

  unsigned kernel_off = EVOASM_POP_KERNEL_OFF(pop, rank, deme_idx, kernel_idx);

  uint16_t kernel_size =
      (uint16_t) _evoasm_prng_rand_between(prng,
                                           params->min_kernel_size,
                                           params->max_kernel_size);

  pop->kernel_data.indiv_data.sizes[kernel_off] = kernel_size;

  for(i = 0; i < kernel_size; i++) {
    unsigned kernel_inst_off = EVOASM_POP_KERNEL_INST_OFF(pop, kernel_off, i);
    evoasm_inst_id_t *insts_ptr = &kernel_data->insts[kernel_inst_off];

    switch(pop->arch_id) {
      case EVOASM_ARCH_X64: {
        evoasm_x64_basic_params_t *params_ptr = &kernel_data->params.x64[kernel_inst_off];
        evoasm_pop_seed_kernel_param_x64(pop, insts_ptr, params_ptr, prng);
        break;
      }
      default:
        evoasm_assert_not_reached();
    }
  }

#if 0
  kernel_params->jmp_selector = (uint8_t) _evoasm_prng_rand8(prng);
  kernel_params->alt_succ_idx = (uint16_t)
      _evoasm_prng_rand_between(prng, 0, program_size - 1);
#endif

}


static void
evoasm_pop_seed_program(evoasm_pop_t *pop,
                        unsigned deme_idx,
                        unsigned program_idx,
                        int tid) {
  unsigned i;
  evoasm_pop_program_data_t *program_data = &pop->program_data;
  evoasm_prng_t *prng = &pop->thread_data[tid].prng;
  evoasm_pop_params_t *params = pop->params;

  unsigned program_off = EVOASM_POP_PROGRAM_OFF(pop, deme_idx, program_idx);

  uint16_t program_size =
      (uint16_t) _evoasm_prng_rand_between(prng,
                                           params->min_program_size,
                                           params->max_program_size);

  pop->program_data.indiv_data.sizes[program_off] = program_size;


  for(i = 0; i < program_size; i++) {
    unsigned kernel_off = program_off * params->max_program_size + i;

    program_data->basic_program_data.jmp_selectors[kernel_off] =
        (uint8_t) _evoasm_prng_rand8(prng);

    program_data->basic_program_data.jmp_offs[kernel_off] =
        (uint16_t) _evoasm_prng_rand_between(prng, 0, program_size - 1);

    program_data->basic_program_data.kernel_idxs[kernel_off] =
        (uint16_t) _evoasm_prng_rand_between(prng, 0, params->kernel_deme_size);

    program_data->basic_program_data.kernel_deme_idxs[kernel_off] =
        (uint16_t) _evoasm_prng_rand_between(prng, 0, params->kernel_deme_count);
  }
}


evoasm_success_t
evoasm_pop_seed(evoasm_pop_t *pop) {

#pragma omp parallel for collapse(2)
  for(unsigned i = 0; i < pop->params->max_program_size; i++) {
    for(unsigned j = 0; j < pop->params->kernel_deme_count; j++) {
      int tid;
#ifdef _OPENMP
      tid = omp_get_thread_num();
#else
      tid = 1;
#endif
      for(unsigned k = 0; k < pop->params->kernel_deme_size; k++) {
        evoasm_pop_seed_kernel(pop, i, j, k, tid);
      }
    }
  }

#pragma omp parallel for collapse(2)
  for(unsigned i = 0; i < pop->params->program_deme_count; i++) {
    int tid;
#ifdef _OPENMP
    tid = omp_get_thread_num();
#else
    tid = 1;
#endif

    for(unsigned j = 0; j < pop->params->program_deme_size; j++) {
      evoasm_pop_seed_program(pop, i, j, tid);
    }
  }

  pop->seeded = true;
  return true;
}

static evoasm_success_t
evoasm_pop_eval_prepare(evoasm_pop_t *pop) {
  evoasm_signal_install((evoasm_arch_id_t) pop->arch_id, 0);
  return true;
}

static evoasm_success_t
evoasm_pop_eval_cleanup(evoasm_pop_t *pop) {
  evoasm_signal_uninstall();
  return true;
}

static void
evoasm_pop_load_program(evoasm_pop_t *pop, evoasm_program_t *program,
                        unsigned deme_idx, unsigned program_idx, int tid) {

  evoasm_pop_program_data_t *program_data = &pop->program_data;
  evoasm_pop_kernel_data_t *inst_data = &pop->kernel_data;

  unsigned program_off = EVOASM_POP_PROGRAM_OFF(pop, deme_idx, program_idx);
  uint16_t program_size = program_data->indiv_data.sizes[program_off];

  program->program_size = program_size;

  for(unsigned i = 0; i < program_size; i++) {
    unsigned program_rank_off = EVOASM_POP_PROGRAM_RANK_OFF(pop, program_off, i);

    uint16_t kernel_deme_idx = program_data->basic_program_data.kernel_deme_idxs[program_rank_off];
    uint16_t kernel_idx = program_data->basic_program_data.kernel_idxs[program_rank_off];

    unsigned kernel_off = EVOASM_POP_KERNEL_OFF(pop, i, kernel_deme_idx, kernel_idx);
    unsigned inst_off = EVOASM_POP_KERNEL_INST_OFF(pop, kernel_off, 0);

    program->kernels[i].insts = &inst_data->insts[inst_off];

    switch(pop->arch_id) {
      case EVOASM_ARCH_X64:
        program->kernels[i].params.x64 = &inst_data->params.x64[inst_off];
        break;
      default:
        evoasm_assert_not_reached();
    }

    program->jmp_offs[i] = program_data->basic_program_data.jmp_offs[program_rank_off];
    program->jmp_selectors[i] = program_data->basic_program_data.jmp_selectors[program_rank_off];
  }
}

static evoasm_success_t
evoasm_pop_assess_program(evoasm_pop_t *pop, evoasm_program_t *program, evoasm_loss_t *loss) {
  evoasm_kernel_t *kernel = &program->kernels[program->program_size - 1];
  evoasm_pop_params_t *params = pop->params;

  if(!evoasm_program_emit(program, params->program_input, true, true, true, true)) {
    *loss = INFINITY;
    return false;
  }

  if(EVOASM_UNLIKELY(kernel->output_reg_count == 0)) {
    *loss = INFINITY;
    return true;
  }

  //evoasm_buf_log(program->buf, EVOASM_LOG_LEVEL_INFO);
  {
    evoasm_signal_set_exception_mask(program->exception_mask);

    if(EVOASM_SIGNAL_TRY()) {
      evoasm_buf_exec(program->buf);
      *loss = evoasm_program_assess(program, params->program_output);
    } else {
      evoasm_log_debug("program %p signaled", (void *) program);
      *loss = INFINITY;
    }
  }
  return true;
}

static void
evoasm_pop_update_loss(evoasm_pop_t *pop, evoasm_loss_t loss, unsigned deme_idx, unsigned program_idx) {

  evoasm_pop_program_data_t *program_data = &pop->program_data;
  evoasm_pop_kernel_data_t *kernel_data = &pop->kernel_data;
  unsigned program_off = EVOASM_POP_PROGRAM_OFF(pop, deme_idx, program_idx);

  program_data->indiv_data.losses[program_off] = loss;
  uint16_t program_size = program_data->indiv_data.sizes[program_off];

  evoasm_loss_t kernel_loss = loss / program_size;

  for(unsigned i = 0; i < program_size; i++) {
    unsigned program_rank_off = EVOASM_POP_PROGRAM_RANK_OFF(pop, program_off, i);

    uint16_t kernel_deme_idx = program_data->basic_program_data.kernel_deme_idxs[program_rank_off];
    uint16_t kernel_idx = program_data->basic_program_data.kernel_idxs[program_rank_off];

    unsigned kernel_off = EVOASM_POP_KERNEL_OFF(pop, i, kernel_deme_idx, kernel_idx);
#pragma omp critical(loss_update)
    {
      if(kernel_data->indiv_data.losses[kernel_off] == INFINITY) {
        kernel_data->indiv_data.losses[kernel_off] = kernel_loss;
      } else {
        kernel_data->indiv_data.losses[kernel_off] = kernel_data->indiv_data.losses[kernel_off] / 2.0f + kernel_loss / 2.0f;
      }
    }

  }
}

static evoasm_success_t
evoasm_pop_eval_program(evoasm_pop_t *pop, unsigned deme_idx, unsigned program_idx, int tid) {

  evoasm_program_t *program = &pop->thread_data[tid].program;
  evoasm_loss_t loss;

  evoasm_pop_load_program(pop, program, deme_idx, program_idx, tid);

  EVOASM_TRY(error, evoasm_pop_assess_program, pop, program, &loss);
  evoasm_log_debug("team %d has loss %lf", program_idx, loss);

  evoasm_pop_update_loss(pop, loss, deme_idx, program_idx);

  return true;
error:
  return false;
}

static evoasm_success_t
evoasm_pop_eval_programs(evoasm_pop_t *pop) {
#pragma omp parallel for collapse(2)
  for(unsigned i = 0; i < pop->params->program_deme_count; i++) {
    for(unsigned j = 0; j < pop->params->program_deme_size; j++) {
      int tid;
#ifdef _OPENMP
      tid = omp_get_thread_num();
#else
      tid = 1;
#endif

      evoasm_pop_eval_program(pop, i, j, tid);
    }
  }
}

static void
evoasm_pop_update_best_losses(evoasm_pop_t *pop) {

  evoasm_pop_unit_data_t *member_data = &pop->member_data;

#pragma omp parallel for collapse(2)
  for(unsigned i = 0; i < pop->params->depth; i++) {
    for(unsigned j = 0; j < pop->params->deme_counts[i]; j++) {
      unsigned deme_off = EVOASM_POP_DEME_OFF(pop, i, j);

      for(unsigned k = 0; k < pop->params->deme_sizes[i]; k++) {
        evoasm_loss_t loss = member_data->losses[EVOASM_POP_MEMBER_OFF(pop, i, j, k)];
        if(loss < member_data->best_losses[deme_off]) {
          member_data->best_losses[deme_off] = loss;
        }
      }
    }
  }
}


evoasm_success_t
evoasm_pop_eval(evoasm_pop_t *pop) {
  bool retval;
  uint32_t example_count = pop->example_count;

  if(!pop->seeded) {
    retval = false;
    evoasm_error(EVOASM_ERROR_TYPE_RUNTIME, EVOASM_ERROR_CODE_NONE,
                 NULL, "not seeded");
    goto done;
  }

  if(!evoasm_pop_eval_prepare(pop)) {
    retval = false;
    goto done;
  }

  if(!evoasm_pop_eval_programs(pop)) {
    retval = false;
    goto done;
  }

  evoasm_pop_update_best_losses(pop);

  retval = true;

done:
  if(!evoasm_pop_eval_cleanup(pop)) {
    retval = false;
  }
  return retval;
}


static void
evoasm_pop_select_deme(evoasm_pop_t *pop, unsigned depth, unsigned deme_idx, int tid) {
  evoasm_prng_t *prng = &pop->thread_data[tid].prng;
  uint16_t deme_size = pop->params->deme_sizes[depth];
  uint16_t *parent_idxs = pop->thread_data[tid].parent_idxs;
  unsigned deme_off = EVOASM_POP_DEME_OFF(pop, depth, deme_idx);
  evoasm_pop_unit_data_t *member_data = &pop->member_data;
  evoasm_loss_t best_loss = member_data->best_losses[deme_off];
  evoasm_loss_t *losses = &pop->member_data.losses[EVOASM_POP_MEMBER_OFF(pop, depth, deme_idx, 0)];
  uint32_t n = 0;

  while(true) {
    for(uint16_t i = 0; i < deme_size; i++) {
      uint32_t r = _evoasm_prng_rand32(prng);
      if(n >= deme_size) goto done;
      if(r < UINT32_MAX * ((best_loss + 1.0) / (losses[i] + 1.0))) {
        parent_idxs[n++] = i;
      }
    }
  }
done:;
}

static void
evoasm_pop_inst_data_copy(evoasm_pop_kernel_data_t *inst_data_dst,
                          evoasm_arch_id_t arch_id,
                          size_t off_dst,
                          evoasm_pop_kernel_data_t *inst_data_src,
                          size_t off_src,
                          size_t len) {

  memcpy(inst_data_dst->insts + off_dst, inst_data_src->insts + off_src,
         sizeof(evoasm_inst_id_t) * len);

  switch(arch_id) {
    case EVOASM_ARCH_X64:
      memcpy(inst_data_dst->params.x64 + off_dst, inst_data_src->params.x64 + off_src,
             sizeof(evoasm_x64_basic_params_t) * len);
      break;
    default:
      evoasm_assert_not_reached();
  }
}

static void
evoasm_pop_team_pos_data_copy(evoasm_pop_program_data_t *team_pos_data_dst,
                              size_t off_dst,
                              evoasm_pop_program_data_t *team_pos_data_src,
                              size_t off_src,
                              size_t len) {

  memcpy(team_pos_data_dst->member_idxs + off_dst, team_pos_data_src->member_idxs + off_src,
         sizeof(uint16_t) * len);
  memcpy(team_pos_data_dst->member_deme_idxs + off_dst, team_pos_data_src->member_deme_idxs + off_src,
         sizeof(uint16_t) * len);
  memcpy(team_pos_data_dst->jmp_offs + off_dst, team_pos_data_src->jmp_offs + off_src,
         sizeof(uint16_t) * len);
  memcpy(team_pos_data_dst->jmp_selectors + off_dst, team_pos_data_src->jmp_selectors + off_src,
         sizeof(uint8_t) * len);
}

static void
evoasm_pop_combine_deme(evoasm_pop_t *pop, unsigned depth, unsigned deme_idx, int tid) {
  evoasm_pop_unit_data_t *member_data = &pop->member_data;
  evoasm_pop_thread_data_t *thread_data = &pop->thread_data[tid];
  evoasm_pop_program_data_t *team_pos_data = &pop->program_data;
  evoasm_pop_kernel_data_t *inst_data = &pop->kernel_data;
  evoasm_prng_t *prng = &thread_data->prng;

  bool kernel_depth = (depth == EVOASM_POP_PARAMS_KERNELS_DEPTH(pop->params));

  for(unsigned i = 0; i < pop->params->deme_sizes[depth]; i += 2) {
    unsigned parent_member_idxs[2] = {thread_data->parent_idxs[i], thread_data->parent_idxs[i + 1]};
    unsigned parent_member_offs[2] = {EVOASM_POP_MEMBER_OFF(pop, depth, deme_idx, parent_member_idxs[0]),
                                      EVOASM_POP_MEMBER_OFF(pop, depth, deme_idx, parent_member_idxs[1])};

    uint16_t parent_member_sizes[2] = {member_data->sizes[parent_member_offs[0]],
                                       member_data->sizes[parent_member_offs[1]]};

    if(parent_member_sizes[0] < parent_member_sizes[1]) {
      EVOASM_SWAP(unsigned, parent_member_idxs[0], parent_member_idxs[1]);
      EVOASM_SWAP(unsigned, parent_member_offs[0], parent_member_offs[1]);
      EVOASM_SWAP(uint16_t, parent_member_sizes[0], parent_member_sizes[1]);
    }

    evoasm_loss_t parent_member_losses[2] = {member_data->losses[parent_member_offs[0]],
                                             member_data->losses[parent_member_offs[1]]};

    /* rough estimate */
    evoasm_loss_t child_member_loss = 0.5f * (parent_member_losses[0] + parent_member_losses[1]);

    /* save parents to local storage, we override originals with children */
    unsigned parent_off = 0;

    for(unsigned j = 0; j < 2; j++) {
      uint16_t parent_member_size = parent_member_sizes[j];
      unsigned parent_member_idx = parent_member_idxs[j];

      if(kernel_depth) {
        unsigned inst0_off = EVOASM_POP_INST_OFF(pop, deme_idx, parent_member_idx, 0);
        evoasm_pop_inst_data_copy(&thread_data->parent_kernel_data, pop->arch_id, parent_off, inst_data, inst0_off,
                                  parent_member_size);
      } else {
        unsigned team_pos0_off = EVOASM_POP_TEAM_POS_OFF(pop, depth, deme_idx, parent_member_idx, 0);
        evoasm_pop_team_pos_data_copy(&thread_data->parent_basic_program_data, parent_off, team_pos_data, team_pos0_off,
                                      parent_member_size);
      }

      parent_off += parent_member_size;
    }

    for(unsigned j = 0; j < 2; j++) {
      uint16_t child_member_size = (uint16_t)
          _evoasm_prng_rand_between(prng, parent_member_sizes[0], parent_member_sizes[1]);

      member_data->sizes[parent_member_offs[j]] = child_member_size;
      member_data->losses[parent_member_offs[j]] = child_member_loss;

      assert(child_member_size > 0);

      /* offset for shorter parent */
      unsigned crossover_point = (unsigned) _evoasm_prng_rand_between(prng,
                                                                      0, child_member_size - parent_member_sizes[1]);
      unsigned crossover_len = (unsigned) _evoasm_prng_rand_between(prng,
                                                                    0, parent_member_sizes[1]);


      if(kernel_depth) {
        unsigned child_inst0_off = EVOASM_POP_INST_OFF(pop, deme_idx, parent_member_idxs[j], 0);
        evoasm_pop_inst_data_copy(inst_data, pop->arch_id, child_inst0_off, &thread_data->parent_kernel_data, 0,
                                  crossover_point);
        evoasm_pop_inst_data_copy(inst_data, pop->arch_id, child_inst0_off + crossover_point,
                                  &thread_data->parent_kernel_data, parent_member_sizes[0] + crossover_point,
                                  crossover_len);
        evoasm_pop_inst_data_copy(inst_data, pop->arch_id, child_inst0_off + crossover_point + crossover_len,
                                  &thread_data->parent_kernel_data, crossover_point + crossover_len,
                                  child_member_size - crossover_point - crossover_len);
      } else {
        unsigned child_team_pos0_off = EVOASM_POP_TEAM_POS_OFF(pop, depth, deme_idx, parent_member_idxs[j], 0);
        evoasm_pop_team_pos_data_copy(team_pos_data, child_team_pos0_off, &thread_data->parent_basic_program_data, 0,
                                      crossover_point);
        evoasm_pop_team_pos_data_copy(team_pos_data, child_team_pos0_off + crossover_point,
                                      &thread_data->parent_basic_program_data, parent_member_sizes[0] + crossover_point,
                                      crossover_len);
        evoasm_pop_team_pos_data_copy(team_pos_data, child_team_pos0_off + crossover_point + crossover_len,
                                      &thread_data->parent_basic_program_data, crossover_point + crossover_len,
                                      child_member_size - crossover_point - crossover_len);
      }
    }
  }
}


evoasm_success_t
evoasm_pop_combine(evoasm_pop_t *pop) {

#pragma omp parallel for collapse(2)
  for(unsigned i = 0; i < pop->params->depth; i++) {
    for(unsigned j = 0; j < pop->params->deme_counts[i]; j++) {
      int tid;
#ifdef _OPENMP
      tid = omp_get_thread_num();
#else
      tid = 1;
#endif
      evoasm_pop_combine_deme(pop, i, j, tid);
    }
  }
  return true;
}

static int loss_compare_func(const void *a, const void *b) {
  evoasm_loss_t loss_a = *(const evoasm_loss_t *) a;
  evoasm_loss_t loss_b = *(const evoasm_loss_t *) b;
  return (loss_a > loss_b) - (loss_a < loss_b);
}

evoasm_success_t
evoasm_pop_get_deme_loss(evoasm_pop_t *pop, unsigned depth, unsigned deme_idx,
                         evoasm_loss_t *summary) {

  evoasm_pop_unit_data_t *member_data = &pop->member_data;
  uint16_t deme_size = pop->params->deme_sizes[depth];
  unsigned member_off = EVOASM_POP_MEMBER_OFF(pop, depth, deme_idx, 0);

  evoasm_loss_t *losses = evoasm_alloca(sizeof(evoasm_loss_t) * deme_size);
  memcpy(losses, &member_data->losses[member_off], sizeof(evoasm_loss_t) * deme_size);
  qsort(losses, deme_size, sizeof(evoasm_loss_t), loss_compare_func);

  summary[0] = losses[0 * deme_size / 4];
  summary[1] = losses[1 * deme_size / 4];
  summary[1] = losses[2 * deme_size / 4];
  summary[2] = losses[3 * deme_size / 4];
  summary[3] = losses[4 * deme_size / 4];

#if 0
  unsigned i;
  double scale = 1.0 / pop->params->size;
  double pop_loss = 0.0;
  *invalid_count = 0;
  for(i = 0; i < pop->params->size; i++) {
    double loss = pop->losses[i];
    if(loss != INFINITY) {
      pop_loss += scale * loss;
    } else {
      (*invalid_count)++;
    }
  }

  if(per_example) pop_loss /= pop->example_count;
#endif

  return true;
}

static void
evoasm_pop_mutate_deme(evoasm_pop_t *pop, unsigned depth, unsigned deme_idx, int tid) {


}

void
evoasm_pop_next_gen(evoasm_pop_t *pop) {

#pragma omp parallel for collapse(2)
  for(unsigned i = 0; i < pop->params->depth; i++) {
    for(unsigned j = 0; j < pop->params->deme_counts[i]; j++) {
      int tid;
      bool deme_retval = true;
#ifdef _OPENMP
      tid = omp_get_thread_num();
#else
      tid = 1;
#endif
      evoasm_pop_select_deme(pop, i, j, tid);
      evoasm_pop_combine_deme(pop, i, j, tid);
      evoasm_pop_mutate_deme(pop, i, j, tid);
    }
  }
}


#if 0

evoasm_pop_select(pop, parent_idxs, pop->params->size);
  {
    double scale = 1.0 / pop->params->program_size;
    double pop_loss = 0.0;
    unsigned inf_count = 0;
    for(i = 0; i < pop->params->program_size; i++) {
      double loss = pop->pop.losses[parent_idxs[i]];
      if(loss != INFINITY) {
        pop_loss += scale * loss;
      }
      else {
        inf_count++;
      }
    }

    evoasm_log_info("pop selected loss: %g/%u", pop_loss, inf_count);
  }

  unsigned i;
  for(i = 0; i < pop->params->program_size; i++) {
    evoasm_program_params_t *program_params = _EVOASM_SEARCH_PROGRAM_PARAMS(pop, pop->pop.indivs, parent_idxs[i]);
    assert(program_params->program_size > 0);
  }

  return evoasm_pop_combine_parents(pop, parent_idxs);
}
#endif


