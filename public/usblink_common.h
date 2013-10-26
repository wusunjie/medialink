#ifndef _USBLINK_COMMON_H
#define _USBLINK_COMMON_H

#include <stdint.h>

struct usblink_params {
	uint32_t bmCapabilities;
	uint16_t wWidth;
	uint16_t wHeight;
	uint32_t bmPixelFormatSupported;
	uint32_t bmEncodingSupported;
};

struct usblink_config {
	uint32_t bmCapabilities;
	uint8_t bPixelFormat;
	uint8_t wPadding;
	uint32_t bmEncodingSupported;
};

struct usblink_version {
	uint8_t major;
	uint8_t minor;
};

enum usblink_trans_mode {
	USBLINK_TRANSMODE_STREAM,
	USBLINK_TRANSMODE_DEMAND
};

#endif
