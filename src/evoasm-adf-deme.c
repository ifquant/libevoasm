//
// Created by jap on 9/16/16.
//


#include "evoasm-signal.h"
#include "evoasm-adf-deme.h"
#include "evoasm-deme-params.h"

EVOASM_DEF_LOG_TAG("adf_deme")

#define _EVOASM_DEME_ADF_PARAMS_FULL(deme, adf_index, ptr) \
  ((evoasm_adf_params_t *)((unsigned char *)(deme)->ptr +\
  (adf_index) * EVOASM_ADF_PARAMS_SIZE(deme->params->max_adf_size, deme->params->max_kernel_size)))

#define _EVOASM_DEME_ADF_PARAMS(deme, adf_index) _EVOASM_DEME_ADF_PARAMS_FULL(deme, adf_index, main_adfs)

#define _EVOASM_ADF_PARAMS_KERNEL_PARAMS(adf_params, max_kernel_size, kernel_index) \
  ((evoasm_kernel_params_t *)((unsigned char *)(adf_params) + sizeof(evoasm_adf_params_t) + (kernel_index) * EVOASM_KERNEL_PARAMS_SIZE(max_kernel_size)))

#define EVOASM_SEARCH_PROLOG_EPILOG_SIZE UINT32_C(1024)


static bool
evoasm_adf_deme_destroy_(evoasm_adf_deme_t *deme, bool free_buf, bool free_body_buf) {
  bool retval = true;

  evoasm_deme_destroy(&deme->deme);

  if(free_buf) EVOASM_TRY(buf_free_failed, evoasm_buf_destroy, &deme->buf);

cleanup:
  if(free_body_buf) EVOASM_TRY(body_buf_failed, evoasm_buf_destroy, &deme->body_buf);
  return retval;

buf_free_failed:
  retval = false;
  goto cleanup;

body_buf_failed:
  return false;
}

evoasm_adf_deme_params_t *
evoasm_adf_deme_params(evoasm_adf_deme_t *adf_deme) {
  return (evoasm_adf_deme_params_t *) adf_deme->deme.params;
}

evoasm_success_t
evoasm_adf_deme_init_domains(evoasm_adf_deme_t *adf_deme) {
  unsigned i, j, k;
  evoasm_domain_t cloned_domain;
  evoasm_deme_t *deme = &adf_deme->deme;

  evoasm_adf_deme_params_t *params = evoasm_adf_deme_params(adf_deme);

  size_t domains_len = (size_t) (params->n_insts * params->deme_params.n_params);
  deme->domains = evoasm_calloc(domains_len,
                                sizeof(evoasm_domain_t));

  if(!adf_deme->deme.domains) goto fail;

  for(i = 0; i < params->n_insts; i++) {
    evoasm_x64_inst_t *inst = _evoasm_x64_inst(params->inst_ids[i]);
    for(j = 0; j < params->deme_params.n_params; j++) {
      evoasm_domain_t *inst_domain = &deme->domains[i * params->deme_params.n_params + j];
      evoasm_param_id_t param_id = params->deme_params.param_ids[j];
      for(k = 0; k < inst->n_params; k++) {
        evoasm_param_t *param = &inst->params[k];
        if(param->id == param_id) {
          evoasm_domain_t *user_domain = params->deme_params.domains[param_id];
          if(user_domain != NULL) {
            if(evoasm_domain_empty(user_domain)) goto empty_domain;

            evoasm_domain_clone(user_domain, &cloned_domain);
            evoasm_domain_intersect(&cloned_domain, param->domain, inst_domain);
            if(evoasm_domain_empty(inst_domain)) goto empty_domain;
          } else {
            evoasm_domain_clone(param->domain, inst_domain);
          }
          goto found;
        }
      }
      /* not found */
      inst_domain->type = EVOASM_N_DOMAIN_TYPES;
found:;
    }
  }

  /*
  for(i = 0; i < domains_len; i++) {
    evoasm_domain_log(&adf_deme->domains[i], EVOASM_LOG_LEVEL_WARN);
  }*/

  return true;

fail:
  return false;

empty_domain:
  evoasm_set_error(EVOASM_ERROR_TYPE_ARG, EVOASM_N_ERROR_CODES,
                   NULL, "Empty domain");
  return false;
}


