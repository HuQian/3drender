#ifndef _AST_H_
#define _AST_H_
#include <string>
#include <vector>
#include <map>
#include <iostream>

#include <Eigen/Dense>


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


struct ObjVec3 {
    double v0;
    double v1;
    double v2;
};

struct ObjVec2 {
    double v0;
    double v1;
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
    int index;

    //ObjVec3 vertex;
    Eigen::Vector3d vertex;
};

struct ElementLine {
    std::vector<int> index_list;

    std::vector<Eigen::Vector3d> vertex_list;
};

struct ElementFace {
    std::vector<int> index_list;
    std::vector<int> index_texture_list;
    std::vector<int> index_normal_list;

    std::vector<Eigen::Vector3d> vertex_list;
    std::vector<Eigen::Vector2d> vertex_texture_list;
    std::vector<Eigen::Vector3d> vertex_normal_list;
};


struct ModelData {
    std::vector<VertexDataV> v_list;
    std::vector<VertexDataVT> vt_list;
    std::vector<VertexDataVN> vn_list;

    std::vector<ElementPoint> point_list;
    std::vector<ElementLine> line_list;
    std::vector<ElementFace> face_list;

    void dump() {
        Eigen::IOFormat CommaInitFmt(Eigen::FullPrecision, Eigen::DontAlignCols, ", ", ", ", "", "", " << ", ";");

        std::cout << "-----Point :-----" << std::endl;
        for (auto& point : this->point_list) {
            std::cout << point.vertex.transpose().format(CommaInitFmt) << std::endl;
        }

        std::cout << "-----Line :-----" << std::endl;
        for (auto& line : this->line_list) {
            printf ("line=>\n");
            for (auto& vec3 : line.vertex_list) {
                std::cout << vec3.transpose().format(CommaInitFmt) << std::endl;
            }
        }

        std::cout << "-----Face :-----" << std::endl;
        for (auto& face : this->face_list) {
            printf ("face=>\n");
            for (auto& vec3 : face.vertex_list) {
                std::cout << vec3.transpose().format(CommaInitFmt) << std::endl;
            }
        }
    }
};


#endif

