#include <gtest/gtest.h>
#include "../src/algo/QueryHandler.h"
#include "../src/algo/QueryParser.h"
#include "../src/io/LogParser.h"

using namespace analyzer;


LogStructure makeEntry(const std::string& raw) {
    LogParser parser;
    return parser.parseLog(const_cast<std::string&>(raw));
}

std::vector<LogStructure> makeLogs() {
    return {
        makeEntry("[2023-10-25T10:00:00] [INFO] [AuthService] User logged in successfully"),
        makeEntry("[2023-10-25T10:05:12] [ERROR] [Database] Connection timeout"),
        makeEntry("[2023-10-25T10:15:30] [WARN] [AuthService] Multiple failed login attempts"),
        makeEntry("[2023-10-25T10:20:00] [ERROR] [Payment] Transaction rejected: insufficient funds"),
        makeEntry("[2023-10-25T10:25:00] [INFO] [Payment] Transaction 12345 processed"),
    };
}


// Testy LogParser
// ============================================================

TEST(LogParser, ParseCorrect) {
    auto entry = makeEntry("[2023-10-25T10:00:00] [INFO] [AuthService] User logged in successfully");
    EXPECT_EQ(entry.level,   "INFO");
    EXPECT_EQ(entry.source,  "AuthService");
    EXPECT_EQ(entry.message, "User logged in successfully");
}

TEST(LogParser, ParseLevelError) {
    auto entry = makeEntry("[2023-10-25T10:05:12] [ERROR] [Database] Connection timeout");
    EXPECT_EQ(entry.level,  "ERROR");
    EXPECT_EQ(entry.source, "Database");
}

TEST(LogParser, ParseMessageWithColon) {
    auto entry = makeEntry("[2023-10-25T10:20:00] [ERROR] [Payment] Transaction rejected: insufficient funds");
    EXPECT_EQ(entry.message, "Transaction rejected: insufficient funds");
}

TEST(LogParser, LoadFile) {
    LogParser parser;
    auto logs = parser.logs(TEST_DATA_PATH);
    EXPECT_EQ(logs.size(), 5);
}


// Testy QueryParser
// ============================================================

TEST(QueryParser, ParseLevel) {
    QueryParser parser;
    auto query = parser.parse(R"(LOG_LEVEL="ERROR")");
    ASSERT_TRUE(query.level.has_value());
    EXPECT_EQ(*query.level, "ERROR");
}

TEST(QueryParser, ParseSource) {
    QueryParser parser;
    auto query = parser.parse(R"(SOURCE="AuthService")");
    ASSERT_TRUE(query.source.has_value());
    EXPECT_EQ(*query.source, "AuthService");
}

TEST(QueryParser, ParseMessage) {
    QueryParser parser;
    auto query = parser.parse(R"(MESSAGE="Transaction")");
    ASSERT_TRUE(query.message.has_value());
    EXPECT_EQ(*query.message, "Transaction");
}

TEST(QueryParser, ParseTimestampFrom) {
    QueryParser parser;
    auto query = parser.parse(R"(TIMESTAMP_FROM="2023-10-25T10:00:00")");
    EXPECT_TRUE(query.timestampFrom.has_value());
}

TEST(QueryParser, ParseCombination) {
    QueryParser parser;
    auto query = parser.parse(R"(LOG_LEVEL="ERROR" SOURCE="Database")");
    ASSERT_TRUE(query.level.has_value());
    ASSERT_TRUE(query.source.has_value());
    EXPECT_EQ(*query.level,  "ERROR");
    EXPECT_EQ(*query.source, "Database");
}

TEST(QueryParser, EmptyLine) {
    QueryParser parser;
    auto query = parser.parse("");
    EXPECT_FALSE(query.level.has_value());
    EXPECT_FALSE(query.source.has_value());
    EXPECT_FALSE(query.message.has_value());
}


// Testy QueryHandler — filtrowanie
// ============================================================

TEST(QueryHandler, FilterByLevel) {
    auto logs = makeLogs();
    QueryHandler handler;
    Query query;
    query.level = "ERROR";

    auto results = handler.execute(logs, query);
    ASSERT_EQ(results.size(), 2);
    for (const auto& r : results)
        EXPECT_EQ(r.level, "ERROR");
}

TEST(QueryHandler, FilterBySource) {
    auto logs = makeLogs();
    QueryHandler handler;
    Query query;
    query.source = "AuthService";

    auto results = handler.execute(logs, query);
    ASSERT_EQ(results.size(), 2);
    for (const auto& r : results)
        EXPECT_EQ(r.source, "AuthService");
}

TEST(QueryHandler, FilterBySubstringOfMessage) {
    auto logs = makeLogs();
    QueryHandler handler;
    Query query;
    query.message = "Transaction";

    auto results = handler.execute(logs, query);
    ASSERT_EQ(results.size(), 2);
}

TEST(QueryHandler, FilterByTimestamp) {
    auto logs = makeLogs();
    QueryHandler handler;
    QueryParser parser;

    auto query = parser.parse(
        R"(TIMESTAMP_FROM="2023-10-25T10:00:00" TIMESTAMP_TO="2023-10-25T10:10:00")"
    );

    auto results = handler.execute(logs, query);
    ASSERT_EQ(results.size(), 2); // 10:00 i 10:05
}

TEST(QueryHandler, FilterCombination) {
    auto logs = makeLogs();
    QueryHandler handler;
    QueryParser parser;

    auto query = parser.parse(
        R"(LOG_LEVEL="ERROR" TIMESTAMP_FROM="2023-10-25T10:00:00" TIMESTAMP_TO="2023-10-25T10:10:00")"
    );

    auto results = handler.execute(logs, query);
    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(results[0].source, "Database");
}

TEST(QueryHandler, NoResults) {
    auto logs = makeLogs();
    QueryHandler handler;
    Query query;
    query.level = "DEBUG"; // nie ma takich wpisów

    auto results = handler.execute(logs, query);
    EXPECT_TRUE(results.empty());
}

TEST(QueryHandler, EmptyQuery_ReturnsAll) {
    auto logs = makeLogs();
    QueryHandler handler;
    Query query; // wszystkie pola puste

    auto results = handler.execute(logs, query);
    EXPECT_EQ(results.size(), logs.size());
}

TEST(QueryHandler, EmptyLogLine) {
    QueryHandler handler;
    Query query;
    query.level = "ERROR";

    auto results = handler.execute({}, query);
    EXPECT_TRUE(results.empty());
}