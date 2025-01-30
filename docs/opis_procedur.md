# common.c

### `check_gender_and_set(int* currentGender, int slot, int gender)`:

- Sprawdza płeć na danym stanowisku i ustawia ją, jeżeli natrafi na puste stanowisko.
- Zwraca wartość 1, jeśli stanowisko jest puste lub jeżeli płeć nowego pasażera pasuje do płci pasażera na stanowisku.
- Zwraca wartość 0, jeżeli płeć pasażera nie pasuje do płci pasażera na stanowisku kontroli.

### `reset_gender(int* currentGender, int slot)`:

- Resetuje płeć na wskazanym stanowisku aby wskazać, że stanowisko jest puste i nikogo nie obsługuje.



# captain.c

### `captain_process(int shmID, sem_t* semaphores, int gateID)`:

- Reprezentuje proces kapitana.
- Dołącza do pamięci współdzielonej i otwiera kolejkę komunikatów.
- Sprawdza, czy samolot stojący przy bramce jest pełen i jeżeli jest, wysyła prośbę o wcześniejszy odlot do dyspozytora lotów.
- Czeka na zatwierdzenie wcześniejszego odlotu przez dyspozytora, a następnie symuluje odlot i powrót samolotu na lotnisko.
- Czeka na załadunek pasażerów, jeżeli samolot nie jest pełny.



# passenger.c

### `passenger_process(int shmID, sem_t* semaphores, int passengerID)`:

- Reprezentuje proces pasażera.
- Dołącza do pamięci współdzielonej i losuje wagę i płeć dla pasażera.
- Symuluje kontrolę bagażu, a następnie dodaje pasażera do kolejki na kontrolę bezpieczeństwa.
- Czeka w kolejce, przechodzi kontrolę bezpieczeństwa i czeka na dostęp do schodów.
- Znajduje pierwszy samolot z wolnym miejsciem i wchodzi na pokład, jeżeli ma dostęp do schodów.
- Aktualizuje pamięć współdzieloną i zwalnia wykorzystywane przez siebie zasoby.

### `verify_baggage(int weight, int isVip)`:

- Weryfikuje, czy waga bagażu mieści się w wygenerowanym ustalonym limicie.
- Zwraca wartość 1, jeżeli bagaż mieści się w limicie.
- Zwraca wartość 0, jeżeli bagaż nie mieści się w limicie.



# dispatcher.c

### `dispatcher_process(int shmID, sem_t* semaphores)`:

- Reprezentuje proces dyspozytora zarządzającego lotniskiem.
- Dołącza do pamięci współdzielonej i tworzy kolejkę komunikatów.
- Przez cały czas sprawdza stan samolotów i pasażerów.
- Obługuje prośby o wcześniejszy odlot wysyłane przez kapitanówi i zatwierdza je.
- Jeżeli żadni pasżerowie nie czekają na odlot, kończy symulację.



# utilities.c

### `generate_random_weight()`:

- Generuje losową wagę bagażu pasażera o zadanym przedziale.

### `generate_random_gender()`:

- Generuje losową płeć dla pasażera (0 dla mężczyzny, 1 dla kobiety).



# main.c

### `main()`:

- Funkcja główna, inicjalizuje zasoby, tworzy procesy dla wszystkich "członków" symulacji.
- Czeka na zakończenie symulacji, a następnie czyści zasoby.

### `signal_handler(int sig)`:

- Obsługuje sygnał **SIGINT**, wykorzystywany do zakończenia symulacji.
- Ustawia flagę zakończenia symulacji w pamięci współdzielonej i wysyła sygnał zakończenia do wszystkich procesów potomnych.
- Czeka na zakończenie procesów potomnych i czyści zasoby.

### `initialize_resources()`

- Inicjalizuje pamięć współdzieloną, semafory oraz kolejkę komunikatów.
- Ustawia określone wartości początkowe w pamięci współdzielonej.

### `cleanup_resources(int shmID, SharedData* sharedDataArg)`:

- Czyści pamięć współdzieloną, semafory i kolejkę komunikatów.
- Odłącza i usuwa pamięć współdzieloną.
- Niszczy semafory i usuwa kolejkę komunikatów.