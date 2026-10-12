#include "file/file.hpp"

namespace Flock::File {
  Maybe<File> File::open(ModeFlag mode) {
    File file{};
  }

  File::~File() {
  }

  void File::release() {
  }

  void File::close() {
  }

  void File::write_str(StringSlice slice) {
  }

  String File::read_str() {
  }

  void File::write_bin(void *ptr, usize size) {
  }

  void File::read_bin(void *ptr, usize size) {
  }
}
