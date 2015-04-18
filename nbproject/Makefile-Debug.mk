#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/1303157189/MySqlApi.o \
	${OBJECTDIR}/_ext/1303157189/MySqlApiImpl.o \
	${OBJECTDIR}/_ext/1303157189/MySqlException.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libMySqlApi.${CND_DLIB_EXT}

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libMySqlApi.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libMySqlApi.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -shared -fPIC

${OBJECTDIR}/_ext/1303157189/MySqlApi.o: /home/max/cpp/MySqlApi/src/MySqlApi.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1303157189
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/mysql -I../My_1_5 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1303157189/MySqlApi.o /home/max/cpp/MySqlApi/src/MySqlApi.cpp

${OBJECTDIR}/_ext/1303157189/MySqlApiImpl.o: /home/max/cpp/MySqlApi/src/MySqlApiImpl.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1303157189
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/mysql -I../My_1_5 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1303157189/MySqlApiImpl.o /home/max/cpp/MySqlApi/src/MySqlApiImpl.cpp

${OBJECTDIR}/_ext/1303157189/MySqlException.o: /home/max/cpp/MySqlApi/src/MySqlException.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1303157189
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/mysql -I../My_1_5 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1303157189/MySqlException.o /home/max/cpp/MySqlApi/src/MySqlException.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libMySqlApi.${CND_DLIB_EXT}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
