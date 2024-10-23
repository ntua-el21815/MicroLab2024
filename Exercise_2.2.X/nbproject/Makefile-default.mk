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
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=obj
DEBUGGABLE_SUFFIX=obj
FINAL_IMAGE=${DISTDIR}/Exercise_2.2.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=obj
DEBUGGABLE_SUFFIX=obj
FINAL_IMAGE=${DISTDIR}/Exercise_2.2.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=
else
COMPARISON_BUILD=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=Exercise2_A.asm Exercise2_B.asm

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/Exercise2_A.obj ${OBJECTDIR}/Exercise2_B.obj
POSSIBLE_DEPFILES=${OBJECTDIR}/Exercise2_A.obj.d ${OBJECTDIR}/Exercise2_B.obj.d

# Object Files
OBJECTFILES=${OBJECTDIR}/Exercise2_A.obj ${OBJECTDIR}/Exercise2_B.obj

# Source Files
SOURCEFILES=Exercise2_A.asm Exercise2_B.asm

# Pack Options 
PACK_ASSEMBLER_OPTIONS=-I "${DFP_DIR}/avrasm/inc"  -i m328PBdef.inc



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

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk ${DISTDIR}/Exercise_2.2.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/Exercise2_A.obj: Exercise2_A.asm  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} ${DISTDIR} 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Exercise2_A.obj 
	${MP_AS}  -fI -W+ie ${PACK_ASSEMBLER_OPTIONS} -d ${DISTDIR}/Exercise_2.2.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  -m ${DISTDIR}/Exercise_2.2.X.${IMAGE_TYPE}.map  -S ${DISTDIR}/Exercise_2.2.X.${IMAGE_TYPE}.tmp Exercise2_A.asm
${OBJECTDIR}/Exercise2_B.obj: Exercise2_B.asm  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} ${DISTDIR} 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Exercise2_B.obj 
	${MP_AS}  -fI -W+ie ${PACK_ASSEMBLER_OPTIONS} -d ${DISTDIR}/Exercise_2.2.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  -m ${DISTDIR}/Exercise_2.2.X.${IMAGE_TYPE}.map  -S ${DISTDIR}/Exercise_2.2.X.${IMAGE_TYPE}.tmp Exercise2_B.asm
else
${OBJECTDIR}/Exercise2_A.obj: Exercise2_A.asm  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} ${DISTDIR} 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Exercise2_A.obj 
	${MP_AS}  -fI -W+ie ${PACK_ASSEMBLER_OPTIONS} -d ${DISTDIR}/Exercise_2.2.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  -S ${DISTDIR}/Exercise_2.2.X.${IMAGE_TYPE}.tmp  -o ${DISTDIR}/Exercise_2.2.X.${IMAGE_TYPE}.hex  -m ${DISTDIR}/Exercise_2.2.X.${IMAGE_TYPE}.map  -l ${DISTDIR}/Exercise_2.2.X.${IMAGE_TYPE}.lss Exercise2_A.asm
${OBJECTDIR}/Exercise2_B.obj: Exercise2_B.asm  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} ${DISTDIR} 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Exercise2_B.obj 
	${MP_AS}  -fI -W+ie ${PACK_ASSEMBLER_OPTIONS} -d ${DISTDIR}/Exercise_2.2.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  -S ${DISTDIR}/Exercise_2.2.X.${IMAGE_TYPE}.tmp  -o ${DISTDIR}/Exercise_2.2.X.${IMAGE_TYPE}.hex  -m ${DISTDIR}/Exercise_2.2.X.${IMAGE_TYPE}.map  -l ${DISTDIR}/Exercise_2.2.X.${IMAGE_TYPE}.lss Exercise2_B.asm
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${DISTDIR}/Exercise_2.2.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	
else
${DISTDIR}/Exercise_2.2.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${OBJECTDIR}
	${RM} -r ${DISTDIR}

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(wildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
