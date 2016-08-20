#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "log.h"
#include "pic.h"
#include "portio.h"
#include "ps2.h"
#include "task.h"

#define CTRLLER_MAX_TRIES     3
#define CHANNEL_MAX_TRIES     3
#define RESEND_MAX_TRIES      5
#define UNRELATED_MAX_TRIES 100

static struct ps2_device ps2_device_1;
static struct ps2_device ps2_device_2;

bool ps2_wait_inbuf (size_t timeout)
{
	if ((inport8 (0x64) & (1 << 1)) == 0)
	{
		return true;
	}
	for (size_t i = 0; i < timeout; i++)
	{
		task_sleep_current (1);
		if ((inport8 (0x64) & (1 << 1)) == 0)
		{
			return true;
		}
	}
	return false;
}

bool ps2_wait_outbuf (size_t timeout)
{
	if ((inport8 (0x64) & (1 << 0)) != 0)
	{
		return true;
	}
	for (size_t i = 0; i < timeout; i++)
	{
		task_sleep_current (1);
		if ((inport8 (0x64) & (1 << 0)) != 0)
		{
			return true;
		}
	}
	return false;
}

bool ps2_read_data (uint8_t* byte, size_t timeout)
{
	if (!ps2_wait_outbuf (timeout))
	{
		return false;
	}
	*byte = inport8 (0x60);
#ifdef PS2_DEBUG
	log_printf ("ps2_read_data() data = 0x%02hhX\n", *byte);
#endif
	return true;
}

bool ps2_write_data (uint8_t byte, size_t timeout)
{
	if (!ps2_wait_inbuf (timeout))
	{
		return false;
	}
	outport8 (0x60, byte);
#ifdef PS2_DEBUG
	log_printf ("ps2_write_data() data = 0x%02hhX\n", byte);
#endif
	return true;
}

bool ps2_write_cmd (uint8_t byte, size_t timeout)
{
	if (!ps2_wait_inbuf (timeout))
	{
		return false;
	}
	outport8 (0x64, byte);
#ifdef PS2_DEBUG
	log_printf ("ps2_write_cmd() cmd = 0x%02hhX\n", byte);
#endif
	return true;
}

bool ps2_write_data_to_chan (uint8_t chan, uint8_t byte, size_t timeout)
{
	if (chan == 2 && !ps2_write_cmd (0xD4, timeout))
	{
		return false;
	}
	return ps2_write_data (byte, timeout);
}

bool ps2_write_cmd_to_chan (uint8_t chan, uint8_t cmd,
                            uint8_t* answer, size_t timeout)
{
	for (size_t i = 0; i < RESEND_MAX_TRIES; i++)
	{
		if (!ps2_write_data_to_chan (chan, cmd, timeout))
		{
			return false;
		}
		for (size_t k = 0; i < UNRELATED_MAX_TRIES; k++)
		{
			if (!ps2_read_data (answer, timeout))
			{
				return false;
			}
			if (*answer == 0xAA && !ps2_read_data (answer, timeout))
			{
				return false;
			}
			if (*answer == 0xFA || *answer == 0xEE)
			{
				return true;
			}
			if (*answer == 0xFE)
			{
				break;
			}
		}
	}
	return false;
}

bool ps2_is_keybd_at (struct ps2_device dev)
{
	return dev.size == 0;
}

bool ps2_is_keybd_mf2_notl (struct ps2_device dev)
{
	return dev.size == 2 && dev.resp[0] == 0xAB && dev.resp[1] == 0x83;
}

bool ps2_is_keybd_mf2_tl (struct ps2_device dev)
{
	return (dev.size == 2 && dev.resp[0] == 0xAB && dev.resp[1] == 0x41) ||
	       (dev.size == 2 && dev.resp[0] == 0xAB && dev.resp[1] == 0xC1);
}

bool ps2_is_keybd (struct ps2_device dev)
{
	return ps2_is_keybd_at (dev) ||
	       ps2_is_keybd_mf2_notl (dev) ||
	       ps2_is_keybd_mf2_tl (dev);
}

bool ps2_is_mouse_std (struct ps2_device dev)
{
	return dev.size == 1 && dev.resp[0] == 0x00;
}

bool ps2_is_mouse_scroll (struct ps2_device dev)
{
	return dev.size == 1 && dev.resp[0] == 0x03;
}

bool ps2_is_mouse_5but (struct ps2_device dev)
{
	return dev.size == 1 && dev.resp[0] == 0x04;
}

bool ps2_is_mouse (struct ps2_device dev)
{
	return ps2_is_mouse_std (dev) ||
	       ps2_is_mouse_scroll (dev) ||
	       ps2_is_mouse_5but (dev);
}

