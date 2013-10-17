#include "usblink_async.h"

#include "usblink_common.h"

class usblink_async_impl: public class usblink_async {
	public:
		static usblink_async_impl *singleton_get_async_instance(
				class usblink_async::listener *listener);
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
	private:
		usblink_async_impl(
				class usblink_async::listener *listener);
		static class usblink_async_impl *instance;
};
