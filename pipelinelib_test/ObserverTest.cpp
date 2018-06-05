#include <sstream>
#include "3rd_party/catch.hpp"
#include "Observer.hpp"

using namespace mfep::Pipeline;

struct TestObservable : public Observable {
    void changed() {
        Observable::changed();
    }
};

class TestObserver : public Observer {
public:
    explicit TestObserver(std::stringstream& ss) : m_ss(ss) {}

protected:
    void targetDeleted() override {
        Observer::targetDeleted();
        m_ss << "deleted";
    }

    void targetChanged() override {
        Observer::targetChanged();
        m_ss << "changed";
    }

private:
    std::stringstream& m_ss;
};

TEST_CASE("Basic observer operation") {
    std::stringstream ss;
    auto* observable = new TestObservable();
    TestObserver obs1(ss), obs2(ss);

    REQUIRE_THROWS_AS(observable->attach(nullptr), PipelineException);
    observable->attach(&obs1);
    observable->attach(&obs2);

    observable->changed();
    REQUIRE(ss.str() == "changedchanged");
    ss.str("");

    observable->detach(&obs1);
    observable->changed();
    REQUIRE(ss.str() == "changed");
    ss.str("");

    {
        TestObserver obs3(ss);
        observable->attach(&obs3);
        observable->changed();
        REQUIRE(ss.str() == "changedchanged");
        ss.str("");
    }
    observable->changed();
    REQUIRE(ss.str() == "changed");
    ss.str("");

    delete observable;
    REQUIRE(ss.str() == "deleted");
}
