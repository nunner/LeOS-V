#include "driver/virtio.h"
#include "driver/virtio-classes.h"

#include "os.h"
#include "memory/heap.h"


static char *types[] = {
	"None", // "Reserved"
	"Network card",
	"Block device",
	"Console",
	"Entropy source",
	"memory ballooning (traditional)",
	"ioMemory",
	"rpmsg",
	"SCSI host",
	"9P transport",
	"mac80211 wlan",
	"rproc serial",
	"virtio CAIF",
	"memory balloon",
	"GPU device",
	"Timer/lLock device"
	"Input device",
};

dev_setup_t	setup_functions[] = {
	0,
	setup_network,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
};

static virtio_dev_t *devices[MMIO_COUNT];

void
setup_device(virtio_dev_t *dev)
{
	int id = get_virtio_field(uint32_t, dev, DeviceID);
		
	if(setup_functions[id] != 0)
		setup_functions[id](dev);
	else
		kprintf("No driver found for %s", types[id]);
}

void
scan_virtio()
{
	kprintf("Scanning virtio devices.\n");

	for(size_t i = MMIO_COUNT - 1; i > 0 ; i--) {
		virtio_dev_t *dev = (virtio_dev_t *) (MMIO_START + i * MMIO_STEP);

		if(get_virtio_field(uint32_t, dev, MagicValue) == MAGIC_VALUE) {
			kprintf("Bus %d: \t%s\n", MMIO_COUNT - i, types[get_virtio_field(uint32_t, dev, DeviceID)]);

			if(get_virtio_field(uint32_t, dev, DeviceID) != 0)
				devices[MMIO_COUNT - 1 - i] = dev;
		}
	}

	kprintf("Setting up devices.\n");
	for(size_t i = 0; i < MMIO_COUNT; i++) {
		if(devices[i] != 0)
			setup_device(devices[i]);
	}
}