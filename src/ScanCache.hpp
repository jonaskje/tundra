#ifndef SCANCACHE_HPP
#define SCANCACHE_HPP

#include "Common.hpp"
#include "Hash.hpp"
#include "ReadWriteLock.hpp"

namespace t2
{
  struct ScanData;
  struct MemAllocHeap;
  struct MemAllocLinear;
  struct MemoryMappedFile;

  void ComputeScanCacheKey(
      HashDigest*        key_out,
      const char*        filename,
      const HashDigest&  scanner_hash);

  struct ScanCacheLookupResult
  {
    int           m_IncludedFileCount;
    const char**  m_IncludedFiles;
  };

  struct ScanCache
  {
    struct Record
    {
      HashDigest   m_Key;
      uint64_t     m_FileTimestamp;
      int          m_IncludeCount;
      const char** m_Includes;
      Record*      m_Next;
    };

    const ScanData* m_FrozenData;

    ReadWriteLock   m_Lock;
    MemAllocHeap*   m_Heap;
    MemAllocLinear* m_Allocator;
    uint32_t        m_RecordCount;
    uint32_t        m_TableSize;
    Record**        m_Table;

    // Table of bits to track whether frozen records have been accessed.
    uint8_t*        m_FrozenAccess;
  };
    
  void ScanCacheInit(ScanCache* self, MemAllocHeap* heap, MemAllocLinear* allocator);

  void ScanCacheSetCache(ScanCache* self, const ScanData* frozen_data);

  void ScanCacheDestroy(ScanCache* self);

  bool ScanCacheLookup(ScanCache* self, const HashDigest& key, uint64_t timestamp, ScanCacheLookupResult* result_out, MemAllocLinear* scratch);

  void ScanCacheInsert(ScanCache* self, const HashDigest& key, uint64_t timestamp, const char** included_files, int count);

  bool ScanCacheDirty(ScanCache* self);

  bool ScanCacheSave(ScanCache* self, const char* fn, MemoryMappedFile* prev_mapping, MemAllocHeap* heap);

}


#endif
