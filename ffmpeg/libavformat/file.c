/*
 * buffered file I/O
 * Copyright (c) 2001 Fabrice Bellard
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "libavutil/avstring.h"
#include "libavutil/internal.h"
#include "libavutil/opt.h"
#include "avformat.h"
#if HAVE_DIRENT_H
#include <dirent.h>
#endif
#include <fcntl.h>
#if HAVE_IO_H
#include <io.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/stat.h>
#include <stdlib.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/sendfile.h>
#include "os_support.h"
#include "url.h"
#define _GNU_SOURCE                    /* See feature_test_macros(7) */
#define FALLOC_FL_KEEP_SIZE 0x1
extern int fallocate (int __fd, int __mode, __off_t __offset, __off_t __len);

/* Some systems may not have S_ISFIFO */
#ifndef S_ISFIFO
#  ifdef S_IFIFO
#    define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#  else
#    define S_ISFIFO(m) 0
#  endif
#endif

/* Not available in POSIX.1-1996 */
#ifndef S_ISLNK
#  ifdef S_IFLNK
#    define S_ISLNK(m) (((m) & S_IFLNK) == S_IFLNK)
#  else
#    define S_ISLNK(m) 0
#  endif
#endif

/* Not available in POSIX.1-1996 */
#ifndef S_ISSOCK
#  ifdef S_IFSOCK
#    define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)
#  else
#    define S_ISSOCK(m) 0
#  endif
#endif

/* standard file protocol */

pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;
#define FILE_SLICE_SIZE (1 * 1024 * 1024)
#define FILE_PREALLOC_SIZE (20 * 1024 * 1024)

typedef struct FileContext {
    const AVClass *class;
    int fd;
    int trunc;
    int blocksize;
    int follow;
    int seekable;
#if HAVE_DIRENT_H
    DIR *dir;
#endif
    int final_fd;
    int64_t pos;
    int total_size;
    int64_t virtual_pos;
    int virtual_size;
    int64_t virtual_pos_base;
    int is_mov_mp4;
    int is2slice;
    int isfalloc;
    int falloc_blk;
    int isfsync;
} FileContext;

static const AVOption file_options[] = {
    { "truncate", "truncate existing files on write", offsetof(FileContext, trunc), AV_OPT_TYPE_BOOL, { .i64 = 1 }, 0, 1, AV_OPT_FLAG_ENCODING_PARAM },
    { "blocksize", "set I/O operation maximum block size", offsetof(FileContext, blocksize), AV_OPT_TYPE_INT, { .i64 = INT_MAX }, 1, INT_MAX, AV_OPT_FLAG_ENCODING_PARAM },
    { "follow", "Follow a file as it is being written", offsetof(FileContext, follow), AV_OPT_TYPE_INT, { .i64 = 0 }, 0, 1, AV_OPT_FLAG_DECODING_PARAM },
    { "seekable", "Sets if the file is seekable", offsetof(FileContext, seekable), AV_OPT_TYPE_INT, { .i64 = -1 }, -1, 0, AV_OPT_FLAG_DECODING_PARAM | AV_OPT_FLAG_ENCODING_PARAM },
    { "toslice", "Slice files when writing", offsetof(FileContext, is2slice), AV_OPT_TYPE_BOOL, { .i64 = 0 }, 0, 1, AV_OPT_FLAG_ENCODING_PARAM },
    { "falloc", "Enable Fallocate", offsetof(FileContext, isfalloc), AV_OPT_TYPE_BOOL, { .i64 = 0 }, 0, 1, AV_OPT_FLAG_ENCODING_PARAM },
    { "fsync", "Enable Fsync", offsetof(FileContext, isfsync), AV_OPT_TYPE_BOOL, { .i64 = 0 }, 0, 1, AV_OPT_FLAG_ENCODING_PARAM },
    { NULL }
};

static const AVOption pipe_options[] = {
    { "blocksize", "set I/O operation maximum block size", offsetof(FileContext, blocksize), AV_OPT_TYPE_INT, { .i64 = INT_MAX }, 1, INT_MAX, AV_OPT_FLAG_ENCODING_PARAM },
    { NULL }
};

static const AVClass file_class = {
    .class_name = "file",
    .item_name  = av_default_item_name,
    .option     = file_options,
    .version    = LIBAVUTIL_VERSION_INT,
};

static const AVClass pipe_class = {
    .class_name = "pipe",
    .item_name  = av_default_item_name,
    .option     = pipe_options,
    .version    = LIBAVUTIL_VERSION_INT,
};

