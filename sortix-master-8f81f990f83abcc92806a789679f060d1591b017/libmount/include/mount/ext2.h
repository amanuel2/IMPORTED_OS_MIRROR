/*
 * Copyright (c) 2015, 2016 Jonas 'Sortie' Termansen.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * mount/ext2.h
 * ext2 filesystem.
 */

#ifndef INCLUDE_MOUNT_EXT2_H
#define INCLUDE_MOUNT_EXT2_H

#include <stdint.h>

#include <mount/filesystem.h>

#define EXT2_SUPER_MAGIC 0xEF53
#define EXT2_VALID_FS 1
#define EXT2_ERROR_FS 2
#define EXT2_FEATURE_COMPAT_DIR_PREALLOC (1U << 0U)
#define EXT2_FEATURE_COMPAT_IMAGIC_INODES (1U << 1U)
#define EXT3_FEATURE_COMPAT_HAS_JOURNAL (1U << 2U)
#define EXT2_FEATURE_COMPAT_EXT_ATTR (1U << 3U)
#define EXT2_FEATURE_COMPAT_RESIZE_INO (1U << 4U)
#define EXT2_FEATURE_COMPAT_DIR_INDEX (1U << 5U)
#define EXT2_FEATURE_INCOMPAT_COMPRESSION (1U << 0U)
#define EXT2_FEATURE_INCOMPAT_FILETYPE (1U << 1U)
#define EXT2_FEATURE_INCOMPAT_RECOVER (1U << 2U)
#define EXT2_FEATURE_INCOMPAT_JOURNAL_DEV (1U << 3U)
#define EXT2_FEATURE_INCOMPAT_META_BG (1U << 4U)
#define EXT2_FEATURE_RO_COMPAT_SPARSE_SUPER (1U << 0U)
#define EXT2_FEATURE_RO_COMPAT_LARGE_FILE (1U << 1U)
#define EXT2_FEATURE_RO_COMPAT_BTREE_DIR (1U << 2U)

struct ext2_superblock
{
	uint32_t s_inodes_count;
	uint32_t s_blocks_count;
	uint32_t s_r_blocks_count;
	uint32_t s_free_blocks_count;
	uint32_t s_free_inodes_count;
	uint32_t s_first_data_block;
	uint32_t s_log_block_size;
	 int32_t s_log_frag_size;
	uint32_t s_blocks_per_group;
	uint32_t s_frags_per_group;
	uint32_t s_inodes_per_group;
	uint32_t s_mtime;
	uint32_t s_wtime;
	uint16_t s_mnt_count;
	uint16_t s_max_mnt_count;
	uint16_t s_magic;
	uint16_t s_state;
	uint16_t s_errors;
	uint16_t s_minor_rev_level;
	uint32_t s_lastcheck;
	uint32_t s_checkinterval;
	uint32_t s_creator_os;
	uint32_t s_rev_level;
	uint16_t s_def_resuid;
	uint16_t s_def_resgid;
// EXT2_DYNAMIC_REV
	uint32_t s_first_ino;
	uint16_t s_inode_size;
	uint16_t s_block_group_nr;
	uint32_t s_feature_compat;
	uint32_t s_feature_incompat;
	uint32_t s_feature_ro_compat;
	uint8_t  s_uuid[16];
	/*uint8_t*/ char s_volume_name[16];
	/*uint8_t*/ char s_last_mounted[64];
	uint32_t s_algo_bitmap;
// Performance Hints
	uint8_t  s_prealloc_blocks;
	uint8_t  s_prealloc_dir_blocks;
	uint16_t alignment0;
// Journaling Support
	uint8_t  s_journal_uuid[16];
	uint32_t s_journal_inum;
	uint32_t s_journal_dev;
	uint32_t s_last_orphan;
// Directory Indexing Support
	uint32_t s_hash_seed[4];
	uint8_t  s_def_hash_version;
	uint8_t  alignment1[3];
// Other options
	uint32_t s_default_mount_options;
	uint32_t s_first_meta_bg;
	uint8_t  alignment2[760];
};

#if defined(__cplusplus)
extern "C" {
#endif

extern const struct filesystem_handler ext2_handler;

void ext2_superblock_decode(struct ext2_superblock*);

#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif
