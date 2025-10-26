#include "obj_loader.h"
#include <iostream>
#include <vector>
#include <memory>
#include <map>

#define TINYOBJLOADER_IMPLEMENTATION
#include "thirdparty/tiny_obj_loader.h"
#include "triangle.h"
#include "vec2.h"
#include "vec3.h"

static vec2 get_uv(const tinyobj::attrib_t& attrib, const tinyobj::index_t& idx) {
    if (idx.texcoord_index >= 0 && (size_t)(2 * idx.texcoord_index + 1) < attrib.texcoords.size()) {
        double u = attrib.texcoords[2 * idx.texcoord_index + 0];
        double v = attrib.texcoords[2 * idx.texcoord_index + 1];
        return vec2(u, v);
    }
    return vec2(0.0, 0.0);
}

static point3 get_pos(const tinyobj::attrib_t& attrib, const tinyobj::index_t& idx) {
    int vi = idx.vertex_index;
    return point3(
        attrib.vertices[3 * vi + 0],
        attrib.vertices[3 * vi + 1],
        attrib.vertices[3 * vi + 2]
    );
}

static vec3 get_normal(const tinyobj::attrib_t& attrib, const tinyobj::index_t& idx) {
    if (idx.normal_index >= 0 && (size_t)(3 * idx.normal_index + 2) < attrib.normals.size()) {
        return vec3(
            attrib.normals[3 * idx.normal_index + 0],
            attrib.normals[3 * idx.normal_index + 1],
            attrib.normals[3 * idx.normal_index + 2]
        );
    }
    return vec3(0, 0, 0);
}

int load_obj_as_triangles(const std::string& filename, hittable_list& out_world, std::shared_ptr<material> default_mat) {
    tinyobj::ObjReaderConfig config;
    config.mtl_search_path = "";
    tinyobj::ObjReader reader;
    if (!reader.ParseFromFile(filename, config)) {
        if (!reader.Error().empty()) std::cerr << "TinyObjReader error: " << reader.Error() << "\n";
        return 0;
    }
    if (!reader.Warning().empty()) std::cerr << "TinyObjReader warning: " << reader.Warning() << "\n";

    const auto& attrib = reader.GetAttrib();
    const auto& shapes = reader.GetShapes();
    
    std::vector<vec3> vertex_normals(attrib.vertices.size() / 3, vec3(0, 0, 0));
    std::vector<int> vertex_counts(attrib.vertices.size() / 3, 0);
    
    bool has_normals = !attrib.normals.empty();
    
    if (!has_normals) {
        for (size_t s = 0; s < shapes.size(); ++s) {
            const tinyobj::mesh_t& mesh = shapes[s].mesh;
            size_t idx_offset = 0;
            for (size_t f = 0; f < mesh.num_face_vertices.size(); ++f) {
                int fv = mesh.num_face_vertices[f];
                if (fv >= 3) {
                    auto i0 = mesh.indices[idx_offset];
                    auto i1 = mesh.indices[idx_offset + 1];
                    auto i2 = mesh.indices[idx_offset + 2];
                    
                    point3 p0 = get_pos(attrib, i0);
                    point3 p1 = get_pos(attrib, i1);
                    point3 p2 = get_pos(attrib, i2);
                    
                    vec3 face_normal = cross(p1 - p0, p2 - p0);
                    
                    vertex_normals[i0.vertex_index] += face_normal;
                    vertex_normals[i1.vertex_index] += face_normal;
                    vertex_normals[i2.vertex_index] += face_normal;
                    vertex_counts[i0.vertex_index]++;
                    vertex_counts[i1.vertex_index]++;
                    vertex_counts[i2.vertex_index]++;
                }
                idx_offset += fv;
            }
        }
        
        for (size_t i = 0; i < vertex_normals.size(); ++i) {
            if (vertex_counts[i] > 0) {
                vertex_normals[i] = unit_vector(vertex_normals[i]);
            }
        }
    }
    
    int tri_count = 0;

    for (size_t s = 0; s < shapes.size(); ++s) {
        const tinyobj::mesh_t& mesh = shapes[s].mesh;
        size_t idx_offset = 0;
        for (size_t f = 0; f < mesh.num_face_vertices.size(); ++f) {
            int fv = mesh.num_face_vertices[f];
            if (fv < 3) {
                idx_offset += fv;
                continue;
            }
            std::vector<tinyobj::index_t> face_idx;
            face_idx.reserve(fv);
            for (size_t v = 0; v < (size_t)fv; ++v)
                face_idx.push_back(mesh.indices[idx_offset + v]);
            
            for (int k = 1; k < fv - 1; ++k) {
                auto i0 = face_idx[0];
                auto i1 = face_idx[k];
                auto i2 = face_idx[k + 1];
                
                point3 p0 = get_pos(attrib, i0);
                point3 p1 = get_pos(attrib, i1);
                point3 p2 = get_pos(attrib, i2);
                
                vec2 uv0 = get_uv(attrib, i0);
                vec2 uv1 = get_uv(attrib, i1);
                vec2 uv2 = get_uv(attrib, i2);
                
                vec3 n0, n1, n2;
                if (has_normals) {
                    n0 = get_normal(attrib, i0);
                    n1 = get_normal(attrib, i1);
                    n2 = get_normal(attrib, i2);
                } else {
                    n0 = vertex_normals[i0.vertex_index];
                    n1 = vertex_normals[i1.vertex_index];
                    n2 = vertex_normals[i2.vertex_index];
                }
                
                out_world.add(std::make_shared<triangle>(p0, p1, p2, uv0, uv1, uv2, n0, n1, n2, default_mat));
                ++tri_count;
            }
            idx_offset += fv;
        }
    }
    return tri_count;
}