#include "public/usblink_async.h"

#include "libusb.h"
#include "usblink_const.h"

#include "stdlib.h"
#include "stdint.h"

enum usblink_async_type {
	USBLINK_ASYNC_TYPE_NONE,
	USBLINK_ASYNC_TYPE_GET_VERSION,
	USBLINK_ASYNC_TYPE_GET_PARAMS
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
	unsigned char destory;
};

static libusb_device_handle *usblink_async_find_device(struct libusb_context *context);
static void usblink_async_ctrl_transfer_cb(struct libusb_transfer *transfer);
static void usblink_async_ctrl_transfer_complete(struct usblink_async_priv *impl, struct libusb_transfer *transfer);
static void usblink_async_destory_later(struct usblink_async *async, unsigned char ctrl_bulk);

static void usblink_async_ctrl_transfer_cb(struct libusb_transfer *transfer)
{
	struct usblink_version version;
	struct usblink_async *async = (struct usblink_async *)(transfer->user_data);
	struct usblink_async_priv *impl = async->impl;
	assert(transfer);
	switch (transfer->status) {
		case LIBUSB_TRANSFER_COMPLETED:
			{
				usblink_async_ctrl_transfer_complete(impl, transfer);
			}
			break;
		default:
			break;
	}
	usblink_async_destory_later(async, 0);
}

static void usblink_async_ctrl_transfer_complete(struct usblink_async_priv *impl, struct libusb_transfer *transfer)
{
	assert(impl && transfer);
	switch (impl->event) {
		case USBLINK_ASYNC_TYPE_GET_VERSION:
			{
				struct usblink_version version;
				assert(transfer->buffer && impl->cb &&
						(USBLINK_CTRL_SETUP_SIZE + USBLINK_GET_VERSION_REQUEST_LENGTH == transfer->actual_length));
				version.major = ((uint16_t)transfer->buffer[0] << 8) | transfer->buffer[1];
				version.minor = ((uint16_t)transfer->buffer[2] << 8) | transfer->buffer[3];
				impl->cb->usblink_async_get_version_finish(&version);
			}
			break;
		case USBLINK_ASYNC_TYPE_GET_PARAMS:
			{
				struct usblink_params params;
				assert(transfer->buffer && impl->cb &&
						(USBLINK_CTRL_SETUP_SIZE + USBLINK_GET_PARAMS_REQUEST_LENGTH == transfer->actual_length));
				params.bmCapabilities = (uint32_t)transfer->buffer[0];
				params.wWidth = (uint16_t)transfer->buffer[4];
				params.wHeight = (uint16_t)transfer->buffer[6];
				params.bmPixelFormatSupported = (uint32_t)transfer->buffer[8];
				params.bmEncodingSupported = (uint32_t)transfer->buffer[12];
				impl->cb->usblink_async_get_params_finish(&params);
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
	switch (ctrl_bulk) {
		case 0:
			{
				if (1 == async->impl->destory) {
					libusb_free_transfer(async->impl->ctrl);
					async->impl->ctrl = 0;
				}
			}
			break;
		case 1:
			{
				if (1 == async->impl->destory) {
					libusb_free_transfer(async->impl->bulk);
					async->impl->bulk = 0;
				}
			}
			break;
		default:
			break;
	}
	if ((0 == async->impl->ctrl)
			&& (0 == async->impl->bulk)) {
		libusb_release_interface(async->impl->handle, 0);
		libusb_close(async->impl->handle);
		libusb_exit(async->impl->context);
		free(async);
	}
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
	async->impl = impl;
	return async;
}

void usblink_async_destory(struct usblink_async *async)
{
	assert(async);
	async->impl->destory = 1;
}

int usblink_async_get_version(struct usblink_async *async, struct usblink_version *version)
{
	unsigned char *ctrl_buffer = 0;
	assert(async && version);
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
	assert(async->impl);
	return libusb_submit_transfer(async->impl->ctrl);
}

int usblink_async_get_params(struct usblink_async *async)
{
	unsigned char *ctrl_buffer = 0;
	assert(async);
	ctrl_buffer = (unsigned char *)malloc(USBLINK_GET_PARAMS_REQUEST_LENGTH + USBLINK_CTRL_SETUP_SIZE);
	assert(ctrl_buffer);
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
}

int usblink_async_set_config(struct usblink_async *async, struct usblink_config *config)
{

}

int usblink_async_start_fb_trans(struct usblink_async *async, enum usblink_trans_mode mode)
{

}

int usblink_async_pause_fb_trans(struct usblink_async *async)
{

}

int usblink_async_stop_fb_trans(struct usblink_async *async)
{

}

int usblink_async_set_mfps(struct usblink_async *async, unsigned char mfps)
{

}
