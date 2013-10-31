#include "usblink_const.h"
#include <stdint.h>

const uint8_t USBLINK_CLASS = 0xff;
const uint8_t USBLINK_SUB_CLASS = 0xcc;
const uint8_t USBLINK_PROTOCOL = 0x01;
const uint8_t USBLINK_INTERFACE_INDEX = 0;
const uint8_t USBLINK_INTERFACE_INDEX = 0;
const uint8_t USBLINK_GET_REQUEST_TYPE = 0xc1;
const uint8_t USBLINK_SET_REQUEST_TYPE = 0x41;
/* TODO: timeout time to be determined. temp value 10 ms */
const uint8_t USBLINK_CTRL_TRANSFER_TIMEOUT = 10;
const uint8_t USBLINK_BULK_TRANSFER_TIMEOUT = 10;
const uint8_t USBLINK_CTRL_SETUP_SIZE = 8;

/* get_versoin request constants. */
const uint8_t USBLINK_GET_VERSION_REQUEST_CODE = 0x40;
const uint8_t USBLINK_GET_VERSION_REQUEST_LENGTH = 2;

/* get_params request constants. */
const uint8_t USBLINK_GET_PARAMS_REQUEST_CODE = 0x42;
const uint8_t USBLINK_GET_PARAMS_REQUEST_VALUE = 0;
/* ??? get_params_request length, 12 in spec 1.2 */
const uint8_t USBLINK_GET_PARAMS_REQUEST_LENGTH = 16;

/* set_config request constants. */
/* ??? set_params_request length, 14 in spec 1.2 */
const uint8_t USBLINK_SET_CONFIG_REQUEST_LENGTH = 10;
const uint8_t USBLINK_SET_CONFIG_REQUEST_CODE = USBLINK_GET_PARAMS_REQUEST_CODE;
const uint8_t USBLINK_SET_CONFIG_REQUEST_VALUE = 0;

/* start_framebuffer_transfer request constants. */
const uint8_t USBLINK_START_FB_TRANS_REQUEST_CODE = 0x43;
const uint8_t USBLINK_START_FB_TRANS_REQUEST_LENGTH = 8;

/* pause_framebuffer_transfer request constants. */
const uint8_t USBLINK_PAUSE_FB_TRANS_REQUEST_LENGTH = 0;
const uint8_t USBLINK_PAUSE_FB_TRANS_REQUEST_CODE = 0x44;
const uint8_t USBLINK_PAUSE_FB_TRANS_REQUEST_VALUE = 0;

/* stop_framebuffer_transfer request constants. */
const uint8_t USBLINK_STOP_FB_TRANS_REQUEST_LENGTH = 0;
const uint8_t USBLINK_STOP_FB_TRANS_REQUEST_CODE = 0x45;
const uint8_t USBLINK_STOP_FB_TRANS_REQUEST_VALUE = 0;

/* stop_framebuffer_transfer request constants. */
const uint8_t USBLINK_SET_MFPS_REQUEST_LENGTH = 0;
const uint8_t USBLINK_SET_MFPS_REQUEST_CODE = 0x46;

