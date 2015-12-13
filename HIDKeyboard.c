/*----------------------------------------------------------------------------
 * Name:    HIDKeyboard.c
 * Purpose: USB HID Keyboard
 *----------------------------------------------------------------------------
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2015 ON5DI. All rights reserved.
 *----------------------------------------------------------------------------*/

#include "LPC13xx.h"                        /* LPC13xx definitions */

#include "type.h"

#include "usb.h"
#include "usbhw_LPC13xx.h"

#include "HIDKeyboard.h"
#include "hid.h"
#include <string.h>

U8 InReport[10];                              /* HID Input Report    */

U8 OutReport;                               /* HID Out Report      */
                                            /*   Bit0..7: LEDs     */

#define LED_0           0x00000001  /* P3.0 */
#define LED_1           0x00000002  /* P3.1 */
#define LED_2           0x00000004  /* P3.2 */
#define LED_3           0x00000008  /* P3.3 */

#define LED_4           0x00000010  /* P2.4 */
#define LED_5           0x00000020  /* P2.5 */
#define LED_6           0x00000040  /* P2.6 */
#define LED_7           0x00000080  /* P2.7 */

keybrdprt_t key;

/*------------------------------------------------------------------------------
  Get HID Input Report -> InReport
 *------------------------------------------------------------------------------*/
void GetInReport (void)
{
	uint8_t pbdata=0;

	if ((LPC_GPIO1->DATA & KBD_SW1) == 0)
	{
		pbdata|=1;
	}
	if ((LPC_GPIO1->DATA & KBD_SW2) == 0)
	{
		pbdata|=2;
	}
	else if ((LPC_GPIO1->DATA & KBD_SW3) == 0)
	{
		pbdata|=4;
	}
	else if ((LPC_GPIO1->DATA & KBD_SW4) == 0)
	{
		pbdata|=8;
	}
	else if ((LPC_GPIO1->DATA & KBD_SW5) == 0)
	{
		pbdata|=0x10;
	}
	if ((LPC_GPIO1->DATA & KBD_S3) == 0)
	{
		pbdata|=0x20;
	}
	if ((LPC_GPIO2->DATA & KBD_S4) == 0)
	{
		pbdata|=0x40;
	}
	
	switch(pbdata)
	{
		case 0x01:
			key.modifier=0;
			key.keycode[0]=0x14;			// this an a not q ! 
			break;
		case 0x02:
			key.modifier=0;
			key.keycode[0]=0x05;
			break;
		case 0x04:
			key.modifier=0;
			key.keycode[0]=0x06;
			break;
		case 0x08:
			key.modifier=0;
			key.keycode[0]=0x07;
			break;
		case 0x10:
			key.modifier=0;
			key.keycode[0]=0x08;
			break;
		case 0x20:
			key.modifier=0;
			key.keycode[0]=0x12;
			key.keycode[1]=0x11;
			key.keycode[2]=0x5d;	//0x22; geeft een haakje !
			key.keycode[3]=0x07;
			key.keycode[4]=0x0c;
			key.keycode[5]=0x28;
			break;
		case 0x40:
			key.modifier=0;
			key.keycode[0]=0x18;
			break;
		default:
			key.modifier=0;
			key.keycode[0]=0;
			key.keycode[1]=0;
			key.keycode[2]=0;
			key.keycode[3]=0;
			key.keycode[4]=0;
			key.keycode[5]=0;
			break;
	}

	memcpy(InReport,&key,sizeof key);
}



void SetLeds(unsigned char v, unsigned char val)
{
	switch (v)
	{
	   case 0:
			if(val)
				LPC_GPIO3->DATA &=~LED_0;
			else
				LPC_GPIO3->DATA |= LED_0;
			break;
	   case 1:
			if(val)
				LPC_GPIO3->DATA &=~LED_1;
			else
				LPC_GPIO3->DATA |= LED_1;
			break;
	   case 2:
			if(val)
				LPC_GPIO3->DATA &=~LED_2;
			else
				LPC_GPIO3->DATA |= LED_2; 	
			break;
	   case 3:
			if(val)
				LPC_GPIO3->DATA &=~LED_3;
			else
				LPC_GPIO3->DATA |= LED_3;
			break;
	   case 4:
			if(val)
				LPC_GPIO2->DATA &=~LED_4;
			else
				LPC_GPIO2->DATA |= LED_4;
			break;
	   case 5:
			if(val)
				LPC_GPIO2->DATA &=~LED_5;
			else
				LPC_GPIO2->DATA |= LED_5;
			break;
	   case 6:
			if(val)
				LPC_GPIO2->DATA &=~LED_6;
			else
				LPC_GPIO2->DATA |= LED_6;
			break;
	   case 7:
			if(val)
				LPC_GPIO2->DATA &=~LED_7;
			else
				LPC_GPIO2->DATA |= LED_7;
			break;
		default:
			break;
	}
}



/*------------------------------------------------------------------------------
  Set HID Output Report <- OutReport
 *------------------------------------------------------------------------------*/
void SetOutReport (void)
{
	int i;

	for (i = 0; i < 8 ; i++)
	{
		if (OutReport & (1<<i))
		{
			SetLeds(i,1);
		} 
		else
		{
			SetLeds(i,0);
		}
	}
}


void SetFeatReport (void)
{
	int i;

	//inversse //

	for (i = 0; i < 8 ; i++)
	{
		if (OutReport & (1<<i))
		{
			SetLeds(i,0);
		} 
		else
		{
			SetLeds(i,1);
		}
	}
}


/*------------------------------------------------------------------------------
  Main Program
 *------------------------------------------------------------------------------*/
int main (void)
{
	LPC_SYSCON->SYSAHBCLKCTRL |= 1 << 6;        /* Enable clock for GPIO */

	LPC_GPIO1->DIR  &= ~KBD_S3;               /* Push Button on P0.1 is input */
	LPC_GPIO2->DIR  &= ~KBD_S4;               /* Push Button on P0.1 is input */

	LPC_GPIO1->DIR  &= ~KBD_SW1;				/* P1.6 */
	LPC_GPIO1->DIR  &= ~KBD_SW2;				/* P1.7 */
	LPC_GPIO1->DIR  &= ~KBD_SW3;				/* P1.8 */
	LPC_GPIO1->DIR  &= ~KBD_SW4;				/* P1.9 */
	LPC_GPIO1->DIR  &= ~KBD_SW5;				/* P1.10 */

	LPC_GPIO2->DIR  |=  0xf0;               /* LEDs on PORT2 defined as Output */
	LPC_GPIO3->DIR  |=  0x0f; 

	LPC_GPIO2->DATA &= 0xf0;               /* LEDs on PORT2 off */
	LPC_GPIO3->DATA &= 0x0f;               /* LEDs on PORT2 off */

	key.modifier=0;
	key.reserved=0;
	key.keycode[0]=0;
	key.keycode[1]=0;
	key.keycode[2]=0;
	key.keycode[3]=0;
	key.keycode[4]=0;
	key.keycode[5]=0;
	
	USB_Init();                                 /* USB Initialization */
	USB_Connect(__TRUE);                        /* USB Connect */

	while (1)                                   /* Loop forever */
	{
	}
}
