#include "blosc_adjustments.h"

int benchmark_default(const char* file_path) {
    blosc2_init();
    blosc2_set_nthreads(1);

    blosc2_schunk* schunk = blosc2_schunk_open(file_path);
    float* data = (float*)malloc(schunk->chunksize);

    read_chunks_crop(schunk, data);

    free(data);
    blosc2_schunk_free(schunk);
    blosc2_destroy();
    return 0;
}

int benchmark_mmap(const char* file_path) {
    blosc2_init();
    blosc2_set_nthreads(1);

    blosc2_io_cb io_cb;
    io_cb.id = 245;
    io_cb.open = (blosc2_open_cb) test_open;
    io_cb.close = (blosc2_close_cb) test_close;
    io_cb.read = (blosc2_read_cb) test_read;
    io_cb.tell = (blosc2_tell_cb) test_tell;
    io_cb.seek = (blosc2_seek_cb) test_seek;
    io_cb.write = (blosc2_write_cb) test_write;
    io_cb.truncate = (blosc2_truncate_cb) test_truncate;

    auto success = blosc2_register_io_cb(&io_cb);
    if (success < 0) {
        fprintf(stderr, "Error registering the IO callback. Error code: %d\n", success);
        return success;
    }

    blosc2_io io = {.id = 245, .params = &io_cb};
    
    blosc2_schunk* schunk = blosc2_schunk_open_udio(file_path, &io);
    float* data = (float*)malloc(schunk->chunksize);

    read_chunks_crop(schunk, data);

    if (munmap(mmap_file.addr, mmap_file.size) == -1) {
        std::cout << "Error un-mmapping the file" << std::endl;
    }
    close(mmap_file.fd);
    free(data);
    blosc2_schunk_free(schunk);
    blosc2_destroy();
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " default|mmap FILE" << std::endl;
        return 1;
    }

    std::string file_path = "/workspaces/blosc_mmap/" ;
    file_path += argv[2];

    if (std::string(argv[1]) == "default") {
        return benchmark_default(file_path.c_str());
    }
    else if (std::string(argv[1]) == "mmap") {
        return benchmark_mmap(file_path.c_str());
    }
    else {
        std::cout << "Unknown option " << argv[1] << std::endl;
        return 1;
    }
}
