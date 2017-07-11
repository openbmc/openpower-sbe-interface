#pragma once

#include <unistd.h>
namespace openpower
{
namespace sbe
{
namespace internal
{

/** @class FileDescriptor
 *  @brief Responsible for handling file descriptor
 */
class FileDescriptor
{
    private:
        /** @brief File descriptor for the SBE FIFO device */
        int fd = -1;

    public:
        FileDescriptor() = delete;
        FileDescriptor(const FileDescriptor&) = delete;
        FileDescriptor& operator=(const FileDescriptor&) = delete;
        FileDescriptor(FileDescriptor&&) = delete;
        FileDescriptor& operator=(FileDescriptor&&) = delete;

        /** @brief Saves File descriptor and uses it to do file operation
         *
         *  @param[in] fd - File descriptor
         */
        FileDescriptor(int fd) : fd(fd)
        {
            // Nothing
        }

        ~FileDescriptor()
        {
            if (fd >= 0)
            {
                close(fd);
            }
        }

        int operator()()
        {
            return fd;
        }
};

} // namespace internal
} // namespace sbe
} // namespace openpower
