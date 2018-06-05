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

}

NodeBase& NodeExecution::registerNode(NodeBase* node) {
    m_nodes.push_back(std::unique_ptr<NodeBase>(node));
    return **(--m_nodes.end());
}

void NodeExecution::execute(NodeBase *endNode) {
    std::vector<NodeBase*> executionList = collectInputNodesToEvaluate(endNode);
    while(!executionList.empty()) {
        auto* node = *--executionList.end();
        node->evaluate();
        executionList.pop_back();
    }
}
