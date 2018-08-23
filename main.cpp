#include <iostream>
#include "vec3.h"
#include "ray.h"


int main() {
  Ray ray(Vec3(0, 0, 0), Vec3(0, 0, 1));
  std::cout << ray(10) << std::endl;
}
