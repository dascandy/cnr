#include <filesystem>
#include <fstream>
#include <span>
#include <vector>
#include <cstdint>
#include <unordered_map>

struct [[gnu::packed]] Entry {
  uint16_t x;
  uint16_t y;
  uint8_t z;
  uint16_t type;
  uint8_t frame;
  uint16_t flags;
  uint16_t count;
  uint8_t npcIndex;
  uint8_t mapIndex;
  uint16_t nextObj;
};

struct [[gnu::packed]] GlobEntry {
  uint8_t x;
  uint8_t y;
  uint8_t z;
  uint16_t shapeindex;
  uint8_t frame;
};

static_assert(sizeof(Entry) == 16);

int main(int argc, const char** argv) {
  std::vector<std::vector<uint8_t>> globs;
  globs.resize(3072);
  for (size_t n = 0; n < 3072; n++) {
    globs[n].resize(std::filesystem::file_size("glob.flx." + std::to_string(n)));
    std::ifstream("glob.flx." + std::to_string(n)).read(reinterpret_cast<char*>(globs[n].data()), globs[n].size());
  }
  std::unordered_map<uint32_t, size_t> counts;
  for (size_t n = 1; n < static_cast<size_t>(argc); n++) {
    std::vector<uint8_t> data;
    data.resize(std::filesystem::file_size(argv[n]));
    std::ifstream(argv[n]).read(reinterpret_cast<char*>(data.data()), data.size());
    Entry* firstEntry = reinterpret_cast<Entry*>(data.data());
    std::span<Entry> entries{firstEntry, firstEntry + data.size() / sizeof(Entry)};
    for (auto& entry : entries) {
      switch(entry.type) {
        case 1003:
        case 1002:
        case 1001:
        printf("%s\n", argv[n]);
      }
      if (entry.type == 0x10) {
        if (entry.count >= globs.size()) {
          printf("invalid glob id %u\n", entry.count);
        } else {
          std::vector<uint8_t>& gv = globs[entry.count];
          std::span<GlobEntry> ge{reinterpret_cast<GlobEntry*>(gv.data() + 2), reinterpret_cast<GlobEntry*>(gv.data() + gv.size())};
          for (auto& e : ge) {
            counts[(e.shapeindex << 8) | e.frame]++;
          }
        }
      } else {
        counts[(entry.type << 8) | entry.frame]++;
      }
    }
  }
  for (auto& [id, count] : counts) {
    printf("%zu: %u/%u\n", count, (id >> 8), (id & 0xFF));
  }
}
