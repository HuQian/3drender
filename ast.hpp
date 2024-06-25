#ifndef _AST_H_
#define _AST_H_
#include <string>
#include <vector>
#include <map>
#include <iostream>

#include <Eigen/Dense>

#include "extlib/libattopng/libattopng.h"

struct VertexDataV {
    double x;
    double y;
    double z;
};

struct VertexDataVT {
    double u;
    double v;
};

struct VertexDataVN {
    double dx;
    double dy;
    double dz;
};

static bool
trans_obj_index_to_arr(int idx, size_t* i, int v_list_size) {
  if (idx >= 1 && idx <= v_list_size) {
    *i = idx - 1;
    return true;
  } else if (-idx >= 1 && -idx <= v_list_size) {
    *i = idx + v_list_size;
    return true;
  }

  return false;
}

/*
    p       Point:                          p v1
    l       Line:                           l v1 v2 ... vn
    f       Face:                           f v1 v2 ... vn
    f       Face with texture coords:       f v1/t1 v2/t2 .... vn/tn
    f       Face with vertex normals:       f v1//n1 v2//n2 .... vn//nn
    f       Face with txt and norms:        f v1/t1/n1 v2/t2/n2 .... vn/tn/nn
*/
struct ElementPoint {
    int index_pos;

    Eigen::Vector4d vertex;

public:
    void loadV(std::vector<VertexDataV>& v_list) {
        size_t i = 0;

        if (!trans_obj_index_to_arr(this->index_pos, &i, v_list.size())) {
                printf ("failed to load point\n");
                return;
        }

        auto& v = v_list[i];
        this->vertex[0] = v.x;
        this->vertex[1] = v.y;
        this->vertex[2] = v.z;
        this->vertex[3] = 1;
    }
};

struct ElementLine {
    std::vector<int> index_pos_list;

    std::vector<Eigen::Vector4d> vertex_pos_list;

public:
    void loadV(std::vector<VertexDataV>& v_list) {
        for (auto index : this->index_pos_list) {
            size_t i = 0;

            if (!trans_obj_index_to_arr(index, &i, v_list.size())) {
                printf ("failed to load line\n");
                return;
            }

            auto& v = v_list[i];

            this->vertex_pos_list.push_back(Eigen::Vector4d(v.x, v.y, v.z, 1));
        }
    }
};

struct ElementFace {
    std::vector<int> index_pos_list;
    std::vector<int> index_texture_list;
    std::vector<int> index_normal_list;

    std::vector<Eigen::Vector4d> vertex_pos_list;
    std::vector<Eigen::Vector3d> vertex_texture_list;
    std::vector<Eigen::Vector4d> vertex_normal_list;

public:
    void loadV(std::vector<VertexDataV>& v_list) {
        for (auto index : this->index_pos_list) {
            size_t i = 0;

            if (!trans_obj_index_to_arr(index, &i,  v_list.size())) {
                printf ("failed to load face\n");
                return;
            }

            auto& v = v_list[i];
            this->vertex_pos_list.push_back(Eigen::Vector4d(v.x, v.y, v.z, 1));
        }
    }

    void loadVT(std::vector<VertexDataVT>& vt_list) {
        for (auto index : this->index_texture_list) {
            size_t i = 0;

            if (!trans_obj_index_to_arr(index, &i, vt_list.size())) {
                printf ("failed to load face\n");
                return;
            }

            auto& vt = vt_list[i];
            this->vertex_texture_list.push_back(Eigen::Vector3d(vt.u, vt.v, 1));
        }
    }

    void loadVN(std::vector<VertexDataVN>& vn_list) {
        for (auto index : this->index_normal_list) {
            size_t i = 0;

            if (!trans_obj_index_to_arr(index, &i, vn_list.size())) {
                printf ("failed to load face\n");
                return;
            }

            auto& normal = vn_list[i];
            this->vertex_normal_list.push_back(Eigen::Vector4d(normal.dx, normal.dy, normal.dz, 0));
        }
    }
};

class PixelMap {
#define RGBA(r, g, b, a) ((r) | ((g) << 8) | ((b) << 16) | ((a) << 24))
#define RGB(r, g, b) RGBA(r, g, b, 0xff)
    struct Pixel {
        bool occupied;
        double depth;
        uint32_t color;
    };

    int w;
    int h;
    Pixel* pixels;
    int pixelCount;

    double depthMin;
    double depthMax;
public:
    PixelMap(int width, int height, double depth_min, double depth_max) {
        this->w = width;
        this->h = height;
        this->depthMax = depth_max;
        this->depthMin = depth_min;
        this->pixelCount = this->w * this->h;
        this->pixels = new Pixel[this->pixelCount];
        for (int i = 0; i < this->pixelCount; i++) {
            this->pixels[i].occupied = false;
        }
    }

    void setPixel(int i, int j, uint32_t color, double depth) {
        Pixel* p = &(this->pixels[i + j * this->w]);
        if (p->occupied) {
            if (depth < p->depth) {
                p->depth = depth;
                p->color = color;
            }
        } else {
            p->depth = depth;
            p->color = color;
            p->occupied = true;
        }
    }

