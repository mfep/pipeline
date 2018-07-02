#ifndef INPUTADAPTER_HPP
#define INPUTADAPTER_HPP

#include <algorithm>
#include "NodeStructure.hpp"

namespace mfep {
namespace Pipeline {

template<typename InputTypesTuple, typename OutputType>
struct AdapterHelper {};
template<typename ... InputTypes, typename OutputType>
class AdapterHelper<tuple<InputTypes...>, OutputType> {
public:
    using InputTypesOutConnPtrTuple = tuple<const OutConn<InputTypes>* ...>;
    static void fillOutConnPtrTuple(InputTypesOutConnPtrTuple& connTuple, const OutConnBase* conn) {
        fillOutConnPtrTupleImpl(connTuple, conn, std::make_index_sequence<TupleSize>{});
    }
    static const OutConnBase* getValidElement(const InputTypesOutConnPtrTuple& connTuple) {
        return getValidElementImpl(connTuple, std::make_index_sequence<TupleSize>{});
    }
    static const OutputType& getConvertedData(const InputTypesOutConnPtrTuple& connTuple) {
        return getConvertedDataImpl(connTuple, std::make_index_sequence<TupleSize>{});
    }

private:
    static constexpr size_t TupleSize = std::tuple_size<std::decay_t<InputTypesOutConnPtrTuple >>::value;

    template<size_t ... Indices>
    static void fillOutConnPtrTupleImpl(InputTypesOutConnPtrTuple& connTuple, const OutConnBase* conn,
                                        std::index_sequence<Indices...>)
    {
        (void)(int[]){ (std::get<Indices>(connTuple) = dynamic_cast<const OutConn<InputTypes>*>(conn), 1)... };
    }
    template<size_t ... Indices>
    static const OutConnBase* getValidElementImpl(const InputTypesOutConnPtrTuple& connTuple,
                                                  std::index_sequence<Indices...>)
    {
        const std::array<const OutConnBase*, TupleSize>
                convertedPtrs { (static_cast<const OutConnBase*>(std::get<Indices>(connTuple)))... };
        const auto it = std::find_if(std::begin(convertedPtrs), std::end(convertedPtrs), [](const auto* ptr){
            return ptr != nullptr; });
        return it == std::end(convertedPtrs) ? nullptr : *it;
    };
    template<size_t ... Indices>
    static const OutputType& getConvertedDataImpl(const InputTypesOutConnPtrTuple& connTuple,
                                               std::index_sequence<Indices...>)
    {
        const OutputType* retPtr = nullptr;
        (void)(int[]) { (retPtr = std::get<Indices>(connTuple) == nullptr
                ? retPtr : static_cast<const OutputType*>(&std::get<Indices>(connTuple)->getData()), 1)... };
        return *retPtr;
    }
};

template<typename InputTypesTuple, typename OutputType>
class AdapterInConn : public InConnBase {
public:
    void connect(const OutConnBase *outConn) override {
        Helper::fillOutConnPtrTuple(m_connections, outConn);
        if (!isConnected()) {
            throw PIPELINE_EXCEPTION("cannot connect any of the adapter types");
        }
    }
    bool isConnected() const override {
        return Helper::getValidElement(m_connections) != nullptr;
    }
    bool isDataAvailable() const override {
        if (!isConnected()) {
            throw PIPELINE_EXCEPTION("Input is not connected");
        }
        return Helper::getValidElement(m_connections)->isDataAvailable();
    }
    NodeBase* getConnectedNode() const override {
        if (!isConnected()) {
            return nullptr;
        }
        return Helper::getValidElement(m_connections)->getOwnerNode();
    }
    const OutputType& getConvertedData() const {
        if (!isDataAvailable()) {
            throw PIPELINE_EXCEPTION("Data is not available on the connected output");
        }
        return Helper::getConvertedData(m_connections);
    }
private:
    using Helper = AdapterHelper<InputTypesTuple, OutputType>;
    typename Helper::InputTypesOutConnPtrTuple m_connections;
};

template<typename InputTypesTuple, typename OutputType>
class InputAdapter : public NodeBase {

};

}   // namespace Pipeline
}   // namespace mfep

#endif //INPUTADAPTER_HPP
