/*----------------------------------------------------------------------------
 * U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 * Name:    usbdesc.c
 * Purpose: USB Descriptors
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
 *				V1.21 Changed to HID Keyboard
 *          V1.20 Changed string descriptor handling
 *                Changed HID Report Descriptor
 *          V1.00 Initial Version
 *----------------------------------------------------------------------------*/

#include "type.h"
#include "usb.h"
#include "usbcfg_LPC13xx.h"
#include "usbdesc.h"
#include "hid.h"

/*------------------------------------------------------------------------------
  HID Report Descriptor
 *------------------------------------------------------------------------------*/

/*   Bit    Input       Output
      0     KBD          LED0
      1     ---          LED1
      2     ---          LED2
      3     ---          LED3
      4     ---          LED4
      5     ---          LED5
      6     ---          LED6
      7     ---          LED7
*/

const U8 HID_ReportDescriptor[] = {
    HID_UsagePage(0x01),           // USAGE_PAGE (Generic Desktop)
    HID_Usage(0x06),               // USAGE (Keyboard)
    HID_Collection(0x01),          // COLLECTION (Application)
    HID_UsagePage(0x07),           //   USAGE_PAGE (Keyboard)
    HID_UsageMin(0xe0),            //   USAGE_MINIMUM (Keyboard LeftControl)
    HID_UsageMax(0xe7),            //   USAGE_MAXIMUM (Keyboard Right GUI)
    HID_LogicalMin(0x00),          //   LOGICAL_MINIMUM (0)
    HID_LogicalMax(0x01),          //   LOGICAL_MAXIMUM (1)
    HID_ReportSize(0x01),          //   REPORT_SIZE (1)
    HID_ReportCount(0x08),         //   REPORT_COUNT (8)
    HID_Input(0x02),               //   INPUT (Data,Var,Abs)
    HID_ReportCount(0x01),         //   REPORT_COUNT (1)
    HID_ReportSize(0x08),          //   REPORT_SIZE (8)
    HID_Input(0x03),               //   INPUT (Cnst,Var,Abs)
    HID_ReportCount(0x05),         //   REPORT_COUNT (5)
    HID_ReportSize(0x01),          //   REPORT_SIZE (1)
    HID_UsagePage(0x08),           //   USAGE_PAGE (LEDs)
    HID_UsageMin(0x01),            //   USAGE_MINIMUM (Num Lock)
    HID_UsageMax(0x05),            //   USAGE_MAXIMUM (Kana)
    HID_Output(0x02),              //   OUTPUT (Data,Var,Abs)
    HID_ReportCount(0x01),         //   REPORT_COUNT (1)
    HID_ReportSize(0x03),          //   REPORT_SIZE (3)
    HID_Output(0x03),              //   OUTPUT (Cnst,Var,Abs)
    HID_ReportCount(0x06),         //   REPORT_COUNT (6)
    HID_ReportSize(0x08),          //   REPORT_SIZE (8)
    HID_LogicalMin(0x00),          //   LOGICAL_MINIMUM (0)
    HID_LogicalMax(0xff),          //   LOGICAL_MAXIMUM (255) 
    HID_UsagePage(0x07),           //   USAGE_PAGE (Keyboard)
    HID_UsageMin(0x00),            //   USAGE_MINIMUM (Reserved (no event indicated))
    HID_UsageMax(0xff),            //   USAGE_MAXIMUM (Keyboard Application)
    HID_Input(0x00),               //   INPUT (Data,Ary,Abs)
    HID_EndCollection              // END_COLLECTION
};


const U16 HID_ReportDescSize = sizeof(HID_ReportDescriptor);


/* USB Standard Device Descriptor */
const U8 USB_DeviceDescriptor[] = {
  USB_DEVICE_DESC_SIZE,              /* bLength */
  USB_DEVICE_DESCRIPTOR_TYPE,        /* bDescriptorType */
  WBVAL(0x0200), /* 2.00 */          /* bcdUSB */
  0x00,                              /* bDeviceClass */
  0x00,                              /* bDeviceSubClass */
  0x00,                              /* bDeviceProtocol */
  USB_MAX_PACKET0,                   /* bMaxPacketSize0 */
  WBVAL(0xC251),                     /* idVendor */
  WBVAL(0x2502),                     /* idProduct */
  WBVAL(0x0100), /* 1.00 */          /* bcdDevice */
  0x01,                              /* iManufacturer */
  0x02,                              /* iProduct */
  0x03,                              /* iSerialNumber */
  0x01                               /* bNumConfigurations: one possible configuration*/
};

