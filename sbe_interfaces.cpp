#include <stdexcept>
#include <vector>
#include "sbe_interfaces.hpp"

namespace openpower
{
namespace sbe
{
namespace scom
{

/** @brief SCOM specific SBE Operations */
enum class SbeScomOperations : uint32_t
{
    READ      = 0x0000A201, /**< Read SCOM register */
    WRITE     = 0x0000A202  /**< Write to SCOM register */
};

constexpr uint64_t higherWordMask = 0xFFFFFFFF00000000ull;
constexpr uint64_t lowerWordMask = 0xFFFFFFFF;



uint64_t read(const char* devPath,
              uint64_t address)
{
    uint64_t value = 0;

    //Validate input device path
    if (devPath == nullptr)
    {
        throw std::runtime_error("NULL FIFO device path");
    }
    //validate input address
    if (!address)
    {
        throw std::runtime_error("Invalid SCOM address");
    }

    //Build SCOM read request command
    std::vector<uint32_t> command;
    //Length of the request command packet
    command.push_back(0x4);
    command.push_back(static_cast<uint32_t>(SbeScomOperations::READ));
    command.push_back((address & higherWordMask) >> 32);
    command.push_back((address & lowerWordMask));

    // TODO: Call an interface to write the command to the SBE FIFO and read the
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
    //validate input address
    if (!address)
    {
        throw std::runtime_error("Invalid SCOM address");
    }

    //Build SCOM write request command
    std::vector<uint32_t> command;
    //Length of the request command packet
    command.push_back(0x6);
    command.push_back(static_cast<uint32_t>(SbeScomOperations::WRITE));
    command.push_back((address & higherWordMask) >> 32);
    command.push_back((address & lowerWordMask));
    command.push_back((data & higherWordMask) >> 32);
    command.push_back((data & lowerWordMask));

    // TODO: Call an interface to write the command to the SBE FIFO and read the
    // response from the SBE FIFO device

}

} // namespace scom
} // namespace sbe
} // namespace openpower

