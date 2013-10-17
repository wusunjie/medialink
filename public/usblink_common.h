#ifndef _USBLINK_COMMON_H
#define _USBLINK_COMMON_H

struct usblink_params {
	unsigned int bmCapabilities;
	unsigned short wWidth;
	unsigned short wHeight;
	unsigned int bmPixelFormatSupported;
	unsigned int bmEncodingSupported;
};

struct usblink_config {
	unsigned int bmCapabilities;
	unsigned char bPixelFormat;
	unsigned char wPadding;
	unsigned int bmEncodingSupported;
};

struct usblink_version {
	unsigned char major;
	unsigned char minor;
};

enum usblink_trans_mode {
	USBLINK_TRANSMODE_STREAM = 0x0000,
	USBLINK_TRANSMODE_DEMAND = 0x0001
};

#endif
