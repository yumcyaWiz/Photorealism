#ifndef TOML_LOADER_H
#define TOML_LOADER_H
#include <iostream>
#include <string>
#include <cstdlib>
#include <map>
#include <vector>
#include "api.h"
#include "cpptoml/include/cpptoml.h"
class TomlLoader {
  public:
    TomlLoader() {};

    void load(const std::string& filename, std::shared_ptr<Scene>& scene, std::shared_ptr<Integrator>& integ) const {
      auto toml = cpptoml::parse_file(filename);


      //film
      auto film_toml = toml->get_table("film");
      if(!film_toml) { std::cerr << "Missing Film" << std::endl; std::exit(1); };
      auto resolution = *film_toml->get_array_of<int64_t>("resolution");
      std::shared_ptr<Film> film = std::make_shared<Film>(resolution[0], resolution[1]);
      std::cout << "Film Loaded" << std::endl;


      //sky
      auto sky = toml->get_table("sky");
      auto sky_type = *sky->get_as<std::string>("type");
      std::shared_ptr<Sky> skyPtr;
      if(sky_type == "uniform") {
        auto color = *sky->get_array_of<double>("color");
        skyPtr = std::make_shared<UniformSky>(RGB(color[0], color[1], color[2]));
      }
      else if(sky_type == "ibl") {
        auto path = *sky->get_as<std::string>("path");
        auto intensity = *sky->get_as<double>("intensity");
        auto theta_offset = *sky->get_as<double>("theta-offset");
        auto phi_offset = *sky->get_as<double>("phi-offset");
        skyPtr = std::make_shared<IBL>(path, intensity, theta_offset, phi_offset);
      }
      else {
        std::cerr << "Invalid Sky type" << std::endl;
        std::exit(1);
      }
      std::cout << "Sky Loaded" << std::endl;


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


      //material
      std::map<std::string, std::shared_ptr<Material>> material_map;
      auto materials = toml->get_table_array("material");
      for(const auto& material : *materials) {
        auto name = *material->get_as<std::string>("name");
        auto type = *material->get_as<std::string>("type");
        std::shared_ptr<Material> mat;
        if(type == "lambert") {
          auto albedo = *material->get_array_of<double>("albedo");
          Vec3 reflectance(albedo[0], albedo[1], albedo[2]);
          mat = std::make_shared<Lambert>(reflectance);
        }
        else if(type == "mirror") {
          auto albedo = *material->get_array_of<double>("albedo");
          Vec3 reflectance(albedo[0], albedo[1], albedo[2]);
          mat = std::make_shared<Mirror>(reflectance);
        }
        else if(type == "glass") {
          auto albedo = *material->get_array_of<double>("albedo");
          Vec3 reflectance(albedo[0], albedo[1], albedo[2]);
          auto ior = *material->get_as<double>("ior");
          mat = std::make_shared<Glass>(reflectance, ior);
        }
        else if(type == "phong") {
          auto albedo = *material->get_array_of<double>("albedo");
          Vec3 reflectance(albedo[0], albedo[1], albedo[2]);
          auto kd = *material->get_as<double>("kd");
          auto alpha = *material->get_as<double>("alpha");
          mat = std::make_shared<Phong>(reflectance, kd, alpha);
        }
        else if(type == "oren-nayar") {
          auto albedo = *material->get_array_of<double>("albedo");
          Vec3 reflectance(albedo[0], albedo[1], albedo[2]);
          auto sigma = *material->get_as<double>("sigma");
          mat = std::make_shared<OrenNayar>(reflectance, sigma);
        }
        else if(type == "beckman") {
          auto albedo = *material->get_array_of<double>("albedo");
          Vec3 reflectance(albedo[0], albedo[1], albedo[2]);
          auto roughness = *material->get_as<double>("roughness");
          auto ior = *material->get_as<double>("ior");
          mat = std::make_shared<Beckman>(reflectance, roughness, ior);
        }
        else {
          std::cerr << "Invalid Material type" << std::endl;
          std::exit(1);
        }

        material_map.insert(std::make_pair(name, mat));
      }
      std::cout << "Material Loaded" << std::endl;


      //mesh
      struct MeshData {
        std::string type;
        float radius;
        MeshData() {};
      };
      std::map<std::string, std::shared_ptr<MeshData>> mesh_map;

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
        else {
          std::cerr << "Invalid Mesh type" << std::endl;
          std::exit(1);
        }

        mesh_map.insert(std::make_pair(name, meshdata));
      }
      std::cout << "Mesh Loaded" << std::endl;


      //object
      auto objects = toml->get_table_array("object");
      std::vector<std::shared_ptr<Primitive>> prims;
      std::map<std::string, std::shared_ptr<Shape>> shape_map;
      std::map<std::string, std::shared_ptr<Primitive>> prim_map;

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
          else {
            std::cerr << "Invalid Transform type" << std::endl;
            std::exit(1);
          }
        }

        auto meshdata = mesh_map.at(mesh);
        auto mat = material_map.at(material);
        if(meshdata->type == "sphere") {
          auto shape = std::make_shared<Sphere>(center, meshdata->radius);
          auto prim = std::make_shared<Primitive>(shape, mat);
          prims.push_back(prim);
          shape_map.insert(std::make_pair(name, shape));
          prim_map.insert(std::make_pair(name, prim));
        }
      }
      std::cout << "Object Loaded" << std::endl;


      //light
      auto light_toml = toml->get_table_array("light");
      std::vector<std::shared_ptr<Light>> lights;
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
            auto shape = shape_map.at(object);
            auto prim = prim_map.at(object);
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


      //init scene
      scene = std::make_shared<Scene>(prims, lights, skyPtr);


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
      else {
        std::cerr << "Invalid Integrator type" << std::endl;
      }
      std::cout << "Renderer Loaded" << std::endl;
    };
};
#endif
