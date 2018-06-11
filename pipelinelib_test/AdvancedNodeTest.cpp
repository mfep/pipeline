#include <vector>
#include "3rd_party/catch.hpp"
#include "ConstNode.hpp"

using namespace mfep::Pipeline;

struct HeapS {
    HeapS() = default;
    HeapS(const HeapS&) = default;
    HeapS(HeapS&&) = default;
    std::vector<int> m_data;
};

using HeapConstNode = ConstNode<HeapS>;
class HeapNode : public Node<tuple<HeapS>, tuple<HeapS>> {
public:
    OutData process(const InData &inData) const override {
        m_data = &std::get<0>(inData).m_data;
        return OutData{ std::make_unique<HeapS>(std::get<0>(inData)) };
    }
    const std::vector<int>* getData () const {
        return m_data;
    }

private:
    mutable const std::vector<int>* m_data = nullptr;
};

TEST_CASE("Test heap allocated type") {
    auto moveOnlyPtr = std::make_unique<HeapS>();
    moveOnlyPtr->m_data.resize(100);
    moveOnlyPtr->m_data[50] = 101;
    REQUIRE(moveOnlyPtr->m_data[50] == 101);

    auto moveOnlyCopy = std::make_unique<HeapS>(*moveOnlyPtr);
    REQUIRE(moveOnlyPtr->m_data[50] == 101);
    moveOnlyCopy->m_data[50] = 201;
    REQUIRE(moveOnlyCopy->m_data[50] == 201);
    REQUIRE(moveOnlyPtr->m_data[50] == 101);

    unique_ptr<HeapS> movedPtr = std::move(moveOnlyPtr);
    REQUIRE(movedPtr->m_data[50] == 101);
}
TEST_CASE("Test heap allocated type's node") {
    HeapS heapS;
    heapS.m_data.resize(100);
    heapS.m_data[50] = 101;

    auto node1 = std::make_unique<HeapConstNode>(std::move(heapS));
    auto node2 = std::make_unique<HeapNode>();
    node2->connect(*node1, 0, 0);
    node1->evaluate();
    node2->evaluate();
    REQUIRE(node2->getData()->at(50) == 101);
}
