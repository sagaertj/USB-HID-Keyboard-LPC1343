/*----------------------------------------------------------------------------
 * U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 * Name:    usbhw_lpc13xx.c
 * Purpose: USB Hardware Layer Module for Philips LPC13xx
 * Version: V1.20
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------
 * History:
 *          V1.20 Added USB_ClearEPBuf
 *          V1.00 Initial Version
 *----------------------------------------------------------------------------*/

#include <LPC13xx.H>                        /* LPC13xx definitions */

#include "type.h"

#include "usb.h"
#include "usbcfg_LPC13xx.h"
#include "usbreg_LPC13xx.h"
#include "usbhw_LPC13xx.h"
#include "usbcore.h"
#include "usbuser.h"


#pragma diag_suppress 1441


#define EP_MSK_CTRL 0x0001      /* Control Endpoint Logical Address Mask */
#define EP_MSK_BULK 0x000E      /* Bulk Endpoint Logical Address Mask */
#define EP_MSK_INT  0x000E      /* Interrupt Endpoint Logical Address Mask */
#define EP_MSK_ISO  0x0010      /* Isochronous Endpoint Logical Address Mask */


/*
 *  Get Endpoint Physical Address
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    Endpoint Physical Address
 */

U32 EPAdr (U32 EPNum) {
  U32 val;

  val = (EPNum & 0x0F) << 1;
  if (EPNum & 0x80) {
    val += 1;
  }
  return (val);
}


/*
 *  Write Command
 *    Parameters:      cmd:   Command
 *    Return Value:    None
 */

void WrCmd (U32 cmd) {

  LPC_USB->DevIntClr = CCEMTY_INT;
  LPC_USB->CmdCode = cmd;
  while ((LPC_USB->DevIntSt & CCEMTY_INT) == 0);
}


/*
 *  Write Command Data
 *    Parameters:      cmd:   Command
 *                     val:   Data
 *    Return Value:    None
 */

void WrCmdDat (U32 cmd, U32 val) {

  LPC_USB->DevIntClr = CCEMTY_INT;
  LPC_USB->CmdCode = cmd;
  while ((LPC_USB->DevIntSt & CCEMTY_INT) == 0);
  LPC_USB->DevIntClr = CCEMTY_INT;
  LPC_USB->CmdCode = val;
  while ((LPC_USB->DevIntSt & CCEMTY_INT) == 0);
}


/*
 *  Write Command to Endpoint
 *    Parameters:      cmd:   Command
 *                     val:   Data
 *    Return Value:    None
 */

void WrCmdEP (U32 EPNum, U32 cmd){

  LPC_USB->DevIntClr = CCEMTY_INT;
  LPC_USB->CmdCode = CMD_SEL_EP(EPAdr(EPNum));
  while ((LPC_USB->DevIntSt & CCEMTY_INT) == 0);
  LPC_USB->DevIntClr = CCEMTY_INT;
  LPC_USB->CmdCode = cmd;
  while ((LPC_USB->DevIntSt & CCEMTY_INT) == 0);
}


/*
 *  Read Command Data
 *    Parameters:      cmd:   Command
 *    Return Value:    Data Value
 */

U32 RdCmdDat (U32 cmd) {

  LPC_USB->DevIntClr = CCEMTY_INT | CDFULL_INT;
  LPC_USB->CmdCode = cmd;
  while ((LPC_USB->DevIntSt & CDFULL_INT) == 0);
  return (LPC_USB->CmdData);
}


/*
 *  USB Initialize Function
 *   Called by the User to initialize USB
 *    Return Value:    None
 */

void USB_Init (void) {

  LPC_SYSCON->SYSAHBCLKCTRL |= ((1<<14) |   /* Enable AHB clock for USB_REG. */
                                (1<<16)  ); /* enable AHB clock for IO configuration block. */
                                            /* D+, D- needs not to be configured  */
  LPC_IOCON->PIO0_3  &= ~0x07;              /* P0.3 VBUS */
  LPC_IOCON->PIO0_3  |=  0x01;              /* Select function USB_VBUS */
  LPC_IOCON->PIO0_6  &= ~0x07;              /* P0.6 SoftConnect */
  LPC_IOCON->PIO0_6  |=  0x01;              /* Selects function USB_CONNECT */

#if USB_LPC13XX_FIQ
  /* only BULK EP3 and FRAME(ISO) can be routed to FIQ but not all at the same time. */
  LPC_USB->DevFIQSel = USB_LPC13XX_FIQ_VAL;
  NVIC_EnableIRQ(USB_FIQn);                 /* enable USB HighPriority interrupt */
#endif /* USB_LPC13XX_FIQ */

  NVIC_EnableIRQ(USB_IRQn);                 /* enable USB LowPriority interrupt */

  USB_Reset();
  USB_SetAddress(0);
}


