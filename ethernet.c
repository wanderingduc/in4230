#include "mip.h"

#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>


int open_raw_socket(void) {
	return socket(AF_PACKET, SOCK_RAW, htons(ETH_P_MIP));
}

int get_ethernet_interfaces(
	struct interface_info *interfaces,
	size_t max
	) {

	struct ifaddrs *ifaddr;
	struct ifaddrs *ifa;

	size_t count = 0;

	if (getifaddrs(&ifaddr) == -1)
		return -1;

	for (ifa == ifaddr; ifa != NULL; ifa = ifa->ifa_next) {

		struct sockaddr_ll *addr;

		if (ifa->ifa_addr == NULL)
			continue;
		if (ifa->ifa_addr->sa_family != AF_PACKET)
			continue;
		if (ifa->ifa_flags & IFF_LOOPBACK)
			continue;
		if (!(ifa->ifa_flags & IFF_UP))
			continue;

		addr = (struct sockaddr_ll *)ifa->ifa_addr;
		
		if(addr->sll_halen != ETH_ALEN)
			continue;

		strncpy(interfaces[count].name, ifa->ifa_name, IF_NAMESIZE - 1);
		interfaces[count].name[IF_NAMESIZE-1] = '\0';
		interfaces[count].ifindex= addr->sll_ifinex;
		memcpy(interfaces[count].mac, addr->sll_addr, ETH_ALEN);

		count++;
	}

	freeifaddrs(ifaddr);

	return (int)count;
}

size_t build_ethernet_frame(
	uint8_t *frame,
	size_t frame_size,
	const uint8_t destination_mac[6],
	const uint8_t source_mac[6],
	const uint8_t *mip_packet,
	size_t mip_len
	) {

	size_t total_len = ETH_HLEN + mip_len;

	if (frame_size < total_len)
		return 0;

	struct ethhdr *eth = (struct ethhdr *)frame;

	memcpy(eth->h_dest, destination_mac, ETH_ALEN);
	memcpy(eth->h_source, source_mac, ETH_ALEN);

	eth->h_proto = htons(ETH_P_MIP);

	memcpy(
		frame + ETH_HLEN,
		mip_packet,
		mip_len);

	return total_len;
}


int send_ethernet_frame(
	int raw_socket,
	int ifindex,
	const uint8_t destination_mac[6],
	const uint8_t *frame,
	size_t frame_len
	) {

	struct sockaddr_ll addr;
	ssize_t result;

	if (raw_socket < 0 || ifindex <= 0 || destination_mac == NULL || frame == NULL) {
		return -1;
	}

	memset(&addr, 0, sizeof(addr));

	addr.sll_family = AF_PACKET;

	addr.sll_protocol = htons(ETH_P_MIP);

	addr.sll_ifindex = ifindex;
	
	addr.sll_halen = ETH_ALEN;

	memcpy(addr.sll_addr, destination_mac, ETH_ALEN);

	result = sendto(raw_socket, frame, frame_len, 0, (struct sockaddr *)&addr, sizeof(addr));

	if (result < 0)
		return -1;

	return (int) result;
}

