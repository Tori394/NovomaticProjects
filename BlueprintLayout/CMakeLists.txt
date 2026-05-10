find_package(nlohmann_json REQUIRED)
find_package(GTest REQUIRED)
find_package(raylib REQUIRED)

add_executable(BlueprintLayout
        src/main.cpp
        src/model/Node.h
        src/model/Edge.h
        src/model/Graph.h
        src/io/DataLoader.cpp
        src/io/DataLoader.h
        src/view/GraphRenderer.cpp
        src/view/GraphRenderer.h
        src/algo/AutoLayout.cpp
        src/algo/AutoLayout.h)
target_link_libraries(BlueprintLayout PRIVATE
        nlohmann_json::nlohmann_json
        raylib)

enable_testing()
add_executable(BlueprintTests tests/layout_tests.cpp)
target_link_libraries(BlueprintTests PRIVATE
        GTest::gtest_main
        nlohmann_json::nlohmann_json
)