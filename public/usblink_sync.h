#ifndef _USBLINK_SYNC_H
#define _USBLINK_SYNC_H

#include "usblink_common.h"

class usblink_sync {
	public:
		virtual ~usblink_sync(void) = 0;
		virtual struct usblink_version *usblink_sync_get_version(
				unsigned char major,
				unsigned char minor) = 0;
		virtual struct usblink_params *usblink_sync_get_params(void) = 0;
		virtual bool usblink_sync_set_config(struct usblink_config *config) = 0;
		virtual bool usblink_sync_start_fb_trans(enum usblink_trans_mode mode) = 0;
		virtual bool usblink_sync_pause_fb_trans(void) = 0;
		virtual bool usblink_sync_stop_fb_trans(void) = 0;
		virtual bool usblink_sync_set_mfps(unsigned char mfps) = 0;
};

#endif
