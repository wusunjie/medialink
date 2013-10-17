#include "public/usblink_sync.h"

#include "public/usblink_common.h"

class usblink_sync_impl: public class usblink_sync {
	public:
		static class usblink_sync_impl *singleton_get_instance(void);
	public:
		virtual ~usblink_sync(void);
		virtual struct usblink_version *usblink_sync_get_version(
				unsigned char major,
				unsigned char minor);
		virtual struct usblink_params *usblink_sync_get_params(void);
		virtual bool usblink_sync_set_config(struct usblink_config *config);
		virtual bool usblink_sync_start_fb_trans(enum usblink_trans_mode mode);
		virtual bool usblink_sync_pause_fb_trans(void);
		virtual bool usblink_sync_stop_fb_trans(void);
		virtual bool usblink_sync_set_mfps(unsigned char mfps);
	private:
		usblink_sync_impl(void);
		static class usblink_sync_impl *instance;
};
