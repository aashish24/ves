/*========================================================================
  VES --- VTK OpenGL ES Rendering Toolkit

      http://www.kitware.com/ves

  Copyright 2011 Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 ========================================================================*/

#include "vesKiwiArchiveUtils.h"

//libarchive includes
#include <sys/types.h>
#include <sys/stat.h>
#include <archive.h>
#include <archive_entry.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


namespace {

int copy_data(struct archive *ar, struct archive *aw)
{
  int r;
  const void *buff;
  size_t size;
  int64_t offset;

  for (;;) {
    r = archive_read_data_block(ar, &buff, &size, &offset);
    if (r == ARCHIVE_EOF)
      return (ARCHIVE_OK);
    if (r != ARCHIVE_OK)
      return (r);
    r = archive_write_data_block(aw, buff, size, offset);
    if (r != ARCHIVE_OK) {
      fprintf(stderr, "%s\n", archive_error_string(aw));
      return (r);
    }
  }
}

}

vesKiwiArchiveUtils::vesKiwiArchiveUtils()
{

}

vesKiwiArchiveUtils::~vesKiwiArchiveUtils()
{

}

bool vesKiwiArchiveUtils::extractArchive(const std::string& filename, const std::string& destDir)
{
  this->mEntries.clear();


  struct archive *a;
  struct archive *ext;
  struct archive_entry *entry;
  int flags;
  int r;

  flags = ARCHIVE_EXTRACT_TIME;
  flags |= ARCHIVE_EXTRACT_PERM;
  flags |= ARCHIVE_EXTRACT_ACL;
  flags |= ARCHIVE_EXTRACT_FFLAGS;

  a = archive_read_new();
  archive_read_support_format_all(a);
  archive_read_support_compression_all(a);
  ext = archive_write_disk_new();
  archive_write_disk_set_options(ext, flags);
  archive_write_disk_set_standard_lookup(ext);

  if ((r = archive_read_open_file(a, filename.c_str(), 10240))) {
    this->setError("Error Opening File", "Failed to open file: " + filename);
    return false;
  }

  for (;;) {
    r = archive_read_next_header(a, &entry);
    if (r == ARCHIVE_EOF)
      break;
    if (r != ARCHIVE_OK)
      fprintf(stderr, "%s\n", archive_error_string(a));
    if (r < ARCHIVE_WARN) {
      this->setError("Error Reading Archive", archive_error_string(a));
      return false;
    }

    std::string destPath = archive_entry_pathname(entry);

    //printf("entry: %s\n", destPath.c_str());

    if (destDir.size()) {
      destPath = destDir + "/" + destPath;
    }

    archive_entry_set_pathname(entry, destPath.c_str());

    r = archive_write_header(ext, entry);
    if (r != ARCHIVE_OK)
      fprintf(stderr, "%s\n", archive_error_string(ext));
    else if (archive_entry_size(entry) > 0) {
      copy_data(a, ext);
      if (r != ARCHIVE_OK)
        fprintf(stderr, "%s\n", archive_error_string(ext));
      if (r < ARCHIVE_WARN) {
        this->setError("Error Reading Archive", archive_error_string(ext));
        return false;
      }

    }
    r = archive_write_finish_entry(ext);
    if (r != ARCHIVE_OK)
      fprintf(stderr, "%s\n", archive_error_string(ext));
    if (r < ARCHIVE_WARN) {
      this->setError("Error Reading Archive", archive_error_string(ext));
      return false;
    }

    this->mEntries.push_back(destPath);
  }

  archive_read_close(a);
  archive_read_free(a);
  archive_write_close(ext);
  archive_write_free(ext);
  return true;

}

const std::vector<std::string>& vesKiwiArchiveUtils::entries() const
{
  return this->mEntries;
}
