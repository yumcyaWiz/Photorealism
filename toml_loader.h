#ifndef TOML_LOADER_H
#define TOML_LOADER_H
#include <iostream>
#include <string>
#include <cstdlib>
#include <map>
#include <vector>
#include "api.h"
#include "cpptoml/include/cpptoml.h"
#include "obj_loader.h"
class TomlLoader {
  public:
    TomlLoader() {};

    void load(const std::string& filename, std::shared_ptr<Scene>& scene, std::shared_ptr<Integrator>& integ) const {
      auto toml = cpptoml::parse_file(filename);

      //filter
      std::shared_ptr<Filter> filter;
      auto filter_toml = toml->get_table("filter");
      if(!filter_toml) { std::cerr << "Missing Filter" << std::endl; std::exit(1); }
      auto filter_type = *filter_toml->get_as<std::string>("type");
      auto filter_radius = *filter_toml->get_array_of<double>("radius");
      Vec2 fr(filter_radius[0], filter_radius[1]);
      if(filter_type == "box") {
        filter = std::make_shared<BoxFilter>(fr);
      }
      else if(filter_type == "triangle") {
        filter = std::make_shared<TriangleFilter>(fr);
      }
      else if(filter_type == "gaussian") {
        auto alpha = *filter_toml->get_as<double>("alpha");
        filter = std::make_shared<GaussianFilter>(fr, alpha);
      }
      else {
        std::cerr << "Invalid Filter type" << std::endl;
        std::exit(1);
      }

      //film
      auto film_toml = toml->get_table("film");
      if(!film_toml) { std::cerr << "Missing Film" << std::endl; std::exit(1); };
      auto resolution = *film_toml->get_array_of<int64_t>("resolution");
      std::shared_ptr<Film> film = std::make_shared<Film>(resolution[0], resolution[1], filter);
      std::cout << "Film Loaded" << std::endl;


      //camera
      auto camera = toml->get_table("camera");
      auto camera_type = *camera->get_as<std::string>("type");

      auto camera_transform = camera->get_table("transform");
      auto camera_transform_type = *camera_transform->get_as<std::string>("type");
      auto camera_transform_origin = *camera_transform->get_array_of<double>("origin");
      auto camera_transform_target = *camera_transform->get_array_of<double>("target");

      Vec3 camPos(camera_transform_origin[0], camera_transform_origin[1], camera_transform_origin[2]);
      Vec3 camTarget(camera_transform_target[0], camera_transform_target[1], camera_transform_target[2]);
      Vec3 camForward = normalize(camTarget - camPos);
      std::shared_ptr<Camera> cam;
      if(camera_type == "ideal-pinhole") {
        cam = std::make_shared<PinholeCamera>(camPos, camForward, film);
      }
      else if(camera_type == "thin-lens") {
        auto lensDistance = *camera->get_as<double>("lens-distance");
        auto focusPoint = *camera->get_array_of<double>("focus-point");
        auto fnumber = *camera->get_as<double>("f-number");
        cam = std::make_shared<ThinLensCamera>(camPos, camForward, film, Vec3(focusPoint[0], focusPoint[1], focusPoint[2]), lensDistance, fnumber);
      }
      else {
        std::cerr << "Invalid Camera type" << std::endl;
        std::exit(1);
      }
      std::cout << "Camera Loaded" << std::endl;


      //sampler
      std::shared_ptr<Sampler> sampler;
      auto sampler_toml = toml->get_table("sampler");
      auto sampler_type = *sampler_toml->get_as<std::string>("type");
      if(sampler_type == "mt") {
        sampler = std::make_shared<Mt>();
      }
      else {
        std::cerr << "Invalid Sampler type" << std::endl;
      }
      std::cout << "Sampler Loaded" << std::endl;


      //renderer
      auto renderer = toml->get_table("renderer");
      if(!renderer) { std::cerr << "Missing Renderer" << std::endl; std::exit(1); }
      int samples = *renderer->get_as<int>("samples");
      std::string integrator = *renderer->get_as<std::string>("integrator");
      if(integrator == "pt") {
        integ = std::make_shared<Pt>(cam, sampler, samples);
      }
      else if(integrator == "pt-explicit") {
        integ = std::make_shared<PtExplicit>(cam, sampler, samples);
      }
      else if(integrator == "direct") {
        integ = std::make_shared<Direct>(cam, sampler, samples);
      }
      else if(integrator == "debug") {
        integ = std::make_shared<Debug>(cam, sampler);
      }
      else if(integrator == "bdpt") {
        integ = std::make_shared<BDPT>(cam, sampler, samples);
      }
      else {
        std::cerr << "Invalid Integrator type" << std::endl;
        std::exit(1);
      }
      std::string polygon_accel = *renderer->get_as<std::string>("polygon-accel");
      std::cout << "Renderer Loaded" << std::endl;


      //sky
      std::vector<std::shared_ptr<Light>> lights;
      auto sky = toml->get_table("sky");
      auto sky_type = *sky->get_as<std::string>("type");
      std::shared_ptr<Sky> skyPtr;
      if(sky_type == "uniform") {
        auto color = *sky->get_array_of<double>("color");
        RGB col = RGB(color[0], color[1], color[2]);
        skyPtr = std::make_shared<UniformSky>(RGB(color[0], color[1], color[2]));
        if(!isZero(col)) {
          lights.push_back(skyPtr);
        }
      }
      else if(sky_type == "ibl") {
        auto path = *sky->get_as<std::string>("path");
        auto intensity = *sky->get_as<double>("intensity");
        auto theta_offset = *sky->get_as<double>("theta-offset");
        auto phi_offset = *sky->get_as<double>("phi-offset");
        skyPtr = std::make_shared<IBL>(path, intensity, phi_offset, theta_offset);
        lights.push_back(skyPtr);
      }
      else {
        std::cerr << "Invalid Sky type" << std::endl;
        std::exit(1);
      }
      std::cout << "Sky Loaded" << std::endl;


      //texture
      std::vector<std::string> texture_names;
      std::vector<std::shared_ptr<Texture>> texture_ptrs;
      auto textures = toml->get_table_array("texture");
      for(const auto& texture : *textures) {
        auto name = *texture->get_as<std::string>("name");
        auto type = *texture->get_as<std::string>("type");
        std::shared_ptr<Texture> tex;
        if(type == "uniform") {
          auto color = *texture->get_array_of<double>("color");
          RGB col(color[0], color[1], color[2]);
          tex = std::make_shared<UniformTexture>(col);
          texture_names.push_back(name);
          texture_ptrs.push_back(tex);
        }
        else if(type == "image") {
          auto path = *texture->get_as<std::string>("path");
          tex = std::make_shared<ImageTexture>(path);
          texture_names.push_back(name);
          texture_ptrs.push_back(tex);
        }
        else if(type == "checkerboard") {
          auto odd = *texture->get_as<std::string>("odd");
          auto even = *texture->get_as<std::string>("even");
          double alphaX = *texture->get_as<double>("alphaX");
          double alphaY = *texture->get_as<double>("alphaY");
          double alphaZ = *texture->get_as<double>("alphaZ");

          int index = std::find(texture_names.begin(), texture_names.end(), odd) - texture_names.begin();
          auto odd_tex = texture_ptrs[index];
          index = std::find(texture_names.begin(), texture_names.end(), even) - texture_names.begin();
          auto even_tex = texture_ptrs[index];

          tex = std::make_shared<Checkerboard>(odd_tex, even_tex, alphaX, alphaY, alphaZ);
          texture_names.push_back(name);
          texture_ptrs.push_back(tex);
        }
        else {
          std::cerr << "Invalid Texture type" << std::endl;
          std::exit(1);
        }
      }
      std::cout << "Texture Loaded" << std::endl;


      //material
      std::vector<std::string> material_names;
      std::vector<std::shared_ptr<Material>> material_ptrs;
      auto materials = toml->get_table_array("material");
      for(const auto& material : *materials) {
        auto name = *material->get_as<std::string>("name");
        auto type = *material->get_as<std::string>("type");

        std::shared_ptr<Material> mat;
        if(type == "lambert") {
          auto albedo = *material->get_as<std::string>("albedo");
          int index = std::find(texture_names.begin(), texture_names.end(), albedo) - texture_names.begin();
          auto reflectance = texture_ptrs[index];
          mat = std::make_shared<Lambert>(reflectance);
        }
        else if(type == "mirror") {
          auto albedo = *material->get_as<std::string>("albedo");
          int index = std::find(texture_names.begin(), texture_names.end(), albedo) - texture_names.begin();
          auto reflectance = texture_ptrs[index];
          mat = std::make_shared<Mirror>(reflectance);
        }
        else if(type == "glass") {
          auto albedo = *material->get_as<std::string>("albedo");
          int index = std::find(texture_names.begin(), texture_names.end(), albedo) - texture_names.begin();
          auto reflectance = texture_ptrs[index];
          auto ior = *material->get_as<double>("ior");
          mat = std::make_shared<Glass>(reflectance, ior);
        }
        else if(type == "phong") {
          auto albedo = *material->get_as<std::string>("albedo");
          int index = std::find(texture_names.begin(), texture_names.end(), albedo) - texture_names.begin();
          auto reflectance = texture_ptrs[index];
          auto kd = *material->get_as<double>("kd");
          auto alpha = *material->get_as<double>("alpha");
          mat = std::make_shared<Phong>(reflectance, kd, alpha);
        }
        else if(type == "oren-nayar") {
          auto albedo = *material->get_as<std::string>("albedo");
          int index = std::find(texture_names.begin(), texture_names.end(), albedo) - texture_names.begin();
          auto reflectance = texture_ptrs[index];
          auto sigma = *material->get_as<double>("sigma");
          mat = std::make_shared<OrenNayar>(reflectance, sigma);
        }
        else if(type == "beckmann") {
          auto albedo = *material->get_as<std::string>("albedo");
          int index = std::find(texture_names.begin(), texture_names.end(), albedo) - texture_names.begin();
          auto reflectance = texture_ptrs[index];
          auto roughness = *material->get_as<double>("roughness");
          auto ior = *material->get_as<double>("ior");
          mat = std::make_shared<Beckmann>(reflectance, roughness, ior);
        }
        else if(type == "ggx") {
          auto albedo = *material->get_as<std::string>("albedo");
          int index = std::find(texture_names.begin(), texture_names.end(), albedo) - texture_names.begin();
          auto reflectance = texture_ptrs[index];
          auto roughness = *material->get_as<double>("roughness");
          auto ior = *material->get_as<double>("ior");
          mat = std::make_shared<GGX>(reflectance, roughness, ior);
        }
        else if(type == "plastic") {
          auto albedo = *material->get_as<std::string>("albedo");
          int index = std::find(texture_names.begin(), texture_names.end(), albedo) - texture_names.begin();
          auto reflectance = texture_ptrs[index];
          auto ior = *material->get_as<double>("ior");
          mat = std::make_shared<Plastic>(reflectance, ior);
        }
        else {
          std::cerr << "Invalid Material type" << std::endl;
          std::exit(1);
        }

        material_names.push_back(name);
        material_ptrs.push_back(mat);
      }
      std::cout << "Material Loaded" << std::endl;


      //mesh
      struct MeshData {
        std::string type;
        float radius;
        std::string path;
        MeshData() {};
      };
      std::vector<std::string> mesh_names;
      std::vector<std::shared_ptr<MeshData>> mesh_ptrs;
      auto meshes = toml->get_table_array("mesh");
      for(const auto& mesh : *meshes) {
        auto name = *mesh->get_as<std::string>("name");
        auto type = *mesh->get_as<std::string>("type");
        auto meshdata = std::make_shared<MeshData>();
        if(type == "sphere") {
          auto radius = *mesh->get_as<double>("radius");
          meshdata->type = type;
          meshdata->radius = radius;
        }
        else if(type == "obj") {
          auto path = *mesh->get_as<std::string>("path");
          meshdata->type = type;
          meshdata->path = path;
        }
        else {
          std::cerr << "Invalid Mesh type" << std::endl;
          std::exit(1);
        }

        mesh_names.push_back(name);
        mesh_ptrs.push_back(meshdata);
      }
      std::cout << "Mesh Loaded" << std::endl;


      //object
      auto objects = toml->get_table_array("object");
      std::vector<std::shared_ptr<Primitive>> prims;
      std::vector<std::string> shape_names;
      std::vector<std::shared_ptr<Shape>> shape_ptrs;
      std::vector<std::shared_ptr<Primitive>> prim_ptrs;

      for(const auto& object : *objects) {
        auto name = *object->get_as<std::string>("name");
        auto mesh = *object->get_as<std::string>("mesh");
        auto material = *object->get_as<std::string>("material");

        auto transforms = object->get_table_array("transform");
        Vec3 center;
        Vec3 scale(1, 1, 1);
        for(const auto& transform : *transforms) {
          auto transform_type = *transform->get_as<std::string>("type");
          if(transform_type == "translate") {
            auto vector = *transform->get_array_of<double>("vector");
            center = Vec3(vector[0], vector[1], vector[2]);
          }
          else if(transform_type == "scale") {
            auto vector = *transform->get_array_of<double>("vector");
            scale = Vec3(vector[0], vector[1], vector[2]);
          }
          else {
            std::cerr << "Invalid Transform type" << std::endl;
            std::exit(1);
          }
        }

        int index = std::find(mesh_names.begin(), mesh_names.end(), mesh) - mesh_names.begin();
        auto meshdata = mesh_ptrs[index];
        index = std::find(material_names.begin(), material_names.end(), material) - material_names.begin();
        auto mat = material_ptrs[index];
        if(meshdata->type == "sphere") {
          auto shape = std::make_shared<Sphere>(center, meshdata->radius);
          auto prim = std::make_shared<Primitive>(shape, mat);
          prims.push_back(prim);
          shape_names.push_back(name);
          shape_ptrs.push_back(shape);
          prim_ptrs.push_back(prim);
        }
        else if(meshdata->type == "obj") {
          std::cout << "obj" << std::endl;
          loadObj(meshdata->path, center, scale, mat, prims, lights, polygon_accel);
        }
        else {
          std::cerr << "Invalid MeshData type" << std::endl;
          std::exit(1);
        }
      }
      std::cout << "Object Loaded" << std::endl;


      //light
      auto light_toml = toml->get_table_array("light");
      if(light_toml) {
        for(const auto& light : *light_toml) {
          auto light_emission = *light->get_array_of<double>("emission");
          RGB emission(light_emission[0], light_emission[1], light_emission[2]);
          auto light_type = *light->get_as<std::string>("type");

          std::shared_ptr<Light> lightPtr;
          if(light_type == "point") {
            auto pos = *light->get_array_of<double>("pos");
            Vec3 lightPos(pos[0], pos[1], pos[2]);
            lightPtr = std::make_shared<PointLight>(emission, lightPos);
          }
          else if(light_type == "area") {
            auto object = *light->get_as<std::string>("object");
            int index = std::find(shape_names.begin(), shape_names.end(), object) - shape_names.begin();
            auto shape = shape_ptrs[index];
            auto prim = prim_ptrs[index];
            lightPtr = std::make_shared<AreaLight>(emission, shape);
            prim->light = lightPtr;
          }
          else {
            std::cerr << "Invalid Light type" << std::endl;
            std::exit(1);
          }

          lights.push_back(lightPtr);
        }
      }
      std::cout << "Light Loaded" << std::endl;


      //Init Accel
      std::shared_ptr<Accel<Primitive>> accelPtr;
      std::string accel = *renderer->get_as<std::string>("accel");
      if(accel == "linear") {
        accelPtr = std::make_shared<Linear<Primitive>>(prims);
      }
      else if(accel == "bvh") {
        accelPtr = std::make_shared<BVH<Primitive>>(prims, 12);
      }


      //init scene
      scene = std::make_shared<Scene>(accelPtr, lights, skyPtr);
    };
};
#endif