static evoasm_success_t
evoasm_adf_deme_destroy(evoasm_adf_deme_t *deme) {
  return evoasm_adf_deme_destroy_(deme, true, true);
}


static void
evoasm_adf_deme_x64_seed_kernel_param(evoasm_adf_deme_t *adf_deme, evoasm_x64_kernel_param_t *kernel_param) {
  unsigned i;
  unsigned n_params = adf_deme->deme.params->n_params;
  evoasm_prng_t *prng = &adf_deme->deme.prng;
  evoasm_adf_deme_params_t *params = evoasm_adf_deme_params(adf_deme);

  int64_t inst_idx = _evoasm_prng_rand_between(prng, 0, params->n_insts - 1);
  evoasm_inst_id_t inst = params->inst_ids[inst_idx];

  kernel_param->inst = (unsigned) inst & EVOASM_X64_INST_BITMASK;

  /* set parameters */
  for(i = 0; i < n_params; i++) {
    evoasm_domain_t *domain = &adf_deme->deme.domains[inst_idx * n_params + i];
    if(domain->type < EVOASM_N_DOMAIN_TYPES) {
      evoasm_x64_param_id_t param_id = (evoasm_x64_param_id_t) adf_deme->deme.params->param_ids[i];
      evoasm_param_val_t param_val;

      param_val = (evoasm_param_val_t) evoasm_domain_rand(domain, prng);
      _evoasm_x64_basic_params_set(&kernel_param->params, param_id, param_val);
    }
  }
}

static void
evoasm_adf_deme_seed_kernel_param(evoasm_adf_deme_t *deme, evoasm_kernel_param_t *kernel_param) {
  switch(deme->arch_info->id) {
    case EVOASM_ARCH_X64: {
      evoasm_adf_deme_x64_seed_kernel_param(deme, &kernel_param->x64);
      break;
    }
    default:
      evoasm_assert_not_reached();
  }
}

static void
evoasm_adf_deme_seed_kernel(evoasm_adf_deme_t *adf_deme, evoasm_kernel_params_t *kernel_params,
                            evoasm_adf_size_t adf_size) {
  unsigned i;

  evoasm_prng_t *prng = &adf_deme->deme.prng;
  evoasm_adf_deme_params_t *params = evoasm_adf_deme_params(adf_deme);

  evoasm_kernel_size_t kernel_size = (evoasm_kernel_size_t) _evoasm_prng_rand_between(prng,
                                                                                      params->min_kernel_size,
                                                                                      params->max_kernel_size);

  assert(kernel_size > 0);
  kernel_params->size = kernel_size;
  kernel_params->jmp_selector = (uint8_t) _evoasm_prng_rand8(prng);
  kernel_params->alt_succ_idx = (evoasm_kernel_size_t)
      _evoasm_prng_rand_between(prng, 0, adf_size - 1);

  for(i = 0; i < kernel_size; i++) {
    evoasm_adf_deme_seed_kernel_param(adf_deme, &kernel_params->params[i]);
  }
}

static void
evoasm_adf_deme_seed_adf(evoasm_adf_deme_t *adf_deme, evoasm_adf_params_t *adf_params) {
  unsigned i;

  evoasm_prng_t *prng = &adf_deme->deme.prng;
  evoasm_adf_deme_params_t *params = evoasm_adf_deme_params(adf_deme);
  evoasm_adf_size_t adf_size = (evoasm_adf_size_t) _evoasm_prng_rand_between(prng,
                                                                             params->min_adf_size,
                                                                             params->max_adf_size);

  assert(adf_size > 0);
  adf_params->size = adf_size;

  for(i = 0; i < adf_size; i++) {
    evoasm_kernel_params_t *kernel_params = _EVOASM_ADF_PARAMS_KERNEL_PARAMS(adf_params,
                                                                             params->max_kernel_size,
                                                                             i);

    evoasm_adf_deme_seed_kernel(adf_deme, kernel_params, adf_size);
  }
}

static evoasm_success_t
evoasm_adf_deme_seed_indiv(evoasm_deme_t *deme, evoasm_indiv_t *indiv) {
  evoasm_adf_deme_seed_adf((evoasm_adf_deme_t *) deme, (evoasm_adf_params_t *) indiv);
  return true;
}

