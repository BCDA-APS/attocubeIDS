# ../../bin/${EPICS_HOST_ARCH}/idsTest st.cmd
< envPaths

dbLoadDatabase("../../dbd/iocidsTestLinux.dbd")
iocidsTestLinux_registerRecordDeviceDriver(pdbbase)

epicsEnvSet("IOCSH_PS1", "$(IOC)>")
epicsEnvSet("PREFIX", "idsTest:")

epicsEnvSet("IDS_PORT", "IDS_COMM")
drvAsynIPPortConfigure("$(IDS_PORT)", "10.54.115.131:9090", 0, 0, 0)

AttocubeIDSConfig("$(IDS_PORT)", "IDS1")

# asynRecord for debugging
dbLoadRecords("$(ASYN)/db/asynRecord.db", "P=$(PREFIX), R=asyn_$(IDS_PORT), PORT=$(IDS_PORT), ADDR=0, OMAX=256, IMAX=256")

###############################################################################
iocInit
###############################################################################

# print the time our boot was finished
date