bool ps2_keybd_set_rate (uint8_t chan, uint8_t value)
{
	uint8_t tmp;
	return ps2_write_cmd_to_chan (chan, 0xF3, &tmp, 100) &&
	       ps2_write_cmd_to_chan (chan, value, &tmp, 100);
}

bool ps2_keybd_init (uint8_t chan)
{
	return ps2_keybd_set_rate (chan, 0x20);
}

bool ps2_mouse_set_rate (uint8_t chan, uint8_t value)
{
	uint8_t tmp;
	return ps2_write_cmd_to_chan (chan, 0xF3, &tmp, 100) &&
	       ps2_write_cmd_to_chan (chan, value, &tmp, 100);
}

bool ps2_mouse_init (uint8_t chan)
{
	struct ps2_device dev;
	if (!ps2_mouse_set_rate (chan, 200) || !ps2_mouse_set_rate (chan, 100) ||
	    !ps2_mouse_set_rate (chan, 80) || !ps2_detect_dev (chan, &dev))
	{
		return false;
	}
	if (!ps2_is_mouse_scroll (dev))
	{
		goto skip;
	}
	if (!ps2_mouse_set_rate (chan, 200) || !ps2_mouse_set_rate (chan, 200) ||
	    !ps2_mouse_set_rate (chan, 80) || !ps2_detect_dev (chan, &dev))
	{
		return false;
	}
skip:
	return ps2_mouse_set_rate (chan, 40);
}

void ps2_handle_keybd (uint8_t chan)
{
	log_printf ("Byte 0x%02hhX on channel %d\n", inport8 (0x60), chan);
}

void ps2_handle_mouse (uint8_t chan)
{
	log_printf ("Byte 0x%02hhX on channel %d\n", inport8 (0x60), chan);
}

void ps2_handle (uint8_t chan)
{
	if (chan == 1 && ps2_device_1.enabled)
	{
		if (ps2_is_keybd (ps2_device_1))
		{
			ps2_handle_keybd (chan);
		}
		if (ps2_is_mouse (ps2_device_1))
		{
			ps2_handle_mouse (chan);
		}
	}
	if (chan == 2 && ps2_device_2.enabled)
	{
		if (ps2_is_keybd (ps2_device_2))
		{
			ps2_handle_keybd (chan);
		}
		if (ps2_is_mouse (ps2_device_2))
		{
			ps2_handle_mouse (chan);
		}
	}
}

bool ps2_detect_dev (uint8_t chan, struct ps2_device* dev)
{
	uint8_t tmp;
	while ((inport8 (0x64) & 0x01) != 0)
	{
		inport8 (0x60);
	}
	if (!ps2_write_cmd_to_chan (chan, 0xF2, &tmp, 100))
	{
		return false;
	}
	dev->size = 0;
	if (ps2_read_data (&dev->resp[0], 200))
	{
		dev->size = 1;
		if (ps2_read_data (&dev->resp[1], 200))
		{
			dev->size = 2;
		}
	}
	return true;
}

