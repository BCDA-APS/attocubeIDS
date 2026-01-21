# NOTE: paths relative to where they get installed (e.g. ./O.rhel9-x86_64)

TOP=../..

include $(TOP)/configure/CONFIG

# C++17 is needed for things like std::optional and structured bindings
USR_CXXFLAGS += -std=c++17

# Define IOC library name
LIBRARY_IOC += attocubeIDS

# install attocubeIDSSupport.dbd into <top>/dbd
DBD += attocubeIDSSupport.dbd
attocubeIDSSupport_DBD += attocubeIDS.dbd

# source files to be compiled and added to the library
attocubeIDS_SRCS += attocubeIDS.cpp

# Libraries needed for attocubeIDS
attocubeIDS_LIBS += asyn
attocubeIDS_LIBS += $(EPICS_BASE_IOC_LIBS)

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