    void drawDepthPicture() {
        libattopng_t *png = libattopng_new(this->w, this->h, PNG_RGBA);
        for (int i = 0; i < this->w; i++) {
            for (int j = 0; j < this->h; j++) {
                Pixel* p = &(this->pixels[i + j * this->w]);
                if (p->occupied) {
                    int depth_color = (p->depth - this->depthMin) / (this->depthMax - this->depthMin) * 255;
                    libattopng_set_pixel(png, i, j, RGBA(depth_color, depth_color, depth_color, 255));
                }
            }
        }
        libattopng_save(png, "test_rgba.png");
        libattopng_destroy(png);
    }

    ~PixelMap() {
        delete[] this->pixels;
    }
};

struct ModelData {
    std::vector<VertexDataV> v_list;
    std::vector<VertexDataVT> vt_list;
    std::vector<VertexDataVN> vn_list;

    std::vector<ElementPoint> point_list;
    std::vector<ElementLine> line_list;
    std::vector<ElementFace> face_list;

    Eigen::Matrix4d model;
    Eigen::Matrix4d view;
    Eigen::Matrix4d project;

    void setModel(Eigen::Matrix4d model) {
        this->model = model;
    }

    void setView(Eigen::Matrix4d view) {
        this->view = view;
    }

    void setProject(Eigen::Matrix4d project) {
        this->project = project;
    }

    bool inFace(double x, double y, ElementFace& face) {
        // auto& face = this->face_list[face_index];
        auto& v0 = face.vertex_pos_list[0];
        auto& v1 = face.vertex_pos_list[1];
        auto& v2 = face.vertex_pos_list[2];

        Eigen::Vector2d a(v0[0], v0[1]);
        Eigen::Vector2d b(v1[0], v1[1]);
        Eigen::Vector2d c(v2[0], v2[1]);

        Eigen::Vector2d m(x, y);

        auto Dab = b - a;
        auto Dam = m - a;
        auto Dbc = c - b;
        auto Dbm = m - b;
        auto Dca = a - c;
        auto Dcm = m - c;

        double c0 = Dab[0] * Dam[1] - Dab[1] * Dam[0];
        double c1 = Dbc[0] * Dbm[1] - Dbc[1] * Dbm[0];
        double c2 = Dca[0] * Dcm[1] - Dca[1] * Dcm[0];

        if (c0 >= 0 && c1 >= 0 && c2 >= 0) {
            return true;
        }
        if (c0 < 0 && c1 < 0 && c2 < 0) {
            return true;
        }

        return false;
    }

    void doRaster(int w, int h) {
        PixelMap pmap(w, h, -5, -10);

        double dx = 2.0 / w;
        double dy = 2.0 / h;

        for (auto& point : this->point_list) {
            int i = point.vertex[0] / dx + w / 2;
            int j = -point.vertex[1] / dy + h / 2;

            printf ("i : %d j : %d\n", i, j);
        }

        for (auto& face : this->face_list) {
            for (int i = 0; i < w; i++) {
                for (int j = 0; j < h; j++) {
                    if (inFace(i * dx - 1, (h-j) * dy -1, face)) {
                        pmap.setPixel(i, j, 0, -9);
                    }

                }
            }
        }
        pmap.drawDepthPicture();
    }

    void doTrans(bool align) {
        for (auto& point : this->point_list) {
            point.vertex = this->project * this->view * this->model * point.vertex;
            if (align)
                point.vertex /= point.vertex[3];
        }
        for (auto& line : this->line_list) {
            for (auto& vec3 : line.vertex_pos_list) {
                vec3 = this->project * this->view * this->model * vec3;
                if (align)
                    vec3 /= vec3[3];
            }
        }
        for (auto& face : this->face_list) {
            for (auto& vec3 : face.vertex_pos_list) {
                vec3 = this->project * this->view * this->model * vec3;
                if (align)
                    vec3 /= vec3[3];
            }
#if 0
            for (auto& vec3 : face.vertex_texture_list) {
                vec3 = this->project * this->view * this->model * vec3;
            }
            for (auto& vec3 : face.vertex_normal_list) {
                vec3 = this->project * this->view * this->model * vec3;
            }
#endif
        }
    }

    void dump() {
        Eigen::IOFormat CommaInitFmt(Eigen::FullPrecision, Eigen::DontAlignCols, ", ", ", ", "", "", " << ", ";");

        std::cout << "---------------Point :---------------" << std::endl;
        for (auto& point : this->point_list) {
            std::cout << "T" << point.vertex.transpose().format(CommaInitFmt) << std::endl;
        }

        std::cout << "---------------Line :---------------" << std::endl;
        for (auto& line : this->line_list) {
            printf ("line=>\n");
            for (auto& vec3 : line.vertex_pos_list) {
                std::cout << "T" << vec3.transpose().format(CommaInitFmt) << std::endl;
            }
        }

        std::cout << "---------------Face :---------------" << std::endl;
        for (auto& face : this->face_list) {
            printf ("face============>\n");
            printf ("pos:\n");
            for (auto& vec3 : face.vertex_pos_list) {
                std::cout << "T" << vec3.transpose().format(CommaInitFmt) << std::endl;
            }
            printf ("texture:\n");
            for (auto& vec3 : face.vertex_texture_list) {
                std::cout << "T" << vec3.transpose().format(CommaInitFmt) << std::endl;
            }
            printf ("normal:\n");
            for (auto& vec3 : face.vertex_normal_list) {
                std::cout << "T" << vec3.transpose().format(CommaInitFmt) << std::endl;
            }
        }
    }
};


#endif

