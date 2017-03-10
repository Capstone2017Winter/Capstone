#include <stdio.h>
#include "dm9000a.h"
#include "dm9000a_regs.h"
#include "basic_io.h"
#include <ether.h>

dm9000a_dev g_dm9ka;
int autoReset = 0;

#define DM9KA dm9000a_dev*

void dm9ka_stats(void * pio, int iface);
int dm9ka_init(int iface);
int dm9ka_pkt_send(PACKET pkt);
int dm9ka_close(int iface);


void dm9000a_iow(unsigned int reg, unsigned int data)
{
  IOWR(DM9000A_INST_BASE, IO_addr, reg);
  usleep(STD_DELAY);
  IOWR(DM9000A_INST_BASE, IO_data, data);
}
unsigned int dm9000a_ior(unsigned int reg)
{
  IOWR(DM9000A_INST_BASE, IO_addr, reg);
  usleep(STD_DELAY);
  return IORD(DM9000A_INST_BASE, IO_data);
}

void phy_write(unsigned int reg, unsigned int value)
{ 
  /* set PHY register address into EPAR REG. 0CH */
  dm9000a_iow(0x0C, reg | 0x40); /* PHY register address setting,
			    and DM9000_PHY offset = 0x40 */

  /* fill PHY WRITE data into EPDR REG. 0EH & REG. 0DH */
  dm9000a_iow(0x0E, ((value >> 8) & 0xFF));   /* PHY data high_byte */
  dm9000a_iow(0x0D, value & 0xFF);            /* PHY data low_byte */

  /* issue PHY + WRITE command = 0xa into EPCR REG. 0BH */
  dm9000a_iow(0x0B, 0x8);                     /* clear PHY command first */
  IOWR(DM9000A_INST_BASE, IO_data, 0x0A);  /* issue PHY + WRITE command */
  usleep(STD_DELAY);
  IOWR(DM9000A_INST_BASE, IO_data, 0x08);  /* clear PHY command again */
  usleep(50);  /* wait 1~30 us (>20 us) for PHY + WRITE completion */
}

