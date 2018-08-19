#pragma once

#include <cstddef>
#include "NodeBase.hpp"

namespace mfep {
namespace Pipeline {

bool isDependentOn(const NodeBase* node, const NodeBase* dependentNode);

}
}
