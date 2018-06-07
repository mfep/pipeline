#ifndef NODEBASE_HPP
#define NODEBASE_HPP

#include <vector>
#include "Observer.hpp"

namespace mfep {
namespace Pipeline {

struct InConnBase;
struct OutConnBase;

struct NodeBase : public Observable {
    virtual bool                   isConnected    () const = 0;
    virtual bool                   isDataAvailable() const = 0;
    virtual void                   evaluate       () = 0;
    virtual InConnBase*            getInConn      (size_t index) = 0;
    virtual const OutConnBase*     getOutConn     (size_t index) const = 0;
    virtual std::vector<NodeBase*> getInputNodes  () const = 0;
};

struct OutConnBase {
    virtual ~OutConnBase() = default;
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

#endif //NODEBASE_HPP
