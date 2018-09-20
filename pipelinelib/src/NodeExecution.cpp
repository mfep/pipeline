#include "NodeExecution.hpp"

using namespace mfep::Pipeline;

namespace {

std::vector<NodeBase*> collectInputNodesToEvaluate(NodeBase* node) {
    std::vector<NodeBase*> retval { node };
    for (auto* inputNode : node->getInputNodes()) {
        retval.push_back(inputNode);
        const auto inputNodes = collectInputNodesToEvaluate(inputNode);
        retval.insert(retval.end(), inputNodes.begin(), inputNodes.end());
    }
    return retval;
}

}   // namespace

NodeBase& NodeExecution::getNode(NodeId id) {
    return *m_nodes[id.index];
}

const NodeBase& NodeExecution::getNode(NodeId id) const {
    return *m_nodes[id.index];
}

void NodeExecution::removeNode(NodeId id) {
    m_nodes[id.index] = nullptr;
    m_freeIndices.push_back(id.index);
}

void NodeExecution::execute(NodeId id) {
    std::vector<NodeBase*> executionList = collectInputNodesToEvaluate(&getNode(id));
    while(!executionList.empty()) {
        auto* node = *--executionList.end();
        node->evaluate();
        executionList.pop_back();
    }
}

NodeId NodeExecution::registerNodeInternal(NodeBase* node) {
    size_t id;
    if (m_freeIndices.empty()) {
        m_nodes.emplace_back(node);
        id = m_nodes.size() - 1;
    } else {
        id = m_freeIndices.back();
        m_nodes[id] = std::unique_ptr<NodeBase>(node);
        m_freeIndices.pop_back();
    }
    return NodeId(id);
}
