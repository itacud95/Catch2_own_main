#include <catch2/catch_all.hpp>
#include <iostream>
#include <string>

#include "EventListener.h"

TEST_CASE("Foo Test") {
    REQUIRE(true);
    CHECK(true);
    CHECK(false);
    CHECK(false);
}
TEST_CASE("Bar Test", "[myTag]") {
    REQUIRE(true);
    CHECK(false);
}

class CatchTestRunner {
   public:
    static CatchTestRunner& GetInstance() {
        static CatchTestRunner instance;
        return instance;
    }
    CatchTestRunner(CatchTestRunner const&) = delete;
    void operator=(CatchTestRunner const&) = delete;

    bool RunTests() {
        std::cout << "Running tests\n";
        
        failedTests.clear();
        output = "";
        
        int result = session.run();

        PrintTags(GetTags());

        return result == 0;
    }

    std::set<std::string> GetTags() { return _tags; }

   private:
    CatchTestRunner() : session(Catch::Session()) {
        SetConfigFetchTags();
        FetchTestTags();

        SetConfig();
    }

    void SetConfigFetchTags() {
        Catch::ConfigData conf;
        conf.filenamesAsTags = true;
        conf.listTests = true;

        session.useConfigData(conf);
        session.run();
    }

    void FetchTestTags() {
        auto const& testSpec = session.config().testSpec();
        for (auto const& test : Catch::getAllTestCasesSorted(session.config())) {
            for (auto const& tag : test.getTestCaseInfo().tags) {
                std::string taggg(tag.original.begin(), tag.original.size());
                _tags.insert(taggg);
            }
        }
    }

    void PrintTags(std::set<std::string> tags) {
        std::cout << "***************\n";
        for (auto tag : tags) {
            std::cout << "***: " << tag << '\n';
        }
        std::cout << "***************\n";
    }

    void SetConfig() {
        Catch::ConfigData conf;
        conf.defaultOutputFilename = "%debug";
        // config.defaultOutputFilename = "output";
        conf.defaultColourMode = Catch::ColourMode::ANSI;
        // config.testsOrTags.push_back("[myTag]");
        // config.testsOrTags = {"[myTag]"};
        session.useConfigData(conf);
    }
    Catch::Session session;
    std::set<std::string> _tags;
};

CATCH_REGISTER_LISTENER(EventListener)

int main() {
    auto tags = CatchTestRunner::GetInstance().GetTags();

    constexpr auto numruns = 4;
    for (int i = 0; i < numruns; i++) {
        auto res = CatchTestRunner::GetInstance().RunTests();
        std::cout << fullOutput << '\n';
    }
}