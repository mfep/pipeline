#pragma once

#include <vector>
#include <memory>
#include "NodeBase.hpp"

namespace mfep {
namespace Pipeline {

class NodeExecution;

struct NodeId {
friend class NodeExecution;
private:
    explicit NodeId(size_t _index) : index(_index) {}
    size_t index;
};

class NodeExecution {
public:
    template<class T>
    std::pair<T&, NodeId> registerNode(T* node) {
        return std::pair<T&, NodeId>{ *node, registerNodeInternal(node) };
    }
    NodeBase& getNode(NodeId id);
    const NodeBase& getNode(NodeId id) const;
    void removeNode(NodeId id);
    void execute(NodeId id);

    template<class T>
    T& getNodeT(NodeId id) { return dynamic_cast<T&>(getNode(id)); }
    template<class T>
    const T& getNodeT(NodeId id) const { return dynamic_cast<const T&>(getNode(id)); }

private:
    NodeId registerNodeInternal(NodeBase* node);
    std::vector<std::unique_ptr<NodeBase>> m_nodes;
    std::vector<size_t> m_freeIndices;
};

}   // namespace Pipeline
}   // namespace mfep
