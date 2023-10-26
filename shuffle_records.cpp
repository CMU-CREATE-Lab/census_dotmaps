#include <iostream>
#include <fstream>
#include <random>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

const size_t recordSize = 16;

int main(int argc, char *argv[]) {
    assert(sizeof(__int128_t) == recordSize);
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <file>\n";
        return 1;
    }

    int fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }

    struct stat fileInfo = {0};
    if (fstat(fd, &fileInfo) == -1) {
        perror("Error getting the file size");
        exit(EXIT_FAILURE);
    }

    if (fileInfo.st_size % recordSize != 0) {
        std::cerr << "File size is not a multiple of record size\n";
        exit(EXIT_FAILURE);
    }

    __int128_t *map = static_cast<__int128_t*>(mmap(0, fileInfo.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
    assert (sizeof(map[0]) == recordSize);
    if (map == MAP_FAILED) {
        close(fd);
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }

    size_t numRecords = fileInfo.st_size / recordSize;

    // Create a 64-bit Mersenne Twister random number generator
    std::random_device rd;
    std::mt19937_64 gen(rd());

    // Fisher-Yates shuffle
    for (size_t i = numRecords - 1; i > 0; --i) {
        // Every 5% progress
        if (i % (numRecords / 20) == 0) {
            std::cout << "Shuffling progress: " << 100 - (i * 100 / numRecords) << "%\n";
        }

        std::uniform_int_distribution<> dis(0, i);
        size_t j = dis(gen);

        // Swap records i and j
        __int128_t temp = map[i];
        map[i] = map[j];
        map[j] = temp;
    }

    if (munmap(map, fileInfo.st_size) == -1) {
        close(fd);
        perror("Error un-mmapping the file");
        exit(EXIT_FAILURE);
    }

    close(fd);
    return 0;
}
