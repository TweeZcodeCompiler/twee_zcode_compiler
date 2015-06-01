//
// Created by philip on 31.05.15.
//

#ifndef PROJECT_TEST_H
#define PROJECT_TEST_H


#include <vector>
#include <string>
#include <bitset>

/*
 * Test Framework:
 * All tests have to extend Test.
 * Use Test::addTest() to add a test to the framework.
 * Use Test::executeAllTests() to execute the added tests.
 * Use Test::deleteTests() after execution to call delete for each added test.
 */
class Test {
private:
    static std::vector<Test*> tests ;
    static std::vector<std::string> names ;
    static int numberOfFailedTests;
    static int numberOfOtherTests;
    static int numberOfSuccessedTests;
    static bool runIgnoredTests;
    std::string bitsetToFormString(std::bitset<8>);

public:
    //Use this method to install a Test with a name to the test framework.
    static void addTest(std::string name, Test *t);
    //This method call delete for each installed test.
    //Run this after test execution to avoid memory leaks.
    static void deleteTests();
    //Run all installed tests.
    //Install a test with Test::addTest
    //If runIgnoredTests is true, the test framework will also perform ignored tests.
    static void executeAllTests(bool runIgnoredTests);

    //All tests have to implement this method.
    //This is the main entry point for each test.
    virtual void runTest(){};

    //Ignore the test
    void ignoreTestResult();
    //Define a custom test result
    void customTestResult(std::string message);
    //Let the test fail.
    void fail(std::string message);

    //The following methods will compare values.
    //Are the submitted values not equal, the test will fail.
    void assertsEquals(int expected, int value);
    void assertsEquals(std::string expected, std::string value);
    void assertsEquals(std::vector<std::bitset<8>> expected, std::vector<std::bitset<8>> value);
    void assertsEquals(bool expected, bool value);

};


#endif //PROJECT_TEST_H
