#ifndef _USBLINK_SYNC_H
#define _USBLINK_SYNC_H

#include "usblink_common.h"

struct usblink_sync {
	struct usblink_sync_priv *priv;
};

struct usblink_sync *usblink_sync_init(void);
struct usblink_version *usblink_sync_get_version(struct usblink_sync *sync, struct usblink_version *version);
struct usblink_params *usblink_sync_get_params(struct usblink_sync *sync);
unsigned char usblink_sync_set_config(struct usblink_sync *sync, struct usblink_config *config);
unsigned char usblink_sync_start_fb_trans(struct usblink_sync *sync, enum usblink_trans_mode mode);
unsigned char usblink_sync_pause_fb_trans(struct usblink_sync *sync);
unsigned char usblink_sync_stop_fb_trans(struct usblink_sync *sync);
unsigned char usblink_sync_set_mfps(struct usblink_sync *sync, unsigned char mfps);
void usblink_sync_destory(struct usblink_sync *sync);

#endif
