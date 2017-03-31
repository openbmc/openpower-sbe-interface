#include <stdexcept>
#include <array>
#include "sbe_interfaces.hpp"

namespace openpower
{
namespace sbe
{

//Helper interfaces
static inline uint32_t upper(uint64_t value)
{
    return ((value & 0xFFFFFFFF00000000ull) >> 32);
}

static inline uint32_t lower(uint64_t value)
{
    return (value & 0xFFFFFFFF);
}

namespace scom
{

//Constants specific to SCOM operations
static constexpr uint32_t READ_OPCODE  = 0x0000A201;
static constexpr uint32_t WRITE_OPCODE = 0x0000A202;
static constexpr uint32_t READ_CMD_LENGTH = 0x4;
static constexpr uint32_t WRITE_CMD_LENGTH = 0x6;

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
    std::array<uint32_t, READ_CMD_LENGTH> command =
    {
        READ_CMD_LENGTH,
        READ_OPCODE,
        upper(address),
        lower(address)
    };

    // TODO: Call an interface to read the command to the SBE FIFO and read the
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
    std::array<uint32_t, WRITE_CMD_LENGTH> command =
    {
        WRITE_CMD_LENGTH,
        WRITE_OPCODE,
        upper(address),
        lower(address),
        upper(data),
        lower(data)
    };

    // TODO: Call an interface to write the command to the SBE FIFO and read the
    // response from the SBE FIFO device

}

} // namespace scom
} // namespace sbe
} // namespace openpower
