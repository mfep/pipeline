#include <sstream>
#include "catch.hpp"
#include "NodeStructure.hpp"
#include "NodeAlgorithms.hpp"
#include "NodeExecution.hpp"

using namespace mfep::Pipeline;

class ConstIntNode : public Node<std::tuple<>, std::tuple<int>> {
public:
    explicit ConstIntNode(int value) : m_value(value) {
    }

private:
    OutData process(const InData&) const override {
        return OutData{ std::make_unique<int>(m_value) };
    }

    const int m_value;
};

class IntAddNode : public Node<std::tuple<int, int>, std::tuple<int>> {
    OutData process(const InData& input) const override {
        return OutData{ std::make_unique<int>(std::get<0>(input) + std::get<1>(input)) };
    }
};

class IntPrinterNode : public Node<std::tuple<int>, std::tuple<int>> {
public:
    explicit IntPrinterNode (std::ostream& stream) : m_stream(stream)
    {
    }

    OutData process(const InData& input) const override {
        const int value = std::get<0>(input);
        m_stream << value;
        return OutData{ std::make_unique<int>(value) };
    }

private:
    std::ostream& m_stream;
};

class ToFloatNode : public Node<std::tuple<int>, std::tuple<float>> {
    OutData process(const InData& input) const override {
        return OutData{ std::make_unique<float>(static_cast<float>(std::get<0>(input))) };
    }
};

TEST_CASE("Node operation on simple types") {
    NodeExecution exec;
    ConstIntNode n1(150), n2(-54);
    IntAddNode add;
    std::stringstream stream;
    IntPrinterNode printer(stream);

    // check being unconnected
    REQUIRE(n1.isConnected());
    REQUIRE(n2.isConnected());
    REQUIRE_FALSE(add.isConnected());
    REQUIRE_FALSE(printer.isConnected());

    // check being connected
    connect(add, n1, 0, 0);
    REQUIRE_FALSE(add.isConnected());
    connect(add, n2, 1, 0);
    REQUIRE(add.isConnected());
    connect(printer, add, 0, 0);
    REQUIRE(printer.isConnected());

    // check dependencies
    REQUIRE_FALSE(isDependentOn(&n1, &n2));
    REQUIRE_FALSE(isDependentOn(&n2, &n1));
    REQUIRE(isDependentOn(&n1, &add));
    REQUIRE(isDependentOn(&n2, &add));
    REQUIRE(isDependentOn(&n1, &printer));
    REQUIRE(isDependentOn(&n2, &printer));
    REQUIRE_FALSE(isDependentOn(&printer, &n1));

    // check data availability
    REQUIRE(n1.isDataAvailable());
    REQUIRE(n2.isDataAvailable());
    REQUIRE_FALSE(add.isDataAvailable());
    REQUIRE_FALSE(printer.isDataAvailable());

    // check evaluation
    n1.evaluate();
    REQUIRE_FALSE(add.isDataAvailable());
    n2.evaluate();
    REQUIRE(add.isDataAvailable());
    add.evaluate();
    REQUIRE(printer.isDataAvailable());
    printer.evaluate();
    REQUIRE(stream.str() == "96");
}
TEST_CASE("Check exceptions") {
    IntAddNode add1, add2;
    ToFloatNode toFloat;

    // cannot evaluate without data
    REQUIRE_THROWS_AS(toFloat.evaluate(), PipelineException);

    // cannot connect different types
    REQUIRE_THROWS_AS(connect(add1, toFloat, 0, 0), PipelineException);

    // circular dependencies are not allowed
    connect(add1, add2, 0, 0);
    REQUIRE_THROWS_AS(connect(add2, add1, 0, 0), PipelineException);
}
