#include <array>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <endian.h>
#include <sbe_chipOp_handler.hpp>
#include <file.hpp>
namespace openpower
{
namespace sbe
{
namespace internal
{

constexpr uint16_t MAGIC_CODE = 0xC0DE;
constexpr auto SBE_OPERATION_SUCCESSFUL = 0;
constexpr auto LENGTH_OF_DISTANCE_HEADER_IN_WORDS = 0x1;
constexpr auto LENGTH_OF_RESP_HEADER_IN_WORDS = 0x2;
constexpr auto MAX_FFDC_LEN_IN_WORDS = 5120;
constexpr auto FOUR = 4;

std::vector<sbe_word_t> writeToFifo(const char* devPath,
                                    const sbe_word_t* cmdBuffer,
                                    const size_t cmdBufLen,
                                    const size_t respBufLen)
{
    int fd = 0;
    int rc = 0;
    size_t len = 0;
    std::vector<sbe_word_t> response;
    std::ostringstream errMsg;

    //Open the FIFO Device
    fd = open(devPath, O_RDWR | O_NONBLOCK);
    if (fd < 0)
    {
        //TODO:use elog infrastructure
        errMsg << "Opening SBE FIFO with device path:" <<
               devPath << ",Failed with errno" << errno;
        throw std::runtime_error(errMsg.str().c_str());
    }
    FileDescriptor fileFd(fd);

    //Wait for FIFO device and perform write operation
    struct pollfd poll_fd = {};
    poll_fd.fd = (fileFd)();
    poll_fd.events = POLLOUT | POLLERR;

    if ((rc = poll(&poll_fd, 1, -1)) < 0)
    {
        //TODO:use elog infrastructure
        errMsg << "Waiting for FIFO device:" << devPath << "to write failed"
               << "rc=" << rc << "errno=" << errno;
        throw std::runtime_error(errMsg.str().c_str());
    }
    if (poll_fd.revents & POLLERR)
    {
        //TODO:use elog infrastructure
        errMsg << "POLLERR while waiting for writeable FIFO,errno:" << errno;
        throw std::runtime_error(errMsg.str().c_str());
    }
    auto bytesToWrite = (cmdBufLen * FOUR);
    //Perform the write operation
    len = write((fileFd)(), cmdBuffer, bytesToWrite);
    if (len < 0)
    {
        //TODO:use elog infrastructure
        errMsg << "Failed to write to FIFO device:" << devPath << " Length "
               "returned= " << len << " errno=" << errno;
        throw std::runtime_error(errMsg.str().c_str());
    }
    else if (len != bytesToWrite)
    {
        //TODO:use elog infrastructure
        errMsg << "Exepcted " << bytesToWrite << " bytes to be writen to FIFO"
               " device:" << devPath << ", but bytes written are " << len
               << " errno=" << errno;
        throw std::runtime_error(errMsg.str().c_str());

    }
    //Wait for FIFO device and perform read operation
    poll_fd.fd = (fileFd)();
    poll_fd.events = POLLIN | POLLERR;
    if ((rc = poll(&poll_fd, 1, -1) < 0))
    {
        //TODO:use elog infrastructure
        errMsg << "Waiting for FIFO device:" << devPath << "to read failed"
               << " rc=" << rc << " and errno=" << errno;
        throw std::runtime_error(errMsg.str().c_str());
    }
    if (poll_fd.revents & POLLERR)
    {
        //TODO:use elog infrastructure
        errMsg << "POLLERR while waiting for readable FIFO,errno:" << errno;
        throw std::runtime_error(errMsg.str().c_str());
    }
    //Derive the total read length which should include the FFDC, which SBE
    //returns in case of failure.
    size_t totalReadLen = respBufLen + MAX_FFDC_LEN_IN_WORDS;
    //Create a temporary buffer
    std::vector<sbe_word_t> buffer(totalReadLen);

    auto bytesToRead = (totalReadLen * FOUR);
    len = read((fileFd)(), buffer.data(), bytesToRead);
    if (len < 0)
    {
        //TODO:use elog infrastructure
        errMsg << "Failed to read the FIFO device:" << devPath << "bytes read ="
               << len << " errno=" << errno;
        throw std::runtime_error(errMsg.str().c_str());
    }
    else if (len != bytesToRead)
    {
        //TODO:use elog infrastructure
        errMsg << "Exepcted " << bytesToRead << " bytes to be read from FIFO"
               " device:" << devPath << ", but bytes read are " << len <<
               " errno= " << errno;
        throw std::runtime_error(errMsg.str().c_str());
    }

    //Extract the valid number of words read.
    for (auto i = 0; i < (rc / 4); ++i)
    {
        response.push_back(be32toh(buffer[i]));
    }

    //Closing of the file descriptor will be handled when the FileDescriptor
    //object will go out of scope.
    return (response);
}

std::vector<sbe_word_t> parseResponse(
    const std::vector<sbe_word_t>& sbeDataBuf)
{

    //Number of 32-bit words obtained from the SBE
    size_t lengthObtained = sbeDataBuf.size();

    //Fetch the SBE header and SBE chiop primary and secondary status
    //Last value in the buffer will have the offset for the SBE header
    size_t distanceToStatusHeader =  sbeDataBuf[sbeDataBuf.size() - 1];

    if (lengthObtained < distanceToStatusHeader)
    {
        //TODO:use elog infrastructure
        std::ostringstream errMsg;
        errMsg << "Distance to SBE status header value " <<
               distanceToStatusHeader << " is greater then total lenght of "
               "response buffer " << lengthObtained;
        throw std::runtime_error(errMsg.str().c_str());
    }

    //Fetch the response header contents
    auto iter = sbeDataBuf.begin();
    std::advance(iter, (lengthObtained - distanceToStatusHeader));
    sbe_word_t l_magicCode = (*iter >> 16);

    //Fetch the primary and secondary response code
    std::advance(iter, 1);
    auto l_priSecResp = *iter;

    //Validate the magic code obtained in the response
    if (l_magicCode != MAGIC_CODE)
    {
        //TODO:use elog infrastructure
        std::ostringstream errMsg;
        errMsg << "Invalid MAGIC keyword in the response header (" <<
               l_magicCode << "),expected keyword " << MAGIC_CODE;
        throw std::runtime_error(errMsg.str().c_str());
    }

    //Validate the Primary and Secondary response value
    if (l_priSecResp != SBE_OPERATION_SUCCESSFUL)
    {
        //Extract the SBE FFDC and throw it to the caller
        size_t ffdcLen = (distanceToStatusHeader -
                          LENGTH_OF_RESP_HEADER_IN_WORDS -
                          LENGTH_OF_DISTANCE_HEADER_IN_WORDS);

        std::vector<sbe_word_t> ffdcData(ffdcLen);
        std::copy_n((sbeDataBuf.begin() + LENGTH_OF_RESP_HEADER_IN_WORDS),
                    ffdcLen, ffdcData.begin());

        //TODO:use elog infrastructure to return the SBE and Hardware procedure
        //FFDC container back to the caller.
        std::ostringstream errMsg;
        errMsg << "Chip operation failed,FFDC data of length:" << ffdcLen <<
               " obtained!";
        throw std::runtime_error(errMsg.str().c_str());
    }

    //In case of success, remove the response header content and send only the
    //data.
    size_t respLen = (lengthObtained - distanceToStatusHeader);
    std::vector<uint32_t> response(respLen);
    std::copy_n(sbeDataBuf.begin(), respLen, response.begin());
    return response;
}

}
}
}


