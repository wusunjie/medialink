#include "public/usblink_async.h"

#include "libusb.h"

#include "stdlib.h"
#include "stdint.h"

enum usblink_async_type {
	USBLINK_ASYNC_TYPE_NONE,
	USBLINK_ASYNC_TYPE_GET_VERSION
}

struct usblink_async_priv {
	struct libusb_context *context;
	struct libusb_device_handle *handle;
	struct libusb_transfer *ctrl;
	/* bulk transfer for framebuffer */
	struct libusb_transfer *bulk;
	struct usblink_async_callback *cb;
	unsigned char *ctrl_buffer;
	unsigned char *frame_buffer;
	enum usblink_async_type event;
};

static libusb_device_handle *find_usblink_device(struct libusb_context *context);
static void usblink_async_ctrl_transfer_cb(struct libusb_transfer *transfer);
static void usblink_async_ctrl_transfer_complete(struct usblink_async_priv *impl, int length);

struct usblink_async *usblink_async_init(struct usblink_async_callback *cb)
{
	struct usblink_async *async = (struct usblink_async *)malloc(sizeof(*async));
	struct usblink_async_priv *impl = (struct usblink_async_priv *)malloc(sizeof(*impl));
	assert(async && impl);
	libusb_init(&(impl->context));
	impl->handle = find_usblink_device(impl->context);
	libusb_claim_interface(impl->handle, 0);
	/* param set to [0] for non-isochronous */
	impl->bulk = libusb_alloc_transfer(0);
	impl->ctrl = libusb_alloc_transfer(0);
	impl->cb = cb;
	impl->ctrl_buffer = 0;
	impl->ctrl_buffer = 0;
	impl->event = USBLINK_ASYNC_TYPE_NONE;
	async->impl = impl;
	return sync;
}

void usblink_async_get_version(struct usblink_async *async, struct usblink_version *version)
{
	assert(async && async->impl && version);
	if (0 == async->impl->ctrl_buffer) {
		async->impl->ctrl_buffer = (unsigned char *)malloc(12);
		libusb_fill_control_setup(async->impl->ctrl_buffer, 0xc1, 0x40,
				(uint16_t)(version->major) << 8 | version->minor, 2);
		/* TODO: timeout time to be determined. temp value 10 ms */
		async->impl->event = USBLINK_ASYNC_TYPE_GET_VERSION;
		libusb_fill_control_transfer(async->impl->ctrl, async->impl->handle,
				async->impl->ctrl_buffer, usblink_async_ctrl_transfer_cb,
				async->impl, 10);
	}
}

static void usblink_async_ctrl_transfer_cb(struct libusb_transfer *transfer)
{
	struct usblink_version version;
	struct usblink_async_priv *impl = (struct usblink_async *)(transfer->user_data)->impl;
	assert(transfer && impl);
	switch (transfer->status) {
		case LIBUSB_TRANSFER_COMPLETE:
			{
				usblink_async_ctrl_transfer_complete(impl, transfer->actual_length);
			}
			break;
	}
	if (0 == impl->ctrl_buffer) {
		free(impl->ctrl_buffer);
		impl->ctrl_buffer = 0;
	}
}

static void usblink_async_ctrl_transfer_complete(struct usblink_async_priv *impl, int length)
{
	assert(impl);
	switch (impl->event) {
		case USBLINK_ASYNC_TYPE_GET_VERSION:
			{
				struct usblink_version version;
				assert(impl->buffer && impl->cb && (4 == length));
				version.major = ((uint16_t)impl->buffer[0] << 8) | impl->buffer[1];
				version.minor = ((uint16_t)impl->buffer[2] << 8) | impl->buffer[3];
				impl->cb->usblink_async_get_version_finish(&version);
			}
			break;
	}
	impl->event =  USBLINK_ASYNC_TYPE_NONE;
}

static libusb_device_handle *find_usblink_device(struct libusb_context *context)
{
	struct libusb_device **list =  0;
	struct libusb_device *device = 0;
	struct libusb_device_handle *handle = 0;
	struct libusb_device_descriptor descriptor;
	ssize_t cnt = 0;
	cnt = libusb_get_device_list(context, &list);
	assert(list);
	for (ssize_t i = 0; i < cnt; i++) {
		libusb_get_device_descriptor(list[i], &descriptor);
		if ((0xff == descriptor.bDeviceClass)
				&& (0xcc == descriptor.bDeviceSubClass)
				&& (0x01 == descriptor.bDeviceProtocol)) {
			device = list[i];
			break;
		}
	}
	if (0 != device) {
		int r = libusb_open(device, &handle);
		assert(0 == r);
	}
	return handle;
}

