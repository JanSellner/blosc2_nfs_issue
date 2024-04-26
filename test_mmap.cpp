#include "blosc_adjustments.h"
#include <cassert>

// Check correctnes of the mmap implementation
int main(int argc, char *argv[]) {
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
    assert(success == 0);

    blosc2_cparams cparams = BLOSC2_CPARAMS_DEFAULTS;
    cparams.typesize = sizeof(float);
    cparams.compcode = BLOSC_BLOSCLZ;
    cparams.clevel = 9;
    cparams.nthreads = 1;

    blosc2_io io = {.id = 245, .params = &io_cb};
    std::string test_file_path = "/workspaces/blosc_mmap/test_files/test_simple.b2nd";
    if (std::filesystem::exists(test_file_path)) {
        std::filesystem::remove(test_file_path);
    }
    blosc2_storage storage = {.contiguous=true, .urlpath=(char*)test_file_path.c_str(), .cparams=&cparams, .dparams=NULL, .io=&io};
    blosc2_schunk *schunk_write = blosc2_schunk_new(&storage);

    float data_buffer[2] = {0.1, 0.2};
    int64_t cbytes = blosc2_schunk_append_buffer(schunk_write, data_buffer, 8);
    assert(cbytes > 0);

    float data_buffer2[2] = {0.3, 0.4};
    cbytes = blosc2_schunk_append_buffer(schunk_write, data_buffer2, 8);
    assert(cbytes > 0);
    
    // Read the data back again
    blosc2_schunk* schunk_read = blosc2_schunk_open_udio(storage.urlpath, &io);
    assert(schunk_read->nchunks == 2);

    float* data = (float*)malloc(schunk_read->chunksize);
    int dsize = blosc2_schunk_decompress_chunk(schunk_read, 0, data, schunk_read->chunksize);
    assert(dsize == 8);
    assert(std::abs(data[0] - 0.1) < 1e-6);
    assert(std::abs(data[1] - 0.2) < 1e-6);

    dsize = blosc2_schunk_decompress_chunk(schunk_read, 1, data, schunk_read->chunksize);
    assert(dsize == 8);
    assert(std::abs(data[0] - 0.3) < 1e-6);
    assert(std::abs(data[1] - 0.4) < 1e-6);

    if (munmap(mmap_file.addr, mmap_file.size) == -1) {
        std::cout << "Error un-mmapping the file" << std::endl;
    }
    close(mmap_file.fd);
    free(data);
    blosc2_schunk_free(schunk_write);
    blosc2_schunk_free(schunk_read);
    blosc2_destroy();
    return 0;
}
