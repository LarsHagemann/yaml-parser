#ifndef YAML_WRITE_HPP
#define YAML_WRITE_HPP

#include "yaml-types.hpp"

#include <iostream>

namespace yaml {

static void YamlWrite(std::ostream& stream, const YamlValue& value) {
    value.Write(stream);
}

}

#endif