static evoasm_success_t
evoasm_adf_deme_eval_adf(evoasm_adf_deme_t *adf_deme,
                         evoasm_adf_t *adf,
                         evoasm_loss_t *loss) {

  evoasm_kernel_t *kernel = &adf->kernels[adf->params->size - 1];
  evoasm_adf_deme_params_t *params = evoasm_adf_deme_params(adf_deme);

  if(!evoasm_adf_emit(adf, params->adf_input, true, true, true, true)) {
    *loss = INFINITY;
    return false;
  }

  if(EVOASM_UNLIKELY(kernel->n_output_regs == 0)) {
    *loss = INFINITY;
    return true;
  }

  //evoasm_buf_log(adf->buf, EVOASM_LOG_LEVEL_INFO);
  {
    evoasm_signal_set_exception_mask(adf->exception_mask);

    if(EVOASM_SIGNAL_TRY()) {
      evoasm_buf_exec(adf->buf);
      *loss = evoasm_adf_assess(adf, params->adf_output);
    } else {
      evoasm_debug("adf %p signaled", adf);
      *loss = INFINITY;
    }
  }
  return true;
}


static evoasm_success_t
evoasm_adf_deme_eval_setup(evoasm_deme_t *deme) {
  evoasm_adf_deme_t *adf_deme = (evoasm_adf_deme_t *) deme;
  evoasm_signal_install((evoasm_arch_id_t) adf_deme->arch_info->id, 0);
  return true;
}

static evoasm_success_t
evoasm_adf_deme_eval_teardown(evoasm_deme_t *deme) {
  evoasm_adf_deme_t *adf_deme = (evoasm_adf_deme_t *) deme;
  evoasm_signal_install((evoasm_arch_id_t) adf_deme->arch_info->id, 0);
}


static evoasm_success_t bool
evoasm_adf_eval_adf_params(evoasm_adf_deme_t *adf_deme, evoasm_adf_params_t *adf_params, evoasm_loss_t *loss) {
  evoasm_adf_deme_params_t *params = evoasm_adf_deme_params(adf_deme);
  bool retval = true;

  /* encode solution */
  evoasm_adf_t adf = {
      .params = adf_params,
      .recur_limit = params->recur_limit,
      .buf = &adf_deme->buf,
      .body_buf = &adf_deme->body_buf,
      .arch_info = adf_deme->arch_info,
  };

  adf.output_vals = adf_deme->output_vals;

  unsigned i;
  for(i = 0; i < adf_params->size; i++) {
    evoasm_kernel_t *kernel = &adf.kernels[i];
    kernel->params = _EVOASM_ADF_PARAMS_KERNEL_PARAMS(adf_params, params->max_kernel_size, i);
    kernel->idx = (evoasm_adf_size_t) i;
  }

  return evoasm_adf_deme_eval_adf(adf_deme, &adf, loss);
}

static evoasm_success_t
evoasm_adf_deme_eval_indiv(evoasm_deme_t *deme, evoasm_indiv_t *indiv, evoasm_loss_t *loss) {
  return evoasm_adf_eval_adf_params((evoasm_adf_deme_t *) deme, (evoasm_adf_params_t *) indiv, loss);
}


static void
evoasm_adf_deme_mutate_kernel(evoasm_adf_deme_t *adf_deme, evoasm_kernel_params_t *child) {

  evoasm_prng_t *prng = &adf_deme->deme.prng;
  evoasm_adf_deme_params_t *params = evoasm_adf_deme_params(adf_deme);
  uint32_t mut_rate = adf_deme->deme.params->mut_rate;

  uint32_t r = _evoasm_prng_rand32(prng);
  evoasm_debug("mutating child: %u < %u", r, mut_rate);
  if(r < mut_rate) {

    r = _evoasm_prng_rand32(prng);
    if(child->size > params->min_kernel_size && r < UINT32_MAX / 16) {
      uint32_t index = r % child->size;

      if(index < (uint32_t) (child->size - 1)) {
        memmove(child->params + index, child->params + index + 1,
                (child->size - index - 1) * sizeof(evoasm_kernel_param_t));
      }
      child->size--;
    }

    r = _evoasm_prng_rand32(prng);
    {
      evoasm_kernel_param_t *param = child->params + (r % child->size);
      evoasm_adf_deme_seed_kernel_param(adf_deme, param);
    }
  }
}

