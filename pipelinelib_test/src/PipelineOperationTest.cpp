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
    void setValue (int value) {
        m_value = value;
        invalidate();
    }

private:
    OutData process(const InData&) const override {
        return OutData{ std::make_unique<int>(m_value) };
    }
    int m_value;
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

class IntDistributorNode : public Node<std::tuple<int>, std::tuple<int, int, int>> {
    OutData process(const InData& input) const override {
        const int value = std::get<0>(input);
        return OutData{ std::make_unique<int>(value), std::make_unique<int>(value), std::make_unique<int>(value) };
    }
};

TEST_CASE("Node operation on simple types") {
    IntDistributorNode dist;
    ConstIntNode n0(1000), n1(150), n2(-54);
    IntAddNode add;
    std::stringstream stream;
    IntPrinterNode printer(stream);

    // check being unconnected
    REQUIRE(n1.isConnected());
    REQUIRE(n2.isConnected());
    REQUIRE_FALSE(add.isConnected());
    REQUIRE_FALSE(printer.isConnected());

    // check being connected
    add.connect(n0, 0, 0);
    REQUIRE_FALSE(add.isConnected());
    add.connect(n2, 1, 0);
    REQUIRE(add.isConnected());
    printer.connect(add, 0, 0);
    REQUIRE(printer.isConnected());

    // check being disconnected
    add.disconnect(0);
    REQUIRE_FALSE(add.isConnected());
    add.connect(n1, 0, 0);

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
    REQUIRE_THROWS_AS(add1.connect(toFloat, 0, 0), PipelineException);

    // circular dependencies are not allowed
    add1.connect(add2, 0, 0);
    REQUIRE_THROWS_AS(add2.connect(add1, 0, 0), PipelineException);
}
TEST_CASE("Test simple execution example") {
    NodeExecution exec;
    auto [n1, n1id] = exec.registerNode(new ConstIntNode(150));
    auto [n2, n2id] = exec.registerNode(new ConstIntNode(-54));
    auto [add, addId] = exec.registerNode(new IntAddNode());
    std::stringstream stream;
    auto [printer, printerId] = exec.registerNode(new IntPrinterNode(stream));

    add.connect(n1, 0, 0);
    add.connect(n2, 1, 0);
    printer.connect(add, 0, 0);

    exec.execute(printerId);
    REQUIRE(stream.str() == "96");
}
TEST_CASE("Execution stress test") {
    NodeExecution exec;
    size_t n = 65536;

    std::vector<NodeBase*> constNodes(n);
    for (int i = 0; i < n; ++i) {
        constNodes[i] = &exec.registerNode(new ConstIntNode(1)).first;
    }
    n /= 2;
    std::vector<NodeBase*> addNodes(n);
    for (int i = 0; i < n; ++i) {
        addNodes[i] = &exec.registerNode(new IntAddNode()).first;
        addNodes[i]->connect(*constNodes[2*i], 0, 0);
        addNodes[i]->connect(*constNodes[2*i+1], 1, 0);
    }
    while (n > 1) {
        n /= 2;
        std::vector<NodeBase*> newNodes(n);
        for (int i = 0; i < n; ++i) {
            newNodes[i] = &exec.registerNode(new IntAddNode()).first;
            newNodes[i]->connect(*addNodes[2*i], 0, 0);
            newNodes[i]->connect(*addNodes[2*i+1], 1, 0);
        }
        addNodes = std::move(newNodes);
    }
    REQUIRE(addNodes.size() == 1);
    std::stringstream ss;
    auto [printer, printerid] = exec.registerNode(new IntPrinterNode(ss));
    printer.connect(*addNodes[0], 0, 0);
    exec.execute(printerid);
    REQUIRE(ss.str() == "65536");

    // test caching
    exec.execute(printerid);
    REQUIRE(ss.str() == "65536");
}
TEST_CASE("Output to multiple nodes") {
    ConstIntNode n(42);
    std::stringstream ss;
    IntPrinterNode printer1(ss), printer2(ss);
    printer1.connect(n, 0, 0);
    printer2.connect(n, 0, 0);
    n.evaluate();
    printer1.evaluate();
    printer2.evaluate();
    REQUIRE(ss.str() == "4242");
}
TEST_CASE("Deleting nodes") {
    auto* n = new ConstIntNode(42);
    std::stringstream ss;
    IntPrinterNode printer(ss);

    printer.connect(*n, 0, 0);
    n->evaluate();
    printer.evaluate();
    REQUIRE(ss.str() == "42");

    delete n;
    REQUIRE_FALSE(printer.isConnected());
}
TEST_CASE("Changing nodes") {
    NodeExecution exec;
    auto [n1, n1id] = exec.registerNode(new ConstIntNode(1));
    auto [n2, n2id] = exec.registerNode(new ConstIntNode(3));
    auto [add, addid] = exec.registerNode(new IntAddNode());
    std::stringstream ss;
    auto [printer, printerid] = exec.registerNode(new IntPrinterNode(ss));

    add.connect(n1, 0, 0);
    add.connect(n2, 1, 0);
    printer.connect(add, 0, 0);
    exec.execute(printerid);
    exec.execute(printerid);
    REQUIRE(ss.str() == "4");
    ss.str("");
    n1.setValue(2);
    exec.execute(printerid);
    exec.execute(printerid);
    REQUIRE(ss.str() == "5");
}
TEST_CASE("Removing nodes from exec") {
    NodeExecution exec;
    auto [n1, n1id] = exec.registerNode(new ConstIntNode(1));
    auto [n2, n2id] = exec.registerNode(new ConstIntNode(3));
    auto [add, addid] = exec.registerNode(new IntAddNode());
    std::stringstream ss;
    auto [printer, printerid] = exec.registerNode(new IntPrinterNode(ss));

    add.connect(n1, 0, 0);
    add.connect(n2, 1, 0);
    printer.connect(add, 0, 0);
    exec.execute(printerid);
    exec.execute(printerid);
    REQUIRE(ss.str() == "4");

    exec.removeNode(n2id);
    REQUIRE_FALSE(add.isConnected());
    auto [n3, n3id] = exec.registerNode(new ConstIntNode(5));
    add.connect(n3, 1, 0);
    ss.str("");
    exec.execute(printerid);
    REQUIRE(ss.str() == "6");
}
