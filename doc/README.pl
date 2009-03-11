QNapi
=====

Autor:                Krzemin <pkrzemin@o2.pl>
Strona WWW:           http://krzemin.iglu.cz/qnapi

Ten program rozprowadzany jest na licencji GNU General Public License w wersji
drugiej lub kazdej pozniejszej.


QNapi jest programem do automatycznego pobierania i dopasowywania napisow do
filmow. Program korzysta z zasobow serwisow napiprojekt.pl i opensubtitles.com.


Wymagania
=========
 * Biblioteka Qt w wersji conajmniej 4.3.0.
 * Program p7zip. Wiecej informacji na stronie http://p7zip.sourceforge.net


Instalacja
==========

Aby zainstalowac program pod systemem Linux, skorzystaj z pakietow dla swojej
dystrybucji, ktore mozesz znalezc w repozytoriach lub na stronie www programu:
http://krzemin.iglu.cz/qnapi

Jesli pakiety pod Twoja dystrybucje nie sa dostepne, mozesz samodzielnie
skompilowac program ze zrodel. Do tego potrzebne beda biblioteki deweloperskie
Qt4 (w niektorych dystrucjach oznaczone jako libqt4-dev, qt4-dev lub podobnie)
i program 7zip (dostepny w pakiecie p7zip-full).

Pobierz i rozpakuj archiwum .tar.gz z kodem zrodlowym programu:

$ tar -zxvf qnapi-wersja.tar.gz

Przejdz do katalogu ze zrodlami programu:

$ cd qnapi-wersja

Aby skompilowac program, wykonaj po kolei polecenia:

$ qmake
lub qmake-qt4; upewnij sie, ze uzyta wersja qmake (qmake --version) przeznaczona
jest dla Qt4

$ make

Jesli powyzsze polecenie zakonczy sie niepowodzeniem, przejdz do sekcji
'Bledy kompilacji' w tym dokumencie, aby zobaczyc  mozliwe przyczyny
i proponowane rozwiazania.

Po prawidlowej kompilacji mozesz zainstalowac program:

$ sudo make install

Teraz mozesz uruchomic program poleceniem:

$ qnapi

Aby odinstalowac program, wpisz w katalogu ze zrodlami:

$ sudo make uninstall


Bledy kompilacji
================

Jesli polecenie make zwroci blad podobny do tego:

"make: *** Brak regul do zrobienia obiektu"

Oznacza to, ze trzeba wygenerowac plik Makefile.
W tym celu wykonaj polecenie:

$ qmake
(lub qmake-qt4; upewnij sie, ze uzywasz qmake w wersji dla Qt 4)

Po prawidlowym uruchomieniu programu qmake, nowy plik Makefile powinien zostac
wygenerowany. Wtedy mozesz przystapic do kompilacji programu. (Szczegoly w
sekcji 'Instalacja')

W przypadku innych bledow kompilacji prosze o zwiezly raport o bledzie.
(Szczegoly w sekcji 'Raportowanie bledow').


Integracja z menedzerami plikow
===============================

Jesli chcesz zintegrowac QNapi ze swoim menedzerem plikow, aby po kliknieciu
prawym przyciskiem myszy na plik z filmem, w menu pojawila sie opcja "Pobierz
napisy do filmu z QNapi", wykonaj ponizsze instrukcje, adekwatne do uzywanego
przez Ciebie programu.

W niektorych dystrybucjach powinny znajdowac sie pakiety qnapi-kde3, qnapi-kde4,
qnapi-gnome itd., ktore automatycznie integruja program z uzywanym
srodowiskiem/menedzerem plikow. Jesli w Twojej dystrybucji nie ma takich
pakietow, skorzystaj z opisu przedstawionego ponizej.


KDE 3.x (Dolphin/Konqueror)
---------------------------

Beda potrzebne pliki qnapi-download.desktop i qnapi-scan.desktop, ktore
znajdziesz w podkatalogu doc/ archiwum zrodlowego lub w /usr/share/doc/qnapi
(po instalacji programu).

Nalezy skopiowac pliki do katalogow:
 *  ~/.kde/share/apps/konqueror/servicemenus/
 *  ~/.kde/share/apps/d3lphin/servicemenus/
 *  ~/.kde/share/apps/dolphin/servicemenus/

Lub globalnie (wymaga uprawnien administratora):
 *  /usr/share/apps/konqueror/servicemenus/
 *  /usr/share/apps/d3lphin/servicemenus/
 *  /usr/share/apps/dolphin/servicemenus/


Powyzsze sciezki do katalogow moga sie minimalnie roznic, w zaleznosci od
uzywanej dystrybucji.


KDE4
----

Polecenie:

$ kde4-config --path services

zwroci liste katalogow oddzielonych dwukropkiem:

/home/user/.kde4/share/kde4/services/:/usr/share/kde4/services/

Pliki qnapi-download.desktop i qnapi-scan.desktop nalezy skopiowac do katalogu:

/home/user/.kde4/share/kde4/services/ServiceMenus

lub:

/usr/share/kde4/services/ServiceMenus

(globalnie, wymaga uprawnien administratora)


Nautilus (GNOME)
----------------
Potrzebny bedzie zainstalowany pakiet nautilus-actions oraz pliki
qnapi-download.schemas i qnapi-scan.schemas, ktore mozesz znalezc w podkatalogu
doc/ archiwum zrodlowego lub w /usr/share/doc/qnapi (po instalacji programu).

Wystarczy skopiowac pliki do katalogu
/usr/share/gconf/schemas/ i wykonac polecenia:

$ sudo gconf-schemas --register /usr/share/gconf/schemas/qnapi-download.schemas
$ sudo gconf-schemas --register /usr/share/gconf/schemas/qnapi-scan.schemas

Jesli podana wyzej metoda nie zadziala, mozesz sprobowac zaimportowac pliki
qnapi-download.schemas i qnapi-scan.schemas narzedziem nautilus-actions-config:

$ nautilus-actions-config


Thunar (XFCE)
-------------
Integracja z Thunarem wymaga uruchomienia programu i wybrania opcji
Edycja -> Konfiguruj akcje...

Dodajemy nową akcję przyciskiem "+". Na zakladce "Podstawowe" nalezy ustawic:

Nazwa: "Pobierz napisy do filmu z QNapi"
Opis: (dowolny)
Polecenie: qnapi %F

Mozna wybrac ikone z pliku graficznego (np. /usr/share/icons/qnapi-48.png).
Na zakladce "Warunki pokazywania" nalezy zaznaczyc tylko "Filmy wideo".
Potem OK i gotowe.

Aby miec mozliwosc skanowania katalogow z poziomu Thunara, dodajemy nowa akcje.
Nazwa i opis dowolna, polecenie ustawiamy na "qnapi %f", natomaist na zakladce
"Warunki pokazywania" zaznaczamy tylko "Katalogi".


Raportowanie bledow
====================
Wszelkie znalezione bledy mozna raportowac za pomoca bugtrackera na
sourceforge.net: http://sourceforge.net/tracker2/?atid=1043757&group_id=218384

Kontakt
=======

Jesli chcesz przygotowac pakiet dla swojej dystrybucji badz masz inne
uwagi/propozycje - napisz do mnie na pkrzemin@o2.pl
