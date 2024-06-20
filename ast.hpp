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
};

struct ElementLine {
    std::vector<int> index_pos_list;

    std::vector<Eigen::Vector4d> vertex_pos_list;
};

struct ElementFace {
    std::vector<int> index_pos_list;
    std::vector<int> index_texture_list;
    std::vector<int> index_normal_list;

    std::vector<Eigen::Vector4d> vertex_pos_list;
    std::vector<Eigen::Vector3d> vertex_texture_list;
    std::vector<Eigen::Vector4d> vertex_normal_list;
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

#if 0
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
#endif

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
        #define RGBA(r, g, b, a) ((r) | ((g) << 8) | ((b) << 16) | ((a) << 24))
        #define RGB(r, g, b) RGBA(r, g, b, 0xff)

        libattopng_t *png = libattopng_new(w+1, h+1, PNG_RGBA);

        double dx = 2.0 / w;
        double dy = 2.0 / h;

        for (auto& point : this->point_list) {
            int i = point.vertex[0] / dx + w / 2;
            int j = -point.vertex[1] / dy + h / 2;

            libattopng_set_pixel(png, i, j, RGBA(255, 0, 255, 255));
            printf ("i : %d j : %d\n", i, j);
        }

        for (auto& face : this->face_list) {
            for (int i = 0; i < w; i++) {
                for (int j = 0; j < h; j++) {
                    if (inFace(i * dx - 1, (h-j) * dy -1, face)) {
                        libattopng_set_pixel(png, i, j, RGBA(0, 0, 255, 255));

                    }

                }
            }
        }

        libattopng_save(png, "test_rgba.png");
        libattopng_destroy(png);
#if 0
        for (int i = 0; i < w; i++) {
            double x0 = dx * i;
            double x1 = x0 + dx;

            for (int j = 0; j < h; j++) {
                double y0 = dy * j;
                double y1 = y0 + dy;


            }
        }
#endif
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

