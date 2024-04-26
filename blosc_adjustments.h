#include <iostream>
#include <filesystem>
#include <vector>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include "blosc2.h"

struct MMappedFile {
    char* addr = NULL;
    std::uintmax_t size;
    int64_t offset;
    int fd;
};

static MMappedFile mmap_file;

void* test_open(const char *urlpath, const char *mode, void *params) {
    if (mmap_file.addr != NULL) {
        // We only open the file once
        return &mmap_file;
    }

    // TODO: Which file size should we use for creating a new file? Must be > 0
    mmap_file.size = std::filesystem::exists(urlpath) ? std::filesystem::file_size(urlpath) : 1;
    mmap_file.offset = 0;

    int prot;
    int flags;
    int flags_open;
    if (strstr(mode, "w") != NULL || strstr(mode, "a") != NULL) {
        prot = PROT_WRITE;
        flags = MAP_SHARED;
        flags_open = O_RDWR | O_CREAT;
    }
    else {
        prot = PROT_READ;
        flags = MAP_PRIVATE;
        flags_open = O_RDONLY;
    }

    int fd = open(urlpath, flags_open);
    if (fd < 0) {
        fprintf(stderr, "Could not open the file.  Error code: %d\n", errno);
        return NULL;
    }

    mmap_file.fd = fd;
    mmap_file.addr = (char*)mmap(NULL, mmap_file.size, prot, flags, mmap_file.fd, 0);
    if (mmap_file.addr == MAP_FAILED) {
        fprintf(stderr, "Could not create the memory mapping.  Error code: %d\n", errno);
        close(mmap_file.fd);
        return NULL;
    }
    else {
        // // Does not seem to have any effect?
        // auto success = madvise(mmap_file.addr, mmap_file.size, MADV_WILLNEED);
        // if (success < 0) {
        //     fprintf(stderr, "Error while advising.  Error code: %d\n%d\n", success, errno);
        //     return NULL;
        // }

        return &mmap_file;
    } 
}

int test_close(void *stream) {
    return 0;
}

int64_t test_tell(void *stream) {
    return mmap_file.offset;
}

int test_seek(void *stream, int64_t offset, int whence) {
    mmap_file.offset = offset;
    return 0;
}

int64_t test_write(const void *ptr, int64_t size, int64_t nitems, void *stream) {
    if (mmap_file.size < mmap_file.offset + size * nitems) {
        int rt = ftruncate(mmap_file.fd, mmap_file.offset + size * nitems);
        if (rt < 0) {
            fprintf(stderr, "Error while truncating the file.  Error code: %d\n", errno);
            return rt;
        }
    }
    
    memcpy(mmap_file.addr + mmap_file.offset, ptr, size * nitems);
    msync(mmap_file.addr + mmap_file.offset, size * nitems, MS_SYNC);
    mmap_file.offset += size * nitems;
    return nitems;
}

int64_t test_read(void *ptr, int64_t size, int64_t nitems, void *stream) {
    memcpy(ptr, mmap_file.addr + mmap_file.offset, size * nitems);
    return nitems;
}

int test_truncate(void *stream, int64_t size) {
    mmap_file.size = size;
    return ftruncate(mmap_file.fd, size);
}

void read_chunks_crop(blosc2_schunk* schunk, float* data) {
    // Read random crop consisting of 27 chunks (similar to the slice reading in main.py in terms of number of chunks)
    std::vector<int> start_chunks = {0, 40, 100, 150, 220, 300, 320, 380, 450};
    for (int nchunk : start_chunks) {
        for (int i = 0; i < 3; i++) {
            auto dsize = blosc2_schunk_decompress_chunk(schunk, nchunk+i, data, schunk->chunksize);
            if (dsize < 0) {
                fprintf(stderr, "Decompression error.  Error code: %d\n", dsize);
            }
        }
    }
}

void read_chunks_sequential(blosc2_schunk* schunk, float* data) {
    for (int nchunk = 0; nchunk < schunk->nchunks; nchunk++) {
        auto dsize = blosc2_schunk_decompress_chunk(schunk, nchunk, data, schunk->chunksize);
        if (dsize < 0) {
            fprintf(stderr, "Decompression error.  Error code: %d\n", dsize);
        }
    }
}