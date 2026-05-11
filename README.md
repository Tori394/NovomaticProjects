<h1 align="center">🚀 Novomatic Projects</h1>

<p align="center">
  Zbiór projektów w języku C++20 napisanych w ramach zadań.
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C++-blue.svg">
  <img src="https://img.shields.io/badge/CMake-blue.svg">
  <img src="https://img.shields.io/badge/Conan-yellow.svg">
  <img src="https://img.shields.io/badge/Status-Finished-brightgreen">
</p>

---

## Struktura Repozytorium

Repozytorium składa się z trzech niezależnych podprojektów:
*  **TicketingSystem** - Zadanie 1: System rezerwacji biletów oparty na architekturze Klient-Serwer.
*  **BlueprintLayout** - Zadanie 2: Aplikacja do wizualizacji i automatycznego układania grafów.
*  **LogAnalyzer** - Zadanie 2: Narzędzie do filtrowania plików logów.

## Wymagania

* Kompilator wspierający **C++20**
* **CMake** (v3.20+)
* **Conan**

## Budowa projektu

Otworzyć główny folder projektu w IDE wspierającym CMake (**CLion** lub **Visual Studio**) i pozwolić środowisku automatycznie pobrać paczki Conana oraz wygenerować pliki binarne.

### Budowanie z poziomu terminala:

1. Sklonuj repozytorium:
   ```bash
   git clone <link-do-repozytorium>
   cd NovomaticProjects
   ```
2. Zainstaluj zależności za pomocą Conana:
   ```bash
   conan install . --output-folder=build --build=missing
   ```
3. Zbuduj projekt przez CMake:
   ```bash
   cd build
   cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
   cmake --build . --config Debug
   ```

---

## 🎮 PROGRAMY

### 1. Ticketing System
* **Komunikacja:** Wykorzystuje mechanizm **Windows Named Pipes (IPC)** do dwukierunkowej wymiany danych między procesami.
* **Logika klienta:** Implementacja biletomatu zawiera **algorytm zachłanny** (Greedy Algorithm) do obliczania optymalnego wydawania reszty z ograniczonej puli monet przechowywanej w lokalnej mapie.
* **Logika serwera:** Wielowątkowy proces zarządzający globalną pulą biletów. Obsługuje rezerwacje czasowe (**Timeout Cleanup Thread**) oraz zapewnia bezpieczeństwo danych przy użyciu blokad **std::mutex**.

**Uruchamianie**

Ponieważ jest to architektura Klient-Serwer, programy muszą być uruchamiane w odpowiedniej kolejności i działać jednocześnie.

1. W pierwszej kolejności uruchom program **`Serwer`**. Otworzy on potok i będzie czekał na połączenia.
2. Następnie uruchom program **`Klient`** (możesz uruchomić kilka instancji klienta jednocześnie, aby symulować wiele biletomatów).
3. Pliki z danymi (`ticketsData.txt` oraz `moneyData.txt`) są automatycznie kopiowane do folderu z plikami `.exe` podczas budowania projektu.

**Struktura plików z danymi**

`ticketsData.txt`
```
[ilość biletów normalnych] [ilość biletów ulgowych]
[cena biletów normalnych] [cena biletów ulgowych]
```

`moneyData.txt`
```
[nominał w groszach (500-1)] [ilość monet tego typu w kasie na początku]
```


**Dostępne komendy na biletomacie:**
* `REZERWUJ NORMALNY` / `REZERWUJ ULGOWY`
* `KUP`
* `ANULUJ` (lub wpisanie `0` podczas płatności)
* `KONIEC`

**Przykład działającej aplikacji:**

<img width="1798" height="883" alt="image" src="https://github.com/user-attachments/assets/dead77d4-8fa3-4e77-9946-5c19cc46d6c8" />



### 2. Blueprint Layout
* **Grafika:** GUI tworzone przy pomocy biblioteki **Raylib**.
* **Dane:** System wczytuje i parsuje pliki konfiguracyjne przy pomocy biblioteki **nlohmann/json**.
* **Algorytm:** Za automatyczne rozmieszczenie elementów na ekranie odpowiada klasa `AutoLayout`.
  
**Uruchamianie**

Uruchom plik wykonywalny **`BlueprintLayout`**

**Przykład działającej aplikacji:**

<img width="1012" height="606" alt="image" src="https://github.com/user-attachments/assets/bdb56561-b4b7-4dcd-ab07-e6441e9b4fbe" />

### 3. Log Analyzer
* **Silnik zapytań:** Implementacja systemu filtrowania logów opartego na strukturze `Query` z polami `std::optional` — niewypełnione pole oznacza brak filtra.
* **Parser logów:** Wczytuje pliki tekstowe i parsuje każdą linię przy pomocy wyrażeń regularnych (`<regex>`) do struktury `LogStructure`.
* **Parser zapytań:** Przetwarza zapytania wpisywane w konsoli w formacie `KLUCZ="wartość"` na strukturę `Query` gotową do wykonania.

**Uruchamianie**

Uruchom plik wykonywalny **`LogAnalyzer`** 

**Dostępne klucze zapytań:**
* `LOG_LEVEL="ERROR"` — filtrowanie po poziomie logowania (`INFO`, `WARN`, `ERROR`)
* `SOURCE="AuthService"` — filtrowanie po źródle
* `MESSAGE="Transaction"` — częściowe dopasowanie treści wiadomości
* `TIMESTAMP_FROM="2023-10-25T10:00:00"` — początek przedziału czasowego
* `TIMESTAMP_TO="2023-10-25T10:10:00"` — koniec przedziału czasowego

Klucze można łączyć w dowolnych kombinacjach w jednej linii.

**Przykład działającej aplikacji:**

<img width="675" height="542" alt="image" src="https://github.com/user-attachments/assets/a90fa8d0-49ff-4b43-b682-c656a6114019" />


---

## Testy (GTest)

Logika biznesowa projektów została pokryta testami jednostkowymi z użyciem frameworka GTest.
