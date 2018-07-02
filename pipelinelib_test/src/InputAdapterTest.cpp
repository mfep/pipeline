#include "catch.hpp"
#include "ConstNode.hpp"
#include "InputAdapter.hpp"

using namespace mfep::Pipeline;

struct NumericWrapper {
    virtual int getInt() const { return 0; }
    virtual float getFloat() const { return 0.f; }
};
class IntWrapper : public NumericWrapper {
public:
    IntWrapper(int _i) : i(_i) {}
    int getInt() const override {
        return i;
    }

private:
    int i;
};
class FloatWrapper : public NumericWrapper {
public:
    FloatWrapper(float _f) : f(_f) {}
    float getFloat() const override {
        return f;
    }

private:
    float f;
};
struct NumericInputNode : Node<tuple<NumericWrapper>, tuple<int>> {
    OutData process(const InData& inData) const override {
        m_wrapper = &std::get<0>(inData);
        return OutData();
    }
    mutable const NumericWrapper* m_wrapper = nullptr;
};

using ConstIntWrapperNode = ConstNode<IntWrapper>;
using ConstFloatWrapperNode = ConstNode<FloatWrapper>;
using ConstStringNode = ConstNode<std::string>;

TEST_CASE("Input Adapter Connection") {
    AdapterInConn<tuple<IntWrapper, FloatWrapper>, NumericWrapper> adapterInConn;
    ConstIntWrapperNode intWrapperNode(10);
    ConstFloatWrapperNode floatWrapperNode(55.f);
    ConstStringNode constStringNode("test string");
    REQUIRE_NOTHROW(adapterInConn.connect(intWrapperNode.getOutConn(0)));
    REQUIRE(adapterInConn.isConnected());
    REQUIRE_FALSE(adapterInConn.isDataAvailable());
    intWrapperNode.evaluate();
    REQUIRE(adapterInConn.isDataAvailable());
    REQUIRE(adapterInConn.getConnectedNode() == &intWrapperNode);
    REQUIRE(adapterInConn.getConvertedData()->getInt() == 10);

    REQUIRE_NOTHROW(adapterInConn.connect(floatWrapperNode.getOutConn(0)));
    REQUIRE(adapterInConn.isConnected());
    REQUIRE_FALSE(adapterInConn.isDataAvailable());
    floatWrapperNode.evaluate();
    REQUIRE(adapterInConn.isDataAvailable());
    REQUIRE(adapterInConn.getConnectedNode() == &floatWrapperNode);
    REQUIRE(adapterInConn.getConvertedData()->getFloat() == 55.f);

    REQUIRE_THROWS_AS(adapterInConn.connect(constStringNode.getOutConn(0)), PipelineException);
    REQUIRE_FALSE(adapterInConn.isConnected());
}
TEST_CASE("Adapter Node Test") {
    ConstIntWrapperNode intWrapperNode(10);
    ConstFloatWrapperNode floatWrapperNode(55.f);
    ConstStringNode constStringNode("test string");
    AdapterNode<tuple<IntWrapper, FloatWrapper>, NumericWrapper> adapterNode;
    REQUIRE_FALSE(adapterNode.isConnected());

    REQUIRE_NOTHROW(adapterNode.connect(intWrapperNode, 0, 0));
    REQUIRE_FALSE(adapterNode.isDataAvailable());
    intWrapperNode.evaluate();
    REQUIRE(adapterNode.isDataAvailable());
    REQUIRE_NOTHROW(adapterNode.evaluate());

    REQUIRE_NOTHROW(adapterNode.connect(floatWrapperNode, 0, 0));
    REQUIRE_FALSE(adapterNode.isDataAvailable());
    floatWrapperNode.evaluate();
    REQUIRE(adapterNode.isDataAvailable());
    REQUIRE_NOTHROW(adapterNode.evaluate());

    REQUIRE_THROWS_AS(adapterNode.connect(constStringNode, 0, 0), PipelineException);

    NumericInputNode numericInputNode;
    REQUIRE_NOTHROW(adapterNode.connect(intWrapperNode, 0, 0));
    REQUIRE_NOTHROW(numericInputNode.connect(adapterNode, 0, 0));
    REQUIRE_NOTHROW(adapterNode.evaluate());
    REQUIRE_NOTHROW(numericInputNode.evaluate());
    REQUIRE(numericInputNode.m_wrapper->getInt() == 10);
}