static void
evoasm_adf_deme_crossover_kernel(evoasm_adf_deme_t *adf_deme,
                                 evoasm_kernel_params_t *parent_a,
                                 evoasm_kernel_params_t *parent_b,
                                 evoasm_kernel_params_t *child) {

  /* NOTE: parent_a must be the longer parent, i.e. parent_size_a >= parent_size_b */
  evoasm_kernel_size_t child_size;
  unsigned crossover_point, crossover_len, i;
  evoasm_prng_t *prng = &adf_deme->deme.prng;

  assert(parent_a->size >= parent_b->size);

  child_size = (evoasm_kernel_size_t)
      _evoasm_prng_rand_between(prng,
                                parent_b->size, parent_a->size);

  assert(child_size > 0);
  assert(child_size >= parent_b->size);

  /* offset for shorter parent */
  crossover_point = (unsigned) _evoasm_prng_rand_between(prng,
                                                         0, child_size - parent_b->size);
  crossover_len = (unsigned) _evoasm_prng_rand_between(prng,
                                                       0, parent_b->size);


  for(i = 0; i < child_size; i++) {
    unsigned index;
    evoasm_kernel_params_t *parent;

    if(i < crossover_point || i >= crossover_point + crossover_len) {
      parent = parent_a;
      index = i;
    } else {
      parent = parent_b;
      index = i - crossover_point;
    }
    child->params[i] = parent->params[index];
  }
  child->size = child_size;

  evoasm_adf_deme_mutate_kernel(adf_deme, child);
}


static void
evoasm_adf_deme_crossover_adf_param(evoasm_adf_deme_t *adf_deme, evoasm_adf_params_t *parent_a,
                                    evoasm_adf_params_t *parent_b,
                                    evoasm_adf_params_t *child) {

  /* NOTE: parent_a must be the longer parent, i.e. parent_size_a >= parent_size_b */
  evoasm_adf_size_t child_size;
  unsigned i;

  evoasm_prng_t *prng = &adf_deme->deme.prng;
  evoasm_adf_deme_params_t *params = evoasm_adf_deme_params(adf_deme);
  unsigned max_kernel_size = params->max_kernel_size;

  assert(parent_a->size >= parent_b->size);
  assert(parent_a->size > 0);
  assert(parent_b->size > 0);

  child_size = (evoasm_adf_size_t)
      _evoasm_prng_rand_between(prng,
                                parent_b->size, parent_a->size);

  assert(child_size > 0);
  assert(child_size >= parent_b->size);


  for(i = 0; i < child_size; i++) {
    evoasm_kernel_params_t *kernel_child = _EVOASM_ADF_PARAMS_KERNEL_PARAMS(child, max_kernel_size, i);

    if(i < parent_b->size) {
      evoasm_kernel_params_t *kernel_parent_a = _EVOASM_ADF_PARAMS_KERNEL_PARAMS(parent_a, max_kernel_size, i);
      evoasm_kernel_params_t *kernel_parent_b = _EVOASM_ADF_PARAMS_KERNEL_PARAMS(parent_b, max_kernel_size, i);

      if(kernel_parent_a->size < kernel_parent_b->size) {
        evoasm_kernel_params_t *t = kernel_parent_a;
        kernel_parent_a = kernel_parent_b;
        kernel_parent_b = t;
      }

      evoasm_adf_deme_crossover_kernel(adf_deme, kernel_parent_a, kernel_parent_b, kernel_child);
    } else {
      memcpy(kernel_child, parent_a, EVOASM_KERNEL_PARAMS_SIZE(max_kernel_size));
      evoasm_adf_deme_mutate_kernel(adf_deme, kernel_child);
    }
  }
  child->size = child_size;
}

