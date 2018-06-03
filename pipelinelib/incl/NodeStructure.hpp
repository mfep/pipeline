#ifndef NODESTRUCTURE_HPP
#define NODESTRUCTURE_HPP

#include <tuple>
#include <array>
#include <memory>
#include <algorithm>
#include "PipelineException.hpp"
#include "NodeBase.hpp"

namespace mfep {
namespace Pipeline {

using std::unique_ptr;
using std::array;
using std::tuple;

template<typename T>
class OutConn : public OutConnBase {
public:
    explicit OutConn (NodeBase* ownerNode) : m_ownerNode(ownerNode)
    {
    }
    bool isDataAvailable() const {
        return m_data != nullptr;
    }
    NodeBase* getOwnerNode() const {
        return m_ownerNode;
    }
    const T& getData() const {
        if (m_data == nullptr) {
            throw PIPELINE_EXCEPTION("Data pointer is null");
        }
        return *m_data;
    }
    void fillData(unique_ptr<T>& newData) {
        m_data = std::move(newData);
    }
    void clearData() {
        m_data = nullptr;
    }

private:
    unique_ptr<T> m_data = nullptr;
    NodeBase* const m_ownerNode;
};

struct DummyInConn : public InConnBase {
    void connect(const OutConnBase* outConn) override {
        throw PIPELINE_EXCEPTION("DummyInConn cannot be connected");;
    }
    bool isConnected() const override {
        return true;
    }
    bool isDataAvailable() const override {
        return true;
    }
    NodeBase* getConnectedNode() const override {
        return nullptr;
    }
};

template<typename T>
class InConn : public InConnBase {
public:
    void connect(const OutConnBase* outConn) override {
        if (outConn == nullptr) {
            m_outConn = nullptr;
        } else {
            auto outConnCast = dynamic_cast<const OutConn<T>*>(outConn);
            if (outConnCast == nullptr) {
                throw PIPELINE_EXCEPTION("Cannot connect to output because types don't match");
            }
            m_outConn = outConnCast;
        }
    }
    bool isConnected() const override {
        return m_outConn != nullptr;
    }
    bool isDataAvailable() const override {
        if (!isConnected()) {
            throw PIPELINE_EXCEPTION("Input is not connected");
        }
        return m_outConn->isDataAvailable();
    }
    NodeBase* getConnectedNode() const override {
        return isConnected() ? m_outConn->getOwnerNode() : nullptr;
    }
    const T& getData() const {
        if (!isDataAvailable()) {
            throw PIPELINE_EXCEPTION("Data is not available on the connected output");
        }
        return m_outConn->getData();
    }
private:
    const OutConn<T>* m_outConn = nullptr;
};

template<typename PtrT, typename Tuple, size_t ... Indices>
array<PtrT, sizeof...(Indices)> tupleToArrayImpl(PtrT, Tuple& tup, std::index_sequence<Indices...>) {
    return { &std::get<Indices>(tup)... };
};
template<typename PtrT, typename Tuple>
array<PtrT, std::tuple_size<std::decay_t<Tuple>>::value> tupleToArray(Tuple& tup) {
    constexpr size_t tupleSize = std::tuple_size<std::decay_t<Tuple>>::value;
    PtrT foo = nullptr;
    return tupleToArrayImpl(foo, tup, std::make_index_sequence<tupleSize>{});
};
template<>
array<InConnBase*, 1> tupleToArray<InConnBase*, tuple<DummyInConn>>(tuple<DummyInConn>& tup) {
    return { &std::get<0>(tup) };
};

template<typename ... DataTs, size_t ... Indices>
tuple<const DataTs&...> extractDataFromInputsImpl(const tuple<InConn<DataTs>...>& inputConns, std::index_sequence<Indices...>) {
    return tuple<const DataTs&...>{ std::get<Indices>(inputConns).getData()... };
};
template<typename ... DataTs>
tuple<const DataTs&...> extractDataFromInputs(const tuple<InConn<DataTs>...>& inputConns) {
    return extractDataFromInputsImpl(inputConns, std::index_sequence_for<DataTs...>{});
}
tuple<> extractDataFromInputs(const tuple<DummyInConn>&) {
    return {};
}

template<typename ... DataTs, size_t ... Indices>
void fillOutputsDataImpl(tuple<OutConn<DataTs...>>& outputs, tuple<unique_ptr<DataTs>...>& data, std::index_sequence<Indices...>) {
    using swallow = int[];
    (void)swallow{ (std::get<Indices>(outputs).fillData(std::get<Indices>(data)),1)... };
}
template<typename ... DataTs>
void fillOutputsData(tuple<OutConn<DataTs...>>& outputs, tuple<unique_ptr<DataTs>...>& data) {
    fillOutputsDataImpl(outputs, data, std::index_sequence_for<DataTs...>{});
}

template<typename ... DataTs>
struct ConnTupHelper {
};
template<>
struct ConnTupHelper<tuple<>> {
    using inTupleType = tuple<DummyInConn>;
    using inArrayType = array<InConnBase*, 1>;
    using inDataType  = tuple<>;
};
template<typename ... DataTs>
struct ConnTupHelper<tuple<DataTs...>> {
    using inTupleType  = tuple<InConn<DataTs>...>;
    using outTupleType = tuple<OutConn<DataTs>...>;
    using inArrayType  = array<InConnBase*, sizeof...(DataTs)>;
    using outArrayType = array<OutConnBase*, sizeof...(DataTs)>;
    using outDataType  = tuple<unique_ptr<DataTs>...>;
    using inDataType   = tuple<const DataTs&...>;

