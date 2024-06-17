#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <Eigen/Dense>
#include "extlib/libattopng/libattopng.h"

// this hpp give a define of yyparse that generated by bison
#include "parser.tab.hpp"

#include "ast.hpp"

using namespace std;

// defined in code that generated by flex
extern FILE *yyin;

void
draw_png() {
#define RGBA(r, g, b, a) ((r) | ((g) << 8) | ((b) << 16) | ((a) << 24))
#define RGB(r, g, b) RGBA(r, g, b, 0xff)

    int W = 640;
    int H = 360;
    libattopng_t *png = libattopng_new(W, H, PNG_RGBA);
    int y, x;

    for (y = 0; y < H; y++) {
        for (x = 0; x < W; x++) {
            libattopng_set_pixel(png, x, y, RGBA((int)(x/640.0*255), 0, 255, 100));
        }
    }
    libattopng_save(png, "test_rgba.png");
    libattopng_destroy(png);
}

ModelData*
loadModel(const char* mode_file) {
  yyin = fopen(mode_file, "r");
  assert(yyin);

  ModelData* model_data = new ModelData();

  auto ret = yyparse(model_data);
  printf ("[%d]\n", ret);
  assert(!ret);

  for (auto& point : model_data->point_list) {
    ssize_t i = point.index-1;

    if (!model_data->v_list.empty()) {
      if ((i < 0) || (i >= model_data->v_list.size()))
        goto out;

      auto& vertex = model_data->v_list[i];

      point.vertex[0] = vertex.x;
      point.vertex[1] = vertex.y;
      point.vertex[2] = vertex.z;
    }
  }

  for (auto& line : model_data->line_list) {
    for (auto index : line.index_list) {
      ssize_t i = index-1;

      if (!model_data->v_list.empty()) {
        if ((i < 0) || (i >= model_data->v_list.size()))
          goto out;

        auto& vertex = model_data->v_list[i];

        line.vertex_list.push_back(Eigen::Vector3d(vertex.x, vertex.y, vertex.z));
      }
    }
  }

  for (auto& face : model_data->face_list) {
    for (auto index : face.index_list) {
      ssize_t i = index-1;

      if (!model_data->v_list.empty()) {
        if ((i < 0) || (i >= model_data->v_list.size()))
          goto out;

        auto& vertex = model_data->v_list[i];

        face.vertex_list.push_back(Eigen::Vector3d(vertex.x, vertex.y, vertex.z));
      }

      if (!model_data->vt_list.empty()) {
        if ((i < 0) || (i >= model_data->vt_list.size()))
          goto out;

        auto& texture = model_data->vt_list[i];

        face.vertex_texture_list.push_back(Eigen::Vector2d(texture.u, texture.v));
      }

      if (!model_data->vn_list.empty()) {
        if ((i < 0) || (i >= model_data->vn_list.size()))
          goto out;

        auto& normal = model_data->vn_list[i];

        face.vertex_normal_list.push_back(Eigen::Vector3d(normal.dx, normal.dy, normal.dz));
      }
    }
  }

  model_data->dump();

out:
  return model_data;
}

int main(int argc, const char *argv[]) {
  draw_png();

  assert(argc == 2);
  auto input = argv[1];

  ModelData* model_data = loadModel(input);

out:

  delete model_data;

  return 0;
}
