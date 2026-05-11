#include <iostream>
#include <string>
#include "io/LogParser.h"
#include "algo/QueryParser.h"
#include "algo/QueryHandler.h"

int main() {

    analyzer::LogParser logParser;
    std::vector<analyzer::LogStructure> logs;
    std::string filename="data/logs.txt";

    try {
        logs = logParser.logs(filename);
    } catch (const std::exception& e) {
        std::cerr << "Blad wczytywania pliku: " << e.what() << "\n";
        return 1;
    }

    std::cout << "Wczytano " << logs.size() << " wpisow.\n";
    std::cout << "Dostepne klucze: LOG_LEVEL, SOURCE, MESSAGE, TIMESTAMP_FROM, TIMESTAMP_TO\n";
    std::cout << "Przyklad: LOG_LEVEL=\"ERROR\" SOURCE=\"Database\"\n";
    std::cout << "Wpisz 'quit' aby wyjsc.\n\n";

    analyzer::QueryParser queryParser;
    analyzer::QueryHandler queryHandler;

    std::string line;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);

        if (line == "quit") break;
        if (line.empty()) continue;

        analyzer::Query query = queryParser.parse(line);
        std::vector<analyzer::LogStructure> results = queryHandler.execute(logs, query);

        if (results.empty()) {
            std::cout << "Brak wynikow.\n\n";
        } else {
            std::cout << "Znaleziono " << results.size() << " wynikow:\n";
            for (const auto& entry : results) {
                std::cout << entry.origin << "\n";
            }
            std::cout << "\n";
        }
    }

    return 0;
}