#include <iostream>
#include <memory>
#include <unistd.h>
#include "api.h"
#include "toml_loader.h"

int main(int argc, char** argv) {
  std::string toml_filepath;
  int opt;
  while((opt = getopt(argc, argv, "i:on")) != -1) {
    switch(opt) {
      case 'i':
        toml_filepath = optarg;
        break;
    }
  }

  std::shared_ptr<Scene> scene;
  std::shared_ptr<Integrator> integrator;

  TomlLoader loader;
  loader.load(toml_filepath, scene, integrator);

  integrator->render(*scene);
  return 0;
}
