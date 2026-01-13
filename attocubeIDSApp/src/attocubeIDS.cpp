#include <iostream>

#include <asynOctetSyncIO.h>
#include <epicsExport.h>
#include <epicsThread.h>
#include <iocsh.h>

#include "json.hpp"
using json = nlohmann::json;

#include "attocubeIDS.hpp"

static void poll_thread_C(void* pPvt) {
    AttocubeIDS* pAttocubeIDS = (AttocubeIDS*)pPvt;
    pAttocubeIDS->poll();
}

constexpr int MAX_CONTROLLERS = 1;
constexpr int INTERFACE_MASK = asynInt32Mask | asynFloat64Mask | asynOctetMask | asynDrvUserMask;
constexpr int INTERRUPT_MASK = asynInt32Mask | asynFloat64Mask | asynOctetMask;
constexpr int ASYN_FLAGS = ASYN_MULTIDEVICE | ASYN_CANBLOCK;

AttocubeIDS::AttocubeIDS(const char* conn_port, const char* driver_port)
    : asynPortDriver(driver_port, MAX_CONTROLLERS, INTERFACE_MASK, INTERRUPT_MASK, ASYN_FLAGS, 1, 0, 0) {

    asynStatus status = pasynOctetSyncIO->connect(conn_port, 0, &pasynUserDriver_, NULL);
    pasynOctetSyncIO->setInputEos(pasynUserDriver_, "}", 1);
    if (status) {
        asynPrint(pasynUserDriver_, ASYN_TRACE_ERROR, "Failed to connect to Attocube IDS3010\n");
        return;
    }

    epicsThreadCreate("AttocubeIDSPoller", epicsThreadPriorityLow,
                      epicsThreadGetStackSize(epicsThreadStackMedium), (EPICSTHREADFUNC)poll_thread_C, this);
}

void AttocubeIDS::poll() {
    while (true) {
	lock();

	json rpc;
        rpc["jsonrpc"] = "2.0";
	rpc["method"] = "com.attocube.ids.displacement.getAxisDisplacement";
	rpc["params"] = {0};
        rpc["id"] = 1;
	std::string out_str = rpc.dump();
	std::copy(out_str.begin(), out_str.end(), out_buffer_.begin());
	std::cout << "out_buffer_ = " << out_buffer_.data() << std::endl;

	size_t nbytesout = 0;
	size_t nbytesin = 0;
	int eom_reason = 0;
	asynStatus status = pasynOctetSyncIO->writeRead(
		pasynUserDriver_,
		out_buffer_.data(), out_buffer_.size(),
		in_buffer_.data(), in_buffer_.size(),
		IO_TIMEOUT,
		&nbytesout, &nbytesin, &eom_reason);

	std::cout << "sent " << nbytesout << " bytes\n";
	std::cout << "recieved " << nbytesin << " bytes\n";
	std::cout << "in_buffer_ = " << in_buffer_.data() << std::endl;
	if (status) {
	    std::cout << "Error!\n";
	} else {
	    try {
		std::string buffer_str(in_buffer_.begin(), in_buffer_.begin()+nbytesin);
		buffer_str.push_back('}');
		std::cout << "buffer_str: " << buffer_str << "\n";
		json data = json::parse(buffer_str);
		if (data.contains("result")) {
		    std::vector<int> results = data["result"].get<std::vector<int>>();
		    std::cout << "position: " << results.at(1) << "\n";
		}
	    } catch (...) {
		std::cout << "parse failed\n";
	    }
	}

        callParamCallbacks();
	unlock();
	epicsThreadSleep(1.0);
    }
}

// asynStatus AttocubeIDS::writeInt32(asynUser* pasynUser, epicsInt32 value) {
    // // int function = pasynUser->reason;
    // bool comm_ok = true;
//
    // callParamCallbacks();
    // return comm_ok ? asynSuccess : asynError;
// }
//
// asynStatus AttocubeIDS::writeFloat64(asynUser* pasynUser, epicsFloat64 value) {
    // // int function = pasynUser->reason;
    // bool comm_ok = true;
//
    // callParamCallbacks();
    // return comm_ok ? asynSuccess : asynError;
// }

// register function for iocsh
extern "C" int AttocubeIDSConfig(const char* conn_port, const char* driver_port) {
    AttocubeIDS* pAttocubeIDS = new AttocubeIDS(conn_port, driver_port);
    pAttocubeIDS = NULL;
    (void)pAttocubeIDS;
    return (asynSuccess);
}

static const iocshArg AttocubeIDSArg0 = {"Connection asyn port", iocshArgString};
static const iocshArg AttocubeIDSArg1 = {"Driver asyn port", iocshArgString};
static const iocshArg* const AttocubeIDSArgs[2] = {&AttocubeIDSArg0, &AttocubeIDSArg1};
static const iocshFuncDef AttocubeIDSFuncDef = {"AttocubeIDSConfig", 2, AttocubeIDSArgs};

static void AttocubeIDSCallFunc(const iocshArgBuf* args) {
    AttocubeIDSConfig(args[0].sval, args[1].sval);
}

void AttocubeIDSRegister(void) { iocshRegister(&AttocubeIDSFuncDef, AttocubeIDSCallFunc); }

extern "C" {
epicsExportRegistrar(AttocubeIDSRegister);
}
