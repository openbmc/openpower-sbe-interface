#include <stdexcept>
#include <array>
#include <sbe_chipOp_handler.hpp>
namespace openpower
{
namespace sbe
{
namespace fifoutils
{

/*! \union sbeRespHeader_t
 *  Defines the breakup of the SBE FIFO chip operation response header.
 */
union sbeRespHeader_t
{
    uint32_t commandWord; /**<Entire 32 bit command word */
    struct
    {
        uint8_t command:8;/**<command value for which response is obtained */ 
        uint8_t commandClass:8;/**< Class of the command */
        uint16_t magic:16; /**< Magic code obtained in the response */
    };
}__attribute__((packed));


static constexpr uint16_t MAGIC_CODE = 0xC0DE;
static constexpr auto SBE_OPERATION_SUCCESSFUL = 0;
static constexpr auto LENGTH_OF_DISTANCE_HEADER_IN_WORDS = 0x1;

#define SBEI_SBE_RESPONSE_SIZE_IN_WORDS  ( (sizeof(sbeRespHeader_t) + \
                                            sizeof(sbe_word_t)) / 4 )


std::vector<sbe_word_t> write(const char* devPath,
        const sbe_word_t* cmdBuffer,
        const size_t cmdBufLen,
        const size_t respBufLen)
{
    //TODO: Add support for reading and writing from the FIFO device
    //Until then just return hardcoded data.
    std::vector<sbe_word_t> response(respBufLen);
    response = {0x80200000, 0x00000000, 0xc0dea201, 0x00000000, 0x00000003};
    return (response);
}

std::vector<sbe_word_t> parseResponse(
    const std::vector<sbe_word_t>& sbeDataBuf)
{

    //Number of 32-bit words obtained from the SBE
    size_t lengthObtained = sbeDataBuf.size();

    //Fetch the SBE header and SBE chiop primary and secondary status
    //Last value in the buffer will have the offset for the SBE header
    size_t DistanceToStatusHeader =  sbeDataBuf[sbeDataBuf.size() - 1];

    if (lengthObtained < DistanceToStatusHeader)
    {
        //TODO:use elog infrastructure
        std::ostringstream errMsg;
        errMsg << "Distance to SBE status header value " <<
               DistanceToStatusHeader << " is greater then total lenght of "
               "response buffer " << lengthObtained;
        throw std::runtime_error(errMsg.str().c_str());
    }

    //Fetch the response header contents
    sbeRespHeader_t l_respHeader;
    auto iter = sbeDataBuf.begin();
    std::advance(iter,(lengthObtained - DistanceToStatusHeader));
    l_respHeader.commandWord = *iter;

    //Fetch the primary and secondary response code
    std::advance(iter,1);
    sbe_word_t l_priSecResp = *iter;

    //Validate the magic code obtained in the response
    if (l_respHeader.magic != MAGIC_CODE)
    {
        //TODO:use elog infrastructure
        std::ostringstream errMsg;
        errMsg << "Invalid MAGIC keyword in the response header (" <<
               l_respHeader.magic << "),expected keyword " << MAGIC_CODE;
        throw std::runtime_error(errMsg.str().c_str());
    }

    //Validate the Primary and Secondary response value
    if (l_priSecResp != SBE_OPERATION_SUCCESSFUL)
    {
        //Extract the SBE FFDC and throw it to the caller
        size_t ffdcLen = (DistanceToStatusHeader -
                          SBEI_SBE_RESPONSE_SIZE_IN_WORDS -
                          LENGTH_OF_DISTANCE_HEADER_IN_WORDS);

        std::vector<sbe_word_t> ffdcData(ffdcLen);
        std::copy_n((sbeDataBuf.begin() + SBEI_SBE_RESPONSE_SIZE_IN_WORDS),
                    ffdcLen, ffdcData.begin());

        //TODO:use elog infrastructure to return the SBE and Hardware procedure
        //FFDC container back to the caller.
        std::ostringstream errMsg;
        errMsg << "Chip operation failed,FFDC data of length:" << ffdcLen <<
               " obtained!";
        throw std::runtime_error(errMsg.str().c_str());
    }
    return std::move(sbeDataBuf);

}

}
}
}


