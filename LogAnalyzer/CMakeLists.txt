find_package(GTest REQUIRED)

add_executable(LogAnalyzer
        src/main.cpp
        src/algo/QueryHandler.cpp
        src/algo/QueryHandler.h
        src/algo/QueryParser.cpp
        src/algo/QueryParser.h
        src/io/LogParser.cpp
        src/io/LogParser.h
        src/model/LogStructure.h
        src/model/Query.h
)

add_custom_command(TARGET LogAnalyzer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/src/data/logs.txt"
        "$<TARGET_FILE_DIR:LogAnalyzer>/logs.txt"
)

enable_testing()
add_executable(LogAnalyzerTests
        tests/test_main.cpp
        src/algo/QueryHandler.cpp
        src/algo/QueryHandler.h
        src/algo/QueryParser.cpp
        src/algo/QueryParser.h
        src/io/LogParser.cpp
        src/io/LogParser.h
        src/model/LogStructure.h
        src/model/Query.h
)
target_compile_definitions(LogAnalyzerTests PRIVATE
        TEST_DATA_PATH="${CMAKE_CURRENT_SOURCE_DIR}/src/data/logs.txt"
)
target_link_libraries(LogAnalyzerTests PRIVATE
        GTest::gtest_main
)
add_test(NAME AllTests COMMAND LogAnalyzerTests)