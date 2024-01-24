#pragma once

#include <dlfcn.h>

#include <catch2/catch_all.hpp>
#include <cstddef>
#include <iostream>
#include <list>
#include <sstream>
#include <string>

std::string output = "no test ran";
std::vector<Catch::TestCaseStats> failedTests;
std::string fullOutput;

struct EventListener : Catch::EventListenerBase {
    using EventListenerBase::EventListenerBase;

    std::string extractFilename(const std::string& path) {
        size_t lastSlash = path.find_last_of("/\\");
        return lastSlash == std::string::npos ? path : path.substr(lastSlash + 1);
    }

    void assertionStarting(Catch::AssertionInfo const& assertionInfo) override {
        totalAssertions++;
    }

    void assertionEnded(Catch::AssertionStats const& assertionStats) override {
        if (!assertionStats.assertionResult.isOk()) {
            std::ostringstream oss;
            auto file = extractFilename(assertionStats.assertionResult.getSourceInfo().file);
            auto line = assertionStats.assertionResult.getSourceInfo().line;
            auto msg = assertionStats.assertionResult.getExpressionInMacro();
            oss << " - " << file << ":" << line << " -> " << msg << '\n';

            currentFailedAssertions.push_back(oss.str());
            failedAssertions++;
        }
    }

    void testCaseStarting(Catch::TestCaseInfo const&) override { totalTestCases++; }

    void testCaseEnded(Catch::TestCaseStats const& testCaseStats) override {
        if (!testCaseStats.totals.assertions.allPassed()) {
            std::ostringstream oss;
            auto file = extractFilename(testCaseStats.testInfo->lineInfo.file);
            auto line = testCaseStats.testInfo->lineInfo.line;
            auto name = testCaseStats.testInfo->name;
            oss << name << " (" << file << ":" << line << ")\n";

            for (const auto& assertion : currentFailedAssertions) {
                oss << assertion;
            }

            failedTestCases.push_back(oss.str());
            currentFailedAssertions.clear();
        }
    }

    void testRunEnded(Catch::TestRunStats const&) final {
        if (failedTestCases.size() < 1) {
            std::stringstream ss;
            ss << "All tests passed (" << totalAssertions << " assertion in " << totalTestCases
               << " test cases)\n";
            fullOutput = ss.str();
            return;
        }

        std::stringstream ss;
        ss << "-----\n";

        auto passedCases = totalTestCases - failedTestCases.size();
        ss << "test cases: " << totalTestCases << " | " << passedCases << " passed | "
           << failedTestCases.size() << " failed\n";

        auto passedAssertions = totalAssertions - failedAssertions;
        ss << "assertions: " << totalAssertions << " | " << passedAssertions << " passed | "
           << failedAssertions << " failed\n";
        ss << "-----\n";

        fullOutput = ss.str();
        for (const auto& failedTest : failedTestCases) {
            fullOutput.append("\n");
            fullOutput.append(failedTest);
        }
    }

    size_t totalAssertions = 0;
    size_t failedAssertions = 0;

    size_t totalTestCases = 0;
    // size_t failedTestCases = 0;

    std::vector<std::string> failedTestCases;
    std::vector<std::string> currentFailedAssertions;
};
