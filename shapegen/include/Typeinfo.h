#include <filesystem>
#include <fstream>
#include <span>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <map>

enum typeflags {
  fixed = 0x00001,
  solid = 0x00002,
  sea = 0x00004,
  land = 0x00008,
  occluded = 0x00010,
  bag = 0x00020,
  damaging = 0x00040,
  noisy = 0x00080,
  draw = 0x00100,
  ignore = 0x00200,
  roof = 0x00400,
  transl = 0x00800,
  editor = 0x01000,
  selectable = 0x02000,
  preload = 0x04000,
  sound = 0x08000,
  targetable = 0x10000,
  npc = 0x20000,
  unk66 = 0x40000,
  unk67 = 0x80000
};

std::map<size_t, std::string> knownFlags = {
  { fixed, "fixed" },
  { solid, "solid" },
  { sea, "sea" },
  { land, "land" },
  { occluded, "occluded" },
  { bag, "bag" },
  { damaging, "damaging" },
  { noisy, "noisy" },
  { draw, "draw" },
  { ignore, "ignore" },
  { roof, "roof" },
  { transl, "transl" },
  { editor, "editor" },
  { selectable, "selectable" },
  { preload, "preload" },
  { sound, "sound" },
  { targetable, "targetable" },
  { npc, "npc" },
  { unk66, "unk66" },
  { unk67, "unk67" },
};

struct Typeinfo {
  Typeinfo(const uint8_t*p) {
    flags = (((p[0]) | (p[1] << 8)) & 0xFFF) | (p[6] << 12);
    family = ((p[1] >> 4) | (p[2] << 4)) & 0x1F;
    equip = (p[2] >> 1) & 0xF;
    x = ((p[2] >> 5) | (p[3] << 3)) & 0x1F;
    y = (p[3] >> 2) & 0x1F;
    z = ((p[3] >> 7) | (p[4] << 1)) & 0x1F;
    animtype = (p[4] >> 4) & 0xF;
    animdata = (p[5]) & 0xF;
    animSpeed = (p[5] >> 4) & 0xF;
    if (animtype && not animSpeed) animSpeed++;
    weight = p[7];
    volume = p[8];
  }
  void print() {
    printf("%4zu %u %u (%u %u %u) (%u %u %u) %u %u (", n, family, equip, x, y, z, animtype, animdata, animSpeed, weight, volume);
    for (auto& [flag, name] : knownFlags) {
      if (flags & flag) printf("%s ", name.c_str());
    }
    printf(")\n");
  }
  uint32_t flags;
  uint8_t family;
  uint8_t equip;
  uint8_t x;
  uint8_t y;
  uint8_t z;
  uint8_t animtype;
  uint8_t animdata;
  uint8_t animSpeed;
  uint8_t weight;
  uint8_t volume;
};

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
  }
}
