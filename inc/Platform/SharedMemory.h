#pragma once
#include <memory>
#include <cstdint>
#include "math/Types.h"

namespace CR::Platform {
  struct ISharedMemory {
  protected:
    ISharedMemory() = default;
  public:
    virtual ~ISharedMemory() = default;
    ISharedMemory(const ISharedMemory&) = delete;
    ISharedMemory& operator=(const ISharedMemory&) = delete;

    //follow stl naming convention for compatibility with non member data/size
    virtual std::size_t size() = 0;
    virtual uchar* data() = 0;
  };

  std::unique_ptr<ISharedMemory> CreateSharedMemory(const char* a_name, size_t a_size);
  std::unique_ptr<ISharedMemory> OpenSharedMemory(const char* a_name, size_t a_size);
}