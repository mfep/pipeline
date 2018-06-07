#ifndef NODEALGORITHMS_HPP
#define NODEALGORITHMS_HPP

#include <cstddef>
#include "NodeBase.hpp"

namespace mfep {
namespace Pipeline {

bool isDependentOn(const NodeBase* node, const NodeBase* dependentNode);
void connect      (NodeBase& inNode, const NodeBase& outNode, size_t inIdx = 0, size_t outIdx = 0);

}
}

#endif //NODEALGORITHMS_HPP
