#include "NodeExecution.hpp"

using namespace mfep::Pipeline;

NodeBase& NodeExecution::registerNode(NodeBase* node) {
    m_nodes.push_back(std::unique_ptr<NodeBase>(node));
    return **(--m_nodes.end());
}