/* DM9000_init I/O routine */
unsigned int dm9000a_reset(unsigned char *mac_address)
{
  unsigned int  i;
  /* set the internal PHY power-on (GPIOs normal settings) */
  dm9000a_iow(0x1E, 0x01);  /* GPCR REG. 1EH = 1 selected
                    		       GPIO0 "output" port for internal PHY */
  dm9000a_iow(0x1F, 0x00);  // GPR  REG. 1FH GEPIO0
                    		    //   Bit [0] = 0 to activate internal PHY */
  msleep(10);                // wait > 2 ms for PHY power-up ready 

  /* software-RESET NIC */
  dm9000a_iow(NCR, 0x03);   /* NCR REG. 00 RST Bit [0] = 1 reset on,
		                           and LBK Bit [2:1] = 01b MAC loopback on */
  
  usleep(20);               /* wait > 10us for a software-RESET ok */
  
  dm9000a_iow(NCR, 0x00);   /* normalize */
  dm9000a_iow(NCR, 0x03);
  usleep(20);
  dm9000a_iow(NCR, 0x00);
  dm9000a_iow(ISR, 0x3F); /* clear the ISR status: PRS, PTS, ROS, ROOS 4 bits,
                     		      by RW/C1 */
  dm9000a_iow(NSR, 0x2C); /* clear the TX status: TX1END, TX2END, WAKEUP 3 bits,
		                          by RW/C1 */

  /* set GPIO0=1 then GPIO0=0 to turn off and on the internal PHY */
  dm9000a_iow(0x1F, 0x01);  // GPR PHYPD Bit [0] = 1 turn-off PHY */
  dm9000a_iow(0x1F, 0x00);  // PHYPD Bit [0] = 0 activate phyxcer */
  msleep(20);               /* wait >4 ms for PHY power-up */

  /* set PHY operation mode */

  phy_write(0,PHY_reset);   /* reset PHY registers back to the default state */
  usleep(50);               /* wait >30 us for PHY software-RESET ok */
  phy_write(16, 0x404);     /* turn off PHY reduce-power-down mode only */
  phy_write(4, PHY_txab);   /* set PHY TX advertised ability:
                   			       ALL + Flow_control */  
  phy_write(0, 0x1200);     /* PHY auto-NEGO re-start enable
                               (RESTART_AUTO_NEGOTIATION +
                               AUTO_NEGOTIATION_ENABLE)
                               to auto sense and recovery PHY registers */
  msleep(20);                /* wait >2 ms for PHY auto-sense
			                         linking to partner */

  /* store MAC address into NIC */
  for (i = 0; i < 6; i++) 
    dm9000a_iow(16 + i, mac_address[i]);

  /* clear any pending interrupt */
  dm9000a_iow(ISR, 0x3F); /* clear the ISR status: PRS, PTS, ROS, ROOS 4 bits,
                     		      by RW/C1 */
  dm9000a_iow(NSR, 0x2C); /* clear the TX status: TX1END, TX2END, WAKEUP 3 bits,
		                          by RW/C1 */

  /* program operating registers~ */
  dm9000a_iow(NCR, NCR_set); /* NCR REG. 00 enable the chip functions
                        (and disable this MAC loopback mode back to normal) */
  dm9000a_iow(0x08, BPTR_set); /* BPTR  REG.08  (if necessary) RX Back Pressure
                                  Threshold in Half duplex moe only:
                                  High Water 3KB, 600 us */
  dm9000a_iow(0x09, FCTR_set);  /* FCTR  REG.09  (if necessary)
                                   Flow Control Threshold setting
                                   High/ Low Water Overflow 5KB/ 10KB */
  dm9000a_iow(0x0A, RTFCR_set); /* RTFCR REG.0AH (if necessary)
                                   RX/TX Flow Control Register enable TXPEN, BKPM
                                   (TX_Half), FLCE (RX) */
  dm9000a_iow(0x0F, 0x00);      /* Clear the all Event */
  // dm9000a_iow(0x2D, 0x80);      /* Switch LED to mode 1 */

  /* set other registers depending on applications */
  dm9000a_iow(ETXCSR, ETXCSR_set); /* Early Transmit 75% */

  /* enable interrupts to activate DM9000 ~on */
  dm9000a_iow(IMR, INTR_set);   /* IMR REG. FFH PAR=1 only,
			                             or + PTM=1& PRM=1 enable RxTx interrupts */

  /* enable RX (Broadcast/ ALL_MULTICAST) ~go */
  dm9000a_iow(RCR, RCR_set | RX_ENABLE | PASS_MULTICAST);
  /* RCR REG. 05 RXEN Bit [0] = 1 to enable the RX machine/ filter */

  /* RETURN "DEVICE_SUCCESS" back to upper layer */
  return  (dm9000a_ior(0x2D)==0x80) ? DMFE_SUCCESS : DMFE_FAIL;
}

unsigned int TransmitPacket(unsigned char *data_ptr, unsigned int tx_len)
{
  unsigned int i;
  
  /* mask NIC interrupts IMR: PAR only */
  dm9000a_iow(IMR, PAR_set);

  /* issue TX packet's length into TXPLH REG. FDH & TXPLL REG. FCH */
  dm9000a_iow(0xFD, (tx_len >> 8) & 0xFF);  /* TXPLH High_byte length */
  dm9000a_iow(0xFC, tx_len & 0xFF);         /* TXPLL Low_byte  length */

  /* wirte transmit data to chip SRAM */
  IOWR(DM9000A_INST_BASE, IO_addr, MWCMD);  /* set MWCMD REG. F8H
					  TX I/O port ready */
  for (i = 0; i < tx_len; i += 2) {
    usleep(STD_DELAY);
    IOWR(DM9000A_INST_BASE, IO_data, (data_ptr[i+1]<<8)|data_ptr[i] );
  }

  /* issue TX polling command activated */
  dm9000a_iow(TCR , TCR_set | TX_REQUEST);  /* TXCR Bit [0] TXREQ auto clear
				       after TX completed */

  /* wait for transmit complete */
  while(!(dm9000a_ior(NSR)&0x0C)) {
    usleep(STD_DELAY);
  }

  /* clear the NSR Register */
  dm9000a_iow(NSR,0x00);

  /* re-enable NIC interrupts */
  dm9000a_iow(IMR, INTR_set);

  /* RETURN "TX_SUCCESS" to upper layer */
  return  DMFE_SUCCESS;
}