bool ps2_init (void)
{
	uint8_t tmp;
	uint8_t config;
	if (!ps2_write_cmd (0xAD, 100) || !ps2_write_cmd (0xA7, 100))
	{
		goto port_fail;
	}
	while ((inport8 (0x64) & 0x01) != 0)
	{
		inport8 (0x60);
	}
	if (!ps2_write_cmd (0x20, 100) || !ps2_read_data (&config, 100))
	{
		goto port_fail;
	}
	config &= 0xFF & ~(1 << 0) & ~(1 << 1) & ~(1 << 6);
#if 0 /* Disabled due to some BIOSes not setting the bit 2 on success. */
	if ((config & (1 << 2)) == 0)
	{
		panic ("Panic: POST not passed.\n");
	}
#endif
	if (!ps2_write_cmd (0x60, 100) || !ps2_write_data (config, 100))
	{
		goto port_fail;
	}
	for (size_t i = 0; i < CTRLLER_MAX_TRIES; i++)
	{
		if (!ps2_write_cmd (0xAA, 100) || !ps2_read_data (&tmp, 500))
		{
			goto port_fail;
		}
		if (tmp == 0x55)
		{
			break;
		}
		if (i == CTRLLER_MAX_TRIES - 1)
		{
			log_printf ("Warning: PS/2 controller self-test failed "
			            "with 0x%02hhX: Aborted\n", tmp);
			return false;
		}
		log_printf ("Warning: PS/2 controller self-test failed "
		            "with 0x%02hhX: Aborted\n", tmp);
	}
	bool double_chan = false;
	if ((config & (1 << 5)) != 0)
	{
		if (!ps2_write_cmd (0xA8, 100) || !ps2_write_cmd (0x20, 100) ||
		    !ps2_read_data (&tmp, 100))
		{
			goto port_fail;
		}
		double_chan = (tmp & (1 << 5)) == 0;
		if (!ps2_write_cmd (0xA7, 100))
		{
			goto port_fail;
		}
	}
	bool chan_1 = false;
	bool chan_2 = false;
	for (size_t i = 0; i < CHANNEL_MAX_TRIES; i++)
	{
		if (!ps2_write_cmd (0xAB, 100) || !ps2_read_data (&tmp, 100))
		{
			goto port_fail;
		}
		if (tmp == 0)
		{
			chan_1 = true;
			break;
		}
		log_printf ("Warning: PS/2 channel 1 test failed "
		            "with 0x%02hhX: %s\n", tmp,
		            i < CHANNEL_MAX_TRIES - 1 ? "Retrying" : "Aborted");
	}
	if (double_chan)
	{
		for (size_t i = 0; i < CHANNEL_MAX_TRIES; i++)
		{
			if (!ps2_write_cmd (0xA9, 100) || !ps2_read_data (&tmp, 100))
			{
				goto port_fail;
			}
			if (tmp == 0)
			{
				chan_2 = true;
				break;
			}
			log_printf ("Warning: PS/2 channel 2 test failed "
			            "with 0x%02hhX: %s\n", tmp,
			            i < CHANNEL_MAX_TRIES - 1 ? "Retrying" : "Aborted");
		}
	}
	if (!chan_1 && !chan_2)
	{
		log_printf ("Warning: No working PS/2 channels found: Aborted");
		return false;
	}
	struct ps2_device dev_1;
	struct ps2_device dev_2;
	if (chan_1)
	{
		if (!ps2_write_cmd (0xAE, 100))
		{
			goto port_fail;
		}
		if (!ps2_write_cmd_to_chan (1, 0xF5, &tmp, 100) ||
		    !ps2_detect_dev (1, &dev_1))
		{
			chan_1 = false;
			goto disable_chan_1;
		}
		if (ps2_is_keybd (dev_1))
		{
			chan_1 = ps2_keybd_init (1) && ps2_detect_dev (1, &dev_1);
		}
		else if (ps2_is_mouse (dev_1))
		{
			chan_1 = ps2_mouse_init (1) && ps2_detect_dev (1, &dev_1);
		}
		else
		{
			chan_1 = false;
		}
	disable_chan_1:
		if (!ps2_write_cmd (0xAD, 100))
		{
			goto port_fail;
		}
	}
	if (chan_2)
	{
		if (!ps2_write_cmd (0xA8, 100))
		{
			goto port_fail;
		}
		if (!ps2_write_cmd_to_chan (2, 0xF5, &tmp, 100) ||
		    !ps2_detect_dev (2, &dev_2))
		{
			chan_2 = false;
			goto disable_chan_2;
		}
		if (ps2_is_keybd (dev_2))
		{
			chan_2 = ps2_keybd_init (2) && ps2_detect_dev (2, &dev_2);
		}
		else if (ps2_is_mouse (dev_2))
		{
			chan_2 = ps2_mouse_init (2) && ps2_detect_dev (2, &dev_2);
		}
		else
		{
			chan_2 = false;
		}
	disable_chan_2:
		if (!ps2_write_cmd (0xA7, 100))
		{
			goto port_fail;
		}
	}
	if ((chan_1 && !ps2_write_cmd (0xAE, 100)) ||
	    (chan_2 && !ps2_write_cmd (0xA8, 100)))
	{
		goto port_fail;
	}
	if (chan_1 && !ps2_write_cmd_to_chan (1, 0xF4, &tmp, 100))
	{
		chan_1 = false;
	}
	if (chan_2 && !ps2_write_cmd_to_chan (2, 0xF4, &tmp, 100))
	{
		chan_2 = false;
	}
	if (!ps2_write_cmd (0x20, 100) || !ps2_read_data (&config, 100))
	{
		goto port_fail;
	}
	if (chan_1)
	{
		config |= 1 << 0;
	}
	if (chan_2)
	{
		config |= 1 << 1;
	}
	if (!ps2_write_cmd (0x60, 100) || !ps2_write_data (config, 100))
	{
		goto port_fail;
	}
	if (chan_1)
	{
		pic_unmask_irq (1);
	}
	if (chan_2)
	{
		pic_unmask_irq (12);
	}
	dev_1.enabled = chan_1;
	dev_2.enabled = chan_2;
	ps2_device_1 = dev_1;
	ps2_device_2 = dev_2;
	return true;
port_fail:
	log_printf ("Warning: PS/2 port read/write failed: Aborted\n");
	return false;
}
