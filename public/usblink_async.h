#ifndef _USBLINK_ASYNC_H
#define _USBLINK_ASYNC_H

#include "usblink_common.h"

class usblink_async {
	public:
		virtual ~usblink_async(void) = 0;
		virtual void usblink_async_get_version(
				unsigned char majar,
				unsigned char minor) = 0;
		virtual void usblink_async_get_params(void) = 0;
		virtual void usblink_async_set_config(struct usblink_config *config) = 0;
		virtual void usblink_async_start_fb_trans(enum usblink_trans_mode mode) = 0;
		virtual void usblink_async_pause_fb_trans(void) = 0;
		virtual void usblink_async_stop_fb_trans(void) = 0;
		virtual void usblink_async_set_mfps(unsigned char mfps) = 0;
		class listener {
			public:
				virtual void usblink_async_get_version_finish(
						struct usblink_version *version) = 0;
				virtual void usblink_async_get_params_finish(
						struct usblink_params *params) = 0;
				virtual void usblink_async_set_config_finish(bool result) = 0;
				virtual void usblink_async_start_fb_trans_finish(bool result) = 0;
				virtual void usblink_async_pause_fb_trans_finish(bool result) = 0;
				virtual void usblink_async_stop_fb_trans(bool result) = 0;
				virtual void usblink_async_set_mfps_finish(bool result) = 0;
		};
};

extern class usblink_async *usblink_get_async(class usblink_async::listener *listener);

#endif
