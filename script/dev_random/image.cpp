#include "runtime-headers.h"

#include "runtime-light/header.h"

const ImageInfo *vk_k2_describe() {
  static ImageInfo imageInfo {"dev_random", 1, 2, {1, 2, 3}};
  return &imageInfo;
}