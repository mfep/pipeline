#ifndef NODEALGORITHMS_HPP
#define NODEALGORITHMS_HPP

#include <cstddef>
#include "NodeBase.hpp"

namespace mfep {
namespace Pipeline {

bool isDependentOn(const NodeBase* node, const NodeBase* dependentNode);

}
}

#endif //NODEALGORITHMS_HPP
