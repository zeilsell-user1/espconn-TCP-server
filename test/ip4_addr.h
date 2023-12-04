/**
 * @file
 * IPv4 address API
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef LWIP_HDR_IP4_ADDR_H
#define LWIP_HDR_IP4_ADDR_H

// #include "lwip/opt.h"
// #include "lwip/def.h"

#define LWIP_IPV4 1
#if LWIP_IPV4

#ifdef __cplusplus
extern "C" {
#endif

/** This is the aligned version of ip4_addr_t,
   used as local variable, on the stack, etc. */
struct ip4_addr {
  unsigned long addr;
};

/** ip4_addr_t uses a struct for convenience only, so that the same defines can
 * operate both on ip4_addr_t as well as on ip4_addr_p_t. */
typedef struct ip4_addr ip4_addr_t;
typedef struct ip4_addr ipv4_addr_t;

/**
 * struct ipaddr2 is used in the definition of the ARP packet format in
 * order to support compilers that don't have structure packing.
 */
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif
// PACK_STRUCT_BEGIN
// struct ip4_addr2 {
//   PACK_STRUCT_FIELD(unsigned short  addrw[2]);
// } PACK_STRUCT_STRUCT;
// PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif

/* Forward declaration to not include netif.h */
struct netif;

/** 255.255.255.255 */
#define IPADDR_NONE         ((unsigned long)0xffffffffUL)
/** 127.0.0.1 */
#define IPADDR_LOOPBACK     ((unsigned long)0x7f000001UL)
/** 0.0.0.0 */
#define IPADDR_ANY          ((unsigned long)0x00000000UL)
/** 255.255.255.255 */
#define IPADDR_BROADCAST    ((unsigned long)0xffffffffUL)

/* Definitions of the bits in an Internet address integer.

   On subnets, host and network parts are found according to
   the subnet mask, not these masks.  */
#define IP_CLASSA(a)        ((((unsigned long)(a)) & 0x80000000UL) == 0)
#define IP_CLASSA_NET       0xff000000
#define IP_CLASSA_NSHIFT    24
#define IP_CLASSA_HOST      (0xffffffff & ~IP_CLASSA_NET)
#define IP_CLASSA_MAX       128

#define IP_CLASSB(a)        ((((unsigned long)(a)) & 0xc0000000UL) == 0x80000000UL)
#define IP_CLASSB_NET       0xffff0000
#define IP_CLASSB_NSHIFT    16
#define IP_CLASSB_HOST      (0xffffffff & ~IP_CLASSB_NET)
#define IP_CLASSB_MAX       65536

#define IP_CLASSC(a)        ((((unsigned long)(a)) & 0xe0000000UL) == 0xc0000000UL)
#define IP_CLASSC_NET       0xffffff00
#define IP_CLASSC_NSHIFT    8
#define IP_CLASSC_HOST      (0xffffffff & ~IP_CLASSC_NET)

#define IP_CLASSD(a)        (((unsigned long)(a) & 0xf0000000UL) == 0xe0000000UL)
#define IP_CLASSD_NET       0xf0000000          /* These ones aren't really */
#define IP_CLASSD_NSHIFT    28                  /*   net and host fields, but */
#define IP_CLASSD_HOST      0x0fffffff          /*   routing needn't know. */
#define IP_MULTICAST(a)     IP_CLASSD(a)

#define IP_EXPERIMENTAL(a)  (((unsigned long)(a) & 0xf0000000UL) == 0xf0000000UL)
#define IP_BADCLASS(a)      (((unsigned long)(a) & 0xf0000000UL) == 0xf0000000UL)

#define IP_LOOPBACKNET      127                 /* official! */

#define PP_HTONL(n) \
  ((((unsigned long)(n) & 0xff) << 24) | \
   (((unsigned long)(n) & 0xff00) << 8) | \
   (((unsigned long)(n) & 0xff0000) >> 8) | \
   (((unsigned long)(n) & 0xff000000) >> 24))

#define LWIP_MAKEU32(a, b, c, d) \
  ((((unsigned long)(a)) << 24) | \
   (((unsigned long)(b)) << 16) | \
   (((unsigned long)(c)) <<  8) | \
   ((unsigned long)(d)))

/** Set an IP address given by the four byte-parts */
#define IP4_ADDR(ipaddr, a,b,c,d)  (ipaddr)->addr = PP_HTONL(LWIP_MAKEU32(a,b,c,d))

/** MEMCPY-like copying of IP addresses where addresses are known to be
 * 16-bit-aligned if the port is correctly configured (so a port could define
 * this to copying 2 unsigned short 's) - no NULL-pointer-checking needed. */
#ifndef IPADDR2_COPY
#define IPADDR2_COPY(dest, src) SMEMCPY(dest, src, sizeof(ip4_addr_t))
#endif

/** Copy IP address - faster than ip4_addr_set: no NULL check */
#define ip4_addr_copy(dest, src) ((dest).addr = (src).addr)
/** Safely copy one IP address to another (src may be NULL) */
#define ip4_addr_set(dest, src) ((dest)->addr = \
                                    ((src) == NULL ? 0 : \
                                    (src)->addr))
