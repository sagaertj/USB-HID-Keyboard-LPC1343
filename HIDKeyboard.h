/*----------------------------------------------------------------------------
 * Name:    demo.h
 * Purpose: USB HID Demo Definitions
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2010 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

/* Push Button Definitions */

#define KBD_S3          0x00000010  /* P1.4 */
#define KBD_S4          0x00000200  /* P2.9 */



/* HID Demo Variables */
extern U8 InReport[];
extern U8 OutReport;

/* HID Demo Functions */
extern void GetInReport  (void);
extern void SetFeatReport (void);
extern void SetOutReport (void);
