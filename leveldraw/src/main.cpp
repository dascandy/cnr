#include <filesystem>
#include <fstream>
#include <span>
#include <vector>
#include <cstdint>
#include <unordered_map>

struct [[gnu::packed]] ShpHeader {
  uint16_t maxX;
  uint16_t maxY;
  uint16_t count;
};

struct [[gnu::packed]] FrameHeader {
  uint32_t frameOffset; // ignore top bit
  uint32_t framesize;
};

struct [[gnu::packed]] FrameData {
  uint16_t imageId;
  uint16_t frameId;
  uint32_t absoluteOffset;
  uint32_t compression;
  uint32_t width;
  uint32_t height;
  int32_t offx;
  int32_t offy;
  uint32_t rowOffsets[1];
};

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

struct Color {
  uint8_t r, g, b;
};

Color palette[256] = {
  { 0x00, 0x00, 0x00 }, { 0x3f, 0x3f, 0x2f }, { 0x3e, 0x3c, 0x08 }, { 0x3f, 0x32, 0x07 },
  { 0x3f, 0x29, 0x05 }, { 0x3f, 0x1f, 0x03 }, { 0x3f, 0x16, 0x02 }, { 0x3f, 0x0c, 0x00 },
  { 0x00, 0x00, 0x00 }, { 0x3f, 0x3f, 0x11 }, { 0x3f, 0x15, 0x14 }, { 0x13, 0x29, 0x14 },
  { 0x00, 0x1d, 0x2f }, { 0x3f, 0x3f, 0x33 }, { 0x3f, 0x3f, 0x32 }, { 0x3f, 0x3f, 0x35 },
  { 0x3f, 0x3f, 0x3f }, { 0x3a, 0x3a, 0x3a }, { 0x35, 0x35, 0x35 }, { 0x30, 0x30, 0x30 },
  { 0x2b, 0x2b, 0x2b }, { 0x26, 0x26, 0x26 }, { 0x22, 0x22, 0x22 }, { 0x1d, 0x1d, 0x1d },
  { 0x19, 0x19, 0x19 }, { 0x16, 0x16, 0x16 }, { 0x12, 0x12, 0x12 }, { 0x0e, 0x0e, 0x0e },
  { 0x0b, 0x0b, 0x0b }, { 0x07, 0x07, 0x07 }, { 0x03, 0x03, 0x03 }, { 0x00, 0x00, 0x00 },
  { 0x35, 0x1b, 0x11 }, { 0x2e, 0x19, 0x11 }, { 0x27, 0x17, 0x11 }, { 0x21, 0x15, 0x12 },
  { 0x1a, 0x13, 0x12 }, { 0x13, 0x10, 0x13 }, { 0x0d, 0x0e, 0x13 }, { 0x06, 0x0c, 0x13 },
  { 0x36, 0x2d, 0x13 }, { 0x31, 0x29, 0x13 }, { 0x2d, 0x24, 0x14 }, { 0x28, 0x20, 0x15 },
  { 0x23, 0x1c, 0x15 }, { 0x1f, 0x18, 0x16 }, { 0x1a, 0x14, 0x17 }, { 0x15, 0x0f, 0x17 },
  { 0x36, 0x36, 0x3a }, { 0x31, 0x32, 0x37 }, { 0x2d, 0x2f, 0x33 }, { 0x29, 0x2b, 0x30 },
  { 0x24, 0x27, 0x2d }, { 0x20, 0x23, 0x29 }, { 0x1b, 0x20, 0x26 }, { 0x17, 0x1c, 0x23 },
  { 0x13, 0x18, 0x1f }, { 0x10, 0x15, 0x1b }, { 0x0d, 0x11, 0x16 }, { 0x0b, 0x0e, 0x12 },
  { 0x08, 0x0b, 0x0e }, { 0x05, 0x07, 0x09 }, { 0x03, 0x04, 0x05 }, { 0x3e, 0x3c, 0x08 },
  { 0x00, 0x18, 0x37 }, { 0x00, 0x0c, 0x1c }, { 0x3e, 0x38, 0x08 }, { 0x22, 0x08, 0x02 },
  { 0x00, 0x2d, 0x03 }, { 0x00, 0x1b, 0x02 }, { 0x3f, 0x35, 0x07 }, { 0x3f, 0x31, 0x06 },
  { 0x2d, 0x2c, 0x22 }, { 0x28, 0x27, 0x1d }, { 0x23, 0x22, 0x18 }, { 0x1d, 0x1d, 0x12 },
  { 0x17, 0x17, 0x0f }, { 0x11, 0x11, 0x0b }, { 0x0a, 0x0a, 0x07 }, { 0x05, 0x05, 0x03 },
  { 0x36, 0x36, 0x36 }, { 0x35, 0x32, 0x32 }, { 0x32, 0x2e, 0x2e }, { 0x2e, 0x2a, 0x2a },
  { 0x2a, 0x26, 0x26 }, { 0x27, 0x22, 0x22 }, { 0x23, 0x1e, 0x1e }, { 0x1f, 0x1a, 0x1a },
  { 0x1d, 0x17, 0x17 }, { 0x1c, 0x14, 0x14 }, { 0x1a, 0x11, 0x11 }, { 0x18, 0x0e, 0x0e },
  { 0x13, 0x0a, 0x0a }, { 0x0e, 0x07, 0x07 }, { 0x09, 0x04, 0x04 }, { 0x3f, 0x2d, 0x06 },
  { 0x3f, 0x3f, 0x3a }, { 0x3f, 0x3b, 0x32 }, { 0x3f, 0x2a, 0x05 }, { 0x3f, 0x26, 0x04 },
  { 0x3f, 0x2f, 0x19 }, { 0x3f, 0x2b, 0x11 }, { 0x3f, 0x27, 0x08 }, { 0x3f, 0x22, 0x04 },
  { 0x38, 0x1e, 0x00 }, { 0x32, 0x19, 0x00 }, { 0x2b, 0x15, 0x00 }, { 0x24, 0x10, 0x00 },
  { 0x1e, 0x0b, 0x00 }, { 0x17, 0x06, 0x00 }, { 0x10, 0x01, 0x00 }, { 0x0a, 0x00, 0x00 },
  { 0x36, 0x2f, 0x2c }, { 0x33, 0x2b, 0x28 }, { 0x31, 0x27, 0x24 }, { 0x2e, 0x23, 0x20 },
  { 0x2b, 0x1f, 0x1c }, { 0x29, 0x1c, 0x19 }, { 0x24, 0x19, 0x17 }, { 0x1f, 0x16, 0x16 },
  { 0x1b, 0x14, 0x15 }, { 0x16, 0x11, 0x13 }, { 0x11, 0x0f, 0x12 }, { 0x0d, 0x0c, 0x11 },
  { 0x0a, 0x0a, 0x0d }, { 0x07, 0x07, 0x0a }, { 0x04, 0x04, 0x06 }, { 0x01, 0x02, 0x03 },
  { 0x35, 0x3b, 0x36 }, { 0x2e, 0x36, 0x31 }, { 0x26, 0x30, 0x2c }, { 0x1f, 0x2a, 0x27 },
  { 0x17, 0x24, 0x22 }, { 0x10, 0x1b, 0x1a }, { 0x08, 0x11, 0x12 }, { 0x01, 0x07, 0x09 },
  { 0x3f, 0x32, 0x2b }, { 0x3a, 0x2b, 0x25 }, { 0x35, 0x24, 0x20 }, { 0x2f, 0x1d, 0x1a },
  { 0x2a, 0x16, 0x15 }, { 0x24, 0x10, 0x10 }, { 0x1c, 0x0a, 0x0c }, { 0x16, 0x04, 0x07 },
  { 0x37, 0x34, 0x2a }, { 0x32, 0x31, 0x28 }, { 0x2d, 0x2e, 0x25 }, { 0x28, 0x2b, 0x23 },
  { 0x22, 0x28, 0x21 }, { 0x1d, 0x25, 0x1f }, { 0x18, 0x22, 0x1d }, { 0x13, 0x1f, 0x1a },
  { 0x11, 0x1b, 0x18 }, { 0x0f, 0x18, 0x15 }, { 0x0c, 0x14, 0x13 }, { 0x0a, 0x11, 0x10 },
  { 0x07, 0x0d, 0x0d }, { 0x05, 0x0a, 0x0b }, { 0x03, 0x06, 0x08 }, { 0x00, 0x03, 0x05 },
  { 0x3f, 0x1e, 0x03 }, { 0x3f, 0x37, 0x2a }, { 0x3f, 0x34, 0x25 }, { 0x3f, 0x32, 0x20 },
  { 0x36, 0x2c, 0x20 }, { 0x2c, 0x27, 0x1f }, { 0x23, 0x21, 0x1e }, { 0x19, 0x1c, 0x1e },
  { 0x10, 0x16, 0x1d }, { 0x06, 0x11, 0x1d }, { 0x05, 0x0f, 0x19 }, { 0x04, 0x0c, 0x15 },
  { 0x03, 0x0a, 0x11 }, { 0x02, 0x07, 0x0d }, { 0x01, 0x05, 0x09 }, { 0x3f, 0x1b, 0x03 },
  { 0x3d, 0x37, 0x33 }, { 0x3a, 0x35, 0x2f }, { 0x37, 0x32, 0x2a }, { 0x33, 0x2f, 0x25 },
  { 0x30, 0x2c, 0x20 }, { 0x2c, 0x29, 0x1b }, { 0x29, 0x25, 0x18 }, { 0x26, 0x22, 0x16 },
  { 0x23, 0x1e, 0x13 }, { 0x1f, 0x1a, 0x10 }, { 0x1a, 0x16, 0x0e }, { 0x16, 0x12, 0x0b },
  { 0x12, 0x0e, 0x08 }, { 0x0d, 0x0a, 0x05 }, { 0x09, 0x06, 0x02 }, { 0x04, 0x01, 0x00 },
  { 0x3f, 0x39, 0x2e }, { 0x3c, 0x33, 0x29 }, { 0x38, 0x2d, 0x24 }, { 0x34, 0x26, 0x1e },
  { 0x30, 0x20, 0x19 }, { 0x2c, 0x19, 0x14 }, { 0x28, 0x13, 0x0e }, { 0x24, 0x0d, 0x09 },
  { 0x21, 0x06, 0x03 }, { 0x1d, 0x05, 0x03 }, { 0x19, 0x04, 0x02 }, { 0x15, 0x04, 0x02 },
  { 0x11, 0x03, 0x02 }, { 0x0d, 0x02, 0x01 }, { 0x09, 0x01, 0x01 }, { 0x06, 0x01, 0x01 },
  { 0x3f, 0x36, 0x2c }, { 0x3c, 0x31, 0x28 }, { 0x39, 0x2d, 0x23 }, { 0x36, 0x29, 0x1f },
  { 0x33, 0x24, 0x1b }, { 0x2f, 0x20, 0x16 }, { 0x2c, 0x1b, 0x12 }, { 0x2a, 0x18, 0x11 },
  { 0x28, 0x16, 0x10 }, { 0x26, 0x13, 0x0f }, { 0x24, 0x10, 0x0d }, { 0x22, 0x0d, 0x0c },
  { 0x1a, 0x0a, 0x0a }, { 0x11, 0x08, 0x08 }, { 0x08, 0x05, 0x06 }, { 0x3f, 0x17, 0x02 },
  { 0x3f, 0x33, 0x2f }, { 0x3f, 0x30, 0x2c }, { 0x3f, 0x2d, 0x29 }, { 0x3e, 0x2a, 0x26 },
  { 0x3e, 0x27, 0x23 }, { 0x3d, 0x24, 0x20 }, { 0x3d, 0x20, 0x1e }, { 0x3d, 0x1d, 0x1b },
  { 0x3c, 0x1a, 0x18 }, { 0x3c, 0x17, 0x15 }, { 0x3b, 0x14, 0x12 }, { 0x3b, 0x11, 0x0f },
  { 0x3b, 0x0e, 0x0c }, { 0x3a, 0x0b, 0x0a }, { 0x3a, 0x07, 0x07 }, { 0x39, 0x04, 0x04 },
  { 0x39, 0x02, 0x02 }, { 0x35, 0x02, 0x02 }, { 0x31, 0x01, 0x01 }, { 0x2e, 0x01, 0x01 },
  { 0x2a, 0x01, 0x01 }, { 0x26, 0x01, 0x01 }, { 0x22, 0x01, 0x01 }, { 0x1e, 0x01, 0x01 },
  { 0x1b, 0x01, 0x01 }, { 0x17, 0x01, 0x01 }, { 0x13, 0x01, 0x01 }, { 0x3f, 0x13, 0x01 },
  { 0x3f, 0x10, 0x01 }, { 0x08, 0x00, 0x00 }, { 0x3f, 0x0c, 0x00 }, { 0x0c, 0x2e, 0x2b },
};