#ifndef MTU
#define MTU 1514
#endif

int prep_dm9000a(int index)
{  
  DM9KA dm9ka = &g_dm9ka;
  NET ifp;

  ifp = nets[index];
  ifp->n_mib->ifAdminStatus = 2;   /* status = down */
  ifp->n_mib->ifOperStatus = 2;    /* will be set up in init()  */
  ifp->n_mib->ifLastChange = cticks * (100/TPS);
  ifp->n_mib->ifPhysAddress = (u_char*)dm9ka->mac_addr;
  ifp->n_mib->ifDescr = (u_char*)"DM9000A series ethernet";
  ifp->n_lnh = ETHHDR_SIZE;        /* ethernet header size */
  ifp->n_hal = 6;                  /* hardware address length */
  ifp->n_mib->ifType = ETHERNET;   /* device type */
  ifp->n_mtu = MTU;                /* max frame size */
  
  /* install our hardware driver routines */
  ifp->n_init   = dm9ka_init;
  ifp->pkt_send = dm9ka_pkt_send;
  ifp->n_close  = dm9ka_close;
  ifp->n_stats  = dm9ka_stats;

#ifdef IP_V6
  ifp->n_flags |= (NF_NBPROT | NF_IPV6);
#else
  ifp->n_flags |= NF_NBPROT;
#endif

  get_mac_addr(dm9ka->netp, dm9ka->mac_addr);
    
  /* set cross-pointers between iface and smsc structs */
  dm9ka->netp    = ifp;
  dm9ka->intnum  = DM9000A_INST_IRQ;
  dm9ka->regbase = DM9000A_INST_BASE;
  dm9ka->sending = 0;
  dm9ka->rx_ints = 0;
  dm9ka->tx_ints = 0;
  dm9ka->rcv_len = 0;
  dm9ka->snd_len = 0;
  dm9ka->tosend.q_len = 0;
  dm9ka->tosend.q_max = 0;
  
  ifp->n_local = (void*)dm9ka;

  return ++index;
}

/* HAL init ...
 * just init dev structs and let the stack know we are here
 */
error_t dm9000a_init(alt_iniche_dev *p_dev)
{
  prep_dm9000a(p_dev->if_num);
  return 0;  
}

static unsigned char dm9000a_rxReady(DM9KA dm9ka)
{
  unsigned char rv = 0;
  /* dummy read a byte from MRCMDX REG. F0H */
  rv = dm9000a_ior(MRCMDX);
  /* got most updated byte: rx_READY */
  rv = IORD(dm9ka->regbase, IO_data) & 0x03;  
  return rv;
}

