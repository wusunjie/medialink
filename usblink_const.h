#ifndef _USBLINK_CONST_H
#define _USBLINK_CONST_H

#include "stdint.h"

extern const uint8_t USBLINK_CLASS = 0xff;
extern const uint8_t USBLINK_SUB_CLASS = 0xcc;
extern const uint8_t USBLINK_PROTOCOL = 0x01;
extern const uint8_t USBLINK_INTERFACE_INDEX = 0;
extern const uint8_t USBLINK_GET_VERSION_CTRL_SIZE = 12;
extern const uint8_t USBLINK_GET_VERSION_REQUEST_TYPE = 0xc1;
extern const uint8_t USBLINK_GET_VERSION_REQUEST_CODE = 0x40;
extern const uint8_t USBLINK_GET_VERSION_REQUEST_LENGTH = 2;
extern const uint8_t USBLINK_CTRL_TRANSFER_TIMEOUT = 10;

#endif
