#include "NodeExecution.hpp"

using namespace mfep::Pipeline;

NodeBase& NodeExecution::registerNode(NodeBase* node) {
    m_nodes.push_back(std::unique_ptr<NodeBase>(node));
    return **(--m_nodes.end());
}

void NodeExecution::execute(NodeBase *endNode) {
    std::vector<NodeBase*> executionList { endNode };
    std::vector<NodeBase*> appendList = endNode->getInputNodes();
    while (!appendList.empty()) {
        executionList.insert(executionList.end(), appendList.begin(), appendList.end());
        std::vector<NodeBase*> tmpList;
        appendList.swap(tmpList);
        for (auto* node : tmpList) {
            auto inputNodes = node->getInputNodes();
            appendList.insert(appendList.end(), inputNodes.begin(), inputNodes.end());
        }
    }
    while(!executionList.empty()) {
        auto* node = *--executionList.end();
        node->evaluate();
        executionList.pop_back();
    }
}
