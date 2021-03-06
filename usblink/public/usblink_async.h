#ifndef _USBLINK_ASYNC_H
#define _USBLINK_ASYNC_H

#include <stdint.h>
#include "usblink_common.h"

struct usbliink_async_priv;

struct usblink_async {
	struct usblink_async_priv *impl;
};

struct usblink_async_callback {
	void (*usblink_async_get_version_finish)(
			struct usblink_version *version);
	void (*usblink_async_get_params_finish)(
			struct usblink_params *params);
	void (*usblink_async_set_config_finish)(void);
	void (*usblink_async_start_fb_trans_finish)(void);
	void (*usblink_async_pause_fb_trans_finish)(void);
	void (*usblink_async_stop_fb_trans_finish)(void);
	void (*usblink_async_set_mfps_finish)(void);
	void (*usblink_async_frameupdate)(unsigned char *, long);
};

extern struct usblink_async *usblink_async_init(struct usblink_async_callback *cb);
extern int usblink_async_get_version(struct usblink_async *async, struct usblink_version *version);
extern int usblink_async_get_params(struct usblink_async *async);
extern int usblink_async_set_config(struct usblink_async *async, struct usblink_config *config);
extern int usblink_async_start_fb_trans(struct usblink_async *async, enum usblink_trans_mode mode);
extern int usblink_async_pause_fb_trans(struct usblink_async *async);
extern int usblink_async_stop_fb_trans(struct usblink_async *async);
extern int usblink_async_set_mfps(struct usblink_async *async, uint8_t mfps);
extern void usblink_async_destory(struct usblink_async *async);
/* NOTE: this is a dead loop, call it to handle the event. */
/* NOTE: if return value is zero, indicate the async object has been destoried. */
extern uint8_t usblink_async_wait_event(struct usblink_async *async);

#endif
