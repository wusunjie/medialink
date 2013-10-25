#include "public/usblink_async.h"

#include "libusb.h"
#include "usblink_const.h"

#include "stdlib.h"
#include "stdint.h"
#include "string.h"
#include "pthread.h"
#include "assert.h"

enum usblink_async_type {
	USBLINK_ASYNC_TYPE_NONE,
	USBLINK_ASYNC_TYPE_GET_VERSION,
	USBLINK_ASYNC_TYPE_GET_PARAMS,
	USBLINK_ASYNC_TYPE_SET_CONFIG,
	USBLINK_ASYNC_TYPE_START_FB_TRANS,
	USBLINK_ASYNC_TYPE_PAUSE_FB_TRANS,
	USBLINK_ASYNC_TYPE_STOP_FB_TRANS,
	USBLINK_ASYNC_TYPE_SET_MFPS
};

struct usblink_async_priv {
	struct libusb_context *context;
	struct libusb_device_handle *handle;
	/* contorl transfer for ctrl */
	struct libusb_transfer *ctrl;
	/* bulk transfer for framebuffer */
	struct libusb_transfer *bulk;
	struct usblink_async_callback *cb;
	enum usblink_async_type event;
	uint8_t destory;
	pthread_t poll_thread;
	pthread_cond_t exit_cond;
	pthread_mutex_t exit_cond_lock;
};

static libusb_device_handle *usblink_async_find_device(struct libusb_context *context);
static void usblink_async_ctrl_transfer_cb(struct libusb_transfer *transfer);
static void usblink_async_ctrl_transfer_complete(struct usblink_async_priv *impl, struct libusb_transfer *transfer);
static void usblink_async_destory_later(struct usblink_async *async, unsigned char ctrl_bulk);

static void usblink_async_ctrl_transfer_cb(struct libusb_transfer *transfer)
{
	assert(transfer);
	struct usblink_version version;
	struct usblink_async *async = (struct usblink_async *)(transfer->user_data);
	struct usblink_async_priv *impl = async->impl;
	switch (transfer->status) {
		case LIBUSB_TRANSFER_COMPLETED:
			{
				usblink_async_ctrl_transfer_complete(impl, transfer);
			}
			break;
		default:
			break;
	}
	/* when all transfer complete, accept the destory request. */
	usblink_async_destory_later(async, 0);
}

static void usblink_async_ctrl_transfer_complete(struct usblink_async_priv *impl, struct libusb_transfer *transfer)
{
	assert(impl && transfer);
	switch (impl->event) {
		case USBLINK_ASYNC_TYPE_GET_VERSION:
			{
				struct usblink_version version;
				assert(transfer->buffer && impl->cb && impl->cb->usblink_async_get_version_finish &&
						(USBLINK_CTRL_SETUP_SIZE + USBLINK_GET_VERSION_REQUEST_LENGTH == transfer->actual_length));
				version.major = ((uint16_t)transfer->buffer[0] << 8) | transfer->buffer[1];
				version.minor = ((uint16_t)transfer->buffer[2] << 8) | transfer->buffer[3];
				impl->cb->usblink_async_get_version_finish(&version);
			}
			break;
		case USBLINK_ASYNC_TYPE_GET_PARAMS:
			{
				struct usblink_params params;
				assert(transfer->buffer && impl->cb && impl->cb->usblink_async_get_params_finish &&
						(USBLINK_CTRL_SETUP_SIZE + USBLINK_GET_PARAMS_REQUEST_LENGTH == transfer->actual_length));
				params.bmCapabilities = (uint32_t)transfer->buffer[0];
				params.wWidth = (uint16_t)transfer->buffer[4];
				params.wHeight = (uint16_t)transfer->buffer[6];
				params.bmPixelFormatSupported = (uint32_t)transfer->buffer[8];
				params.bmEncodingSupported = (uint32_t)transfer->buffer[12];
				impl->cb->usblink_async_get_params_finish(&params);
			}
			break;
		case USBLINK_ASYNC_TYPE_SET_CONFIG:
			{
				assert(impl && impl->cb && impl->cb->usblink_async_set_config_finish);
				impl->cb->usblink_async_set_config_finish();
			}
			break;
		case USBLINK_ASYNC_TYPE_START_FB_TRANS:
			{
				assert(impl && impl->cb && impl->cb->usblink_async_start_fb_trans_finish);
				impl->cb->usblink_async_start_fb_trans_finish();
			}
			break;
		case USBLINK_ASYNC_TYPE_PAUSE_FB_TRANS:
			{
				assert(impl && impl->cb && impl->cb->usblink_async_pause_fb_trans_finish);
				impl->cb->usblink_async_pause_fb_trans_finish();
			}
			break;
		case USBLINK_ASYNC_TYPE_STOP_FB_TRANS:
			{
				assert(impl && impl->cb && impl->cb->usblink_async_stop_fb_trans_finish);
				impl->cb->usblink_async_stop_fb_trans_finish();
			}
			break;
		case USBLINK_ASYNC_TYPE_SET_MFPS:
			{
				assert(impl && impl->cb && impl->cb->usblink_async_set_mfps_finish);
				impl->cb->usblink_async_set_mfps_finish();
			}
			break;
		default:
			break;
	}
	impl->event =  USBLINK_ASYNC_TYPE_NONE;
}

