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


/*
 * Build MIP datagram.
 *
 * buffer: MIP-packet as byte array.
 * buffer_size: size of buffer in bytes.
 * destination: 8-bit destination MIP-address.
 * source: 8-bit source MIP-address.
 * ttl: 4-bit ttl.
 * sdu_type: 3-bit SDU type.
 * sdu: pointer to SDU payload.
 * sdu_len: 9-bit length of payload.
 *
 */
size_t build_mip_datagram(
	uint8_t *buffer,
	size_t buffer_size,
	uint8_t destination,
	uint8_t source,
	uint8_t ttl,
	uint8_t sdu_type,
	const uint8_t *sdu,
	size_t sdu_len
);


struct ethernet_frame(
	uint8_t *frame,
	size_t frame_size,
	const uint8_t destination_mac[6],
	const uint8_t source_mac[6],
	const uint8_t *mip_packet,
	size_t mip_len
);


#endif
