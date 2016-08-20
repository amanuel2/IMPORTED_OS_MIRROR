#ifndef PS2_H
#define PS2_H

#include <stdbool.h>
#include <stdint.h>

struct ps2_device
{
	uint8_t resp[2];
	uint8_t size;
	bool enabled;
};

bool ps2_wait_inbuf (size_t);
bool ps2_wait_outbuf (size_t);

bool ps2_read_data (uint8_t*, size_t);
bool ps2_write_data (uint8_t, size_t);
bool ps2_write_cmd (uint8_t, size_t);
bool ps2_write_data_to_chan (uint8_t, uint8_t, size_t);
bool ps2_write_cmd_to_chan (uint8_t, uint8_t, uint8_t*, size_t);

bool ps2_is_keybd_at (struct ps2_device);
bool ps2_is_keybd_mf2_notl (struct ps2_device);
bool ps2_is_keybd_mf2_tl (struct ps2_device);
bool ps2_is_keybd (struct ps2_device);

bool ps2_is_mouse_std (struct ps2_device);
bool ps2_is_mouse_scroll (struct ps2_device);
bool ps2_is_mouse_5but (struct ps2_device);
bool ps2_is_mouse (struct ps2_device);

bool ps2_keybd_set_rate (uint8_t, uint8_t);
bool ps2_keybd_init (uint8_t);

bool ps2_mouse_set_rate (uint8_t, uint8_t);
bool ps2_mouse_init (uint8_t);

void ps2_handle_keybd (uint8_t);
void ps2_handle_mouse (uint8_t);
void ps2_handle (uint8_t);

bool ps2_detect_dev (uint8_t, struct ps2_device*);
bool ps2_init (void);

#endif