static libusb_device_handle *usblink_async_find_device(struct libusb_context *context)
{
	struct libusb_device **list =  0;
	struct libusb_device *device = 0;
	struct libusb_device_handle *handle = 0;
	struct libusb_device_descriptor descriptor;
	ssize_t cnt = 0, i;
	cnt = libusb_get_device_list(context, &list);
	assert(list);
	for (i = 0; i < cnt; i++) {
		libusb_get_device_descriptor(list[i], &descriptor);
		if ((USBLINK_CLASS == descriptor.bDeviceClass)
				&& (USBLINK_SUB_CLASS == descriptor.bDeviceSubClass)
				&& (USBLINK_PROTOCOL == descriptor.bDeviceProtocol)) {
			device = list[i];
			break;
		}
	}
	libusb_free_device_list(list, 1);
	if (0 != device) {
		libusb_open(device, &handle);
	}
	return handle;
}

static void usblink_async_destory_later(struct usblink_async *async, unsigned char ctrl_bulk)
{
	assert(async && async->impl);
	if (1 == async->impl->destory) {
		switch (ctrl_bulk) {
			case 0:
				{
					libusb_free_transfer(async->impl->ctrl);
					async->impl->ctrl = 0;
				}
				break;
			case 1:
				{
					libusb_free_transfer(async->impl->bulk);
					async->impl->bulk = 0;
				}
				break;
			case 2:
				{
					if (LIBUSB_TRANSFER_COMPLETED == async->impl->ctrl->status) {
						libusb_free_transfer(async->impl->ctrl);
						async->impl->ctrl = 0;
					}
					if (LIBUSB_TRANSFER_COMPLETED == async->impl->bulk->status) {
						libusb_free_transfer(async->impl->bulk);
						async->impl->bulk = 0;
					}
				}
				break;
			default:
				break;
		}
	}
	if ((0 == async->impl->ctrl)
			&& (0 == async->impl->bulk)) {
		async->impl->destory = 2;
		pthread_mutex_lock(&(async->impl->exit_cond_lock));
		pthread_cond_signal(&(impl->exit_cond));
		pthread_mutex_unlock(&(async->impl->exit_cond_lock));
	}
}

static void *usblink_async_event_handler(void *args)
{
	struct usblink_async_priv *impl = (struct usblink_async_priv *)args;
	int r = 0;
	assert(impl);
	while (0 == impl->destory) {
		struct timeval tv = {1, 0};
		r = libusb_handle_events_timeout_completed(0, &tv, 0);
		if (r < 0) {
			impl->destory = 1;
		}
		pthread_mutex_lock(&(async->impl->exit_cond_lock));
		pthread_cond_signal(&(impl->exit_cond));
		pthread_mutex_unlock(&(async->impl->exit_cond_lock));
	}
	return 0;
}

struct usblink_async *usblink_async_init(struct usblink_async_callback *cb)
{
	struct usblink_async *async = (struct usblink_async *)malloc(sizeof(*async));
	struct usblink_async_priv *impl = (struct usblink_async_priv *)malloc(sizeof(*impl));
	assert(async && impl);
	libusb_init(&(impl->context));
	impl->handle = usblink_async_find_device(impl->context);
	libusb_claim_interface(impl->handle, USBLINK_INTERFACE_INDEX);
	/* param set to [0] for non-isochronous */
	impl->bulk = libusb_alloc_transfer(0);
	impl->ctrl = libusb_alloc_transfer(0);
	impl->ctrl->flags = LIBUSB_TRANSFER_FREE_BUFFER;
	impl->bulk->flags = LIBUSB_TRANSFER_FREE_BUFFER;
	impl->cb = cb;
	impl->event = USBLINK_ASYNC_TYPE_NONE;
	impl->destory = 0;
	pthread_cond_init(&(impl->exit_cond), 0);
	pthread_mutex_init(&(impl->exit_cond_lock), 0);
	pthread_create(&(impl->poll_thread), 0, usblink_async_event_handler, impl);
	async->impl = impl;
	return async;
}

void usblink_async_destory(struct usblink_async *async)
{
	assert(async && async->impl);
	async->impl->destory = 1;
	usblink_async_destory_later(async, 2);
}

