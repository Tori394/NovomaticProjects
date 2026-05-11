#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <vector>
#include <string>
#include <sstream>
#include <chrono>
#include <windows.h>
#include "Ticket.h"

using namespace biletomat;

// MAGAZYN
std::vector<Ticket> allTickets;
std::mutex ticketMutex;

// Watek do kontroli timeoutow
void timeoutClean() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::lock_guard<std::mutex> lock(ticketMutex);
        auto now = std::chrono::steady_clock::now();

        for (auto& ticket : allTickets) {
            if (ticket.status == TicketStatus::ZAREZERWOWANY) {
                auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(now - ticket.reservationTime).count();
                if (elapsedSeconds >= 60) {
                    std::cout << "[TIMEOUT] Rezerwacja na bilet nr " << ticket.id << " anulowana\n";
                    ticket.status = TicketStatus::DOSTEPNY;
                    ticket.ownerClientId = -1;
                }
            }
        }
    }
}

// Obsługa klienta
void handleClient(HANDLE hPipe) {
    char buffer[256];
    DWORD bytesRead, bytesWritten;

    if (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
        buffer[bytesRead] = '\0';
        std::string request(buffer);
        std::string response = "Nieznana komenda";

        std::istringstream iss(request);
        std::string action, typeStr;
        int clientId, ticketId;

        // Kody klienta
        if (request.find("REZERWUJ") == 0) {
            iss >> action >> typeStr >> clientId;
            TicketType reqType = (typeStr == "NORMALNY") ? TicketType::NORMALNY : TicketType::ULGOWY;

            // tranzakcja
            std::lock_guard<std::mutex> lock(ticketMutex);
            bool found = false;
            for (auto& ticket : allTickets) {
                if (ticket.type == reqType && ticket.status == TicketStatus::DOSTEPNY) {
                    ticket.status = TicketStatus::ZAREZERWOWANY;
                    ticket.ownerClientId = clientId;
                    ticket.reservationTime = std::chrono::steady_clock::now();
                    response = "OK id rezerwacji: " + std::to_string(ticket.id);
                    found = true;
                    break;
                }
            }
            if (!found) response = "!!! Brak biletow tego typu !!!";

        } else if (request.find("KUP") == 0) {
            iss >> action >> ticketId >> clientId;

            std::lock_guard<std::mutex> lock(ticketMutex);
            for (auto& ticket : allTickets) {
                if (ticket.id == ticketId) {
                    if (ticket.status == TicketStatus::ZAREZERWOWANY && ticket.ownerClientId == clientId) {
                        ticket.status = TicketStatus::KUPIONY;
                        response = "Bilet wydrukowany";
                    } else {
                        response = "!!!Sesja wygasla lub zly nr rezerwacji!!!";
                    }
                    break;
                }
            }
        } else if (request.find("ANULUJ") == 0) {
            iss >> action >> ticketId >> clientId;

            std::lock_guard<std::mutex> lock(ticketMutex);
            for (auto& ticket : allTickets) {
                if (ticket.id == ticketId && ticket.ownerClientId == clientId) {
                    ticket.status = TicketStatus::DOSTEPNY;
                    ticket.ownerClientId = -1;
                    response = "Anulowano rezerwacje";
                    break;
                }
            }
        }

        // informacja zwrotna
        WriteFile(hPipe, response.c_str(), response.length(), &bytesWritten, NULL);
    }

    FlushFileBuffers(hPipe);
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
}

//===================== MAIN =========================

int main() {
    // Inicjalizacja testowej puli biletów
    std::ifstream file("ticketsData.txt");
    int norAmount = 0;
    int redAmount = 0;
    if (file.is_open()) {
        if (file >> norAmount)
            for (int i = 1; i <= norAmount; ++i) {
                allTickets.push_back({i, TicketType::NORMALNY, TicketStatus::DOSTEPNY, -1});
            }
        if (file >> redAmount)
            for (int i = 1; i <= redAmount; ++i) {
            allTickets.push_back({i, TicketType::ULGOWY, TicketStatus::DOSTEPNY, -1});
        }
        file.close();
    }
    else {
        std::cout<<"Blad otwarcia pliku";
        return 1;
    }

    std::cout << "=== SERWER URUCHOMIONY ===\n";
    std::thread(timeoutClean).detach(); //osobny watek do ciaglego sprzatania
    std::cout << "Dostepne bilety: \nNormalne - " << norAmount << "\nUlogiwe - " << redAmount << "\n";

    // pipe z klientem
    while (true) {
        HANDLE hPipe = CreateNamedPipe("\\\\.\\pipe\\BiletomatPipe",
            PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES, 512, 512, 0, NULL);

        if (hPipe != INVALID_HANDLE_VALUE) {
            if (ConnectNamedPipe(hPipe, NULL)) {
                std::thread(handleClient, hPipe).detach();
            } else {
                CloseHandle(hPipe);
            }
        }
    }
    return 0;
}