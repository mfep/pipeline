#ifndef NODEEXECUTION_HPP
#define NODEEXECUTION_HPP

#include <vector>
#include <memory>
#include "NodeBase.hpp"

namespace mfep {
namespace Pipeline {

class NodeExecution {
public:
    template<typename T>
    T& registerNode(T* node) {
        m_nodes.push_back(std::unique_ptr<NodeBase>(node));
        return *node;
    }
    void execute(NodeBase* endNode);

private:
    std::vector<std::unique_ptr<NodeBase>> m_nodes;
};

}
}

#endif //NODEEXECUTION_HPP