int usblink_async_get_version(struct usblink_async *async, struct usblink_version *version)
{
	unsigned char *ctrl_buffer = 0;
	assert(async && async->impl && version);
	if (USBLINK_ASYNC_TYPE_NONE == async->impl->event) {
		ctrl_buffer = (unsigned char *)malloc(USBLINK_GET_VERSION_REQUEST_LENGTH + USBLINK_CTRL_SETUP_SIZE);
		assert(ctrl_buffer);
		libusb_fill_control_setup(ctrl_buffer,
				USBLINK_GET_REQUEST_TYPE,
				USBLINK_GET_VERSION_REQUEST_CODE,
				(uint16_t)(version->major) << 8 | version->minor,
				USBLINK_INTERFACE_INDEX,
				USBLINK_GET_VERSION_REQUEST_LENGTH);
		async->impl->event = USBLINK_ASYNC_TYPE_GET_VERSION;
		libusb_fill_control_transfer(async->impl->ctrl, async->impl->handle,
				ctrl_buffer, usblink_async_ctrl_transfer_cb,
				async, USBLINK_CTRL_TRANSFER_TIMEOUT);
		return libusb_submit_transfer(async->impl->ctrl);
	} else {
		return 0;
	}
}

int usblink_async_get_params(struct usblink_async *async)
{
	unsigned char *ctrl_buffer = 0;
	assert(async && async->impl);
	ctrl_buffer = (unsigned char *)malloc(USBLINK_GET_PARAMS_REQUEST_LENGTH + USBLINK_CTRL_SETUP_SIZE);
	assert(ctrl_buffer);
	if (USBLINK_ASYNC_TYPE_NONE == async->impl->event) {
		libusb_fill_control_setup(ctrl_buffer,
				USBLINK_GET_REQUEST_TYPE,
				USBLINK_GET_PARAMS_REQUEST_CODE,
				USBLINK_GET_PARAMS_REQUEST_VALUE,
				USBLINK_INTERFACE_INDEX,
				USBLINK_GET_PARAMS_REQUEST_LENGTH);
		async->impl->event = USBLINK_ASYNC_TYPE_GET_PARAMS;
		libusb_fill_control_transfer(async->impl->ctrl, async->impl->handle,
				ctrl_buffer, usblink_async_ctrl_transfer_cb,
				async, USBLINK_CTRL_TRANSFER_TIMEOUT);
		assert(async->impl);
		return libusb_submit_transfer(async->impl->ctrl);
	} else {
		return 0;
	}
}

int usblink_async_set_config(struct usblink_async *async, struct usblink_config *config)
{
	unsigned char *ctrl_buffer = 0;
	assert(async && async->impl);
	if (USBLINK_ASYNC_TYPE_NONE == async->impl->event) {
		ctrl_buffer = (unsigned char *)malloc(USBLINK_SET_CONFIG_REQUEST_LENGTH + USBLINK_CTRL_SETUP_SIZE);
		assert(ctrl_buffer);
		libusb_fill_control_setup(ctrl_buffer,
				USBLINK_SET_REQUEST_TYPE,
				USBLINK_SET_CONFIG_REQUEST_CODE,
				USBLINK_SET_CONFIG_REQUEST_VALUE,
				USBLINK_INTERFACE_INDEX,
				USBLINK_SET_CONFIG_REQUEST_LENGTH);
		async->impl->event = USBLINK_ASYNC_TYPE_SET_CONFIG;
		memcpy(ctrl_buffer + USBLINK_CTRL_SETUP_SIZE, config, USBLINK_SET_CONFIG_REQUEST_LENGTH);
		libusb_fill_control_transfer(async->impl->ctrl, async->impl->handle,
				ctrl_buffer, usblink_async_ctrl_transfer_cb,
				async, USBLINK_CTRL_TRANSFER_TIMEOUT);
		return libusb_submit_transfer(async->impl->ctrl);
	} else {
		return 0;
	}
}

int usblink_async_start_fb_trans(struct usblink_async *async, enum usblink_trans_mode mode)
{
	unsigned char *ctrl_buffer = 0;
	uint16_t trans_mode = 0;
	assert(async && async->impl);
	if (USBLINK_ASYNC_TYPE_NONE == async->impl->event) {
	ctrl_buffer = (unsigned char *)malloc(USBLINK_START_FB_TRANS_REQUEST_LENGTH + USBLINK_CTRL_SETUP_SIZE);
	assert(ctrl_buffer);
	if (USBLINK_TRANSMODE_STREAM == mode) {
		trans_mode = 0x0000;
	} else {
		trans_mode = 0x0001;
	}
	libusb_fill_control_setup(ctrl_buffer,
			USBLINK_SET_REQUEST_TYPE,
			USBLINK_START_FB_TRANS_REQUEST_CODE,
			trans_mode,
			USBLINK_INTERFACE_INDEX,
			USBLINK_START_FB_TRANS_REQUEST_LENGTH);
	async->impl->event = USBLINK_ASYNC_TYPE_START_FB_TRANS;
	libusb_fill_control_transfer(async->impl->ctrl, async->impl->handle,
			ctrl_buffer, usblink_async_ctrl_transfer_cb,
			async, USBLINK_CTRL_TRANSFER_TIMEOUT);
	return libusb_submit_transfer(async->impl->ctrl);
	} else {
		return 0;
	}
}

