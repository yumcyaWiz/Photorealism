#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H
#include <string>
#include <vector>

#ifndef TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"


loadObj(const std::string& filename) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  std::string err;
  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str());
  if(!err.empty())
    std::cerr << err << std::endl;
  if(!ret)
    std::exit(1);

  bool mtl = !materials.empty();

  int face_count = 0;
  int vertex_count = 0;
  for(size_t s = 0; s < shapes.size(); s++) {
    std::cout << "Loading " << shapes[s].name << std::endl;

    std::vector<std::shared_ptr<Triangle>> triangles;
    size_t index_offset = 0;
    int prev_material_id = 0;
    for(size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
      int fv = shapes[s].mesh.num_face_vertices[f];
      std::vector<Vec3> vertex;
      std::vector<Vec3> normal;
      std::vector<Vec2> uv;
      for(size_t v = 0; v < fv; v++) {
        tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
        tinyobj::real_t vx = attrib.vertices[3*idx.vertex_index+0];
        tinyobj::real_t vy = attrib.vertices[3*idx.vertex_index+1];
        tinyobj::real_t vz = attrib.vertices[3*idx.vertex_index+2];
        vertex.push_back(Vec3((float)vx, (float)vy, (float)vz));

        if(idx.normal_index > 0) {
          tinyobj::real_t nx = attrib.normals[3*idx.normal_index+0];
          tinyobj::real_t ny = attrib.normals[3*idx.normal_index+1];
          tinyobj::real_t nz = attrib.normals[3*idx.normal_index+2];
          normal.push_back(Vec3((float)vx, (float)vy, (float)vz));
        }
        vertex_count++;
      }
      index_offset += fv;

      std::shared_ptr<Triangle> triangle;
      if(normal.size() > 0) {
        triangle = std::make_shared<Triangle>(center + scale*vertex[0], center + scale*vertex[1], center + scale*vertex[2], normal[0], normal[1], normal[2]);
      }
      else {
        triangle = std::make_shared<Triangle>(center + scale*vertex[0], center + scale*vertex[1], center + scale*vertex[2]);
      }
      triangles.push_back(triangle);
      face_count++;
    }
  }
  std::cout << "Vertex: " << vertex_count << std::endl;
  std::cout << "Face: " << face_count << std::endl;
}


#endif
