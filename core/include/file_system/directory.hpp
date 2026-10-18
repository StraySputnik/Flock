#pragma once

#include "common.hpp"
#include "path.hpp"

namespace Flock::FileSystem {
  struct FLK_API DirectoryEntry {
    Filepath path         = {};
    bool     is_directory = false;
  };

  class FLK_API Directory {
  public:
    [[nodiscard]] static bool                       exists(const Filepath &filepath);
    static bool                                     create(const Filepath &filepath);
    [[nodiscard]] static Maybe<Vec<DirectoryEntry>> list(const Filepath &filepath, bool recursive = false);
    [[nodiscard]] static Maybe<Vec<Filepath>>       list_files(const Filepath &filepath, bool recursive = false);
  };
}
