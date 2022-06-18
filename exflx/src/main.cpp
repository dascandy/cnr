#include <filesystem>
#include <fstream>
#include <span>
#include <vector>
#include <cstdint>

struct Header {
  char tag[84];
  uint32_t fileCount;
  uint32_t one;
  uint32_t fileSize;
  uint32_t pad[8];
};

struct FileEntry {
  uint32_t offset;
  uint32_t size;
};

int main(int, const char** argv) {
  std::vector<uint8_t> data;
  data.resize(std::filesystem::file_size(argv[1]));
  std::ifstream(argv[1]).read(reinterpret_cast<char*>(data.data()), data.size());
  Header* header = reinterpret_cast<Header*>(data.data());
  FileEntry* firstEntry = reinterpret_cast<FileEntry*>(header+1);
  std::span<FileEntry> entries{firstEntry, firstEntry + header->fileCount};
  size_t index = 0;
  for (auto& entry : entries) {
    if (entry.offset == 0) continue;
    const char* filedata = reinterpret_cast<const char*>(data.data() + entry.offset);
    std::ofstream(argv[1] + std::string(".") + std::to_string(index)).write(filedata, entry.size);
    index++;
  }
}
