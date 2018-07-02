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
    REQUIRE(adapterInConn.getConvertedData().getInt() == 10);

    REQUIRE_NOTHROW(adapterInConn.connect(floatWrapperNode.getOutConn(0)));
    REQUIRE(adapterInConn.isConnected());
    REQUIRE_FALSE(adapterInConn.isDataAvailable());
    floatWrapperNode.evaluate();
    REQUIRE(adapterInConn.isDataAvailable());
    REQUIRE(adapterInConn.getConnectedNode() == &floatWrapperNode);
    REQUIRE(adapterInConn.getConvertedData().getFloat() == 55.f);

    REQUIRE_THROWS_AS(adapterInConn.connect(constStringNode.getOutConn(0)), PipelineException);
    REQUIRE_FALSE(adapterInConn.isConnected());
}
