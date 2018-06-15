#include "ping.h"

#include <stdlib.h>
#include <string.h>
#include "lwip/sockets.h"
#include "lwip/ip.h"
struct iphdr
{
	uint32_t ihl : 4;
	uint32_t version : 4;

	uint8_t tos;
	uint16_t tot_len;
	uint16_t id;
	uint16_t frag_off;
	uint8_t ttl;
	uint8_t protocol;
	uint16_t check;
	uint32_t saddr;
	uint32_t daddr;
	/*The options start here. */
};

struct icmphdr
{
	uint8_t type; /* message type */
	uint8_t code; /* type sub-code */
	uint16_t checksum;
	union {
		struct
		{
			uint16_t id;
			uint16_t sequence;
		} echo;			   /* echo datagram */
		uint32_t gateway; /* gateway address */
		struct
		{
			uint16_t __unused;
			uint16_t mtu;
		} frag; /* path mtu discovery */
	} un;
};

#define ICMP_ECHOREPLY 0	   /* Echo Reply			*/
#define ICMP_DEST_UNREACH 3	/* Destination Unreachable	*/
#define ICMP_SOURCE_QUENCH 4   /* Source Quench		*/
#define ICMP_REDIRECT 5		   /* Redirect (change route)	*/
#define ICMP_ECHO 8			   /* Echo Request			*/
#define ICMP_TIME_EXCEEDED 11  /* Time Exceeded		*/
#define ICMP_PARAMETERPROB 12  /* Parameter Problem		*/
#define ICMP_TIMESTAMP 13	  /* Timestamp Request		*/
#define ICMP_TIMESTAMPREPLY 14 /* Timestamp Reply		*/
#define ICMP_INFO_REQUEST 15   /* Information Request		*/
#define ICMP_INFO_REPLY 16	 /* Information Reply		*/
#define ICMP_ADDRESS 17		   /* Address Mask Request		*/
#define ICMP_ADDRESSREPLY 18   /* Address Mask Reply		*/
#define NR_ICMP_TYPES 18

extern void bsp_printf(const char *format, ...);

uint16_t checksum(uint16_t *buf, uint32_t bufsz)
{
	/*-- var --*/
	uint32_t u32Csum = 0u;
	uint32_t u32temp1, u32temp2;

	/*-- begin --*/
	while (bufsz > 1u)
	{
		u32Csum += *buf;
		buf++;
		bufsz -= 2;
	}

	if (bufsz == 1u)
	{
		u32Csum += *(uint8_t *)buf;
	}
	u32temp1 = u32Csum;
	u32temp1 &= 0x0000ffffu;
	u32temp2 = u32Csum;
	u32temp2 >>= 16;
	u32Csum = u32temp1 + u32temp2;
	u32temp1 = u32Csum;
	u32temp1 &= 0x0000ffffu;
	u32temp2 = u32Csum;
	u32temp2 >>= 16;
	u32Csum = u32temp1 + u32temp2;

	return (uint16_t)~u32Csum;
}

_Bool ping(uint32_t destIP)
{
	int sock;
	struct icmphdr hdr;
	struct sockaddr_in addr;
	int n;
	uint32_t u32Timeout = 1000u;

	char buf[2000];
	struct icmphdr *icmphdrptr;
	struct iphdr *iphdrptr;

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = destIP;

	/* RAWソケットを作成します */
	sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sock < 0)
	{
		bsp_printf("[%s (%d)] NG : socket\r\n", __FUNCTION__, __LINE__);
		return false;
	}else{
		bsp_printf("[%s (%d)] socket SUCCESS\r\n", __FUNCTION__, __LINE__);
	}

#if 0
	if(lwip_setsockopt( sock,
						SOL_SOCKET,
						SO_RCVTIMEO,
						&u32Timeout,
						sizeof(uint32_t)) == -1)
	{
		close(sock);
		return false;
	}
#endif
	memset(&hdr, 0, sizeof(hdr));

	/* ICMPヘッダを用意します */
	hdr.type = ICMP_ECHO;
	hdr.code = 0;
	hdr.checksum = 0;
	hdr.un.echo.id = 0;
	hdr.un.echo.sequence = 0;

	/* ICMPヘッダのチェックサムを計算します */
	hdr.checksum = checksum((unsigned short *)&hdr, sizeof(hdr));

	/* ICMPヘッダだけのICMPパケットを送信します */
	/* ICMPデータ部分はプログラムを簡潔にするために省いています */
	n = sendto(sock,
			   (char *)&hdr, sizeof(hdr),
			   0, (struct sockaddr *)&addr, sizeof(addr));
	if (n < 1)
	{
		bsp_printf("[%s (%d)] NG : sendto\r\n", __FUNCTION__, __LINE__);
	}else{
		bsp_printf("[%s (%d)] sendto SUCCESS\r\n", __FUNCTION__, __LINE__);
	}

	/* ICMP送信部分はここまでです*/
	/* ここから下はICMP ECHO REPLY受信部分になります */

	memset(buf, 0, sizeof(buf));

	/* 相手ホストからのICMP ECHO REPLYを待ちます */
	n = recv(sock, buf, sizeof(buf), 0);
	if (n < 1)
	{
		bsp_printf("[%s (%d)] NG : recv\r\n", __FUNCTION__, __LINE__);
	}else{
		bsp_printf("[%s (%d)] recv SUCCESS\r\n", __FUNCTION__, __LINE__);
	}

	/* 受信データからIPヘッダ部分へのポインタを取得します */
	iphdrptr = (struct iphdr *)buf;

	/* 受信データからICMPヘッダ部分へのポインタを取得します */
	icmphdrptr = (struct icmphdr *)(buf + (iphdrptr->ihl * 4));

	/* ICMPヘッダからICMPの種類を特定します */
	if (icmphdrptr->type == ICMP_ECHOREPLY)
	{
		bsp_printf("received ICMP ECHO REPLY\r\n");
	}
	else
	{
		bsp_printf("received ICMP %d\r\n", icmphdrptr->type);
	}

	/* 終了 */
	close(sock);

	return true;
}

void CmdPing(uint32_t argc, const char *argv[])
{
	
	if (argc != 2)
	{
		bsp_printf("usage : %s IPADDR\r\n", argv[0]);
		return;
	}else{
		ping(inet_addr(argv[1]));
	}
}