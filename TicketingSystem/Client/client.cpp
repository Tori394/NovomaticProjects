#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <windows.h>

// Pipe z serwerem
std::string sendToServer(const std::string& message) {
    HANDLE hPipe = CreateFile("\\\\.\\pipe\\BiletomatPipe",
        GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    if (hPipe != INVALID_HANDLE_VALUE) {
        DWORD bytesWritten, bytesRead;
        char buffer[256];

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

public:
    TicketMachine() {
        clientId = GetCurrentProcessId();

        std::ifstream file("moneyData.txt");
        if (file.is_open()) {
            int amount;
            int value;
            while (file>>value>>amount) {
                cashRegister[value] = amount;
            }
            file.close();
        }
        else {
            std::cout<<"Blad otwarcia pliku";
        }
    }

    int getId() const {return clientId;}

    std::vector<int> calculateChange(int amountToReturn) {
        std::vector<int> changeToGive;
        auto tempRegister = cashRegister;

        //zachłanny plecakowy do reszty
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

    // mock flow
    void startPurchaseFlow() {

        // rezerwacja
        std::string response = sendToServer("REZERWUJ NORMALNY " + std::to_string(clientId));
        std::cout << "Serwer: " << response << "\n";

        if (response.find("OK") == 0) {
            size_t lastSpace = response.find_last_of(' ');
            int ticketId = std::stoi(response.substr(lastSpace + 1));

            // Cena: 3.50zl, wrzucono: 5.00zl
            std::cout << "Prosze wrzucic monety (Cena: 3.50zl)... [Wrzucasz 5.00zl]\n";
            int insertedCoin = 500;
            int changeNeeded = insertedCoin - 350;

            std::vector<int> change = calculateChange(changeNeeded);

            // reszta
            if (change.empty() && changeNeeded > 0) {
                std::cout << "BLAD: Automat nie ma jak wydac reszty! Zwracam monete.\n";
                sendToServer("ANULUJ " + std::to_string(ticketId) + " " + std::to_string(clientId));
            } else {
                cashRegister[insertedCoin]++;
                std::string finalResp = sendToServer("KUP " + std::to_string(ticketId) + " " + std::to_string(clientId));
                std::cout << "Serwer: " << finalResp << "\nReszta wydana poprawnie.\n";
            }
        }
    }
};

int main() {
    TicketMachine machine;
    std::cout << "--- BILETOMAT (ID: " << machine.getId() << ") AKTYWNY ---\n";
    std::cout << "KOMENDY:\n- REZERWUJ NORMALNY\n- REZERWUJ UGLOWY\n- KUP [id rezerwacji]";
    machine.startPurchaseFlow();

    std::cout << "\nNacisnij ENTER, aby wyjsc...";
    std::cin.get();
    return 0;
}