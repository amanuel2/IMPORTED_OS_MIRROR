#ifndef VBE_H
#define VBE_H

#include <stdint.h>

struct __attribute__ ((packed)) vbe_mode_info
{
	uint16_t mode_attr;
	uint8_t winA_attr;
	uint8_t winB_attr;
	uint16_t win_granul;
	uint16_t win_size;
	uint16_t winA_seg;
	uint16_t winB_seg;
	uint32_t win_func_addr;
	uint16_t pitch;
	uint16_t x_res;
	uint16_t y_res;
	uint8_t x_char_size;
	uint8_t y_char_size;
	uint8_t planes;
	uint8_t bpp;
	uint8_t banks;
	uint8_t memmodel;
	uint8_t bank_size;
	uint8_t img_pages;
	uint8_t reserved0;
	uint8_t red_mask_size;
	uint8_t red_field_pos;
	uint8_t green_mask_size;
	uint8_t green_field_pos;
	uint8_t blue_mask_size;
	uint8_t blue_field_pos;
	uint8_t reserved_mask_size;
	uint8_t reserved_field_pos;
	uint8_t direct_color_attr;
	uint32_t framebuffer_addr;
	uint32_t reserved1;
	uint16_t reserved2;
	uint16_t lin_pitch;
	uint8_t bnk_img_pages;
	uint8_t lin_img_pages;
	uint8_t lin_red_mask_size;
	uint8_t lin_red_field_pos;
	uint8_t lin_green_mask_size;
	uint8_t lin_green_field_pos;
	uint8_t lin_blue_mask_size;
	uint8_t lin_blue_field_pos;
	uint8_t lin_reserved_mask_size;
	uint8_t lin_reserved_field_pos;
	uint32_t max_pixel_clock;
};

struct __attribute__ ((packed)) vbe_ctrl_info
{
	uint32_t signature;
	uint16_t version;
	uint32_t oem_str_addr;
	uint32_t capabilities;
	uint32_t video_mode_addr;
	uint16_t blocks;
	uint16_t oem_software_revision;
	uint32_t oem_vendor_name_addr;
	uint32_t oem_product_name_addr;
	uint32_t oem_product_revision_addr;
};

extern struct vbe_mode_info* vbe_mode_info;

extern struct vbe_ctrl_info* vbe_ctrl_info;

#endif
