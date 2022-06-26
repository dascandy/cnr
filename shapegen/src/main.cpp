#include <filesystem>
#include <fstream>
#include <span>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <map>

int main(int, const char** argv) {
  std::vector<uint8_t> data;
  data.resize(std::filesystem::file_size(argv[1]));
  std::ifstream(argv[1]).read(reinterpret_cast<char*>(data.data()), data.size());
  for (size_t n = 1; n < 2048; n++) {
    FILE* mtl = fopen(("crusader_" + std::to_string(n-1) + ".mtl").c_str(), "wb");
    uint8_t* p = data.data() + n * 9;
    float x = ((p[2] >> 5) | (p[3] << 3)) & 0x1F;
    float y = (p[3] >> 2) & 0x1F;
    float z = ((p[3] >> 7) | (p[4] << 1)) & 0x1F;
    if (x == 0) { x = 0.1; }
    if (y == 0) { y = 0.1; }
    float x1 = -x / 2, x2 = x / 2;
    float y1 = -y / 2, y2 = y / 2;
    float z1 = -z / 2, z2 = z / 2;
    if (z == 0) {
      z1 = -0.1;
    }
    std::vector<uint8_t> sdata;
    sdata.resize(6);
    std::ifstream("shapes.flx." + std::to_string(n-1)).read(reinterpret_cast<char*>(sdata.data()), sdata.size());
    size_t frames = sdata[4] + sdata[5] * 256;
    printf("%zu\n", frames);
    for (size_t f = 0; f < frames; f++) {
      fprintf(mtl, "newmtl crus_%zu_%zu\n", n-1, f);
      fprintf(mtl, "  Ka 1.000 1.000 1.000\n  Kd 1.000 1.000 1.000\n  Ks 0.000 0.000 0.000\n");
      fprintf(mtl, "  map_Ka shapes.flx.%zu.%zu.bmp\n  map_Kd shapes.flx.%zu.%zu.bmp\n\n", n-1, f, n-1, f);
      FILE* out = fopen((std::to_string(n - 1) + "_" + std::to_string(f) + ".obj").c_str(), "wb");
      fprintf(out, "mtllib crusader_%zu.mtl\nusemtl crus_%zu_%zu\n", n-1, n-1, f);
      fprintf(out, "v %f %f %f\n", x1, z1, y1);
      fprintf(out, "v %f %f %f\n", x1, z1, y2);
      fprintf(out, "v %f %f %f\n", x1, z2, y1);
      fprintf(out, "v %f %f %f\n", x1, z2, y2);
      fprintf(out, "v %f %f %f\n", x2, z1, y1);
      fprintf(out, "v %f %f %f\n", x2, z1, y2);
      fprintf(out, "v %f %f %f\n", x2, z2, y1);
      fprintf(out, "v %f %f %f\n\n", x2, z2, y2);

      fprintf(out, "vn -1 0 0\nvn 1 0 0\nvn 0 -1 0\nvn 0 1 0\nvn 0 0 -1\nvn 0 0 1\n\n");
      fprintf(out, "vt 0 0\nvt 0 1\nvt 1 0\nvt 1 1\n\n");
      fprintf(out, "f 4/4/1 1/1/1 3/2/1\n");
      fprintf(out, "f 1/1/1 4/4/1 2/3/1\n");
      fprintf(out, "f 8/3/2 7/1/2 5/2/2\n");
      fprintf(out, "f 5/2/2 6/4/2 8/3/2\n");
      fprintf(out, "f 6/3/3 5/1/3 1/2/3\n");
      fprintf(out, "f 1/2/3 2/4/3 6/3/3\n");
      fprintf(out, "f 8/4/4 3/1/4 7/2/4\n");
      fprintf(out, "f 3/1/4 8/4/4 4/3/4\n");
      fprintf(out, "f 7/4/5 1/1/5 5/2/5\n");
      fprintf(out, "f 1/1/5 7/4/5 3/3/5\n");
      fprintf(out, "f 8/2/6 6/4/6 2/3/6\n");
      fprintf(out, "f 2/3/6 4/1/6 8/2/6\n");
      fclose(out);
    }
    fclose(mtl);
  }
}
