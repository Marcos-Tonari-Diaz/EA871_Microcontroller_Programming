#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=mkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=obj
DEBUGGABLE_SUFFIX=obj
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/atividade9.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=obj
DEBUGGABLE_SUFFIX=obj
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/atividade9.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS

else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=atividade9.asm

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/atividade9.obj
POSSIBLE_DEPFILES=${OBJECTDIR}/atividade9.obj.d

# Object Files
OBJECTFILES=${OBJECTDIR}/atividade9.obj

# Source Files
SOURCEFILES=atividade9.asm

# Pack Options 
PACK_ASSEMBLER_OPTIONS=-I "${DFP_DIR}/avrasm/inc"  -i m328Pdef.inc



CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

# The following macros may be used in the pre and post step lines
Device=ATmega328P
ProjectDir=/home/marcos/Documents/6SEM/EA871/atividade9
ProjectName=atividade9
ConfName=default
ImagePath=dist/default/${IMAGE_TYPE}/atividade9.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
ImageDir=dist/default/${IMAGE_TYPE}
ImageName=atividade9.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IsDebug="true"
else
IsDebug="false"
endif

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/atividade9.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
	@echo "--------------------------------------"
	@echo "User defined post-build step: [/home/marcos/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino14/bin/avrdude -C"/home/marcos/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino14/etc/avrdude.conf" -v -patmega328p -carduino -P/dev/ttyACM0 -b115200 -D -Uflash:w:${ProjectDir}/${ImageDir}/${ProjectName}.${IMAGE_TYPE}.hex:i]"
	@/home/marcos/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino14/bin/avrdude -C"/home/marcos/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino14/etc/avrdude.conf" -v -patmega328p -carduino -P/dev/ttyACM0 -b115200 -D -Uflash:w:${ProjectDir}/${ImageDir}/${ProjectName}.${IMAGE_TYPE}.hex:i
	@echo "--------------------------------------"

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/atividade9.obj: atividade9.asm  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/atividade9.obj 
	${MP_AS}  -fI -W+ie ${PACK_ASSEMBLER_OPTIONS} -d dist/${CND_CONF}/${IMAGE_TYPE}/atividade9.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  -m dist/${CND_CONF}/${IMAGE_TYPE}/atividade9.${IMAGE_TYPE}.map  -S dist/${CND_CONF}/${IMAGE_TYPE}/atividade9.${IMAGE_TYPE}.tmp atividade9.asm
else
${OBJECTDIR}/atividade9.obj: atividade9.asm  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/atividade9.obj 
	${MP_AS}  -fI -W+ie ${PACK_ASSEMBLER_OPTIONS} -d dist/${CND_CONF}/${IMAGE_TYPE}/atividade9.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  -S dist/${CND_CONF}/${IMAGE_TYPE}/atividade9.${IMAGE_TYPE}.tmp  -o dist/${CND_CONF}/${IMAGE_TYPE}/atividade9.${IMAGE_TYPE}.hex  -m dist/${CND_CONF}/${IMAGE_TYPE}/atividade9.${IMAGE_TYPE}.map  -l dist/${CND_CONF}/${IMAGE_TYPE}/atividade9.${IMAGE_TYPE}.lss atividade9.asm
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/atividade9.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/atividade9.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell "${PATH_TO_IDE_BIN}"mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
