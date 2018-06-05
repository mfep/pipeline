#ifndef OBSERVER_HPP
#define OBSERVER_HPP

#include <set>
#include "PipelineException.hpp"

namespace mfep {
namespace Pipeline {

class Observer;

class Observable {
public:
    virtual ~Observable();
    void attach(Observer* obs);
    void detach(Observer* obs);

protected:
    void changed() const;

private:
    std::set<Observer*> m_observers;
};

class Observer {
friend class Observable;
public:
    virtual ~Observer();

protected:
    virtual void targetDeleted();
    virtual void targetChanged();

private:
    Observable* m_target = nullptr;
};

}
}

#endif //OBSERVER_HPP
