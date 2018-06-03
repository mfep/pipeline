#ifndef NODEEXECUTION_HPP
#define NODEEXECUTION_HPP

#include <vector>
#include <memory>
#include "NodeBase.hpp"

namespace mfep {
namespace Pipeline {

class NodeExecution {
public:
    NodeBase& registerNode(NodeBase* node);

private:
    std::vector<std::unique_ptr<NodeBase>> m_nodes;
};

}
}

#endif //NODEEXECUTION_HPP
