/*
 * Copyright (C) 2014 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#define _LARGEFILE64_SOURCE

#include <stdio.h>

#include <dlfcn.h>
#include <assert.h>

#include <f2fs_fs.h>
#include <f2fs_format_utils.h>
#if defined(__linux__)
#define F2FS_DYN_LIB "libf2fs_fmt_host_dyn.so"
#elif defined(__APPLE__) && defined(__MACH__)
#define F2FS_DYN_LIB "libf2fs_fmt_host_dyn.dylib"
#else
#error "Not supported OS"
#endif

int (*f2fs_format_device_dl)(void);
void (*f2fs_init_configuration_dl)(void);
void (*flush_sparse_buffs_dl)(void);
void (*init_sparse_file_dl)(unsigned int, int64_t);
void (*finalize_sparse_file_dl)(int);
struct f2fs_configuration *f2fs_config;

int f2fs_format_device(void) {
	assert(f2fs_format_device_dl);
	return f2fs_format_device_dl();
}
void f2fs_init_configuration_proxy(void) {
	assert(f2fs_init_configuration_dl);
	//f2fs_init_configuration_dl(config);
	f2fs_init_configuration_dl();
}
void flush_sparse_buffs(void) {
	assert(flush_sparse_buffs_dl);
	return flush_sparse_buffs_dl();
}
void init_sparse_file(unsigned int block_size, int64_t len) {
	assert(init_sparse_file_dl);
	return init_sparse_file_dl(block_size, len);
}
void finalize_sparse_file(int fd) {
	assert(finalize_sparse_file_dl);
	return finalize_sparse_file_dl(fd);
}
int dlopenf2fs() {
	void* f2fs_lib;

	f2fs_lib = dlopen(F2FS_DYN_LIB, RTLD_NOW);
	if (!f2fs_lib) {
		return -1;
	}
	f2fs_format_device_dl = dlsym(f2fs_lib, "f2fs_format_device");
	f2fs_init_configuration_dl = dlsym(f2fs_lib, "f2fs_init_configuration");
	flush_sparse_buffs_dl = dlsym(f2fs_lib, "flush_sparse_buffs");
	init_sparse_file_dl = dlsym(f2fs_lib, "init_sparse_file");
	finalize_sparse_file_dl = dlsym(f2fs_lib, "finalize_sparse_file");
	f2fs_config = dlsym(f2fs_lib, "config");
	if (!f2fs_format_device_dl || !f2fs_init_configuration_dl ||
			!flush_sparse_buffs_dl || !f2fs_config ||
			!init_sparse_file_dl || !finalize_sparse_file_dl) {
		return -1;
	}
	return 0;
}