/** Set complete address to zero */
#define ip4_addr_set_zero(ipaddr)     ((ipaddr)->addr = 0)
/** Set address to IPADDR_ANY (no need for lwip_htonl()) */
#define ip4_addr_set_any(ipaddr)      ((ipaddr)->addr = IPADDR_ANY)
/** Set address to loopback address */
#define ip4_addr_set_loopback(ipaddr) ((ipaddr)->addr = PP_HTONL(IPADDR_LOOPBACK))
/** Check if an address is in the loopback region */
#define ip4_addr_isloopback(ipaddr)    (((ipaddr)->addr & PP_HTONL(IP_CLASSA_NET)) == PP_HTONL(((unsigned long)IP_LOOPBACKNET) << 24))
/** Safely copy one IP address to another and change byte order
 * from host- to network-order. */
#define ip4_addr_set_hton(dest, src) ((dest)->addr = \
                               ((src) == NULL ? 0:\
                               lwip_htonl((src)->addr)))
/** IPv4 only: set the IP address given as an unsigned long */
#define ip4_addr_set_u32(dest_ipaddr, src_u32) ((dest_ipaddr)->addr = (src_u32))
/** IPv4 only: get the IP address as an unsigned long */
#define ip4_addr_get_u32(src_ipaddr) ((src_ipaddr)->addr)

/** Get the network address by combining host address with netmask */
#define ip4_addr_get_network(target, host, netmask) do { ((target)->addr = ((host)->addr) & ((netmask)->addr)); } while(0)

/**
 * Determine if two address are on the same network.
 *
 * @arg addr1 IP address 1
 * @arg addr2 IP address 2
 * @arg mask network identifier mask
 * @return !0 if the network identifiers of both address match
 */
#define ip4_addr_netcmp(addr1, addr2, mask) (((addr1)->addr & \
                                              (mask)->addr) == \
                                             ((addr2)->addr & \
                                              (mask)->addr))
#define ip4_addr_cmp(addr1, addr2) ((addr1)->addr == (addr2)->addr)

#define ip4_addr_isany_val(addr1)   ((addr1).addr == IPADDR_ANY)
#define ip4_addr_isany(addr1) ((addr1) == NULL || ip4_addr_isany_val(*(addr1)))

#define ip4_addr_isbroadcast(addr1, netif) ip4_addr_isbroadcast_u32((addr1)->addr, netif)
unsigned char ip4_addr_isbroadcast_u32(unsigned long addr, const struct netif *netif);

#define ip_addr_netmask_valid(netmask) ip4_addr_netmask_valid((netmask)->addr)
unsigned char ip4_addr_netmask_valid(unsigned long netmask);

#define ip4_addr_ismulticast(addr1) (((addr1)->addr & PP_HTONL(0xf0000000UL)) == PP_HTONL(0xe0000000UL))

#define ip4_addr_islinklocal(addr1) (((addr1)->addr & PP_HTONL(0xffff0000UL)) == PP_HTONL(0xa9fe0000UL))

#define ip4_addr_debug_print_parts(debug, a, b, c, d) \
  LWIP_DEBUGF(debug, ("%" U16_F ".%" U16_F ".%" U16_F ".%" U16_F, a, b, c, d))
#define ip4_addr_debug_print(debug, ipaddr) \
  ip4_addr_debug_print_parts(debug, \
                      (unsigned short )((ipaddr) != NULL ? ip4_addr1_16(ipaddr) : 0),       \
                      (unsigned short )((ipaddr) != NULL ? ip4_addr2_16(ipaddr) : 0),       \
                      (unsigned short )((ipaddr) != NULL ? ip4_addr3_16(ipaddr) : 0),       \
                      (unsigned short )((ipaddr) != NULL ? ip4_addr4_16(ipaddr) : 0))
#define ip4_addr_debug_print_val(debug, ipaddr) \
  ip4_addr_debug_print_parts(debug, \
                      ip4_addr1_16(&(ipaddr)),       \
                      ip4_addr2_16(&(ipaddr)),       \
                      ip4_addr3_16(&(ipaddr)),       \
                      ip4_addr4_16(&(ipaddr)))

/* Get one byte from the 4-byte address */
#define ip4_addr1(ipaddr) (((const unsigned char*)(&(ipaddr)->addr))[0])
#define ip4_addr2(ipaddr) (((const unsigned char*)(&(ipaddr)->addr))[1])
#define ip4_addr3(ipaddr) (((const unsigned char*)(&(ipaddr)->addr))[2])
#define ip4_addr4(ipaddr) (((const unsigned char*)(&(ipaddr)->addr))[3])
/* These are cast to unsigned short , with the intent that they are often arguments
 * to printf using the U16_F format from cc.h. */
#define ip4_addr1_16(ipaddr) ((unsigned short )ip4_addr1(ipaddr))
#define ip4_addr2_16(ipaddr) ((unsigned short )ip4_addr2(ipaddr))
#define ip4_addr3_16(ipaddr) ((unsigned short )ip4_addr3(ipaddr))
#define ip4_addr4_16(ipaddr) ((unsigned short )ip4_addr4(ipaddr))

#define IP4ADDR_STRLEN_MAX  16

/** For backwards compatibility */
#define ip_ntoa(ipaddr)  ipaddr_ntoa(ipaddr)

unsigned long ipaddr_addr(const char *cp);
int ip4addr_aton(const char *cp, ip4_addr_t *addr);
/** returns ptr to static buffer; not reentrant! */
char *ip4addr_ntoa(const ip4_addr_t *addr);
char *ip4addr_ntoa_r(const ip4_addr_t *addr, char *buf, int buflen);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_IPV4 */

#endif /* LWIP_HDR_IP_ADDR_H */