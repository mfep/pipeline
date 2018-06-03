#include <iostream>
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
    OutData process(const InData& input) const override {
        const int value = std::get<0>(input);
        std::cout << value << '\n';
        return OutData{ std::make_unique<int>(value) };
    }
};

int main() {
    try {
        NodeExecution exec;
        auto& n1 = exec.registerNode(new ConstIntNode(150));
        auto& n2 = exec.registerNode(new ConstIntNode(-54));
        auto& add = exec.registerNode(new IntAddNode());
        auto& printer = exec.registerNode(new IntPrinterNode());

        connect(add, n1, 0, 0);
        connect(add, n2, 1, 0);
        connect(printer, add, 0, 0);
        exec.execute(&printer);

        std::cout << isDependentOn(&n1, &n2) << "\n"
                  << isDependentOn(&n2, &n1) << "\n"
                  << isDependentOn(&n1, &add) << "\n"
                  << isDependentOn(&n1, &printer) << "\n"
                  << isDependentOn(&printer, &n1) << std::endl;
    } catch (const PipelineException& ex) {
        ex.print(std::cout);
        return 1;
    }
    return 0;
}