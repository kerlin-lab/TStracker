PROJ_NAME=ImageWidthHeight
SPINVIEW_DIR=/usr/src/spinnaker
PROJECT_ROOT=../../
OPT_INC = ${PROJECT_ROOT}/common/make/common_spin.mk
-include ${OPT_INC}

################################################################################
# Key paths and settings
################################################################################
CFLAGS += -std=c++11
ifeq ($(wildcard ${OPT_INC}),)
CXX = g++ ${CFLAGS}
ODIR  = .obj/build${D}
SDIR  = .
MKDIR = mkdir -p
PLATFORM = $(shell uname)
ifeq ($(PLATFORM),Darwin)
OS = mac
endif
endif

OUTPUTNAME = ${PROJ_NAME}${D}
OUTDIR = .

################################################################################
# Dependencies
################################################################################
# Spinnaker deps
SPINNAKER_LIB = -L${SPINVIEW_DIR}/lib -lSpinnaker${D} ${SPIN_DEPS}

################################################################################
# Master inc/lib/obj/dep settings
################################################################################
_OBJ = ${PROJ_NAME}.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
INC = -I${SPINVIEW_DIR}/include
ifneq ($(OS),mac)
INC += -I/usr/include/spinnaker
LIB += -Wl,-Bdynamic ${SPINNAKER_LIB}
LIB += -Wl,-rpath-link=${SPINVIEW_DIR}/lib 
else
INC += -I/usr/local/include/spinnaker
LIB += -rpath ${SPINVIEW_DIR}/lib/
LIB += ${SPINNAKER_LIB}
endif

################################################################################
# Rules/recipes
################################################################################
# Final binary
${OUTPUTNAME}: ${OBJ}
	${CXX} -o ${OUTPUTNAME} ${OBJ} ${LIB}

# Intermediate object files
${OBJ}: ${ODIR}/%.o : main.cpp
	@${MKDIR} ${ODIR}
	${CXX} ${CFLAGS} ${INC} -Wall -D LINUX -c $< -o $@

# Clean up intermediate objects
clean_obj:
	rm -f ${OBJ}
	@echo "intermediate objects cleaned up!"

# Clean up everything.
clean: clean_obj
	rm -f ${OUTDIR}/${OUTPUTNAME}
	@echo "all cleaned up!"
