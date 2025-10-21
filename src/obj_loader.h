#pragma once
#include <string>
#include <memory>
#include "hittable_list.h"
#include "material.h"

int load_obj_as_triangles(const std::string& filename,
    hittable_list& out_world,std::shared_ptr<material> default_mat);