static int file_read(URLContext *h, unsigned char *buf, int size)
{
    FileContext *c = h->priv_data;
    int ret;
    size = FFMIN(size, c->blocksize);
    // av_log(h, AV_LOG_WARNING, "Enter file_read(%d) \n", size);
    ret = read(c->fd, buf, size);
    if (ret == 0 && c->follow)
        return AVERROR(EAGAIN);
    if (ret == 0)
        return AVERROR_EOF;
    return (ret == -1) ? AVERROR(errno) : ret;
}

static int is_mp4_mov_suffix(const char* str_source)
{
    int len = strlen(str_source);
    int compare_len = 4;
    if (len > compare_len)
    {
        const char* pc = str_source + len - compare_len;
        if (*pc == 'm')
        {
            if (strncmp(pc+1, "ov", 2) == 0 || strncmp(pc+1, "p4", 2) == 0)
            {
                return 1;
            }
        }
    }
    return 0;
}

static void read_fullfilepath(int fd)
{
    char buf[256];
    char filepath[1024]="";
    int filepath_size = 1024, bufsize = 256;

    snprintf(buf, bufsize, "/proc/self/fd/%d", fd);
    if (readlink(buf, filepath, filepath_size) > 0)
        av_log(NULL, AV_LOG_WARNING, "Enter file_open(%s) \n", filepath);
}

static int create_tmpfile(const char* strTemplate)
{
    int fd;
    char filepath[128]="";
    strcpy(filepath, strTemplate);
    fd = mkstemp(filepath);
    // fd = open(filepath, O_CREAT | O_RDWR | O_TRUNC, 0666);
    if (fd == -1)
        return -1;
    remove(filepath);
    return fd;
}

static int file_write(URLContext *h, const unsigned char *buf, int size)
{
    FileContext *c = h->priv_data;
    int ret;
    // av_log(h, AV_LOG_WARNING, "Enter file_write(%ld:%d + %d) \n", c->pos, c->total_size, size);
    size = FFMIN(size, c->blocksize);

	if(access(h->filename, F_OK) != 0) {
		av_log(h, AV_LOG_WARNING, "File %s not exist\n", h->filename);
		return -1;
	}

	if (c->isfalloc) {
		int fd = (c->is2slice) ? c->final_fd : c->fd;
		if (c->pos + size >= c->falloc_blk * FILE_PREALLOC_SIZE) {
			pthread_mutex_lock(&file_mutex);
			fallocate(fd, FALLOC_FL_KEEP_SIZE, c->falloc_blk * FILE_PREALLOC_SIZE, FILE_PREALLOC_SIZE);
			pthread_mutex_unlock(&file_mutex);
			c->falloc_blk++;
		}
	}

	if (!c->is2slice)
    {}
    else if (c->pos > c->total_size)
    {
        av_log(h, AV_LOG_WARNING, "file_write out-of-range \n");
        return size;
    }
    else if (c->virtual_pos < c->virtual_pos_base)
    {
        off_t cur_off =  lseek(c->final_fd, 0, SEEK_CUR);
        // av_log(h, AV_LOG_WARNING, "file_write rollback \n");
        lseek(c->final_fd, c->virtual_pos, SEEK_SET);
        ret = write(c->final_fd, buf, size);
        lseek(c->final_fd, cur_off, SEEK_SET);
        return (ret == -1) ? AVERROR(errno) : ret;
    }
    else if (c->pos + size < c->total_size)
    {
        // av_log(h, AV_LOG_WARNING, "file_write rollback case.2 \n");
    }
    else if (c->pos + size >= FILE_SLICE_SIZE)
    {
        int remain = c->pos + size - FILE_SLICE_SIZE;
        off_t offset = 0;
        ssize_t send_size = 0;
        ret = write(c->fd, buf, size - remain);
        pthread_mutex_lock(&file_mutex);
        send_size = sendfile(c->final_fd, c->fd, &offset, FILE_SLICE_SIZE);
        pthread_mutex_unlock(&file_mutex);
        if (send_size != FILE_SLICE_SIZE)
            av_log(h, AV_LOG_WARNING, "sendfile un-finished %ld ?\n", send_size);

        c->virtual_pos_base += FILE_SLICE_SIZE;
        ftruncate(c->fd, 0);
        lseek(c->fd, 0, SEEK_SET);
        c->pos = c->total_size = 0;
        if (remain > 0) {
            int ret2 = write(c->fd, buf + size - remain, remain);
            c->pos = c->total_size = ret2;
            // av_log(h, AV_LOG_WARNING, "file_write warp file %d %d\n", remain, ret2);
            ret += ret2;
        }
        if (ret > 0 && c->virtual_size < c->virtual_pos + ret) {
            c->virtual_pos += ret;
            c->virtual_size = c->virtual_pos;
        }
        return (ret == -1) ? AVERROR(errno) : ret;
    }
    ret = write(c->fd, buf, size);
    if (ret > 0) {
        c->pos += ret;
        if (c->total_size < c->pos)
            c->total_size = c->pos;
    }
    if (ret > 0) {
        c->virtual_pos += ret;
        if (c->virtual_size < c->virtual_pos)
            c->virtual_size = c->virtual_pos;
    }
    return (ret == -1) ? AVERROR(errno) : ret;
}

