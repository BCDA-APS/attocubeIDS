#pragma once
#include <array>
#include <asynPortDriver.h>


inline constexpr size_t BUFFER_SIZE = 512;
inline constexpr double IO_TIMEOUT = 1.0;

class AttocubeIDS : public asynPortDriver {
  public:
    AttocubeIDS(const char* conn_port, const char* driver_port);
    virtual void poll(void);
    // virtual asynStatus writeInt32(asynUser* pasynUser, epicsInt32 value);
    // virtual asynStatus writeFloat64(asynUser* pasynUser, epicsFloat64 value);

  private:
    asynUser* pasynUserDriver_; // pointer to the asynUser for this driver
    std::array<char, BUFFER_SIZE> in_buffer_;
    std::array<char, BUFFER_SIZE> out_buffer_;
};
