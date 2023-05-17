#include <sys/mman.h>
#include <stdio.h>
#include <assert.h>
#include <string>
#include <vector>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef struct sample {
    float x;
    float y;
    int blockIdx;
    int subIdx;
} Sample;

int main(int argc, char **argv) {
    std::string dir(argv[1]);
    int z = atoi(argv[2]);
    double subsample = atof(argv[3]);
    assert(sizeof(int) == 4);
    assert(sizeof(float) == 4);
    assert(sizeof(Sample) == 16);
    std::string master_filename = dir + "/prototile_all_dots_shuffled.bin";
    FILE *master_file = fopen(master_filename.c_str(), "rb");
    if (!master_file) {
        printf("Couldn't open %s\n", master_filename.c_str());
        return 1;
    }
    fseek(master_file, 0, SEEK_END);
    unsigned long master_len = ftell(master_file);
    unsigned long num_records = master_len / sizeof(Sample);
    assert(num_records * sizeof(Sample) == master_len);
    Sample *master = (Sample*)mmap(NULL, master_len, PROT_READ, MAP_SHARED, fileno(master_file), 0);
    printf("Mapped %.06fM records from %s at %lx\n", num_records/1e6, master_filename.c_str(), (long)master);
    std::string tile_dir = dir + "/" + std::to_string(z);
    printf("Creating %s\n", tile_dir.c_str());

    int dim = 1 << z;

    std::vector<std::vector<Sample>> tiles(dim*dim);

    if (subsample < 1.0) {
        num_records = (unsigned long)round(num_records * subsample);
        printf("Subsampling by %.8g yields %.06fM records\n", subsample, num_records/1e6);
    }                 
    for (unsigned long i = 0; i < num_records; i++) {
        Sample &record = master[i];
        int xtile = (int)(record.x * dim / 256);
        int ytile = (int)(record.y * dim / 256);
        tiles[ytile*dim + xtile].push_back(record);
    }
    mkdir(tile_dir.c_str(), 0774);
    for (int y = 0; y < dim; y++) {
        for (int x = 0; x < dim; x++) {
            auto &tile = tiles[y*dim + x];
            mkdir((tile_dir + "/" + std::to_string(x)).c_str(), 0774);
            std::string dest = tile_dir + "/" + std::to_string(x) + "/" + std::to_string(y) + ".bin";
            FILE *out = fopen(dest.c_str(), "wb");
            fwrite(&tile[0], sizeof(Sample), tile.size(), out);
            fclose(out);
            if (tile.size()) {
                printf("Wrote %.06fM records to %s\n", tile.size()/1e6, dest.c_str());
            }
        }
    }
    printf("Wrote a total of %d tiles\n", dim * dim);
    return 0;
}
