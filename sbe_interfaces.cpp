#include <iostream>
#include <stdexcept>
#include <array>
#include <endian.h>
#include "sbe_interfaces.hpp"
#include "sbe_chipOp_handler.hpp"

namespace openpower
{
namespace sbe
{

constexpr size_t RESP_HEADER_LEN = 0x3;

//Helper interfaces
static inline uint32_t upper(uint64_t value)
{
    return ((value & 0xFFFFFFFF00000000ull) >> 32);
}

static inline uint32_t lower(uint64_t value)
{
    return (value & 0xFFFFFFFF);
}

using sbe_word_t = uint32_t;

namespace scom
{

//Constants specific to SCOM operations
static constexpr sbe_word_t READ_OPCODE  = 0x0000A201;
static constexpr sbe_word_t WRITE_OPCODE = 0x0000A202;
static constexpr size_t READ_CMD_LENGTH = 0x4;
static constexpr size_t WRITE_CMD_LENGTH = 0x6;
static constexpr size_t READ_RESP_LENGTH = 0x2;

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

    //Build SCOM read request command.
    //Handle byte order mismatch ,SBE is big endian and BMC is
    //little endian.
    std::array<sbe_word_t, READ_CMD_LENGTH> command =
    {
        static_cast<sbe_word_t>(htobe32(READ_CMD_LENGTH)),
        htobe32(READ_OPCODE),
        htobe32(upper(address)),
        htobe32(lower(address))
    };

    //Buffer to hold the response data along with the SBE header
    const size_t respLength = RESP_HEADER_LEN + READ_RESP_LENGTH ;
    std::array<sbe_word_t, respLength> response = {};

    //Write the command buffer to the SBE FIFO and obtain the response from the
    //SBE FIFO device.This interface will parse the obtained SBE response and
    //any internal SBE failures will be communicated via exceptions
    invokeSBEChipOperation(devPath, command, response);

    value = (((static_cast<uint64_t>(response[0])) << 32) | response[1]);
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
    //Handle byte order mismatch, SBE is big endian and BMC is
    //little endian.
    std::array<sbe_word_t, WRITE_CMD_LENGTH> command =
    {
        static_cast<sbe_word_t>(htobe32(WRITE_CMD_LENGTH)),
        htobe32(WRITE_OPCODE),
        htobe32(upper(address)),
        htobe32(lower(address)),
        htobe32(upper(data)),
        htobe32(lower(data))
    };
  
    //Buffer to hold the SBE response status
    const size_t respLength = RESP_HEADER_LEN;
    std::array<sbe_word_t, respLength> response = {};

    //Write the command buffer to the SBE FIFO and obtain the response from the
    //SBE FIFO device.This interface will parse the obtained SBE response and
    //any internal SBE failures will be communicated via exceptions
    invokeSBEChipOperation(devPath, command, response);
}
} // namespace scom

namespace threadcontrol
{

//constant specific to thread control operations
static constexpr sbe_word_t RESET_OPCODE  = 0x0000A701;
static constexpr size_t RESET_CMD_LENGTH = 0x3;
static constexpr uint8_t NO_EXIT_ON_FIRST_ERROR = 0x1;
static constexpr uint8_t SRESET_OPERATION = 0x3;

//Bitmap for building the command request for
//instruction control Chip-Op
typedef union
{
  uint32_t l_data;
  struct __attribute__((__packed__))
  {
     uint8_t   threadOps:4;
     uint8_t   threadId:4;
     uint8_t   coreChipletId:8;
     uint8_t   mode:4;
     uint16_t  reserved:12;
  };
}instControlWord_t;


void reset(const char* devPath,
           const uint8_t coreChipletId,
           const uint8_t threadNum)
{
    //Validate input device path
    if (devPath == nullptr)
    {
        throw std::runtime_error("NULL FIFO device path");
    }

    //Build the request to perform the S-RESET on the thread related to the
    //input core.
    instControlWord_t l_word;
    l_word.l_data = 0;
    l_word.mode = NO_EXIT_ON_FIRST_ERROR;
    l_word.coreChipletId  = coreChipletId;
    l_word.threadId = threadNum;
    l_word.threadOps = SRESET_OPERATION;

    std::array<sbe_word_t, RESET_CMD_LENGTH> command =
    {
        static_cast<sbe_word_t>(htobe32(RESET_CMD_LENGTH)),
        htobe32(RESET_OPCODE),
        htobe32(l_word.l_data)
    };

    //Buffer to hold the SBE response status
    const size_t respLength = RESP_HEADER_LEN;
    std::array<sbe_word_t, respLength> response = {};

    //Write the command buffer to the SBE FIFO and obtain the response from the
    //SBE FIFO device.This interface will parse the obtained SBE response and
    //any internal SBE failures will be communicated via exceptions
    invokeSBEChipOperation(devPath, command, response);
}

} //namespace threadcontrol
} // namespace sbe
} // namespace openpower
