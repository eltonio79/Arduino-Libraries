#ifndef SD_LIST_H
#define SD_LIST_H

#include <SdFat.h>

class SdList
{
public:
    SdList();

    bool chdir(const char* path);
    bool exists(const char* name);
    bool mkdir(const char* path);
    bool rmdir(const char* path);
    bool remove(const char* name);
    bool rename(const char *oldPath, const char *newPath);

    bool nextFile(char * name, bool * pIsF = NULL, uint32_t * pSize = NULL,
        uint16_t * pCreateDate = NULL, uint16_t * pCreateTime = NULL,
        uint16_t * pLastWriteDate = NULL, uint16_t * pLastWriteTime = NULL);
    bool openFile(SdFile * pFile, const char* name, uint8_t oflag);

    float capacity();
    float free();
};

#endif // SD_LIST_H