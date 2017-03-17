/*
 * sbei_data_struct.h: Data structures requried for performing hardware 
 * access operations via SBE(Self Boot Engine).
 *
 */

#ifndef _SBEI_DATA_STRUCT_H_
#define _SBEI_DATA_STRUCT_H_

/**
 * Maximum FFDC(First failure data capture) packets which any SBE access related
 * failure can obtain.
 */
#define SBEI_SBE_MAX_FFDC_PKTS 20


/**
 * struct sbei_sbe_ffdc_packet - Defines SBE FFDC packet structure
 * @rc :        Failure reason code
 * @ffdclength: Length of the SBE FFDC packet
 * @ffdcdata:   pointer to the FFDC data
 *
 * This defines structure of the SBE FFDC data packet.This packet is obtained
 * when SBE fails to perform the requested hardware access.
 */
typedef struct sbei_sbe_ffdc_packet {
         uint32_t rc;
         uint32_t ffdclength;
         uint32_t *ffdcdata;
} sbei_sbe_ffdc_packet_t;


/**
 * struct sbei_sbe_error_data - Defines SBE FFDC response data packet
 * @sbe_response:   Represents operation execution status by SBE.
 * @num_ffdc_pkts:  Number of FFDC packets.
 * @ffdc_pkts:      Array of FFDC packets.It can contain both the FFDC for the
 *                  SBE failure and the underlying hardware procedure failure.
 *
 * This defines structure of the SBE FFDC response data packet.This structure
 * will contain FFDC of all failures obtained to perform the specific hardware
 * access via SBE.
 */
typedef struct sbei_sbe_error_data {
        uint32_t sbe_response;
        uint32_t num_ffdc_pkts;
        sbei_sbe_ffdc_packet_t ffdc_pkts[SBEI_SBE_MAX_FFDC_PKTS];
} sbei_sbe_error_data_t;


#endif

