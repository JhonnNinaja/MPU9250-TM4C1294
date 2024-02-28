
PART=TM4C1294NCPDT

#
# The base directory for TivaWare.
#
ROOT=../../../..

#
# Include the common make definitions.
#
include ${ROOT}/makedefs

VPATH = ../drivers
VPATH+=../../../../utils
#
# Where to find header files that do not live in the source directory.
#
IPATH=..
IPATH+=../../../..

#
# The default rule, which causes the blinky example to be built.
#
all: ${COMPILER}
all: ${COMPILER}/test_mpu9250.axf

#
# The rule to clean out all the build products.
#
clean:
	@rm -rf ${COMPILER} ${wildcard *~}

#
# The rule to create the target directory.
#
${COMPILER}:
	@mkdir -p ${COMPILER}

#
# Rules for building the test_mpu9250 example.
#

${COMPILER}/test_mpu9250.axf: ${COMPILER}/startup_${COMPILER}.o
${COMPILER}/test_mpu9250.axf: ${COMPILER}/test_mpu9250.o
${COMPILER}/test_mpu9250.axf: ${COMPILER}/uartstdio.o
${COMPILER}/test_mpu9250.axf: ${ROOT}/driverlib/${COMPILER}/libdriver.a
${COMPILER}/test_mpu9250.axf: ${ROOT}/sensorlib/${COMPILER}/libsensor.a
${COMPILER}/test_mpu9250.axf: test_mpu9250.ld
SCATTERgcc_test_mpu9250=test_mpu9250.ld
ENTRY_test_mpu9250=ResetISR
CFLAGSgcc=-DTARGET_IS_TM4C129_RA1

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif
