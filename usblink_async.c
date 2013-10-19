#include "public/usblink_async.h"

#include "libusb.h"

#include "stdlib.h"

struct usblink_async_priv {
	struct libusb_context *context;
	struct libusb_device_handle *handle;
	struct libusb_transfer *ctrl;
	/* bulk transfer for framebuffer */
	struct libusb_transfer *bulk;
	struct usblink_async_callback *cb;
	unsigned char *ctrl_buffer;
	unsigned char *frame_buffer;
};

static libusb_device_handle *find_usblink_device(struct libusb_context *context);
static void usblink_async_ctrl_transfer_cb(struct libusb_transfer *transfer);

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
	async->impl = impl;
	return sync;
}

void usblink_async_get_version(struct usblink_async *async, struct usblink_version *version)
{
	assert(async && async->impl && version);
	if (0 == async->impl->ctrl_buffer) {
		async->impl->ctrl_buffer = (unsigned char *)malloc(12);
		libusb_fill_control_setup(async->impl->ctrl_buffer, 0xc1, 0x40,
		(unsigned short)(version->major) << 8 | version->minor, 2);
		/* TODO: timeout time to be determined. temp value 10 ms */
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
			assert(transfer->actual_length == 4);
			version.major = ((unsigned short)transfer->buffer[0] << 8) | transfer->buffer[1];
			version.minor = ((unsigned short)transfer->buffer[2] << 8) | transfer->buffer[3];
			impl->cb->usblink_async_get_version_finish(&version);
		}
		break;
	}
	if (0 == impl->ctrl_buffer) {
		free(impl->ctrl_buffer);
	}
}