static evoasm_success_t
evoasm_adf_deme_crossover_adf_params(evoasm_adf_deme_t *deme, evoasm_adf_params_t *parent_a, evoasm_adf_params_t *parent_b,
                          evoasm_adf_params_t *child_a, evoasm_adf_params_t *child_b) {

  if(parent_a->size < parent_b->size) {
    evoasm_adf_params_t *t = parent_a;
    parent_a = parent_b;
    parent_b = t;
  }

  //memcpy(_EVOASM_SEARCH_ADF_PARAMS(search, indivs, index), parent_a, EVOASM_ADF_PARAMS_SIZE(search));
  //memcpy(_EVOASM_SEARCH_ADF_PARAMS(search, indivs, index + 1), parent_a, EVOASM_ADF_PARAMS_SIZE(search));

  evoasm_adf_deme_crossover_adf_param(deme, parent_a, parent_b, child_a);
  evoasm_adf_deme_crossover_adf_param(deme, parent_a, parent_b, child_b);

  return true;
}

static evoasm_success_t
evoasm_adf_deme_crossover(evoasm_deme_t *deme,
                          evoasm_indiv_t *parent_a,
                          evoasm_indiv_t *parent_b,
                          evoasm_indiv_t *child_a,
                          evoasm_indiv_t *child_b) {
  return evoasm_adf_deme_crossover_adf_params((evoasm_adf_deme_t *) deme,
                                       (evoasm_adf_params_t *) parent_a,
                                       (evoasm_adf_params_t *) parent_b,
                                       (evoasm_adf_params_t *) child_a,
                                       (evoasm_adf_params_t *) child_a);
}

static const evoasm_deme_cls_t _evoasm_adf_deme_cls = {
    .seed_indiv_func = evoasm_adf_deme_seed_indiv,
    .eval_setup_func = evoasm_adf_deme_eval_setup,
    .eval_teardown_func = evoasm_adf_deme_eval_teardown,
    .eval_indiv_func = evoasm_adf_deme_eval_indiv,
    .crossover_func = evoasm_adf_deme_crossover
};

static evoasm_success_t
evoasm_adf_deme_init(evoasm_adf_deme_t *adf_deme, evoasm_arch_id_t arch_id, evoasm_adf_deme_params_t *params) {

  if(!evoasm_adf_deme_params_valid(params)) {
    return false;
  }

  unsigned n_examples = EVOASM_ADF_INPUT_N_EXAMPLES(params->adf_input);
  size_t indiv_size = EVOASM_ADF_PARAMS_SIZE(params->max_adf_size, params->max_kernel_size);

  if(!evoasm_deme_init(&adf_deme->deme, (evoasm_deme_params_t *) params, &_evoasm_adf_deme_cls, indiv_size,
                       n_examples)) {
    return false;
  }

  adf_deme->arch_info = evoasm_arch_info(arch_id);
  adf_deme->output_vals = evoasm_malloc(EVOASM_ADF_OUTPUT_VALS_SIZE(params->adf_input));
  if(!adf_deme->output_vals) goto alloc_failed;

  /* FIXME: find a way to calculate tighter bound */
  size_t body_buf_size = (size_t) (2 * params->max_adf_size * params->max_kernel_size *
                                   adf_deme->arch_info->max_inst_len);
  size_t buf_size = n_examples * (body_buf_size + EVOASM_SEARCH_PROLOG_EPILOG_SIZE);

  EVOASM_TRY(domains_init_failed, evoasm_adf_deme_init_domains, adf_deme);

  EVOASM_TRY(buf_alloc_failed, evoasm_buf_init, &adf_deme->buf, EVOASM_BUF_TYPE_MMAP, buf_size);
  EVOASM_TRY(body_buf_alloc_failed, evoasm_buf_init, &adf_deme->body_buf, EVOASM_BUF_TYPE_MALLOC, body_buf_size);

  EVOASM_TRY(prot_failed, evoasm_buf_protect, &adf_deme->buf,
             EVOASM_MPROT_RWX);


  return true;

alloc_failed:
  return false;

domains_init_failed:
buf_alloc_failed:
  evoasm_adf_deme_destroy_(adf_deme, false, false);
  return false;

body_buf_alloc_failed:
  evoasm_adf_deme_destroy_(adf_deme, true, false);
  return false;

prot_failed:
  evoasm_adf_deme_destroy_(adf_deme, true, true);
  return false;
}