static int file_get_handle(URLContext *h)
{
    FileContext *c = h->priv_data;
    return c->fd;
}

static int file_check(URLContext *h, int mask)
{
    int ret = 0;
    const char *filename = h->filename;
    av_strstart(filename, "file:", &filename);

    {
#if HAVE_ACCESS && defined(R_OK)
    if (access(filename, F_OK) < 0)
        return AVERROR(errno);
    if (mask&AVIO_FLAG_READ)
        if (access(filename, R_OK) >= 0)
            ret |= AVIO_FLAG_READ;
    if (mask&AVIO_FLAG_WRITE)
        if (access(filename, W_OK) >= 0)
            ret |= AVIO_FLAG_WRITE;
#else
    struct stat st;
#   ifndef _WIN32
    ret = stat(filename, &st);
#   else
    ret = win32_stat(filename, &st);
#   endif
    if (ret < 0)
        return AVERROR(errno);

    ret |= st.st_mode&S_IRUSR ? mask&AVIO_FLAG_READ  : 0;
    ret |= st.st_mode&S_IWUSR ? mask&AVIO_FLAG_WRITE : 0;
#endif
    }
    return ret;
}

static int file_delete(URLContext *h)
{
#if HAVE_UNISTD_H
    int ret;
    const char *filename = h->filename;
    av_strstart(filename, "file:", &filename);

    ret = rmdir(filename);
    if (ret < 0 && (errno == ENOTDIR
#   ifdef _WIN32
        || errno == EINVAL
#   endif
        ))
        ret = unlink(filename);
    if (ret < 0)
        return AVERROR(errno);

    return ret;
#else
    return AVERROR(ENOSYS);
#endif /* HAVE_UNISTD_H */
}

static int file_move(URLContext *h_src, URLContext *h_dst)
{
    const char *filename_src = h_src->filename;
    const char *filename_dst = h_dst->filename;
    av_strstart(filename_src, "file:", &filename_src);
    av_strstart(filename_dst, "file:", &filename_dst);
    // av_log(NULL, AV_LOG_WARNING, "Enter file_move(%s %s) \n", filename_src, filename_dst);

    if (rename(filename_src, filename_dst) < 0)
        return AVERROR(errno);

    return 0;
}

#if CONFIG_FILE_PROTOCOL

static int file_open(URLContext *h, const char *filename, int flags)
{
    FileContext *c = h->priv_data;
    int access;
    int fd;
    struct stat st;

    av_strstart(filename, "file:", &filename);

    c->is_mov_mp4 = is_mp4_mov_suffix(filename);

    if (flags & AVIO_FLAG_WRITE && flags & AVIO_FLAG_READ) {
        access = O_CREAT | O_RDWR;
        if (c->trunc)
            access |= O_TRUNC;
    } else if (flags & AVIO_FLAG_WRITE) {
        access = O_CREAT | O_WRONLY;
        if (c->trunc)
            access |= O_TRUNC;
    } else {
        access = O_RDONLY;
    }
#ifdef O_BINARY
    access |= O_BINARY;
#endif
    fd = avpriv_open(filename, access, 0666);
    if (fd == -1)
        return AVERROR(errno);
    c->fd = fd;
    h->is_streamed = !fstat(fd, &st) && S_ISFIFO(st.st_mode);

    read_fullfilepath(fd);
    if (c->is2slice) {
        c->final_fd = fd;
        fd = create_tmpfile("/tmp/sdcardXXXXXX");
        if (fd == -1)
            return AVERROR(errno);
        c->fd = fd;
    }
	if (c->isfalloc) {
		c->falloc_blk = 0;
	}
    /* Buffer writes more than the default 32k to improve throughput especially
     * with networked file systems */
    if (!h->is_streamed && flags & AVIO_FLAG_WRITE)
        h->min_packet_size = h->max_packet_size = 262144;

    if (c->seekable >= 0)
        h->is_streamed = !c->seekable;

    return 0;
}