static void dm9000a_isr(int iface)
{
  unsigned char rx_rdy, istatus;
  unsigned int  tmp, rx_sts, i, rx_len;
  struct ethhdr * eth;
  PACKET pkt;
  DM9KA dm9ka = (DM9KA)nets[iface]->n_local;
  
  /* mask NIC interrupts IMR: PAR only */
  dm9000a_iow(IMR, PAR_set);
  istatus = dm9000a_ior(ISR);

  rx_rdy = dm9000a_rxReady(dm9ka);  
  usleep(STD_DELAY);
  
  while(rx_rdy == DM9000_PKT_READY)
  {
    /* get RX Status & Length from RX SRAM */
    /* set MRCMD REG. F2H RX I/O port ready */    
    IOWR(dm9ka->regbase, IO_addr, MRCMD); 
    usleep(STD_DELAY);
    rx_sts = IORD(dm9ka->regbase,IO_data);
    usleep(STD_DELAY);
    rx_len = IORD(dm9ka->regbase,IO_data);
    
    /* Check this packet_status: GOOD or BAD? */
    if( !(rx_sts & 0xBF00) && (rx_len < MAX_PACKET_SIZE) )
    {
      if ((pkt = pk_alloc(rx_len + ETHHDR_BIAS)) == NULL)   
      { /* couldn't get a free buffer for rx */
        dm9ka->netp->n_mib->ifInDiscards++;
        /* treat packet as bad, dump it from RX SRAM */
        for (i = 0; i < rx_len; i += 2) {
          usleep(STD_DELAY);
          tmp = IORD(dm9ka->regbase, IO_data);
        }
      }
      else
      { /* packet allocation succeeded */
        unsigned char* data_ptr = pkt->nb_buff + ETHHDR_BIAS;
        /* read 1 received packet from RX SRAM into RX packet buffer */
        for (i = 0; i < rx_len; i += 2) {
          usleep(STD_DELAY);
          tmp = IORD(dm9ka->regbase, IO_data);
          *data_ptr++ = tmp & 0xFF;
          *data_ptr++ = (tmp>>8) & 0xFF;
        }

        pkt->nb_prot = pkt->nb_buff + ETHHDR_SIZE;
        pkt->nb_plen = rx_len - 14;
        pkt->nb_tstamp = cticks;
        pkt->net = dm9ka->netp;

        /* set packet type for demux routine */
        eth = (struct ethhdr *)(pkt->nb_buff + ETHHDR_BIAS);
        pkt->type = eth->e_type;

        /* shove packet into iniche stack's recv queue */
        //printf("rx: 0x%x l %d s %x:%x:%x\n", eth->e_type, rx_len,
        //       eth->e_src[0], eth->e_src[1], eth->e_src[2]);
        putq(&rcvdq, pkt);
        SignalPktDemux();
      }      
    } else {
      /* this packet is bad, dump it from RX SRAM */
      for (i = 0; i < rx_len; i += 2) {
        usleep(STD_DELAY);
        tmp = IORD(dm9ka->regbase, IO_data);
      }
      rx_len = 0;
    }

    usleep(STD_DELAY);
    rx_rdy = dm9000a_rxReady(dm9ka);
    usleep(STD_DELAY);    
  }
  
  if (rx_rdy & 0x02)
  { /* status check first byte: rx_READY Bit[1:0] must be "00"b or "01"b */
    /* software-RESET NIC */
    printf("whoa ... got a strange thing here ... \n");
    autoReset++;
    dm9000a_iow(NCR, 0x03);   /* NCR REG. 00 RST Bit [0] = 1 reset on,
                          			 and LBK Bit [2:1] = 01b MAC loopback on */
    usleep(20);               /* wait > 10us for a software-RESET ok */
    dm9000a_iow(NCR, 0x00);   /* normalize */
    dm9000a_iow(NCR, 0x03);
    usleep(20);
    dm9000a_iow(NCR, 0x00);    
    /* program operating registers~ */
    dm9000a_iow(NCR, NCR_set); /* NCR REG. 00 enable the chip functions
                   			 (and disable this MAC loopback mode back to normal) */
    dm9000a_iow(0x08, BPTR_set); /* BPTR  REG.08 (if necessary) RX Back Pressure
                                    Threshold in Half duplex mode only:
                                    High Water 3KB, 600 us */
    dm9000a_iow(0x09, FCTR_set);  /* FCTR  REG.09  (if necessary)
                                   Flow Control Threshold setting High/Low Water
                                   Overflow 5KB/ 10KB */
    dm9000a_iow(0x0A, RTFCR_set); /* RTFCR REG.0AH (if necessary)
                          			     RX/TX Flow Control Register
			                               enable TXPEN, BKPM (TX_Half), FLCE (RX) */
    dm9000a_iow(0x0F, 0x00);      /* Clear the all Event */
    dm9000a_iow(0x2D, 0x80);      /* Switch LED to mode 1 */
    /* set other registers depending on applications */
    dm9000a_iow(ETXCSR, ETXCSR_set); /* Early Transmit 75% */
    /* enable interrupts to activate DM9000 ~on */
    dm9000a_iow(IMR, INTR_set);   /* IMR REG. FFH PAR=1 only,
			                               or + PTM=1& PRM=1 enable RxTx interrupts */
    /* enable RX (Broadcast/ ALL_MULTICAST) ~go */
    dm9000a_iow(RCR , RCR_set | RX_ENABLE | PASS_MULTICAST);
    /* RCR REG. 05 RXEN Bit [0] = 1 to enable the RX machine/ filter */
  }
  
  /* Clear the DM9000A ISR: PRS, PTS, ROS, ROOS 4 bits, by RW/C1 */
  dm9000a_iow(ISR, 0x3F);  
  /* Re-enable DM9000A interrupts */
  dm9000a_iow(IMR, INTR_set);  
}


