Struktura projektu:

Program składa się z 3 głównych plików:

zmonitor2.cpp - głównego pliku z kodem, który jest opisem działania monitora, nie wymaga żadnej interakcji z programistom chcącym z niego korzystać.
zmonitor.hpp - plik nagłówkowy, który należy dołączyć gdy chcemy korzystać z monitora w naszym projekcie.
config.hpp -  plik nagłówkowy zawierający zmienną określającą ilość monitorów oraz trzy zmienne tablicowe: clients - opisujący adresy pod którymi znajdują się monitory, serwers opisujący na jakich portach/adresach mają zostać wystartowane monitory oraz priorities umożliwiające wskazanie, które monitory mają zachowywać się w sposób uprzywilejowany (tzn. gdy 2 będą się ubiegać w tym samym czasie priorytet zdecyduje który uzyska strefę krytyczną jako pierwszy).

Użycie:
Programy należy skompilować w następujący sposób:
g++ [nazwa] -std=c++11 -lzmq
Przed uruchomieniem programu korzystającego z monitora należy uruchomić sam monitor uprzednio go kompilując na danym węźle:
g++ zmonitor2.cpp -o mon -std=c++11 -lzmq
I uruchamiając go z id, które określa który jest to węzeł wedle zapisanych adresów w config.hpp:
./mon [id]

Samo użycie monitora pokazane jest w pliku straznik.cpp, który pobiera bufor z sekcji krytycznej, następnie modyfikuje go i odsyła do monitora. Aby to umożliwić dostępne są 2 funkcje z pliku nagłówkowego zmonitor.hpp:
acquireCS(id) - wysyła żądanie zajęcia sekcji krytycznej do lokalnego monitora, który następnie komunikuje się z pozostałymi monitorami rozproszonymi i po zajęciu zwraca bufor (w postaci stringa, kwestia zapisu pozostałych typów zmiennych pozostaje w gestii programisty, co można uzyskać za pomocą serializacji).
po zakończeniu pracy z sekcją krytyczną należy użyć funkcji release_CS(id,data) - podając jako parametr data zmodyfikowany bufor.

Do zapewniania wzajemnego wykluczania sekcji krytycznej został użyty algorytm Ricarda-Agrawali uwzględniający specyfikę komunikacji socketów REQ/REP ZeroMQ - wymagana wymienność operacji send/recv oraz w przypadku podłączenie do jednego socketu (jak ma to miejsce w przypadku socketu requester) wielu adresów tego że operacji wysyłanie są dokonywane w kolejność Round Robin.
Dodatkowo zaimplementowany jest zegar logiczny, służący do aktualizacji aktualnej wartości bufora na wszystkich węzłach.