static std::array<uint8_t, 54> bmpheader = {
  0x42, 0x4d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x23, 0x2e, 0x00, 0x00, 0x23, 0x2e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

struct Bitmap {
  std::vector<uint8_t> buffer;
  size_t rowstride;
  size_t w, h;
  Bitmap() {
  }
  Bitmap(size_t w, size_t h)
  : buffer{bmpheader.begin(), bmpheader.end()}
  , w(w)
  , h(h)
  {
    rowstride = w * 3;
    while (rowstride & 0x3) rowstride++;
    uint32_t imageByteCount = rowstride * h;
    buffer.resize(imageByteCount + bmpheader.size());
    buffer[18] = w & 0xFF;
    buffer[19] = (w >> 8) & 0xFF;
    buffer[22] = h & 0xFF;
    buffer[23] = (h >> 8) & 0xFF;
    buffer[2] = buffer.size() & 0xFF;
    buffer[3] = (buffer.size() >> 8) & 0xFF;
    buffer[4] = (buffer.size() >> 16) & 0xFF;
    buffer[5] = (buffer.size() >> 24) & 0xFF;
    buffer[34] = ((imageByteCount)) & 0xFF;
    buffer[35] = ((imageByteCount) >> 8) & 0xFF;
    buffer[36] = ((imageByteCount) >> 16) & 0xFF;
    buffer[37] = ((imageByteCount) >> 24) & 0xFF;
  }
  void put(size_t x, size_t y, Color color) {
    uint8_t* p = buffer.data() + sizeof(bmpheader) + rowstride * (h - y - 1) + x * 3;
    p[0] = color.b * 4;
    p[1] = color.g * 4;
    p[2] = color.r * 4;
  }
  void Save(const std::string& name) {
    std::ofstream(name).write((const char*)buffer.data(), buffer.size());
  }
} _b;

static size_t deltax = 0, deltay = 0, imagewidth = 0, imageheight = 0;
bool draw = false;
static size_t maxx = 0, maxy = 0, minx = 2147483647, miny = 2147483647;
void putcolor(uint32_t x, uint32_t y, Color color) {
  if (draw) {
    if (color.r == 0 && color.g == 0 && color.b == 0) return;
    _b.put(x, y, color);
  }
  if (x < minx) minx = x;
  if (x > maxx) maxx = x;
  if (y < miny) miny = y;
  if (y > maxy) maxy = y;
}

void drawShape(uint16_t shape, uint16_t frame, uint32_t dx, uint32_t dy, uint32_t dz) {
  switch(shape) {
  case 1592:
  case 1593:
  case 1594:
  case 1608:
  case 1609:
    return;
  }
  shape--;
  std::vector<uint8_t> data;
  try {
    data.resize(std::filesystem::file_size("shapes.flx." + std::to_string(shape)));
    std::ifstream("shapes.flx." + std::to_string(shape)).read(reinterpret_cast<char*>(data.data()), data.size());
  } catch (...) {
    printf("Cannot draw %s\n", std::to_string(shape).c_str());
    return;
  }
  ShpHeader* h = reinterpret_cast<ShpHeader*>(data.data());
  std::span<FrameHeader> fhs{reinterpret_cast<FrameHeader*>(data.data() + sizeof(ShpHeader)),
                            reinterpret_cast<FrameHeader*>(data.data() + sizeof(ShpHeader)) + h->count};
  std::vector<std::span<const uint8_t>> fds;
  for (auto& fh : fhs) {
    uint32_t offset = fh.frameOffset & 0x7FFFFFFF;
    uint32_t size = fh.framesize;
    fds.emplace_back(data.data() + offset, data.data() + offset + size);
  }
  if (fds.size() <= frame) return;

  auto& fd = fds[frame];
  FrameData* fdata = (FrameData*)fd.data();
  static constexpr int32_t S = 2;
  static constexpr uint32_t drawY = 32768;
  uint32_t drawx = (int(dx) - int(dy)) / S - fdata->offx + drawY - deltax;
  uint32_t drawy = (dx + dy) / (S*2) - dz - fdata->offy + drawY - deltay;

  for (size_t row = 0; row < fdata->height; row++) {
    uint8_t* inbuf = (uint8_t*)&fdata->rowOffsets[row] + fdata->rowOffsets[row];

    uint32_t x = 0;

    while(x < fdata->width) {
      // Skip N pixels
      x += *inbuf;
      inbuf++;
      if(x >= fdata->width)
        break;

      uint8_t length = *inbuf++;
      uint8_t type = 0;

      if (fdata->compression == 1) {
        type = length & 1;
        length >>= 1;
      }

      if(type == 0) {
        for (size_t n = 0; n < length; n++) {
          putcolor(drawx + x, drawy + row, palette[*inbuf++]);
          x++;
        }
      } else {
        for (size_t n = 0; n < length; n++) {
          putcolor(drawx + x, drawy + row, palette[*inbuf]);
          x++;
        }
        inbuf++;
      }
    }
  }
}

std::vector<std::vector<uint8_t>> globs;
struct Shape {
    uint16_t shape;
    uint8_t frame;
    int x, y, z;
};

void drawLevel(const char* name) {
    maxx = 0;
    maxy = 0;
    minx = 2147483647;
    miny = 2147483647;
    std::vector<uint8_t> data;
    data.resize(std::filesystem::file_size(name));
    std::ifstream(name).read(reinterpret_cast<char*>(data.data()), data.size());
    Entry* firstEntry = reinterpret_cast<Entry*>(data.data());
    std::span<Entry> entries{firstEntry, firstEntry + data.size() / sizeof(Entry)};
    std::vector<Shape> shapes;
    for (auto& entry : entries) {
      if (entry.type == 0x10) {
        if (entry.count >= globs.size()) {
          printf("invalid glob id %u\n", entry.count);
        } else {
          std::vector<uint8_t>& gv = globs[entry.count];
          std::span<GlobEntry> ge{reinterpret_cast<GlobEntry*>(gv.data() + 2), reinterpret_cast<GlobEntry*>(gv.data() + gv.size())};
          for (auto& e : ge) {
            shapes.push_back({e.shapeindex, e.frame, entry.x + e.x*2 - 512, entry.y + e.y*2 - 512, entry.z + e.z});
          }
        }
      } else {
        shapes.push_back({entry.type, entry.frame, entry.x, entry.y, entry.z});
      }
    }
    std::sort(shapes.begin(), shapes.end(), [](const Shape& a, const Shape& b) {
      if (a.z < b.z) return true;
      else if (a.z > b.z) return false;
      if (a.y + a.x < b.y + b.x) return true;
      else if (a.y + a.x > b.y + b.x) return false;
      return false;
    });
    for (auto& s : shapes) {
      drawShape(s.shape, s.frame, s.x, s.y, s.z);
    }
}

int main(int argc, const char** argv) {
  globs.resize(3072);
  for (size_t n = 0; n < 3072; n++) {
    globs[n].resize(std::filesystem::file_size("glob.flx." + std::to_string(n)));
    std::ifstream("glob.flx." + std::to_string(n)).read(reinterpret_cast<char*>(globs[n].data()), globs[n].size());
  }
  for (size_t n = 1; n < static_cast<size_t>(argc); n++) {
    draw = false;
    drawLevel(argv[n]);
    printf("%zu %zu %zu %zu\n", minx, miny, maxx, maxy);
    deltax = minx;
    deltay = miny;
    imagewidth = maxx - minx + 1;
    imageheight = maxy - miny + 1;
    _b = Bitmap(imagewidth, imageheight);
    draw = true;
    drawLevel(argv[n]);
    printf("%zu %zu %zu %zu\n", minx, miny, maxx, maxy);
    _b.Save(argv[n] + std::string(".bmp"));
  }
}