/* XXX: use llseek */
static int64_t file_seek(URLContext *h, int64_t pos, int whence)
{
    FileContext *c = h->priv_data;
    int64_t ret;
    // av_log(h, AV_LOG_WARNING, "Enter file_seek(%ld %d) \n", pos, whence);

    if (whence == AVSEEK_SIZE) {
        struct stat st;
        ret = fstat(c->fd, &st);
        return ret < 0 ? AVERROR(errno) : (S_ISFIFO(st.st_mode) ? 0 : st.st_size);
    }
    if (SEEK_SET == whence) {
        c->virtual_pos = pos;
    } else if (SEEK_CUR == whence) {
        c->virtual_pos += pos;
    }
    if (c->is2slice) {
        if (c->virtual_pos > c->virtual_pos_base && c->virtual_pos_base > 0) {
            // av_log(h, AV_LOG_WARNING, "file_seek out-of-range (%ld %ld %ld %d)\n", c->virtual_pos, c->virtual_pos_base, pos, c->total_size);
            pos = c->virtual_pos - c->virtual_pos_base;
            whence = SEEK_SET;
        }
        else if (c->virtual_pos < c->virtual_pos_base)
        {
            // av_log(h, AV_LOG_WARNING, "file_seek back2past (%ld %ld %ld %d)\n", c->virtual_pos, c->virtual_pos_base, pos, c->total_size);
            return c->virtual_pos;
        }
    }
    ret = lseek(c->fd, pos, whence);
    if (ret >= 0){
        if (SEEK_SET == whence) {
            c->pos = pos;
        } else if (SEEK_CUR == whence) {
            c->pos += pos;
        }
    }

    return ret < 0 ? AVERROR(errno) : ret;
}

static void sanitize_mp4end(int target_fd)
{
    struct stat st;
    if (fstat(target_fd, &st) == 0)
    {
        off_t end_off =  lseek(target_fd, 0, SEEK_CUR);
        // file size larger than file end, sanitize it at most 8 bytes quickly
        if (st.st_size > end_off)
        {
            char buf[8] = "";
            int len = (st.st_size - end_off);
            len = (len > 8) ? 8 : len;
            write(target_fd, buf, len);
        }
    }
}

static int file_close(URLContext *h)
{
    FileContext *c = h->priv_data;
    if (c->is2slice) {
        int size, free_size;
        // ssize_t send_size = 0;
        char buf[8] = "abcdfree";
        size = lseek(c->fd, 0, SEEK_END);
        free_size = (size % FILE_SLICE_SIZE) ;
        if (free_size) {
            free_size = FILE_SLICE_SIZE - free_size;
            if (c->is_mov_mp4) {
                int free_nssize = htonl(free_size);
                memcpy(buf, &free_nssize, 4);
                write(c->fd, buf, 8);
            }
            ftruncate(c->fd, size + free_size);
        }
        lseek(c->fd, 0, SEEK_SET);
        pthread_mutex_lock(&file_mutex);
        if (size + free_size) {
            //send_size =
            sendfile(c->final_fd, c->fd, NULL, size + free_size);
        }
        pthread_mutex_unlock(&file_mutex);
        // av_log(h, AV_LOG_WARNING, "Enter file_close %lu (%d+%d)\n", send_size, size, free_size);
        if (c->is_mov_mp4) {
            sanitize_mp4end(c->final_fd);
        }
        close(c->final_fd);
    }
    else if (c->is_mov_mp4)
    {
        sanitize_mp4end(c->fd);
    }
	if (c->isfalloc) {
		ftruncate(c->fd, FILE_PREALLOC_SIZE*c->falloc_blk);
		c->falloc_blk = 0;
	}

    if (c->isfsync) {
        printf("Do fsync %s\n",h->filename);
        fsync(c->fd);
    }
    return close(c->fd);
}

static int file_open_dir(URLContext *h)
{
#if HAVE_LSTAT
    FileContext *c = h->priv_data;

    c->dir = opendir(h->filename);
    if (!c->dir)
        return AVERROR(errno);

    return 0;
#else
    return AVERROR(ENOSYS);
#endif /* HAVE_LSTAT */
}

