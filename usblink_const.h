#ifndef _USBLINK_CONST_H
#define _USBLINK_CONST_H

#include "stdint.h"

extern const uint8_t USBLINK_CLASS;
extern const uint8_t USBLINK_SUB_CLASS;
extern const uint8_t USBLINK_PROTOCOL;
extern const uint8_t USBLINK_INTERFACE_INDEX;
extern const uint8_t USBLINK_INTERFACE_INDEX;
extern const uint8_t USBLINK_GET_REQUEST_TYPE;
extern const uint8_t USBLINK_SET_REQUEST_TYPE;
/* TODO: timeout time to be determined. temp value 10 ms */
extern const uint8_t USBLINK_CTRL_TRANSFER_TIMEOUT;
extern const uint8_t USBLINK_CTRL_SETUP_SIZE;

/* get_versoin request constants. */
extern const uint8_t USBLINK_GET_VERSION_REQUEST_CODE;
extern const uint8_t USBLINK_GET_VERSION_REQUEST_LENGTH;

/* get_params request constants. */
extern const uint8_t USBLINK_GET_PARAMS_REQUEST_CODE;
extern const uint8_t USBLINK_GET_PARAMS_REQUEST_VALUE;
/* ??? get_params_request length, 12 in spec 1.2 */
extern const uint8_t USBLINK_GET_PARAMS_REQUEST_LENGTH;

/* set_config request constants. */
/* ??? set_params_request length, 14 in spec 1.2 */
extern const uint8_t USBLINK_SET_CONFIG_REQUEST_LENGTH;
extern const uint8_t USBLINK_SET_CONFIG_REQUEST_CODE;
extern const uint8_t USBLINK_SET_CONFIG_REQUEST_VALUE;

/* start_framebuffer_transfer request constants. */
extern const uint8_t USBLINK_START_FB_TRANS_REQUEST_CODE;
extern const uint8_t USBLINK_START_FB_TRANS_REQUEST_LENGTH;

/* pause_framebuffer_transfer request constants. */
extern const uint8_t USBLINK_PAUSE_FB_TRANS_REQUEST_LENGTH;
extern const uint8_t USBLINK_PAUSE_FB_TRANS_REQUEST_CODE;
extern const uint8_t USBLINK_PAUSE_FB_TRANS_REQUEST_VALUE;

/* stop_framebuffer_transfer request constants. */
extern const uint8_t USBLINK_STOP_FB_TRANS_REQUEST_LENGTH;
extern const uint8_t USBLINK_STOP_FB_TRANS_REQUEST_CODE;
extern const uint8_t USBLINK_STOP_FB_TRANS_REQUEST_VALUE;

/* stop_framebuffer_transfer request constants. */
extern const uint8_t USBLINK_SET_MFPS_REQUEST_LENGTH;
extern const uint8_t USBLINK_SET_MFPS_REQUEST_CODE;

#endif
