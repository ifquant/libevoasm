/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2016, Julian Aron Prenner <jap@polyadic.com>
 */

#include "evoasm-signal.h"

#ifdef EVOASM_UNIX

#define EVOASM_SIGNAL_EXCEPTION_MASK_GET(exc) (_evoasm_signal_ctx.exception_mask & (1u << exc))

_Thread_local evoasm_signal_ctx_t _evoasm_signal_ctx;

static void
evoasm_signal_handler(int sig, siginfo_t *siginfo, void *ctx) {
  bool handle = false;

  switch(_evoasm_signal_ctx.arch_id) {
    case EVOASM_ARCH_X64: {
      switch(sig) {
        case SIGFPE: {
          int exception = EVOASM_X64_EXCEPTION_DE;
          bool catch_div_by_zero = siginfo->si_code == FPE_INTDIV &&
                                   EVOASM_SIGNAL_EXCEPTION_MASK_GET(exception);
          _evoasm_signal_ctx.last_exception = exception;
          handle = catch_div_by_zero;
          break;
        }
        default:
          break;
      }
      break;
    }
    default:
      evoasm_assert_not_reached();
  }

  if(handle) {
    siglongjmp(_evoasm_signal_ctx.env, 1);
  } else {
    raise(sig);
  }
}

void
evoasm_signal_install(evoasm_arch_id_t arch_id) {
  struct sigaction action = {0};

  _evoasm_signal_ctx.arch_id = (volatile evoasm_arch_id_t) arch_id;
  _evoasm_signal_ctx.exception_mask = 0;
  _evoasm_signal_ctx.last_exception = 0;

  action.sa_sigaction = evoasm_signal_handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = SA_SIGINFO;

  if(sigaction(SIGFPE, &action, &_evoasm_signal_ctx.prev_action) < 0) {
    perror("sigaction");
    exit(1);
  }
}

void
evoasm_signal_set_exception_mask(uint64_t exception_mask) {
  _evoasm_signal_ctx.exception_mask = exception_mask;
}

void
evoasm_signal_uninstall() {
  if(sigaction(SIGFPE, &_evoasm_signal_ctx.prev_action, NULL) < 0) {
    perror("sigaction");
    exit(1);
  }
}

void *
evoasm_signal_try(evoasm_arch_id_t arch, uint64_t exception_mask, evoasm_signal_try_func_t try_func,
                  evoasm_signal_catch_func_t catch_func, void *data) {

  void *retval = NULL;
  evoasm_signal_install(arch);
  evoasm_signal_set_exception_mask(exception_mask);

  if(EVOASM_SIGNAL_TRY()) {
    retval = try_func(data);
  } else {
    retval = catch_func(_evoasm_signal_ctx.last_exception, data);
  }
  evoasm_signal_uninstall();
  return retval;
}

#else
#error
#endif
