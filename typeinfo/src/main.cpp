#include <filesystem>
#include <fstream>
#include <span>
#include <vector>
#include <cstdint>
#include <unordered_map>

int main(int, const char** argv) {
  std::vector<uint8_t> data;
  data.resize(std::filesystem::file_size(argv[1]));
  std::ifstream(argv[1]).read(reinterpret_cast<char*>(data.data()), data.size());
  for (size_t n = 0; n < 2048; n++) {
    uint8_t* p = data.data() + n * 9;
    uint16_t flags = (((p[0]) | (p[1] << 8)) & 0xFFF) | (p[6] << 12);
    uint8_t family = ((p[1] >> 4) | (p[2] << 4)) & 0x1F;
    uint8_t equip = (p[2] >> 1) & 0xF;
    uint8_t x = ((p[2] >> 5) | (p[3] << 3)) & 0x1F;
    uint8_t y = (p[3] >> 2) & 0x1F;
    uint8_t z = ((p[3] >> 7) | (p[4] << 1)) & 0x1F;
    uint8_t animtype = (p[4] >> 4) & 0xF;
    uint8_t animdata = (p[5]) & 0xF;
    uint8_t animSpeed = (p[5] >> 4) & 0xF;
    if (animtype && not animSpeed) animSpeed++;
    uint8_t weight = p[7];
    uint8_t volume = p[8];
    printf("%05x %u %u (%u %u %u) (%u %u %u) %u %u\n", flags, family, equip, x, y, z, animtype, animdata, animSpeed, weight, volume);
  }
}
