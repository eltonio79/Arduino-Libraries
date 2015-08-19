#include "SdList.h"

extern SdFat sd;

SdList::SdList()
{
}

bool SdList::chdir(const char* path)
{
    return sd.chdir(path, true); //, set_cwd );
}

// Work only with name, not path
// Must call SdList::chdir( path ) before

bool SdList::exists(const char* name)
{
    return sd.exists(name);
}

// Work only with name, not path
// Must call SdList::exists( name ) before

bool SdList::remove(const char* name)
{
    return sd.remove(name);
}

bool SdList::mkdir(const char* path)
{
    return sd.mkdir(path);
}

// Work only with relative path, not full path
// Must call SdList::exists( path ) before

bool SdList::rmdir(const char* path)
{
    return sd.chdir(path) && sd.vwd()->rmRfStar();
}

bool SdList::rename(const char* oldPath, const char* newPath)
{
    return sd.rename(oldPath, newPath);
}

bool SdList::nextFile(char* name, bool* pIsF, uint32_t* pSize,
    uint16_t* pCreateDate, uint16_t* pCreateTime,
    uint16_t* pLastWriteDate, uint16_t* pLastWriteTime)
{
    SdFile file;
    dir_t dir;

    bool ok = file.openNext(sd.vwd(), O_READ);
    if (ok)
    {
        file.getFilename(name);
        file.dirEntry(&dir);
        if (pIsF != NULL)
            *pIsF = file.isFile();
        if (pSize != NULL)
            *pSize = file.fileSize();
        if (pCreateDate != NULL)
            *pCreateDate = dir.creationDate;
        if (pCreateTime != NULL)
            *pCreateTime = dir.creationTime;
        if (pLastWriteDate != NULL)
            *pLastWriteDate = dir.lastWriteDate;
        if (pLastWriteTime != NULL)
            *pLastWriteTime = dir.lastWriteTime;
        file.close();
    }
    return ok;
}

bool SdList::openFile(SdFile* pFile, const char* name, uint8_t oflag)
{
    return pFile->open(name, oflag);                // file opened by its short name
}

// return the capacity in Megabytes of the SD card

float SdList::capacity()
{
    Sd2Card* p_card = sd.card();
    return 0.000512 * p_card->cardSize();
}

// return the amount of free space in Megabytes in the SD card

float SdList::free()
{
    SdVolume* p_vol = sd.vol();
    return 0.000512 * p_vol->freeClusterCount() * p_vol->blocksPerCluster();
}