/*
 *  USB Connect Function
 *   Called by the User to Connect/Disconnect USB
 *    Parameters:      con:   Connect/Disconnect
 *    Return Value:    None
 */

void USB_Connect (BOOL con) {
  WrCmdDat(CMD_SET_DEV_STAT, DAT_WR_BYTE(con ? DEV_CON : 0));
}


/*
 *  USB Reset Function
 *   Called automatically on USB Reset
 *    Return Value:    None
 */

void USB_Reset (void) {

  LPC_USB->DevIntClr = 0xFFFFFFFF;                        /* clear all interrupts */
//  LPC_USB->DevIntEn  = (0x1F << 1)    |                   /* enable EP0..4        interrupt */
  LPC_USB->DevIntEn  = (0xFF << 1)    |                   /* enable EP0..4        interrupt */
                       DEV_STAT_INT   |                   /* enable device status interrupt */
                       (USB_SOF_EVENT ? FRAME_INT : 0);   /* enable frame         interrupt */

}


/*
 *  USB Suspend Function
 *   Called automatically on USB Suspend
 *    Return Value:    None
 */

void USB_Suspend (void) {
  /* Performed by Hardware */
}


/*
 *  USB Resume Function
 *   Called automatically on USB Resume
 *    Return Value:    None
 */

void USB_Resume (void) {
  /* Performed by Hardware */
}


/*
 *  USB Remote Wakeup Function
 *   Called automatically on USB Remote Wakeup
 *    Return Value:    None
 */

void USB_WakeUp (void) {

  if (USB_DeviceStatus & USB_GETSTATUS_REMOTE_WAKEUP) {
    WrCmdDat(CMD_SET_DEV_STAT, DAT_WR_BYTE(DEV_CON));
  }
}


/*
 *  USB Remote Wakeup Configuration Function
 *    Parameters:      cfg:   Enable/Disable
 *    Return Value:    None
 */

void USB_WakeUpCfg (BOOL cfg) {
  /* Not needed */
}


/*
 *  USB Set Address Function
 *    Parameters:      adr:   USB Address
 *    Return Value:    None
 */

void USB_SetAddress (U32 adr) {
  WrCmdDat(CMD_SET_ADDR, DAT_WR_BYTE(DEV_EN | adr)); /* Don't wait for next */
  WrCmdDat(CMD_SET_ADDR, DAT_WR_BYTE(DEV_EN | adr)); /*  Setup Status Phase */
}


/*
 *  USB Configure Function
 *    Parameters:      cfg:   Configure/Deconfigure
 *    Return Value:    None
 */

void USB_Configure (BOOL cfg) {

  WrCmdDat(CMD_CFG_DEV, DAT_WR_BYTE(cfg ? CONF_DVICE : 0));
}


/*
 *  Configure USB Endpoint according to Descriptor
 *    Parameters:      pEPD:  Pointer to Endpoint Descriptor
 *    Return Value:    None
 */

void USB_ConfigEP (USB_ENDPOINT_DESCRIPTOR *pEPD) {
  /*
     EPs need not to be configured.
     EPs use fix maxPacketSize: Control, Bulk, Interrupt  64Byte
                                Isochronous              512Byte
   */
}


/*
 *  Set Direction for USB Control Endpoint
 *    Parameters:      dir:   Out (dir == 0), In (dir <> 0)
 *    Return Value:    None
 */

void USB_DirCtrlEP (U32 dir) {
  /* Not needed */
}


