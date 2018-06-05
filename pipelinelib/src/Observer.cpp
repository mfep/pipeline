#include "Observer.hpp"

mfep::Pipeline::Observable::~Observable() {
    for (auto* observer : m_observers) {
        observer->m_target = nullptr;
        observer->targetDeleted();
    }
}

void mfep::Pipeline::Observable::attach(mfep::Pipeline::Observer *obs) {
    if (obs == nullptr) {
        throw PIPELINE_EXCEPTION("Cannot insert nullptr to observers");
    }
    obs->m_target = this;
    m_observers.insert(obs);
}

void mfep::Pipeline::Observable::detach(mfep::Pipeline::Observer *obs) {
    m_observers.erase(obs);
}

void mfep::Pipeline::Observable::changed() const {
    for (auto* observer : m_observers) {
        observer->targetChanged();
    }
}

mfep::Pipeline::Observer::~Observer() {
    m_target->detach(this);
}

void mfep::Pipeline::Observer::targetDeleted() {}

void mfep::Pipeline::Observer::targetChanged() {}