/* USB Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor) */
const U8 USB_ConfigDescriptor[] = {
/* Configuration 1 */
  USB_CONFIGUARTION_DESC_SIZE,       /* bLength */
  USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType */
  WBVAL(                             /* wTotalLength */
    USB_CONFIGUARTION_DESC_SIZE +
    USB_INTERFACE_DESC_SIZE     +
    HID_DESC_SIZE               +
    USB_ENDPOINT_DESC_SIZE
  ),
  0x01,                              /* bNumInterfaces */
  0x01,                              /* bConfigurationValue: 0x01 is used to select this configuration */
  0x00,                              /* iConfiguration: no string to describe this configuration */
  USB_CONFIG_BUS_POWERED /*|*/       /* bmAttributes */
/*USB_CONFIG_REMOTE_WAKEUP*/,
  USB_CONFIG_POWER_MA(100),          /* bMaxPower, device power consumption is 100 mA */

/* Interface 0, Alternate Setting 0, HID Class */
  USB_INTERFACE_DESC_SIZE,           /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,     /* bDescriptorType */
  0x00,                              /* bInterfaceNumber */
  0x00,                              /* bAlternateSetting */
  0x01,                              /* bNumEndpoints */
  USB_DEVICE_CLASS_HUMAN_INTERFACE,  /* bInterfaceClass */
  HID_SUBCLASS_NONE,                 /* bInterfaceSubClass */
  HID_PROTOCOL_KEYBOARD,             /* bInterfaceProtocol */
  0x04,                              /* iInterface */
/* HID Class Descriptor */
/* HID_DESC_OFFSET = 0x0012 */
  HID_DESC_SIZE,                     /* bLength */
  HID_HID_DESCRIPTOR_TYPE,           /* bDescriptorType */
  WBVAL(0x0100), /* 1.00 */          /* bcdHID */
  0x00,                              /* bCountryCode */
  0x01,                              /* bNumDescriptors */
  HID_REPORT_DESCRIPTOR_TYPE,        /* bDescriptorType */
  WBVAL(HID_REPORT_DESC_SIZE),       /* wDescriptorLength */
/* Endpoint, HID Interrupt In */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  USB_ENDPOINT_IN(1),                /* bEndpointAddress */
  USB_ENDPOINT_TYPE_INTERRUPT,       /* bmAttributes */
  WBVAL(0x0040),                     /* wMaxPacketSize */
  0x40,          /* 64ms */          /* bInterval */
/* Terminator */
  0                                  /* bLength */
};




/* USB String Descriptor (optional) */
const U8 USB_StringDescriptor[] = {
/* Index 0x00: LANGID Codes */
  0x04,                              /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  WBVAL(0x0409), /* US English */    /* wLANGID */

/* Index 0x01: Manufacturer */
  (13*2 + 2),                        /* bLength (13 Char + Type + lenght) */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'O',0,
  'N',0,
  '5',0,
  'D',0,
  'I',0,
  ' ',0,
  'H',0,
  'A',0,
  'M',0,
  'W',0,
  'A',0,
  'R',0,
  'E',0,

/* Index 0x02: Product */
  (11*2 + 2),                        /* bLength (14 Char + Type + lenght) */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'N',0,
  '1',0,
  'M',0,
  'M',0,
  ' ',0,
  'F',0,
  'O',0,
  'O',0,
  'T',0,
  'S',0,
  'W',0,
  
/* Index 0x03: Serial Number */
  (12*2 + 2),                        /* bLength (12 Char + Type + lenght) */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  '0',0,                             /* allowed characters are       */
  '0',0,                             /*   0x0030 - 0x0039 ('0'..'9') */
  '0',0,                             /*   0x0041 - 0x0046 ('A'..'F') */
  '1',0,                             /*   length >= 26               */
  'A',0,
  '0',0,
  '0',0,
  '0',0,
  '0',0,
  '0',0,
  '0',0,
  '0',0,

/* Index 0x04: Interface 0, Alternate Setting 0 */
  ( 3*2 + 2),                        /* bLength (3 Char + Type + lenght) */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'H',0,
  'I',0,
  'D',0,
};
