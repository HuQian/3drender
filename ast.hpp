#ifndef _AST_H_
#define _AST_H_
#include <string>
#include <vector>
#include <map>


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

    ObjVec3 vertex;
};

struct ElementLine {
    std::vector<int> index_list;

    std::vector<ObjVec3> vertex_list;
};

struct ElementFace {
    std::vector<int> index_list;
    std::vector<int> index_texture_list;
    std::vector<int> index_normal_list;

    std::vector<ObjVec3> vertex_list;
    std::vector<ObjVec2> vertex_texture_list;
    std::vector<ObjVec3> vertex_normal_list;
};


struct ModelData {
    std::vector<VertexDataV> v_list;
    std::vector<VertexDataVT> vt_list;
    std::vector<VertexDataVN> vn_list;

    std::vector<ElementPoint> point_list;
    std::vector<ElementLine> line_list;
    std::vector<ElementFace> face_list;

    void dump() {
        std::cout << "-----Point :-----" << std::endl;
        for (auto& point : this->point_list) {
            auto& vec3 = point.vertex;

            printf ("%lf %lf %lf\n", vec3.v0, vec3.v1, vec3.v2);
        }

        std::cout << "-----Line :-----" << std::endl;
        for (auto& line : this->line_list) {
            printf ("line=>\n");
            for (auto& vec3 : line.vertex_list) {
                printf ("%lf %lf %lf\n", vec3.v0, vec3.v1, vec3.v2);
            }
        }

        std::cout << "-----Face :-----" << std::endl;
        for (auto& face : this->face_list) {
            printf ("face=>\n");
            for (auto& vec3 : face.vertex_list) {
                printf ("%lf %lf %lf\n", vec3.v0, vec3.v1, vec3.v2);
            }
        }
    }
};


#endif

