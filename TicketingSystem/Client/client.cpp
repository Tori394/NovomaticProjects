#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <format>
#include <fstream>
#include <windows.h>
#include <sstream>

// Pipe z serwerem
std::string sendToServer(const std::string& message) {
    HANDLE hPipe = CreateFile("\\\\.\\pipe\\BiletomatPipe",
        GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    if (hPipe != INVALID_HANDLE_VALUE) {
        DWORD bytesWritten, bytesRead;
        char buffer[1024];

        WriteFile(hPipe, message.c_str(), message.length(), &bytesWritten, NULL);

        if (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
            buffer[bytesRead] = '\0';
            CloseHandle(hPipe);
            return std::string(buffer);
        }
        CloseHandle(hPipe);
    }
    return "BLAD_POLACZENIA";
}

// Pojedynczy biletomat
class TicketMachine {
private:
    std::map<int, int, std::greater<int>> cashRegister;
    int clientId;
    int ticketId = 0;

public:
    TicketMachine() {
        clientId = GetCurrentProcessId();

        std::ifstream file("moneyData.txt");
        if (file.is_open()) {
            int amount;
            int value;
            while (file >> value >> amount) {
                cashRegister[value] = amount;
            }
            file.close();
        }
        else {
            std::cout << "Blad otwarcia pliku";
        }
    }

    TicketMachine(std::map<int, int, std::greater<int>> testCash) {
        cashRegister = testCash;
        clientId = 9999; // Test id
    }

    int getId() const { return clientId; }

    std::vector<int> calculateChange(int amountToReturn) {
        std::vector<int> changeToGive;
        auto tempRegister = cashRegister;

        // zachłanny do reszty
        for (auto& [coinValue, coinCount] : tempRegister) {
            while (amountToReturn >= coinValue && coinCount > 0) {
                amountToReturn -= coinValue;
                coinCount--;
                changeToGive.push_back(coinValue);
            }
        }

        if (amountToReturn > 0) return {}; // Brak mozliwosci wydania

        cashRegister = tempRegister; // Zatwierdzenie nowej kasy
        return changeToGive;
    }

    void storeNewCoins(std::vector<int> coins) {
        for (int coin : coins) {
            cashRegister[coin]++;
        }
    }

    // Zwraca cenę lub -1 przy błędzie połączenia
    int handleReservation(std::string request) {
        std::string response = sendToServer(request + " " + std::to_string(clientId));

        if (response == "BLAD_POLACZENIA") {
            std::cout << "Blad: Brak polaczenia z serwerem.\n";
            return -1;
        }

        std::cout << "Serwer: " << response << "\n";

        // "OK" "id" "rezerwacji:" ticketId "cena:" price
        std::stringstream ss(response);
        std::string temp;
        int price = 0;

        ss >> temp; // "OK"
        if (temp != "OK") {
            std::cout << "Serwer odmowil rezerwacji.\n";
            return -1;
        }
        ss >> temp >> temp; // "id" "rezerwacji:"
        ss >> ticketId;     // numer biletu
        ss >> temp;         // "cena:"
        ss >> price;        // cena w groszach

        return price;
    }

    void handleCancel() {
        std::cout << "Anulowanie\n";
        sendToServer("ANULUJ " + std::to_string(ticketId) + " " + std::to_string(clientId));
        ticketId = 0;
    }

    // Zwraca true jeśli transakcja zakończona (kupiono lub anulowano)
    bool handlePurchase(std::string request, int price) {
        std::vector<int> insertedCoins;
        int inserted = 0;

        std::cout << "\n=== EKRAN PLATNOSCI ===\n";
        std::cout << "Cena biletu: " << std::format("{:.2f}", price / 100.0) << " zl\n";
        std::cout << "Akceptowane monety (w groszach): 500, 200, 100, 50, 20, 10, 5, 2, 1\n";
        std::cout << "Wpisz 0 by anulowac\n";

        while (inserted < price) {
            std::cout << "Wrzucono: " << std::format("{:.2f}", inserted / 100.0)
                      << " / " << std::format("{:.2f}", price / 100.0)
                      << " zl. Wrzuc monete (w gr): ";

            int coin;
            if (!(std::cin >> coin)) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                continue;
            }
            std::cin.ignore(10000, '\n');

            if (coin == 0) {
                handleCancel();
                return true; // transakcja zakończona (anulowana)
            } else if (coin == 500 || coin == 200 || coin == 100 || coin == 50 ||
                       coin == 20  || coin == 10  || coin == 5   || coin == 2  || coin == 1) {
                inserted += coin;
                insertedCoins.push_back(coin);
            } else {
                std::cout << "UWAGA: Nierozpoznana moneta. Zwracam: " << coin << " gr\n";
            }
        }

        int changeNeeded = inserted - price;
        std::cout << "\nPrzetwarzanie platnosci... Do wydania: "
                  << std::format("{:.2f}", changeNeeded / 100.0) << " zl\n";

        std::vector<int> change = calculateChange(changeNeeded);

        if (change.empty() && changeNeeded > 0) {
            std::cout << "BLAD: Automat nie ma jak wydac reszty! Zwracam monety.\n";
            sendToServer("ANULUJ " + std::to_string(ticketId) + " " + std::to_string(clientId));
            ticketId = 0;
        } else {
            std::string finalResp = sendToServer("KUP " + std::to_string(ticketId) + " " + std::to_string(clientId));
            std::cout << "Serwer: " << finalResp << "\n";

            std::stringstream ss(finalResp);
            std::string temp;
            ss>>temp;
            if (temp!="!!!") {
                storeNewCoins(insertedCoins);
                if (!change.empty() ) {
                    std::cout << "Wydana reszta (w gr):";
                    for (int c : change) std::cout << " " << c;
                    std::cout << "\n";
                }
            }
            ticketId = 0;
        }
        return true;
    }
};

int main() {
    TicketMachine machine;
    int price = 0;
    std::cout << "--- BILETOMAT (ID: " << machine.getId() << ") AKTYWNY ---\n";
    std::cout << "KOMENDY:\n- REZERWUJ NORMALNY\n- REZERWUJ ULGOWY\n- KUP\n- ANULUJ\n- KONIEC\n\n";

    std::string request = "";
    while (request != "KONIEC") {
        std::cout << "> ";
        std::getline(std::cin, request);

        if (request.find("KUP") == 0) {
            if (price <= 0) {
                std::cout << "Nalezy najpierw zarezerwowac bilet!\n";
            } else {
                machine.handlePurchase(request, price);
                price = 0;
            }
        }
        else if (request.find("REZERWUJ") == 0) {
            if (price > 0) {
                std::cout << "Juz wybrano inny bilet! Anuluj go najpierw.\n";
            } else {
                int result = machine.handleReservation(request);
                if (result > 0) {
                    price = result;
                }
            }
        }
        else if (request.find("ANULUJ") == 0) {
            if (price <= 0) {
                std::cout << "Brak aktywnej rezerwacji.\n";
            } else {
                machine.handleCancel();
                price = 0; //
            }
        }
        else if (request != "KONIEC") {
            std::cout << "Nieprawidlowa komenda\n";
        }
    }

    std::cout << "Do widzenia!\n";
    return 0;
}