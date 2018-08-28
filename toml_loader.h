#ifndef TOML_LOADER_H
#define TOML_LOADER_H
#include <string>
#include <cstdlib>
#include "vec3.h"
#include "film.h"
#include "sky.h"
#include "cpptoml/cpptoml.h"
class TomlLoader {
  public:
    TomlLoader() {};

    load(const std::string& filename) const {
      auto toml = cpptoml::parse_file(filename);


      //film
      auto film_toml = toml->get_table("film");
      if(!film_toml) { std::cerr << "Missing Film" << std::endl; std::exit(1) };
      auto resolution = *film_toml->get_array_of<int64_t>("resolution");
      std::shared_ptr<Film> film = std::make_shared<Film>(resolution[0], resolution[1]);
      std::cout << "Film Loaded" << std::endl;


      //sky
      auto sky = toml->get_table("sky");
      auto sky_type = *sky->get_as<std::string>("type");
      std::shared_ptr<Sky> sky;
      if(sky_type == "uniform") {
        auto color = *sky->get_array_of<double>("color");
        sky = std::make_shared<UniformSky>(RGB(color[0], color[1], color[2]));
      }
      else if(sky_type == "ibl") {
        auto path = *sky->get_as<std::string>("path");
        auto intensity = *sky->get_as<double>("intensity");
        auto theta_offset = *sky->get_as<double>("theta-offset");
        auto phi_offset = *sky->get_as<double>("phi-offset");
        sky = std::make_shared<IBL>(path, intensity, theta_offset, phi_offset);
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

    };
};
#endif