    static outTupleType createOutTuple(NodeBase* node) {
        return outTupleType{ OutConn<DataTs>(node)... };
    }
};

template<typename InTup, typename OutTup>
class Node : public NodeBase {
public:
    Node () :
        m_inTup (),
        m_outTup (ConnTupHelper<OutTup>::createOutTuple(this)),
        m_inArr (tupleToArray<InConnBase*, InConnTup>(m_inTup)),
        m_outArr (tupleToArray<OutConnBase*, OutConnTup>(m_outTup))
    {
    }
    InConnBase* getInConn(size_t index) override {
        if (index >= m_inArr.size()) {
            throw PIPELINE_EXCEPTION("Input overindexed");
        }
        return m_inArr[index];
    }
    const OutConnBase* getOutConn(size_t index) const override {
        if (index >= m_outArr.size()) {
            throw PIPELINE_EXCEPTION("Output overindexed");
        }
        return m_outArr[index];
    }
    bool isConnected() const override {
        for (const auto* inConn : m_inArr) {
            if (!inConn->isConnected()) {
                return false;
            }
        }
        return true;
    }
    bool isDataAvailable() const override {
        if (!isConnected()) {
            return false;
        }
        for (const auto* inConn : m_inArr) {
            if (!inConn->isDataAvailable()) {
                return false;
            }
        }
        return true;
    }
    void evaluate() override {
        if(!isDataAvailable()) {
            throw PIPELINE_EXCEPTION("Cannot evaluate, there's no data on every input");
        }
        auto inputData = extractDataFromInputs(m_inTup);
        auto outData = process(inputData);
        fillOutputsData(m_outTup, outData);
    }
    std::vector<NodeBase*> getInputNodes() const override {
        std::vector<NodeBase*> retval;
        for (const auto& inputNode : m_inArr) {
            auto* connectedNode = inputNode->getConnectedNode();
            if (connectedNode != nullptr && std::find(retval.begin(), retval.end(), connectedNode) == retval.end()) {
                retval.push_back(connectedNode);
            }
        }
        return retval;
    }

protected:
    using InData  = typename ConnTupHelper<InTup>::inDataType;
    using OutData = typename ConnTupHelper<OutTup>::outDataType;
    virtual OutData process(const InData& inData) const = 0;

private:
    using InConnTup  = typename ConnTupHelper<InTup>::inTupleType;
    using OutConnTup = typename ConnTupHelper<OutTup>::outTupleType;
    InConnTup  m_inTup;
    OutConnTup m_outTup;
    typename ConnTupHelper<InTup>::inArrayType   m_inArr;
    typename ConnTupHelper<OutTup>::outArrayType m_outArr;
};

}   // namespace Pipeline
}   // namespace mfep

#endif //NODESTRUCTURE_HPP
