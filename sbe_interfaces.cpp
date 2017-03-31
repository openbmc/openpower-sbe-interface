#include <stdexcept>
#include <array>
#include "sbe_interfaces.hpp"

namespace openpower
{
namespace sbe
{
namespace scom
{

//Constants specific to SCOM operations
constexpr uint32_t READ_OP  = 0x0000A201;
constexpr uint32_t WRITE_OP = 0x0000A202;

//Helper interfaces
static inline uint32_t upper(uint64_t value)
{
    return (( value & 0xFFFFFFFF00000000ull) >> 32);
}

static inline uint32_t lower(uint64_t value)
{
    return (value & 0xFFFFFFFF);
}

//Reading SCOM Registers
uint64_t read(const char* devPath,
              uint64_t address)
{
    uint64_t value = 0;

    //Validate input device path
    if (devPath == nullptr)
    {
        throw std::runtime_error("NULL FIFO device path");
    }

    //Build SCOM read request command
    std::array<uint32_t,4> command = 
    {
        0x4,READ_OP,upper(address),lower(address)
    };

    // response from the SBE FIFO device

    return value;

}

void write(const char* devPath,
           uint64_t address,
           uint64_t data)
{
    //Validate input device path
    if (devPath == nullptr)
    {
        throw std::runtime_error("NULL FIFO device path");
    }

    //Build SCOM write request command
    std::array<uint32_t,6> command = 
    {
        0x6,WRITE_OP,upper(address),lower(address),upper(data),lower(data)
    };

    // TODO: Call an interface to write the command to the SBE FIFO and read the
    // response from the SBE FIFO device

}

} // namespace scom
} // namespace sbe
} // namespace openpower

