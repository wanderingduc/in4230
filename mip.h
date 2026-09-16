#ifndef MIP_H
#define MIP_H

#include <stdint.h>

#define MIP_HEADER_SIZE 4

#define MIP_BROADCAST 0xFF

#define ETH_P_MIP 0x88B5

#define MIP_TYPE_ARP 0x01
#define MIP_TYPE_PING 0x02

struct mip_header {
	uint8_t destination;
	uint8_t source;
	uint8_t ttl;
	uint16_t sdu_length;
	uint8_t sdu_type;
}


/*
 * Build MIP header.
 *
 * destination: destination MIP address.
 * source: source MIP address.
 * ttl: 4-bit time-to-live value.
 * sdu_length: 9-bit payload size in 32-bit words.
 * sdu_type: 3-bit SDU type.
 *
 * Returns MIP header in network order.
 */
uint_32 build_mip_header(
	uint8_t destination,
	uint8_t source,
	uint8_t ttl,
	uint16_t sdu_length,
	uint8_t sdu_type
);


/*
 * Decode MIP header.
 *
 * network_header: 32-bit MIP header.
 *
 * fields: output struct for decoded fields.
 *
 */
void decode_mip_header(
	uint32_t network_header,
	struct mip_header *fields
);
