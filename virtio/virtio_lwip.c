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

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */

#include "lwip/opt.h"

#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/ethip6.h"
#include "lwip/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/timeouts.h"
#include "netif/ethernet.h"

#include "virtio_internal.h"
#include <stdio.h>

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
 struct ethernetif {
     struct eth_addr *ethaddr;
     /* Add whatever per-interface state that is needed here. */
 };

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    vnet *vn = netif->state;
    struct pbuf *q;

    console("outputy\n");
    print_u64(p->tot_len);
    console("\n");
    print_u64(p->len);
    console("\n");
    // initiate transfer();


    for (q = p; q != NULL; q = q->next) {
        /* Send the data from the pbuf to the interface, one pbuf at a
           time. The size of the data in each pbuf is kept in the ->len
           variable. */
        // send data from(q->payload, q->len);
    }

    // signal that packet should be sent();

    MIB2_STATS_NETIF_ADD(netif, ifoutoctets, p->tot_len);
    if (((u8_t *)p->payload)[0] & 1) {
        /* broadcast or multicast packet*/
        MIB2_STATS_NETIF_INC(netif, ifoutnucastpkts);
    } else {
        /* unicast packet */
        MIB2_STATS_NETIF_INC(netif, ifoutucastpkts);
    }
    /* increase ifoutdiscards or ifouterrors on error */

    LINK_STATS_INC(link.xmit);

    return ERR_OK;
}


static void ethernetif_input(struct netif *netif)
{
    struct ethernetif *ethernetif;
    struct eth_hdr *ethhdr;
    /* move received packet into a new pbuf */
    vnet vn= netif->state;
    struct pbuf *p, *q;
    u16_t len = 0x1e; // bytes

    /* We allocate a pbuf chain of pbufs from the pool. */
    /* do this on prepost */
    p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

    if (p != NULL) {
        /* We iterate over the pbuf chain until we have read the entire
         * packet into the pbuf. */
        for (q = p; q != NULL; q = q->next) {
            /* In this case, ensure the tot_len member of the
             * pbuf is the sum of the chained pbuf len members.
             */
            // read data into(q->payload, q->len);
        }
        MIB2_STATS_NETIF_ADD(netif, ifinoctets, p->tot_len);
        if (((u8_t *)p->payload)[0] & 1) {
            /* broadcast or multicast packet*/
            MIB2_STATS_NETIF_INC(netif, ifinnucastpkts);
        } else {
            /* unicast packet*/
            MIB2_STATS_NETIF_INC(netif, ifinucastpkts);
        }
        LINK_STATS_INC(link.recv);
    } else {
        // drop packet();
        LINK_STATS_INC(link.memerr);
        LINK_STATS_INC(link.drop);
        MIB2_STATS_NETIF_INC(netif, ifindiscards);
    }

    /* if no packet could be read, silently ignore this */
    if (p != NULL) {
        /* pass all packets to ethernet_input, which decides what packets it supports */
        if (netif->input(p, netif) != ERR_OK) {
            LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
            pbuf_free(p);
            p = NULL;
        }
    }
}

static err_t virtioif_init(struct netif *netif)
{
    vnet vn = netif->state;
    /* Initialize interface hostname */
    //    netif->hostname = "virtiosomethingsomething";


    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;
    /* We directly use etharp_output() here to save a function call.
     * You can instead declare your own function an call etharp_output()
     * from it if you have to do some checks before sending (e.g. if link
     * is available...) */
    netif->output = etharp_output;
    netif->linkoutput = low_level_output;

    //    ethernetif->ethaddr = (struct eth_addr *) & (netif->hwaddr[0]);

    netif->hwaddr_len = ETHARP_HWADDR_LEN;
    vnet_hardware_address(vn, netif->hwaddr);
    console("calling printf\n");
    printf ("hardware address: %02x%02x%02x%02x%02x%02x%02x\n",
            netif->hwaddr[0],
            netif->hwaddr[1],
            netif->hwaddr[2],
            netif->hwaddr[3],
            netif->hwaddr[4],
            netif->hwaddr[5]);
    netif->mtu = 1500;

    /* device capabilities */
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP | NETIF_FLAG_UP;
    return ERR_OK;
}

void poll_interface(vnet vn)
{
}

void register_lwip_interface(vnet vn)
{
    struct netif *n = allocate(general, sizeof(struct netif));
    
    n->state = vn;

    console("allocated\n");
    print_u64((u64)n);
    
    netif_add(n,
              0, 0, 0, //&x, &x, &x,
              vn, // i dont understand why this is getting passed
              virtioif_init,
              ethernet_input);

    console("starting dhcp\n");
    dhcp_start(n);
    
    while(1) {
        poll_interface(vn);
        sys_check_timeouts();
    }
}
