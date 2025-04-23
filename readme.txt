Aplikacja zapisuje do bazy danych wszystkie dane pobrane przez API.

Posiada plik konfiguracyjny config.ini, w którym ustawia się współrzędne geograficzne punktu (domyślnie ustawione
na campus Politechniki Poznańskiej), względem którego będzie liczona odległość do stacji pomiarowych.


Układ okna aplikacji
Okno aplikacji podzielone jest na trzy panele:
Panel po lewej stronie – główny panel z przyciskami i informacjami o aktualnie wybranych danych.
Panel u góry po prawej – wyświetla wykres liniowy wybranych danych.
Panel na dole po prawej – prezentuje analizę wybranych danych.


Panel po lewej stronie – Działanie przycisków:
Choose station    –  pobiera listę stacji pomiarowych i wyświetla okno wyboru jednej z nich. Stacje są posortowane
		     według odległości od ustawionego punktu.
Choose sensor	  –  pobiera listę sensorów i wyświetla okno wyboru jednego z nich.
Choose data	  –  wyświetla listę plików z danymi, przesyła je do wyświetlenia na wykresie oraz do analizy.
View stored data  –  otwiera okno z wcześniej pobranymi stacjami, sensorami i danymi w formacie:
		     Rok-Miesiąc-Dzień_Godzina_do_Rok-Miesiąc-Dzień_Godzina.


Dane wyświetlane w lewym panelu:
Pierwszy wiersz: ID oraz nazwa wybranej stacji.
Drugi wiersz: ID wybranego sensora oraz typ zbieranych danych.
Trzeci wiersz: nazwa wybranego pliku z danymi.


Panel górny po prawej stronie:
Wyświetla tytuł wykresu oraz informację, czego on dotyczy (np. NO₂ – dwutlenek azotu).
Oś pionowa: przedstawia wartości (np. stężenie).
Oś pozioma: przedstawia daty i godziny pomiarów.


Panel dolny po prawej stronie:
Prezentuje obliczone wartości statystyczne:
-Minimum
-Maksimum
-Średnia
-Trend