static int file_read_dir(URLContext *h, AVIODirEntry **next)
{
#if HAVE_LSTAT
    FileContext *c = h->priv_data;
    struct dirent *dir;
    char *fullpath = NULL;

    *next = ff_alloc_dir_entry();
    if (!*next)
        return AVERROR(ENOMEM);
    do {
        errno = 0;
        dir = readdir(c->dir);
        if (!dir) {
            av_freep(next);
            return AVERROR(errno);
        }
    } while (!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, ".."));

    fullpath = av_append_path_component(h->filename, dir->d_name);
    if (fullpath) {
        struct stat st;
        if (!lstat(fullpath, &st)) {
            if (S_ISDIR(st.st_mode))
                (*next)->type = AVIO_ENTRY_DIRECTORY;
            else if (S_ISFIFO(st.st_mode))
                (*next)->type = AVIO_ENTRY_NAMED_PIPE;
            else if (S_ISCHR(st.st_mode))
                (*next)->type = AVIO_ENTRY_CHARACTER_DEVICE;
            else if (S_ISBLK(st.st_mode))
                (*next)->type = AVIO_ENTRY_BLOCK_DEVICE;
            else if (S_ISLNK(st.st_mode))
                (*next)->type = AVIO_ENTRY_SYMBOLIC_LINK;
            else if (S_ISSOCK(st.st_mode))
                (*next)->type = AVIO_ENTRY_SOCKET;
            else if (S_ISREG(st.st_mode))
                (*next)->type = AVIO_ENTRY_FILE;
            else
                (*next)->type = AVIO_ENTRY_UNKNOWN;

            (*next)->group_id = st.st_gid;
            (*next)->user_id = st.st_uid;
            (*next)->size = st.st_size;
            (*next)->filemode = st.st_mode & 0777;
            (*next)->modification_timestamp = INT64_C(1000000) * st.st_mtime;
            (*next)->access_timestamp =  INT64_C(1000000) * st.st_atime;
            (*next)->status_change_timestamp = INT64_C(1000000) * st.st_ctime;
        }
        av_free(fullpath);
    }

    (*next)->name = av_strdup(dir->d_name);
    return 0;
#else
    return AVERROR(ENOSYS);
#endif /* HAVE_LSTAT */
}

static int file_close_dir(URLContext *h)
{
#if HAVE_LSTAT
    FileContext *c = h->priv_data;
    closedir(c->dir);
    return 0;
#else
    return AVERROR(ENOSYS);
#endif /* HAVE_LSTAT */
}

const URLProtocol ff_file_protocol = {
    .name                = "file",
    .url_open            = file_open,
    .url_read            = file_read,
    .url_write           = file_write,
    .url_seek            = file_seek,
    .url_close           = file_close,
    .url_get_file_handle = file_get_handle,
    .url_check           = file_check,
    .url_delete          = file_delete,
    .url_move            = file_move,
    .priv_data_size      = sizeof(FileContext),
    .priv_data_class     = &file_class,
    .url_open_dir        = file_open_dir,
    .url_read_dir        = file_read_dir,
    .url_close_dir       = file_close_dir,
    .default_whitelist   = "file,crypto,data"
};

#endif /* CONFIG_FILE_PROTOCOL */

#if CONFIG_PIPE_PROTOCOL

static int pipe_open(URLContext *h, const char *filename, int flags)
{
    FileContext *c = h->priv_data;
    int fd;
    char *final;
    av_strstart(filename, "pipe:", &filename);

    fd = strtol(filename, &final, 10);
    if((filename == final) || *final ) {/* No digits found, or something like 10ab */
        if (flags & AVIO_FLAG_WRITE) {
            fd = 1;
        } else {
            fd = 0;
        }
    }
#if HAVE_SETMODE
    setmode(fd, O_BINARY);
#endif
    c->fd = fd;
    h->is_streamed = 1;
    return 0;
}

const URLProtocol ff_pipe_protocol = {
    .name                = "pipe",
    .url_open            = pipe_open,
    .url_read            = file_read,
    .url_write           = file_write,
    .url_get_file_handle = file_get_handle,
    .url_check           = file_check,
    .priv_data_size      = sizeof(FileContext),
    .priv_data_class     = &pipe_class,
    .default_whitelist   = "crypto,data"
};

#endif /* CONFIG_PIPE_PROTOCOL */
