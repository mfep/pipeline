#ifndef CONSTNODE_HPP
#define CONSTNODE_HPP

#include "NodeStructure.hpp"

namespace mfep {
namespace Pipeline {

template<typename T>
class ConstNode : public Node<tuple<>, tuple<T>> {
public:
    ConstNode() : m_data()
    {
    }
    explicit ConstNode(T&& data) : m_data(data)
    {
    }
    tuple<unique_ptr<T>> process(const tuple<>& inData) const override {
        return tuple<unique_ptr<T>>{ std::make_unique<T>(m_data) };
    }
    const T& getData() const {
        return m_data;
    }
    void setData(const T& data) {
        m_data = data;
    }

private:
    T m_data;
};

}
}

#endif //CONSTNODE_HPP
