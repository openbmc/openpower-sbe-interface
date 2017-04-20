#pragma once

#include <stdexcept>
#include <array>
#include <sstream>
#include <algorithm>

namespace openpower
{
namespace sbe
{

using sbe_word_t = uint32_t;

/**
 * @brief Utility to perform write and read operation on the SBE FIFO device.
 * Internal device driver failures will be conveyed via respective exceptions.
 *
 * Exceptions thrown for:
 * - Device driver internal failures
 *
 * @param[in] FIFO device path associated with SBE.
 * @param[in] Command buffer to be written to the SBE FIFO
 * @param[in] Length of command buffer
 * @param[in] Expected response buffer length
 *
 * @return Response buffer returned by the SBE for the input command.
 */
std::vector<sbe_word_t>  writeAndReadSBEFifo(const char* devPath,
        const sbe_word_t* cmdBuffer,
        const size_t cmdBufLen,
        const size_t expectRespLen);

/**
 * @brief Utility to parse the data obtained from the SBE. The header and SBE
 * response will be verified and on success the required data will be returned
 * to the caller. SBE interface failure will be conveyed via respective exceptions.
 *
 * Exceptions thrown for:
 * - SBE Internal failures
 *
 * @param[in] SBE data obtained from the SBE FIFO device
 * @return Valid chip operation response obtained by SBE.
 */
std::vector<sbe_word_t> parseSBERespAndExtractData(
    std::vector<sbe_word_t> const& sbeDataBuf);

/**
 * @brief Interface to write and read from the SBE FIFO device. Internally it
 * validates the data obtained by the SBE and throws exception for any SBE
 * internal failures.
 *
 * Exceptions thrown for:
 * - Device driver failures
 * - SBE internal failures
 *
 * @param[in] FIFO device path associated with the SBE.
 * @param[in] Request packet for the data to be read.
 * @param[in] Data obtained by the SBE.
 */
template<size_t S1, size_t S2>
void invokeSBEChipOperation(const char* devPath,
                            std::array<sbe_word_t, S1> const& request,
                            std::array<sbe_word_t, S2>& chipOpData)
{
    //Write and read from the FIFO device.
    std::vector<sbe_word_t> sbeFifoResp;
    sbeFifoResp = writeAndReadSBEFifo(devPath, request.data(), request.size(),
                                      chipOpData.size());

    //Parse the obtained data
    std::vector<sbe_word_t> response;
    response = parseSBERespAndExtractData(sbeFifoResp);

    if (response.size() != chipOpData.size())
    {
        //TODO:use elog infrastructure
        std::ostringstream errMsg;
        errMsg << "Obtained chip operation response length (" << response.size()
               << "from SBE is not equal to the expected length of data (" <<
               chipOpData.size();

        throw std::runtime_error(errMsg.str().c_str());
    }

    std::move(response.begin(), response.end(), chipOpData.begin());
}

}
}


