#ifndef MIP_H
#define MIP_H

#include <stdint.h>
#include <stddef.h>
#include <net/if.h>

#define MIP_HEADER_SIZE 4
#define ETH_P_MIP 0x88B5

#define MIP_BROADCAST 0xFF
#define MIP_TYPE_ARP 0x01
#define MIP_TYPE_PING 0x02

#define ARP_REQUEST 0
#define ARP_RESPONSE 1
#define ARP_CACHE_SIZE 256

#define MAX_INTERFACES 8

#define MAX_PACKET_SIZE 2048
#define MAX_FRAME_SIZE 4096
#define MAX_MESSAGE_SIZE 1024

struct mip_header {
	uint8_t destination;
	uint8_t source;
	uint8_t ttl;
	uint16_t sdu_length;
	uint8_t sdu_type;
};

struct interface_info {
	char name[IF_NAMESIZE];
	int ifindex;
	uint8_t mac[6];
};


struct arp_entry {
	int valid;
	uint8_t mac[6];
	int ifindex;
};


/*
 * mip.c
 */

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
uint_32_t build_mip_header(
	uint8_t destination,
	uint8_t source,
	uint8_t ttl,
	uint16_t sdu_length,
	uint8_t sdu_type
);


/*
 * Decode MIP header.
 *
 * source_header: 32-bit MIP header.
 * fields: output struct for decoded fields.
 *
 */
void parse_mip_header(
	uint32_t source_header,
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
 * sdu_len: length of payload in number of bytes.
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


/*
 * mip_arp.c
 */

/* 
 * Build message for MIP-ARP protocol.
 *
 * type: operation type.
 * address: MIP address in MIP-ARP message.
 *
 * Returns 32-bit MIP-ARP message.
 */
uint32_t build_arp_message(
	uint8_t type,
	uint8_t address
);


/*
 * Parse MIP-ARP message
 *
 * raw: MIP-ARP message in network byte-order.
 * type: pointer to where to store type.
 * address: pointer to where to store MIP address.
 *
 */
void parse_arp_message(
	uint32_t raw,
	uint8_t *type,
	uint8_t *address
);


/*
 * Search local MIP-ARP cache for address.
 *
 * cache: pointer to cache array.
 * mip: MIP address to search for.
 *
 * Returns pointer to arp_entry if address exists, or NULL if not.
 */
struct arp_entry *arp_lookup(
	struct arp_entry *cache,
	uint8_t mip
);


/*
 * Add or replace address mapping in cache.
 *
 * cache: pointer to cache array.
 * mip: address to add.
 * mac: mac address to neighbor.
 * ifindex: reachable interface of neighor.
 *
 */
void arp_update(
	struct arp_entry *cache,
	uint8_t mip,
	const uint8_t mac[6],
	int ifindex
);


/*
 * Print entries in MIP-ARP cache.
 *
 * cache: cache to print.
 *
 */
void arp_print(
	struct arp_entry *cache
);


/* 
 * ethernet.c
 */

/*
 * Create RAW socket.
 *
 */
int open_raw_socket(void);


/*
 * Discover useable ethernet interfaces.
 *
 * interfaces: output array for discovered interfaces.
 * max: maximum number of interfaces.
 *
 */
int get_ethernet_interfaces(
	struct interface_info *interfaces,
	size_t max
);


/*
 * Build ethernet frame containing MIP PDU.
 *
 * frame: output buffer.
 * frame_size: size of output buffer.
 * destination_mac: destination ethernet MAC address.
 * source_mac: source ethernet MAC address.
 * mip_packet: MIP PDU.
 * mip_len: size of MIP PDU.
 *
 * Returns frame size in bytes, or 0 on error.
 */
size_t build_ethernet_frame(
	uint8_t *frame,
	size_t frame_size,
	const uint8_t destination_mac[6],
	const uint8_t source_mac[6],
	const uint8_t *mip_packet,
	size_t mip_len
);


/* Send ethernet frame.
 *
 * raw_socekt: AF_PACKET raw socket.
 * ifindex: linux interface to send on.
 * destination_mac: ethernet destination.
 * frame: ethernet frame to send.
 * frame_len: number of valid bytes in ethernet frame.
 *
 * Returns number of bytes sent, or -1 on error.
 *
 */
int send_ethernet_frame(
	int raw_socket,
	int ifindex,
	const uint8_t destination_mac[6],
	const uint8_t *frame,
	size_t frame_len
);

#endif
