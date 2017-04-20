#include <stdexcept>
#include <array>
#include <sbe_chipOp_handler.hpp>
namespace openpower
{
namespace sbe
{

//SBE Response header
struct sbeRespHeader_t
{
    uint8_t command;
    uint8_t commandClass;
    uint16_t magic;
    sbe_word_t priSecStatus;
};

static constexpr uint16_t MAGIC_CODE = 0xC0DE;
static constexpr auto SBE_OPERATION_SUCCESSFUL = 0x00;
static constexpr auto LENGTH_OF_DISTANCE_HEADER_IN_WORDS = 0x1;

#define SBEI_SBE_RESPONSE_SIZE_IN_WORDS  (sizeof(sbeRespHeader_t)/4)


std::vector<sbe_word_t> writeAndReadSBEFifo(const char* devPath,
        const sbe_word_t* cmdBuffer,
        const size_t cmdBufLen,
        const size_t expectRespLen)
{
    std::vector<sbe_word_t> response(expectRespLen);
    response = {0x80200000, 0x00000000, 0xc0dea201, 0x00000000, 0x00000003};
    //TODO: Add support for reading and writing from the FIFO device
    return (std::move(response));
}

std::vector<sbe_word_t> parseSBERespAndExtractData(
    std::vector<sbe_word_t> const& sbeDataBuf)
{

    const sbe_word_t* responseData = sbeDataBuf.data();

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

    sbeRespHeader_t* l_respHeader = (sbeRespHeader_t*)(responseData +
                                    (lengthObtained - DistanceToStatusHeader));

    if (l_respHeader->magic != MAGIC_CODE)
    {
        //TODO:use elog infrastructure
        std::ostringstream errMsg;
        errMsg << "Invalid MAGIC keyword in the response header (" <<
               l_respHeader->magic << "),expected keyword " << MAGIC_CODE;
        throw std::runtime_error(errMsg.str().c_str());
    }

    if (l_respHeader->priSecStatus != SBE_OPERATION_SUCCESSFUL)
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


