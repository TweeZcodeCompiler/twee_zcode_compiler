//
// Created by philip on 31.05.15.
//

#include "Test.h"
#include <iostream>
#include <sstream>

using std::cout;
using std::string;

std::vector<Test *> Test::tests = std::vector<Test *>();
std::vector<string> Test::names = std::vector<string>();
int Test::numberOfFailedTests = 0;
int Test::numberOfSuccessedTests = 0;
int Test::numberOfOtherTests = 0;
bool Test::runIgnoredTests = false;

void Test::addTest(string name, Test *t) {

    tests.push_back(t);
    names.push_back(name);
}

void Test::executeAllTests(bool runIgnoredTests) {
    Test::numberOfFailedTests = 0;
    Test::numberOfSuccessedTests = 0;
    Test::runIgnoredTests = runIgnoredTests;
    cout << "Run " << tests.size() << " tests:\n";
    for (int i = 0; i < tests.size(); i++) {
        Test *t = tests[i];
        string name = names[i];
        cout << "\n\n>>> Run '" << name << "' :";
        try {
            t->runTest();
            cout << "\n>>> SUCCESSFUL";
            Test::numberOfSuccessedTests++;
        } catch (string s) {
            if (s == "ignoreTestResult") {
                Test::numberOfOtherTests++;
                cout << "\n>>> IGNORED";
            } else if (s == "fail") {
                Test::numberOfFailedTests++;
                cout << "\n>>> FAILED";
            } else {
                Test::numberOfOtherTests++;
                cout << "\n>>> CUSTOM [" << s << "]";
            }
        }
    }
    cout << "\n\nTest execution finished:\nTotal:\t\t" << (Test::tests.size()) << "\n"
    << "Okay:\t\t" << Test::numberOfSuccessedTests << "\n"
    << "Failed:\t\t" << Test::numberOfFailedTests << "\n" <<
    "Other:\t\t" << Test::numberOfOtherTests << "\nEND";
}

void Test::deleteTests() {
    for (Test *t: tests) {
        delete t;
    }
}

void Test::assertsEquals(int expected, int value) {
    if (expected != value) {
        cout << "\nexpects <" << expected << "> but found <" << value << ">";
        throw string("fail");
    }
}

void Test::assertsEquals(std::string expected, std::string value) {
    if (expected != value) {
        cout << "\nexpects <" << expected << ">, but found <" << value << ">";
        throw string("fail");
    }
}

void Test::assertsEquals(std::vector<std::bitset<8>> expected, std::vector<std::bitset<8>> value) {
    if (expected.size() != expected.size()) {
        cout << "\nexpects vector size <" << expected.size() << ">, but found size <" << value.size() << ">";
        throw "fail";
    }
    for (int i = 0; i < expected.size(); i++) {
        if (expected[i] != value[i]) {
            cout << "\n expects value <" << bitsetToFormString(expected[i]) << ">, but found <" <<
            bitsetToFormString(value[i]) <<
            "> at " << i;
            throw string("fail");
        }
    }
}

void Test::assertsEquals(bool expected, bool value) {
    if (expected != value) {
        cout << "\nexpects <" << expected << ">, but found <" << value << ">";
        throw string("fail");
    }
}


std::string Test::bitsetToFormString(std::bitset<8> bitset) {
    std::ostringstream o;
    o << "[";
    for (int i = 7; i >= 0; i--) {
        o << bitset[i];
    }
    o << "] (";
    o << bitset.to_ullong() << ")";
    return o.str();
}

void Test::fail(std::string message) {
    cout << "\n" << message;
    throw string("fail");
}

void Test::ignoreTestResult() {
    if (!Test::runIgnoredTests) {
        throw string("ignoreTestResult");
    }
}

void Test::customTestResult(std::string message) {
    throw string(message);
}