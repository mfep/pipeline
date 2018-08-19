#pragma once

#include <vector>
#include "Observer.hpp"

namespace mfep {
namespace Pipeline {

struct InConnBase;
struct OutConnBase;

struct NodeBase : public Observable, public Observer {
    virtual bool                   isConnected    () const = 0;
    virtual bool                   isDataAvailable() const = 0;
    virtual std::vector<NodeBase*> getInputNodes  () const = 0;
    virtual const OutConnBase*     getOutConn     (size_t index) const = 0;
    virtual void                   evaluate       () = 0;
    virtual void                   connect        (NodeBase& inputNode, size_t inputIdx, size_t outputIdx) = 0;
    virtual void                   disconnect     (size_t inputIdx) = 0;
};

struct OutConnBase {
    virtual ~OutConnBase() = default;
    virtual bool isDataAvailable() const = 0;
    virtual NodeBase* getOwnerNode() const = 0;
};

struct InConnBase {
    virtual          ~InConnBase      () = default;
    virtual void      connect         (const OutConnBase* outConn) = 0;
    virtual bool      isConnected     () const = 0;
    virtual bool      isDataAvailable () const = 0;
    virtual NodeBase* getConnectedNode() const = 0;
};

}
}