int usblink_async_pause_fb_trans(struct usblink_async *async)
{
	unsigned char *ctrl_buffer = 0;
	assert(async && async->impl);
	if (USBLINK_ASYNC_TYPE_NONE == async->impl->event) {
	ctrl_buffer = (unsigned char *)malloc(USBLINK_PAUSE_FB_TRANS_REQUEST_LENGTH + USBLINK_CTRL_SETUP_SIZE);
	assert(ctrl_buffer);
	libusb_fill_control_setup(ctrl_buffer,
			USBLINK_SET_REQUEST_TYPE,
			USBLINK_PAUSE_FB_TRANS_REQUEST_CODE,
			USBLINK_PAUSE_FB_TRANS_REQUEST_VALUE,
			USBLINK_INTERFACE_INDEX,
			USBLINK_PAUSE_FB_TRANS_REQUEST_LENGTH);
	async->impl->event = USBLINK_ASYNC_TYPE_PAUSE_FB_TRANS;
	libusb_fill_control_transfer(async->impl->ctrl, async->impl->handle,
			ctrl_buffer, usblink_async_ctrl_transfer_cb,
			async, USBLINK_CTRL_TRANSFER_TIMEOUT);
	return libusb_submit_transfer(async->impl->ctrl);
	} else {
		return 0;
	}
}

int usblink_async_stop_fb_trans(struct usblink_async *async)
{
	unsigned char *ctrl_buffer = 0;
	assert(async && async->impl);
	if (USBLINK_ASYNC_TYPE_NONE == async->impl->event) {
		ctrl_buffer = (unsigned char *)malloc(USBLINK_STOP_FB_TRANS_REQUEST_LENGTH + USBLINK_CTRL_SETUP_SIZE);
		assert(ctrl_buffer);
		libusb_fill_control_setup(ctrl_buffer,
				USBLINK_SET_REQUEST_TYPE,
				USBLINK_STOP_FB_TRANS_REQUEST_CODE,
				USBLINK_STOP_FB_TRANS_REQUEST_VALUE,
				USBLINK_INTERFACE_INDEX,
				USBLINK_STOP_FB_TRANS_REQUEST_LENGTH);
		async->impl->event = USBLINK_ASYNC_TYPE_STOP_FB_TRANS;
		libusb_fill_control_transfer(async->impl->ctrl, async->impl->handle,
				ctrl_buffer, usblink_async_ctrl_transfer_cb,
				async, USBLINK_CTRL_TRANSFER_TIMEOUT);
		return libusb_submit_transfer(async->impl->ctrl);
	} else {
		return 0;
	}
}

int usblink_async_set_mfps(struct usblink_async *async, uint8_t mfps)
{
	unsigned char *ctrl_buffer = 0;
	assert(async && async->impl);
	if (USBLINK_ASYNC_TYPE_NONE == async->impl->event) {
		ctrl_buffer = (unsigned char *)malloc(USBLINK_SET_MFPS_REQUEST_LENGTH + USBLINK_CTRL_SETUP_SIZE);
		assert(ctrl_buffer);
		libusb_fill_control_setup(ctrl_buffer,
				USBLINK_SET_REQUEST_TYPE,
				USBLINK_SET_MFPS_REQUEST_CODE,
				mfps,
				USBLINK_INTERFACE_INDEX,
				USBLINK_SET_MFPS_REQUEST_LENGTH);
		async->impl->event = USBLINK_ASYNC_TYPE_SET_MFPS;
		libusb_fill_control_transfer(async->impl->ctrl, async->impl->handle,
				ctrl_buffer, usblink_async_ctrl_transfer_cb,
				async, USBLINK_CTRL_TRANSFER_TIMEOUT);
		return libusb_submit_transfer(async->impl->ctrl);
	} else {
		return 0;
	}
}

uint8_t usblink_async_wait_event(struct usblink_async *async)
{
	assert(async && async->impl);
	if (2 != async->impl->destory) {
		pthread_mutex_lock(&(async->impl->exit_cond_lock));
		pthread_cond_wait(&(async->impl->exit_cond), &(async->impl->exit_cond_lock));
		pthread_mutex_unlock(&(async->impl->exit_cond_lock));
		return 1;
	} else {
		pthread_join(async->impl->poll_thread, 0);
		libusb_release_interface(async->impl->handle, 0);
		libusb_close(async->impl->handle);
		libusb_exit(async->impl->context);
		free(async);
		return 0;
	}
}