/*
 *  Enable USB Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USB_EnableEP (U32 EPNum) {
  WrCmdDat(CMD_SET_EP_STAT(EPAdr(EPNum)), DAT_WR_BYTE(0));
}


/*
 *  Disable USB Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USB_DisableEP (U32 EPNum) {
  WrCmdDat(CMD_SET_EP_STAT(EPAdr(EPNum)), DAT_WR_BYTE(EP_STAT_DA));
}


/*
 *  Reset USB Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USB_ResetEP (U32 EPNum) {
  WrCmdDat(CMD_SET_EP_STAT(EPAdr(EPNum)), DAT_WR_BYTE(0));
}


/*
 *  Set Stall for USB Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USB_SetStallEP (U32 EPNum) {
  WrCmdDat(CMD_SET_EP_STAT(EPAdr(EPNum)), DAT_WR_BYTE(EP_STAT_ST));
}


/*
 *  Clear Stall for USB Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USB_ClrStallEP (U32 EPNum) {
  WrCmdDat(CMD_SET_EP_STAT(EPAdr(EPNum)), DAT_WR_BYTE(0));
}


/*
 *  Clear USB Endpoint Buffer
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USB_ClearEPBuf (U32 EPNum) {
  WrCmdEP(EPNum, CMD_CLR_BUF);
}


/*
 *  Read USB Endpoint Data
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *                     pData: Pointer to Data Buffer
 *    Return Value:    Number of bytes read
 */

U32 USB_ReadEP (U32 EPNum, U8 *pData) {
  U32 cnt, n;

  LPC_USB->Ctrl = ((EPNum & 0x0F) << 2) | CTRL_RD_EN;
  for (n = 0; n < 3; n++) __nop();   /* 3 clock cycles to fetch the packet length from RAM. */

  do {
    cnt = LPC_USB->RxPLen;
  } while ((cnt & PKT_DV) == 0);
  cnt &= PKT_LNGTH_MASK;

  for (n = 0; n < (cnt + 3) / 4; n++) {
    *((__packed U32 *)pData) = LPC_USB->RxData;
    pData += 4;
  }

  LPC_USB->Ctrl = 0;

  if (((EP_MSK_ISO >> (EPNum & 0x0F)) & 1) == 0) {   /* Non-Isochronous Endpoint */
    WrCmdEP(EPNum, CMD_CLR_BUF);
  }

  return (cnt);
}


/*
 *  Write USB Endpoint Data
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *                     pData: Pointer to Data Buffer
 *                     cnt:   Number of bytes to write
 *    Return Value:    Number of bytes written
 */

U32 USB_WriteEP (U32 EPNum, U8 *pData, U32 cnt) {
  U32 n;

  LPC_USB->Ctrl = ((EPNum & 0x0F) << 2) | CTRL_WR_EN;
//  for (n = 0; n < 3; n++) __nop();   /* 3 clock cycles to fetch the packet length from RAM. */
  LPC_USB->TxPLen = cnt;

  for (n = 0; n < (cnt + 3) / 4; n++) {
    LPC_USB->TxData = *((__packed U32 *)pData);
    pData += 4;
  }

  LPC_USB->Ctrl = 0;

  WrCmdEP(EPNum, CMD_VALID_BUF);

  return (cnt);
}


/*
 *  Get USB Last Frame Number
 *    Parameters:      None
 *    Return Value:    Frame Number
 */

U32 USB_GetFrame (void) {
  U32 val;

  WrCmd(CMD_RD_FRAME);
  val = RdCmdDat(DAT_RD_FRAME);
  val = val | (RdCmdDat(DAT_RD_FRAME) << 8);

  return (val);
}


/*
 *  USB Low Priority Interrupt Service Routine
 */

