#include <dlfcn.h>

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
        failedTests.clear();
        output = "";
        static int counter = 0;
        SetConfig();
        int result = session.run();

        std::set<std::string> tags = GetTags();
        PrintTags(tags);

        return result == 0;
    }

   private:
    CatchTestRunner() : session(Catch::Session()) {
        SetConfig();
        _tags = GetTags();
        PrintTags(_tags);
    }

    std::set<std::string> GetTags() {
        std::set<std::string> tags;
        auto const& testSpec = session.config().testSpec();
        // if (testSpec.hasFilters()) {
        for (auto const& test : Catch::getAllTestCasesSorted(session.config())) {
            for (auto const& tag : test.getTestCaseInfo().tags) {
                std::string taggg(tag.original.begin(), tag.original.size());
                tags.insert(taggg);
            }
        }
        // }
        return tags;
    }

    void PrintTags(std::set<std::string> tags){
        std::cout << "***************\n";
        for (auto tag : tags) {
            std::cout << "***: " << tag << '\n';
        }
        std::cout << "***************\n";
    }

    void SetConfig() {
        auto& config = session.configData();
        config.filenamesAsTags = true;
        config.defaultOutputFilename = "%debug";
        config.defaultOutputFilename = "output";// %debug
        // config.defaultColourMode = Catch::ColourMode::ANSI;
        // config.testsOrTags.push_back("[myTag]");
        // config.testsOrTags = {"[myTag]"};
        // config.listTags = true;
        // session.useConfigData(config);
    }
    Catch::Session session;
    std::set<std::string> _tags;
};

CATCH_REGISTER_LISTENER(EventListener)

int main() {
    constexpr auto numruns = 1;
    for (int i = 0; i < numruns; i++) {
        auto res = CatchTestRunner::GetInstance().RunTests();
        std::cout << fullOutput << '\n';
    }
}