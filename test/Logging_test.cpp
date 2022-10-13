//
// Created by Debraj Ray on 2022-06-10.
//

#include <gtest/gtest.h>
#include "../Logger.h"


TEST(LogStrings, BasicAssertions) {
    using namespace RTS;
    Logger logger1(LOG_LEVEL::INFO);
    logger1.logInfo("1.This should be printed\n");
    logger1.logDebug("This should NOT be printed\n");

    Logger logger2(LOG_LEVEL::DEBUG);
    logger2.logInfo("2.This should be printed\n");
    logger2.logDebug("3.This should be printed\n");

    Logger logger3(LOG_LEVEL::OFF);
    logger3.logInfo("This should NOT be printed\n");
    logger3.logDebug("This should NOT be printed\n");

    Logger::GLOBAL_LOG_LEVEL = LOG_LEVEL::DEBUG;

    Logger logger4(LOG_LEVEL::INFO);
    logger4.logInfo("4.This should be printed\n");
    logger4.logDebug("5.This should be printed\n");

    Logger logger5(LOG_LEVEL::DEBUG);
    logger5.logInfo("6.This should be printed\n");
    logger5.logDebug("7.This should be printed\n");

    Logger logger6(LOG_LEVEL::OFF);
    logger6.logInfo("This should NOT be printed\n");
    logger6.logDebug("This should NOT be printed\n");

    Logger::GLOBAL_LOG_LEVEL = LOG_LEVEL::OFF;

    Logger logger7(LOG_LEVEL::INFO);
    logger7.logInfo("This should NOT be printed\n");
    logger7.logDebug("This should NOT be printed\n");

    Logger logger8(LOG_LEVEL::DEBUG);
    logger8.logInfo("This should NOT be printed\n");
    logger8.logDebug("This should NOT be printed\n");

    Logger logger9(LOG_LEVEL::OFF);
    logger9.logInfo("This should NOT be printed\n");
    logger9.logDebug("This should NOT be printed\n");
}

TEST(LogStringsMultiple, BasicAssertions) {
    using namespace RTS;
    std::unique_ptr<Logger> logger = std::make_unique<Logger>(LOG_LEVEL::INFO);
    logger->logInfo("This")->logInfo(" ")->logInfo("is")->logInfo(" ")->logInfo("a ")->logInfo("test")->endLineInfo();
    logger->logInfo("Print this number ")->logInfo(44)->endLineInfo();
    logger->logInfo("Print this float number ")->logInfo(44.44)->endLineInfo();
}