void USB_IRQHandler (void) {
  U32 disr, val, n, m;
  U32 episr, episrCur;

  disr = LPC_USB->DevIntSt;                        /* Device Interrupt Status */
  LPC_USB->DevIntClr = disr;                       /* clear interrupts */  

  /* Device Status Interrupt (Reset, Connect change, Suspend/Resume) */
  if (disr & DEV_STAT_INT) {
    WrCmd(CMD_GET_DEV_STAT);
    val = RdCmdDat(DAT_GET_DEV_STAT);              /* Device Status */
    if (val & DEV_RST) {                           /* Reset */
      USB_Reset();
#if   USB_RESET_EVENT
      USB_Reset_Event();
#endif
    }
    if (val & DEV_CON_CH) {                        /* Connect change */
#if   USB_POWER_EVENT
      USB_Power_Event(val & DEV_CON);
#endif
    }
    if (val & DEV_SUS_CH) {                        /* Suspend/Resume */
      if (val & DEV_SUS) {                         /* Suspend */
        USB_Suspend();
#if     USB_SUSPEND_EVENT
        USB_Suspend_Event();
#endif
      } else {                                     /* Resume */
        USB_Resume();
#if     USB_RESUME_EVENT
        USB_Resume_Event();
#endif
      }
    }
    goto isr_end;
  }

#if USB_SOF_EVENT
  /* Start of Frame Interrupt */
  if (disr & FRAME_INT) {
    USB_SOF_Event();
  }
#endif

#if USB_ERROR_EVENT
  /* Error Interrupt not available */
  WrCmd(CMD_GET_ERR_CODE);                         /* get error status from command engine */
  val = RdCmdDat(DAT_GET_ERR_CODE);
  if (val != 0) {
    USB_Error_Event(val);
  }
#endif

  /* logical Endpoint0..3 Interrupts */
  if (disr & EP_INT) {
    episrCur = 0;
    episr    = (disr & EP_INT) >> 1;               /* handle only EP interrupts */
    for (n = 0; n < 8; n++) {                      /* Check All Endpoints (ISO EP do not create interrupts) */
      if (episr == episrCur) break;                /* break if all EP interrupts handled */
      if (episr & (1 << n)) {
        episrCur |= (1 << n);
        m = n >> 1;
  
        WrCmd(CMD_SEL_EP_CLRI(n));                 /* clear EP interrupt by sending cmd to the command engine. */
        val = RdCmdDat(DAT_SEL_EP_CLRI(n));
  
        if ((n & 1) == 0) {                        /* OUT Endpoint */
          if (n == 0) {                            /* Control OUT Endpoint */
            if (val & EP_SEL_STP) {                /* Setup Packet */
              if (USB_P_EP[0]) {
                USB_P_EP[0](USB_EVT_SETUP);
                continue;
              }
            }
          }
          if (USB_P_EP[m]) {
            USB_P_EP[m](USB_EVT_OUT);
          }
        } else {                                   /* IN Endpoint */
          if (USB_P_EP[m]) {
            USB_P_EP[m](USB_EVT_IN);
          }
        }
      }
    }

  }

isr_end:
  ;
}


#if USB_LPC13XX_FIQ
#pragma diag_suppress 177, 550
/*
 *  USB High Priority Interrupt Service Routine
 */

void USB_FIQHandler(void) {
  U32 disr, val, n, m;
  U32 episr, episrCur;

  disr = LPC_USB->DevIntSt;                        /* Device Interrupt Status */
  LPC_USB->DevIntClr = disr;                       /* clear interrupts */  

#if USB_SOF_EVENT && (USB_LPC13XX_FIQ_VAL & 0x01)
  /* Start of Frame Interrupt */
  if (disr & FRAME_INT) {
    USB_SOF_Event();
  }
#endif

#if (USB_LPC13XX_FIQ_VAL & 0x06)
  /* logical Endpoint3 Interrupts */
  if (disr & (EP7_INT | EP6_INT)) {
    episrCur = 0;
    episr    = (disr & (EP7_INT | EP6_INT)) >> 1;  /* handle only logical EP3 interrupts */
    for (n = 6; n < 8; n++) {                      /* Check logical Endpoint3 */
      if (episr == episrCur) break;                /* break if all EP3 interrupts handled */
      if (episr & (1 << n)) {
        episrCur |= (1 << n);
        m = n >> 1;
  
        WrCmd(CMD_SEL_EP_CLRI(n));                 /* clear EP interrupt by sending cmd to the command engine. */
        val = RdCmdDat(DAT_SEL_EP_CLRI(n));
  
        if ((n & 1) == 0) {                        /* OUT Endpoint */
          if (USB_P_EP[m]) {
            USB_P_EP[m](USB_EVT_OUT);
          }
        } else {                                   /* IN Endpoint */
          if (USB_P_EP[m]) {
            USB_P_EP[m](USB_EVT_IN);
          }
        }
      }
    }

  }
#endif

}
#endif /* USB_LPC13XX_FIQ */
