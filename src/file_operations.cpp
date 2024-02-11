#include "file_operations.h"
#include <fstream>
#include <spdlog/spdlog.h>

namespace haru {
cpp::result<void, Error> write_cmake_lists(const std::filesystem::path& workpath, const std::string& contents) {
  auto filepath = workpath;
  filepath += "/CMakeLists.txt";
  std::ofstream output(filepath);
  output << contents;
  if (output.rdstate() & (std::ofstream::failbit | std::ofstream::badbit))
    return cpp::fail(Error(Error::Write, "Failed writing CMakeLists.txt"));

  spdlog::info("Wrote {}", filepath.string());
  return {};
}
}// namespace haru
