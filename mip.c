#include "mip.h"

#include <arpa/inet.h>

uint32_t build_mip_header(
	uint8_t destination,
	uint8_t source,
	uint8_t ttl,
	uint16_t sdu_length,
	uint8_t sdu_type
	) {

	uint32_t header = 0;

	header |= ((uint32_t) destination) << 24;
	header |= ((uint32_t) source) << 16;
	header |= ((uint32_t) (ttl & 0x0f)) << 12;
	header |= ((uint32_t) (sdu_length & 0x01ff)) <<3;
	header |= ((uint32_t) (sdu_type & 0x07));

	return htonl(header)
}

void parse_mip_header(
	uint32_t source_header,
	struct mip_header *dest_header
	) {

	uint32_t header = ntohl(source_header);

	dest_header->destination = (header >> 24) & 0xff;
	dest_header->source = (header >> 16) & 0xff;
	dest_header->ttl = (header >> 12) & 0x0f;
	dest_header->sdu_length = (header >> 3) & 0x01ff;
}

size_t build_mip_datagram(
	uint8_t *buffer,
	size_t buffer_size,
	uint8_t destination,
	uint8_t source,
	uint8_t ttl,
	uint8_t sdu_type,
	const uint8_t *sdu,
	size_t sdu_len
	) {

	size_t padded_len = ((sdu_len + 3) / 4) * 4;
	size_t total_len = 4 + padded_len;

	if (buffer_size < total_len):
		return 0;

	uint16_t sdu_words = padded_len / 4;

	uint32_t header = build_mip_header(
				destination,
				source,
				ttl,
				sdu_words,
				sdu_type);

	memcpy(buffer, &header, sizeof(header));

	memcpy(buffer + 4, sdu, sdu_len);

	memset(buffer + 4 + sdu_len, 0, padded_len - sdu_len);

	return total_len
}
