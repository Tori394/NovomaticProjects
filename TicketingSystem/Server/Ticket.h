#pragma once
#include <chrono>

namespace biletomat {

    enum class TicketStatus {
        DOSTEPNY,
        ZAREZERWOWANY,
        KUPIONY
    };
    enum class TicketType {
        NORMALNY,
        ULGOWY };

    struct Ticket {
        int id;
        TicketType type;
        TicketStatus status = TicketStatus::DOSTEPNY;

        int ownerClientId = -1;

        std::chrono::steady_clock::time_point reservationTime;
    };

}