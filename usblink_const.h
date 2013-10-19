#ifndef _USBLINK_CONST_H
#define _USBLINK_CONST_H

#include "stdint.h"

extern const uint8_t USBLINK_CLASS = 0xff;
extern const uint8_t USBLINK_SUB_CLASS = 0xcc;
extern const uint8_t USBLINK_PROTOCOL = 0x01;
extern const uint8_t USBLINK_INTERFACE_INDEX = 0;
extern const uint8_t USBLINK_INTERFACE_INDEX = 0;
extern const uint8_t USBLINK_GET_REQUEST_TYPE = 0xc1;
extern const uint8_t USBLINK_SET_REQUEST_TYPE = 0x41;
/* TODO: timeout time to be determined. temp value 10 ms */
extern const uint8_t USBLINK_CTRL_TRANSFER_TIMEOUT = 10;
extern const uint8_t USBLINK_CTRL_SETUP_SIZE = 8;

/* get_versoin request constants. */
extern const uint8_t USBLINK_GET_VERSION_REQUEST_CODE = 0x40;
extern const uint8_t USBLINK_GET_VERSION_REQUEST_LENGTH = 2;

/* get_params request constants. */
extern const uint8_t USBLINK_GET_PARAMS_REQUEST_CODE = 0x42;
extern const uint8_t USBLINK_GET_PARAMS_REQUEST_VALUE = 0;
/* ??? get_params_request length, 12 in spec 1.2 */
extern const uint8_t USBLINK_GET_PARAMS_REQUEST_LENGTH = 16;

#endif