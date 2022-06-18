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
  uint32_t x;
  uint32_t y;
  uint32_t rowOffsets[1];
};

static std::array<uint8_t, 54> bmpheader = {
  0x42, 0x4d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x23, 0x2e, 0x00, 0x00, 0x23, 0x2e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

struct Color {
  uint8_t r, g, b;
};

Color palette[256] = {
  { 0x00, 0x00, 0x00 },
  { 0x3f, 0x3f, 0x2f },
  { 0x3e, 0x3c, 0x08 },
  { 0x3f, 0x32, 0x07 },
  { 0x3f, 0x29, 0x05 },
  { 0x3f, 0x1f, 0x03 },
  { 0x3f, 0x16, 0x02 },
  { 0x3f, 0x0c, 0x00 },
  { 0x00, 0x00, 0x00 },
  { 0x3f, 0x3f, 0x11 },
  { 0x3f, 0x15, 0x14 },
  { 0x13, 0x29, 0x14 },
  { 0x00, 0x1d, 0x2f },
  { 0x3f, 0x3f, 0x33 },
  { 0x3f, 0x3f, 0x32 },
  { 0x3f, 0x3f, 0x35 },
  { 0x3f, 0x3f, 0x3f },
  { 0x3a, 0x3a, 0x3a },
  { 0x35, 0x35, 0x35 },
  { 0x30, 0x30, 0x30 },
  { 0x2b, 0x2b, 0x2b },
  { 0x26, 0x26, 0x26 },
  { 0x22, 0x22, 0x22 },
  { 0x1d, 0x1d, 0x1d },
  { 0x19, 0x19, 0x19 },
  { 0x16, 0x16, 0x16 },
  { 0x12, 0x12, 0x12 },
  { 0x0e, 0x0e, 0x0e },
  { 0x0b, 0x0b, 0x0b },
  { 0x07, 0x07, 0x07 },
  { 0x03, 0x03, 0x03 },
  { 0x00, 0x00, 0x00 },
  { 0x35, 0x1b, 0x11 },
  { 0x2e, 0x19, 0x11 },
  { 0x27, 0x17, 0x11 },
  { 0x21, 0x15, 0x12 },
  { 0x1a, 0x13, 0x12 },
  { 0x13, 0x10, 0x13 },
  { 0x0d, 0x0e, 0x13 },
  { 0x06, 0x0c, 0x13 },
  { 0x36, 0x2d, 0x13 },
  { 0x31, 0x29, 0x13 },
  { 0x2d, 0x24, 0x14 },
  { 0x28, 0x20, 0x15 },
  { 0x23, 0x1c, 0x15 },
  { 0x1f, 0x18, 0x16 },
  { 0x1a, 0x14, 0x17 },
  { 0x15, 0x0f, 0x17 },
  { 0x36, 0x36, 0x3a },
  { 0x31, 0x32, 0x37 },
  { 0x2d, 0x2f, 0x33 },
  { 0x29, 0x2b, 0x30 },
  { 0x24, 0x27, 0x2d },
  { 0x20, 0x23, 0x29 },
  { 0x1b, 0x20, 0x26 },
  { 0x17, 0x1c, 0x23 },
  { 0x13, 0x18, 0x1f },
  { 0x10, 0x15, 0x1b },
  { 0x0d, 0x11, 0x16 },
  { 0x0b, 0x0e, 0x12 },
  { 0x08, 0x0b, 0x0e },
  { 0x05, 0x07, 0x09 },
  { 0x03, 0x04, 0x05 },
  { 0x3e, 0x3c, 0x08 },
  { 0x00, 0x18, 0x37 },
  { 0x00, 0x0c, 0x1c },
  { 0x3e, 0x38, 0x08 },
  { 0x22, 0x08, 0x02 },
  { 0x00, 0x2d, 0x03 },
  { 0x00, 0x1b, 0x02 },
  { 0x3f, 0x35, 0x07 },
  { 0x3f, 0x31, 0x06 },
  { 0x2d, 0x2c, 0x22 },
  { 0x28, 0x27, 0x1d },
  { 0x23, 0x22, 0x18 },
  { 0x1d, 0x1d, 0x12 },
  { 0x17, 0x17, 0x0f },
  { 0x11, 0x11, 0x0b },
  { 0x0a, 0x0a, 0x07 },
  { 0x05, 0x05, 0x03 },
  { 0x36, 0x36, 0x36 },
  { 0x35, 0x32, 0x32 },
  { 0x32, 0x2e, 0x2e },
  { 0x2e, 0x2a, 0x2a },
  { 0x2a, 0x26, 0x26 },
  { 0x27, 0x22, 0x22 },
  { 0x23, 0x1e, 0x1e },
  { 0x1f, 0x1a, 0x1a },
  { 0x1d, 0x17, 0x17 },
  { 0x1c, 0x14, 0x14 },
  { 0x1a, 0x11, 0x11 },
  { 0x18, 0x0e, 0x0e },
  { 0x13, 0x0a, 0x0a },
  { 0x0e, 0x07, 0x07 },
  { 0x09, 0x04, 0x04 },
  { 0x3f, 0x2d, 0x06 },
  { 0x3f, 0x3f, 0x3a },
  { 0x3f, 0x3b, 0x32 },
  { 0x3f, 0x2a, 0x05 },
  { 0x3f, 0x26, 0x04 },
  { 0x3f, 0x2f, 0x19 },
  { 0x3f, 0x2b, 0x11 },
  { 0x3f, 0x27, 0x08 },
  { 0x3f, 0x22, 0x04 },
  { 0x38, 0x1e, 0x00 },
  { 0x32, 0x19, 0x00 },
  { 0x2b, 0x15, 0x00 },
  { 0x24, 0x10, 0x00 },
  { 0x1e, 0x0b, 0x00 },
  { 0x17, 0x06, 0x00 },
  { 0x10, 0x01, 0x00 },
  { 0x0a, 0x00, 0x00 },
  { 0x36, 0x2f, 0x2c },
  { 0x33, 0x2b, 0x28 },
  { 0x31, 0x27, 0x24 },
  { 0x2e, 0x23, 0x20 },
  { 0x2b, 0x1f, 0x1c },
  { 0x29, 0x1c, 0x19 },
  { 0x24, 0x19, 0x17 },
  { 0x1f, 0x16, 0x16 },
  { 0x1b, 0x14, 0x15 },
  { 0x16, 0x11, 0x13 },
  { 0x11, 0x0f, 0x12 },
  { 0x0d, 0x0c, 0x11 },
  { 0x0a, 0x0a, 0x0d },
  { 0x07, 0x07, 0x0a },
  { 0x04, 0x04, 0x06 },
  { 0x01, 0x02, 0x03 },
  { 0x35, 0x3b, 0x36 },
  { 0x2e, 0x36, 0x31 },
  { 0x26, 0x30, 0x2c },
  { 0x1f, 0x2a, 0x27 },
  { 0x17, 0x24, 0x22 },
  { 0x10, 0x1b, 0x1a },
  { 0x08, 0x11, 0x12 },
  { 0x01, 0x07, 0x09 },
  { 0x3f, 0x32, 0x2b },
  { 0x3a, 0x2b, 0x25 },
  { 0x35, 0x24, 0x20 },
  { 0x2f, 0x1d, 0x1a },
  { 0x2a, 0x16, 0x15 },
  { 0x24, 0x10, 0x10 },
  { 0x1c, 0x0a, 0x0c },
  { 0x16, 0x04, 0x07 },
  { 0x37, 0x34, 0x2a },
  { 0x32, 0x31, 0x28 },
  { 0x2d, 0x2e, 0x25 },
  { 0x28, 0x2b, 0x23 },
  { 0x22, 0x28, 0x21 },
  { 0x1d, 0x25, 0x1f },
  { 0x18, 0x22, 0x1d },
  { 0x13, 0x1f, 0x1a },
  { 0x11, 0x1b, 0x18 },
  { 0x0f, 0x18, 0x15 },
  { 0x0c, 0x14, 0x13 },
  { 0x0a, 0x11, 0x10 },
  { 0x07, 0x0d, 0x0d },
  { 0x05, 0x0a, 0x0b },
  { 0x03, 0x06, 0x08 },
  { 0x00, 0x03, 0x05 },
  { 0x3f, 0x1e, 0x03 },
  { 0x3f, 0x37, 0x2a },
  { 0x3f, 0x34, 0x25 },
  { 0x3f, 0x32, 0x20 },
  { 0x36, 0x2c, 0x20 },
  { 0x2c, 0x27, 0x1f },
  { 0x23, 0x21, 0x1e },
  { 0x19, 0x1c, 0x1e },
  { 0x10, 0x16, 0x1d },
  { 0x06, 0x11, 0x1d },
  { 0x05, 0x0f, 0x19 },
  { 0x04, 0x0c, 0x15 },
  { 0x03, 0x0a, 0x11 },
  { 0x02, 0x07, 0x0d },
  { 0x01, 0x05, 0x09 },
  { 0x3f, 0x1b, 0x03 },
  { 0x3d, 0x37, 0x33 },
  { 0x3a, 0x35, 0x2f },
  { 0x37, 0x32, 0x2a },
  { 0x33, 0x2f, 0x25 },
  { 0x30, 0x2c, 0x20 },
  { 0x2c, 0x29, 0x1b },
  { 0x29, 0x25, 0x18 },
  { 0x26, 0x22, 0x16 },
  { 0x23, 0x1e, 0x13 },
  { 0x1f, 0x1a, 0x10 },
  { 0x1a, 0x16, 0x0e },
  { 0x16, 0x12, 0x0b },
  { 0x12, 0x0e, 0x08 },
  { 0x0d, 0x0a, 0x05 },
  { 0x09, 0x06, 0x02 },
  { 0x04, 0x01, 0x00 },
  { 0x3f, 0x39, 0x2e },
  { 0x3c, 0x33, 0x29 },
  { 0x38, 0x2d, 0x24 },
  { 0x34, 0x26, 0x1e },
  { 0x30, 0x20, 0x19 },
  { 0x2c, 0x19, 0x14 },
  { 0x28, 0x13, 0x0e },
  { 0x24, 0x0d, 0x09 },
  { 0x21, 0x06, 0x03 },
  { 0x1d, 0x05, 0x03 },
  { 0x19, 0x04, 0x02 },
  { 0x15, 0x04, 0x02 },
  { 0x11, 0x03, 0x02 },
  { 0x0d, 0x02, 0x01 },
  { 0x09, 0x01, 0x01 },
  { 0x06, 0x01, 0x01 },
  { 0x3f, 0x36, 0x2c },
  { 0x3c, 0x31, 0x28 },
  { 0x39, 0x2d, 0x23 },
  { 0x36, 0x29, 0x1f },
  { 0x33, 0x24, 0x1b },
  { 0x2f, 0x20, 0x16 },
  { 0x2c, 0x1b, 0x12 },
  { 0x2a, 0x18, 0x11 },
  { 0x28, 0x16, 0x10 },
  { 0x26, 0x13, 0x0f },
  { 0x24, 0x10, 0x0d },
  { 0x22, 0x0d, 0x0c },
  { 0x1a, 0x0a, 0x0a },
  { 0x11, 0x08, 0x08 },
  { 0x08, 0x05, 0x06 },
  { 0x3f, 0x17, 0x02 },
  { 0x3f, 0x33, 0x2f },
  { 0x3f, 0x30, 0x2c },
  { 0x3f, 0x2d, 0x29 },
  { 0x3e, 0x2a, 0x26 },
  { 0x3e, 0x27, 0x23 },
  { 0x3d, 0x24, 0x20 },
  { 0x3d, 0x20, 0x1e },
  { 0x3d, 0x1d, 0x1b },
  { 0x3c, 0x1a, 0x18 },
  { 0x3c, 0x17, 0x15 },
  { 0x3b, 0x14, 0x12 },
  { 0x3b, 0x11, 0x0f },
  { 0x3b, 0x0e, 0x0c },
  { 0x3a, 0x0b, 0x0a },
  { 0x3a, 0x07, 0x07 },
  { 0x39, 0x04, 0x04 },
  { 0x39, 0x02, 0x02 },
  { 0x35, 0x02, 0x02 },
  { 0x31, 0x01, 0x01 },
  { 0x2e, 0x01, 0x01 },
  { 0x2a, 0x01, 0x01 },
  { 0x26, 0x01, 0x01 },
  { 0x22, 0x01, 0x01 },
  { 0x1e, 0x01, 0x01 },
  { 0x1b, 0x01, 0x01 },
  { 0x17, 0x01, 0x01 },
  { 0x13, 0x01, 0x01 },
  { 0x3f, 0x13, 0x01 },
  { 0x3f, 0x10, 0x01 },
  { 0x08, 0x00, 0x00 },
  { 0x3f, 0x0c, 0x00 },
  { 0x0c, 0x2e, 0x2b },
};

int main(int argc, const char** argv) {
  for (size_t n = 1; n < static_cast<size_t>(argc); n++) {
    std::vector<uint8_t> data;
    data.resize(std::filesystem::file_size(argv[n]));
    std::ifstream(argv[n]).read(reinterpret_cast<char*>(data.data()), data.size());
    ShpHeader* h = reinterpret_cast<ShpHeader*>(data.data());
    std::span<FrameHeader> fhs{reinterpret_cast<FrameHeader*>(data.data() + sizeof(ShpHeader)),
                              reinterpret_cast<FrameHeader*>(data.data() + sizeof(ShpHeader)) + h->count};
    std::vector<std::span<const uint8_t>> fds;
    for (auto& fh : fhs) {
      uint32_t offset = fh.frameOffset & 0x7FFFFFFF;
      uint32_t size = fh.framesize;
      fds.emplace_back(data.data() + offset, data.data() + offset + size);
    }
    size_t frameno = 0;
    for (auto& fd : fds) {
      FrameData* data = (FrameData*)fd.data();
      std::vector<uint8_t> image{bmpheader.begin(), bmpheader.end()};
      uint32_t rowstride = data->width * 3;
      while (rowstride & 0x3) rowstride++;
      uint32_t imageByteCount = rowstride * data->height;
      image.resize(imageByteCount + bmpheader.size() + 4096);
      image[18] = data->width & 0xFF;
      image[19] = (data->width >> 8) & 0xFF;
      image[22] = data->height & 0xFF;
      image[23] = (data->height >> 8) & 0xFF;
      image[2] = image.size() & 0xFF;
      image[3] = (image.size() >> 8) & 0xFF;
      image[4] = (image.size() >> 16) & 0xFF;
      image[5] = (image.size() >> 24) & 0xFF;
      image[34] = ((imageByteCount)) & 0xFF;
      image[35] = ((imageByteCount) >> 8) & 0xFF;
      image[36] = ((imageByteCount) >> 16) & 0xFF;
      image[37] = ((imageByteCount) >> 24) & 0xFF;
      for (size_t row = 0; row < data->height; row++) {
        uint8_t* rowbuf = image.data() + sizeof(bmpheader) + rowstride * (data->height - row - 1);
        uint8_t* inbuf = (uint8_t*)&data->rowOffsets[row] + data->rowOffsets[row];

        size_t rowlen = data->rowOffsets[row+1] - data->rowOffsets[row] + 4;
        printf("%zu\n", rowlen);
        uint32_t x = 0;

        while(x < data->width) {
          // Skip N pixels
          rowbuf += 3 * *inbuf;
          x += *inbuf;
          inbuf++;
          if(x >= data->width)
            break;

          uint8_t length = *inbuf++;
          uint8_t type = 0;

          if (data->compression == 1) {
            type = length & 1;
            length >>= 1;
          }

          if(type == 0) {
            for (size_t n = 0; n < length; n++) {
              Color& col = palette[*inbuf++];
              *rowbuf++ = col.b<<2;
              *rowbuf++ = col.g<<2;
              *rowbuf++ = col.r<<2;
            }
          } else {
            Color& col = palette[*inbuf];
            for (size_t n = 0; n < length; n++) {
              *rowbuf++ = col.b<<2;
              *rowbuf++ = col.g<<2;
              *rowbuf++ = col.r<<2;
            }
            inbuf++;
          }

          x += length;
        }
      }
      image.resize(imageByteCount + bmpheader.size());
      std::ofstream(argv[n] + std::string(".") + std::to_string(frameno) + ".bmp").write((const char*)image.data(), image.size());
      frameno++;
    }

  }
}
