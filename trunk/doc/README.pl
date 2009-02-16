QNapi
=====

Autor:                Krzemin <pkrzemin@o2.pl>
Strona WWW:           http://krzemin.iglu.cz/qnapi

Ten program rozprowadzany jest na licencji GNU General Public License w wersji
drugiej lub kaýdej p—niejszej.


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
Qt4 (w niektorych dystrucjach oznaczone jako libqt4-dev lub podobnie)
i program 7zip (dostepny w pakiecie p7zip-full).

Pobierz i rozpakuj archiwum .tar.gz z kodem zrodlowym programu:

$ tar -zxvf qnapi-wersja.tar.gz

Przejdz do katalogu ze zrodlami programu:

$ cd qnapi-wersja

Aby skompilowac program, wykonaj po kolei polecenia:

$ qmake
(lub qmake-qt4)

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

KDE (Dolphin/Konqueror)
-----------------------
UWAGA: Od wersji 0.1.4 QNapi powinien automatycznie integrowac sie z programami
Dolphin oraz Konqueror. Jesli automatyczna integracja nie bedzie dzialala,
mozesz skorzystac z alternatywnej metody:

Bedzie potrzebny plik qnapi-download.dekstop, ktory mozesz znalezc w podkatalogu
doc/ archiwum zrodlowego, w /usr/share/doc/qnapi (po instalacji programu) lub
na stronie www http://krzemin.iglu.cz/qnapi.

Nalezy skopiowac plik do katalogow:
 *  ~/.kde/share/apps/konqueror/servicemenus/
 *  ~/.kde/share/apps/d3lphin/servicemenus/
 *  ~/.kde/share/apps/dolphin/servicemenus/

Lub globalnie (wymaga uprawnien administratora):
 *  /usr/share/apps/konqueror/servicemenus/
 *  /usr/share/apps/d3lphin/servicemenus/
 *  /usr/share/apps/dolphin/servicemenus/

Dla KDE4:

* ~/.kde4/share/kde4/services/ServiceMenus/

Globalnie dla KDE4:
 * /usr/lib/kde4/share/kde4/services/ServiceMenus/

Powyzsze katalogi moga byc nieco inne w Twojej dystrybucji, w zaleznosci od
tego, gdzie zostalo zainstalowane KDE.

Nautilus (GNOME)
----------------
UWAGA: Od wersji 0.1.5 QNapi powinien automatycznie integrowac sie z Nautilusem
po zainstalowaniu pakietu qnapi-gnome, wlasciwego dla Twojej dystrybucji oraz po
ponownym uruchomieniu systemu (lub przeladowaniu demona gconfd).

Jesli automatyczna integracja nie zadziala, badz nie chcesz (nie mozesz)
zainstalowac dodatkowego pakietu, mozesz skorzystac z alternatywnego spososbu na
integracje QNapi z Nautilusem:

Potrzebny bedzie zainstalowany pakiet nautilus-actions oraz plik
qnapi-download.schemas, ktory mozesz znalezc w podkatalogu doc/ archiwum
zrodlowego, w /usr/share/doc/qnapi (po instalacji programu) lub na stronie www
http://krzemin.iglu.cz/qnapi.

Wystarczy skopiowac plik qnapi-download.schemas do katalogu
/usr/share/gconf/schemas/ i wykonac polecenie:

$ sudo gconf-schemas --register /usr/share/gconf/schemas/qnapi-download.schemas

Jesli to by nie zadzialalo, mozna sprobowac zaimportowac plik
qnapi-download.schemas narzedziem nautilus-actions-config.

Thunar (XFCE)
-------------
Integracja z Thunarem wymaga uruchomienia programu i wybrania opcji
Edycja -> Konfiguruj akcje...

Na zakladce "Podstawowe" nalezy ustawic:

Nazwa: "Pobierz napisy do filmu z QNapi"
Opis: (dowolny)
Polecenie: qnapi %F

Mozna wybrac ikone z pliku graficznego (np. /usr/share/icons/qnapi-48.png).
Na zakladce "Warunki pokazywania" nalezy zaznaczyc tylko "Filmy wideo".
Potem OK i gotowe.


Raportowanie bledow
====================
Wszelkie znalezione bledy mozna raportowac za pomoca bugtrackera na
sourceforge.net: http://sourceforge.net/tracker2/?atid=1043757&group_id=218384

Kontakt
=======

Jesli chcesz przygotowac pakiet dla swojej dystrybucji badz masz inne
uwagi/propozycje - napisz do mnie na pkrzemin@o2.pl
