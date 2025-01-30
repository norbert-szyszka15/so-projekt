# Opis testów
### Test nr #1 - mało pasażerów, dużo samolotów.
Pasażerowie przechodzą odprawę, oczekiwanie w kolejce niemal nie zachodzi, kapitan nie wysyła próśb o wcześniejszy odlot – symulacja kończy się normalnie po odlocie pierwszego samolotu (co jest równoznaczne w tym przypadku z przewiezieniem wszystkich pasażerów), wszystko działa nominalnie.

### Test nr #2 - dużo pasażerów, mało samolotów.
Program trwa znacząco dłużej względem poprzedniego przypadku, oczekiwanie w kolejce trwa długo, kapitanowie wysyłają dużo próśb o wcześniejsze odloty, a symulacja kończy się po przewiezieniu wszystkich pasażerów.

### Test nr #3 - dużo pasażerów, dużo samolotów.
Symulacja w „najnormalniejszym” ze swoich stanów – pasażerowie przechodzą odprawę i czekają na samoloty, które w stosunkowo „średnim” tempie są obsługiwane przez kapitanów i dyspozytora. Symulacja ponownie kończy się po przewiezieniu wszystkich pasażerów.

### Test nr #4 - użycie komendy `pkill airplane`.
Symulacja kończy się natychmiast, pamięć zostaje zwolniona i semafory zostają zniszczone (nie dochodzi do błędu semaforów). Żaden proces nie zotaje procesem zombie, wszystkie procesy kończą się nominalnie.