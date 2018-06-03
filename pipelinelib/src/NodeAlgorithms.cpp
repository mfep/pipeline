#include "NodeAlgorithms.hpp"
#include "PipelineException.hpp"

bool mfep::Pipeline::isDependentOn(const mfep::Pipeline::NodeBase* node,
                                   const mfep::Pipeline::NodeBase* dependentNode) {
    for (const NodeBase* n : dependentNode->getInputNodes()) {
        if (n == node) {
            return true;
        }
        if (isDependentOn(node, n)) {
            return true;
        }
    }
    return false;
}

void mfep::Pipeline::connect(NodeBase& inNode, const NodeBase& outNode, size_t inIdx, size_t outIdx) {
    if (isDependentOn(&inNode, &outNode)) {
        throw PIPELINE_EXCEPTION("Cannot connect: output node is dependent on input node");
    }
    inNode.getInConn(inIdx)->connect(outNode.getOutConn(outIdx));
};
