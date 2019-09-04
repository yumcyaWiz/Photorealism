#include <iostream>
#include <memory>
#include <unistd.h>
#include "api.h"
#include "toml_loader.h"

int main(int argc, char** argv) {
  std::shared_ptr<Scene> scene;
  std::shared_ptr<Integrator> integrator;

  TomlLoader loader;
  loader.load("rtcamp.toml", scene, integrator);

  integrator->render_rtcamp(*scene);
  return 0;
}
