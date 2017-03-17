/*
 * sbei_interfaces.h: Data structures and interfaces requried for performing 
 * hardware access operations via SBE(Self Boot Engine).
 *
 */

#ifndef _SBEI_H_
#define _SBEI_H_


#ifdef __cplusplus
extern "C" {
#endif


#include "sbei_data_struct.h"
// SBE FIFO header files and other required files will  be included later


/**
 * sbei_get_scom() - Read a SCOM register
 * @fifo_dev_path: SBE FIFO device path which will be used to perform the
 *                 required operation.
 * @address:       SCOM Address used to read the data
 * @value  :       Value obtained after SCOM read operation.
 * @sbe_error_data:Pointer to the SBE error data structure. Non-zero
 *                 value of  sbe_response member in the sbei_sbe_error_data
 *                 structure indicates that the sbe failed to complete the 
 *                 operation. This interface will allocate required memory and
 *                 the caller needs to de-allocte the memory.
 *
 * @return:        Response returned by the SBE FIFO driver. 0 on Success and
 *                 -1 on failure with errno variable  set with the 
 *                 appropriate value.
 *
 * Used to read the SCOM register via the SBE.
 */
int sbei_get_scom(
         char *fifo_dev_path,
         const uint64_t address,
         uint64_t *value,
         sbei_sbe_error_data_t *sbe_error_data);



/**
 * sbei_put_scom() - Write to the SCOM register
 * @fifo_dev_path: SBE FIFO device path which will be used to perform the
 *                 required operation.
 * @address:       SCOM register address to be written.
 * @value  :       Data to be written to the SCOM register.
 * @sbe_error_data:Pointer to the SBE error data structure. The non zero value
 *                 of  sbe_response member in the sbei_sbe_error_data structure
 *                 indicates that the sbe failed to complete the operation. This
 *                 interface will allocate required memory and the caller needs
 *                 to de-allocate the memory.
 *
 * @return:        Response returned by the SBE FIFO driver. 0 on Success and -1
 *                 on failure with errno variable set with the appropriate
 *                 value.
 *
 * Used to modify the bits in the SCOM register via the SBE.
 */
int sbei_put_scom(
         char *fifo_dev_path,
         const uint64_t address,
         const uint64_t value,
         sbei_sbe_error_data_t *sbe_error_data);


#ifdef __cplusplus
}
#endif


#endif //_SBEI_H_
