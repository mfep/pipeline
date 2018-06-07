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
