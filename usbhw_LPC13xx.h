/*----------------------------------------------------------------------------
 * U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 * Name:    usbhw_lpc13xx.h
 * Purpose: USB Hardware Layer Definitions
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

#ifndef __USBHW_LPC13xx_H__
#define __USBHW_LPC13xx_H__

/* USB Error Codes (see usbreg.h) */
#define USB_ERR_NOERROR         0x00  /* No Error */
#define USB_ERR_PID_ENCODE      0x01  /* PID Encoding Error */
#define USB_ERR_UNKNOWN_PID     0x02  /* Unknown PID */
#define USB_ERR_UNEXPECT_PKT    0x03  /* Unexpected Packet */
#define USB_ERR_TCRC            0x04  /* Error in Token CRC */
#define USB_ERR_DCRC            0x05  /* Error in Data CRC */
#define USB_ERR_TIMEOUT         0x06  /* Time Out Error */
#define USB_ERR_BABBIE          0x07  /* Babble */
#define USB_ERR_EOF_PKT         0x08  /* Error in End of Packet */
#define USB_ERR_TX_RX_NAK       0x09  /* Sent/Received NAK */
#define USB_ERR_SENT_STALL      0x0A  /* Sent Stall */
#define USB_ERR_BUF_OVERRUN     0x0B  /* Buffer Overrun Error */
#define USB_ERR_SENT_EPT_PKT    0x0C  /* Sent Empty Packet (ISO Endpoints only) */
#define USB_ERR_BIT_STUFF       0x0D  /* Bitstuff Error */
#define USB_ERR_SYNC            0x0E  /* Error in Sync */
#define USB_ERR_TOGGLE_BIT      0x0F  /* Wrong Toggle Bi t  in Data PID */

/* USB Hardware Functions */
extern void USB_Init        (void);
extern void USB_Connect     (BOOL con);
extern void USB_Reset       (void);
extern void USB_Suspend     (void);
extern void USB_Resume      (void);
extern void USB_WakeUp      (void);
extern void USB_WakeUpCfg   (BOOL cfg);
extern void USB_SetAddress  (U32  adr);
extern void USB_Configure   (BOOL cfg);
extern void USB_ConfigEP    (USB_ENDPOINT_DESCRIPTOR *pEPD);
extern void USB_DirCtrlEP   (U32  dir);
extern void USB_EnableEP    (U32  EPNum);
extern void USB_DisableEP   (U32  EPNum);
extern void USB_ResetEP     (U32  EPNum);
extern void USB_SetStallEP  (U32  EPNum);
extern void USB_ClrStallEP  (U32  EPNum);
extern void USB_ClearEPBuf  (U32  EPNum);
extern U32  USB_ReadEP      (U32  EPNum, U8 *pData);
extern U32  USB_WriteEP     (U32  EPNum, U8 *pData, U32 cnt);
extern U32  USB_GetFrame    (void);


#endif  /* __USBHW_LPC13xx_H__ */
