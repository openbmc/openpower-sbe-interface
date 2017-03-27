#pragma once

#include <stdint.h>
#include <string>
namespace openpower
{
namespace sbe
{

/**
 * @brief Reads a SCOM register in processor chip.
 *
 * Throws an exception on error.
 *
 * @param[in] FIFO device path associated with the SBE.
 * @param[in] SCOM register address.
 * @return The register data
 */
uint64_t getScom(const std::string& devPath,
                 uint64_t address);


/**
 * @brief Writes to a SCOM register in processor chip.
 *
 * Throws an exception on error.
 *
 * @param[in] FIFO device path associated with the SBE.
 * @param[in] SCOM register address.
 * @param[in] Data to be written into the register.
 */
void putScom(const std::string& devPath,
             uint64_t address,
             uint64_t data);

}//namespace sbe
}//namespace openpower