int netisrs = 0;

void dm9Ka_isr_wrap(void *context, u_long intnum)
{
  netisrs++;
  dm9000a_isr((int)context);
}

int dm9ka_init(int iface)
{
  int err;
  DM9KA   dm9ka;
  printf("dm9ka_init\n");

   /* get pointer to device structure */
  dm9ka = (DM9KA)nets[iface]->n_local;

  err = dm9000a_reset(dm9ka->mac_addr);

  /* register the ISR with the ALTERA HAL interface */
  err = alt_irq_register (dm9ka->intnum, (void *)iface, dm9Ka_isr_wrap);
  if (err)
    return (err);
  
  nets[iface]->n_mib->ifAdminStatus = 1;    /* status = UP */
  nets[iface]->n_mib->ifOperStatus  = 1;
  
  return (0);
}


int dm9ka_close(int iface)
{
  DM9KA   dm9ka;
  printf("dm9ka_close\n");
  nets[iface]->n_mib->ifAdminStatus = 2;    /* status = down */
  
   /* get pointer to device structure */
  dm9ka = (DM9KA)nets[iface]->n_local;

    /* software-RESET NIC */
  dm9000a_iow(NCR, 0x03);   /* NCR REG. 00 RST Bit [0] = 1 reset on,
                          			 and LBK Bit [2:1] = 01b MAC loopback on */
  usleep(20);               /* wait > 10us for a software-RESET ok */
  dm9000a_iow(NCR, 0x00);   /* normalize */
  dm9000a_iow(NCR, 0x03);
  usleep(20);
  dm9000a_iow(NCR, 0x00);

  /* this should reset these registers anyway, but 'just in case' */
  dm9000a_iow(IMR,  0x00);  /* no interrupts */
  dm9000a_iow(RCR , 0x00);  /* disable receive */
  dm9000a_iow(0x0F, 0x00);  /* Clear the all Event */
  
  //s91_port_close(smsc);   /* release the ISR */
  //s91_reset(smsc);        /* reset the chip */

  nets[iface]->n_mib->ifOperStatus = 2;     /* status = down */
  return 0;  
}

void dm9ka_stats(void * pio, int iface)
{
  DM9KA dm9ka;
  printf("dm9ka_stats\n");
  dm9ka = (DM9KA)(nets[iface]->n_local);

  /*
    ns_printf(pio, "Interrupts: rx:%lu, tx:%lu alloc:%lu, total:%lu\n",
            smsc->rx_ints, smsc->tx_ints, smsc->alloc_ints, smsc->total_ints);
    ns_printf(pio, "coll1:%lu collx:%lu overrun:%lu mdint:%lu\n",
            smsc->coll1, smsc->collx, smsc->rx_overrun, smsc->mdint);
    ns_printf(pio, "Sendq max:%d, current %d. IObase: 0x%lx ISR %d\n", 
            smsc->tosend.q_max, smsc->tosend.q_len, smsc->regbase, smsc->intnum);
  */
  
  return;
}

extern void irq_Mask(void);
extern void irq_Unmask(void);

int dm9ka_pkt_send(PACKET pkt)
{
  unsigned int rv, slen;  
  DM9KA dm9ka = (DM9KA)pkt->net->n_local;
  slen = pkt->nb_plen - ETHHDR_BIAS;
  if(slen < 64) slen = 64;

  //irq_Mask();
    
  rv = TransmitPacket(pkt->nb_prot + ETHHDR_BIAS, slen);
  if(rv == DMFE_SUCCESS) {
    /* update packet statistics */
    dm9ka->netp->n_mib->ifOutOctets += (u_long)pkt->nb_plen;
    if(*pkt->nb_prot & 0x80)
      dm9ka->netp->n_mib->ifOutNUcastPkts++;
    else
      dm9ka->netp->n_mib->ifOutUcastPkts++;
  }  
    
  //irq_Unmask();
  
  if(pkt) pk_free(pkt);
  
  return (0);      /* alloc done interrupt will start xmit */
}
