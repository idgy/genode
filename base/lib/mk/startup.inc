SRC_S   += crt0.s
SRC_CC  += _main.cc

REP_INC_DIR += src/platform

LIBS += syscall

vpath _main.cc $(BASE_DIR)/src/platform
