QNapi
=====

Autor:			Krzemin <pkrzemin@o2.pl>
Strona WWW:		http://krzemin.iglu.cz/qnapi

Ten program rozprowadzany jest na licencji GNU General Public License.

QNapi jest nieoficjalnym klonem programu NAPI-PROJEKT (http://napiprojekt.pl)
napisanym w bibliotece Qt4 z myślą o użytkownikach Linuksa oraz innych
systemów, pod które oryginalny NAPI-PROJEKT nie jest dostępny. Od wersji
0.1.4 istnieje także port na systemy Windows. Po więcej informacji zapraszam
na stronę WWW projektu.


Wymagania
=========
 * Biblioteka Qt w wersji conajmniej 4.3.0. Jeśli w Twoim systemie jest starsza
   wersja, musisz ją zaktualizować lub własnoręcznie skompilować źródła, pobrane
   ze strony http://trolltech.com/developer/downloads/qt/x11
 * Program p7zip. Więcej informacji na stronie http://p7zip.sourceforge.net/


Instalacja
==========

Aby zainstalować program, skorzystaj z pakietów dla swojej dystrybucji,
które możesz znależć na stronie programu: http://krzemin.iglu.cz/qnapi

Jeśli na stronie nie ma pakietów pod Twoją dystrybucję, możesz zainstalować
program ze źródeł. Do tego będą potrzebne biblioteki Qt4 (libqt4-core,
libqt4-gui, a do kompilacji także libqt4-dev) oraz program 7z (który
znajdziesz w pakiecie p7zip-full).

Ściągnij i rozpakuj archiwum .tar.gz ze źródłami programu:

$ tar -zxvf qnapi-wersja.tar.gz

Przejdź do katalogu ze źródłami:

$ cd qnapi-wersja

Aby skompilować program, wykonaj po kolei polecenia:

$ qmake
(lub qmake-qt4, jeśli masz zainstalowane również Qt3)

$ make

Jeśli komenda make zakończy się niepowodzeniem, przejdź do sekcji
'Błędy kompilacji' w tym dokumencie, aby zobaczyć możliwe przyczyny
i rozwiązania.

Po prawidłowej kompilacji możesz zainstalować program:

$ sudo make install

W tej chwili możesz uruchomić program poleceniem:

$ qnapi

Aby odinstalować program, wpisz:

$ sudo make uninstall


Błędy kompilacji
================

Jeśli polecenie make wyrzuci błąd podobny do tego:

"make: *** Brak reguł do zrobienia obiektu"

Oznacza to, że trzeba ponownie wygenerowac plik Makefile.
W tym celu wykonaj polecenie:

$ qmake
(lub qmake-qt4, jesli masz zainstalowane takze Qt3)

Po prawidłowym uruchomieniu programu qmake, nowy plik Makefile powinien zostać
wygenerowany. Wtedy możesz przystąpić do kompilacji programu. (Szczegóły w
sekcji 'Instalacja')


Integracja z menedżerami plików
===============================

Jeśli chcesz zintegrować QNapi ze swoim menedżerem plików, aby po kliknięciu
prawym przyciskiem myszy na plik z filmem, w menu pojawiła się opcja "Pobierz
napisy do filmu z QNapi", wykonaj poniższe instrukcje, adekwatne do uzywanego
przez Ciebie programu.

KDE (Dolphin/Konqueror)
-----------------------
UWAGA: Od wersji 0.1.4 QNapi powinien automatycznie integrować się z programami Dolphin
oraz Konqueror. Jesli natomiast automatyczna integracja nie bedzie działać, możesz
skorzystac z drugiego sposobu:

Będzie potrzebny plik qnapi-download.dekstop, który możesz znaleźć w katalogu
/usr/share/doc/qnapi lub na stronie domowej programu (http://krzemin.iglu.cz/qnapi).

Należy skopiować ów plik do katalogów:
 *  ~/.kde/share/apps/konqueror/servicemenus/
 *  ~/.kde/share/apps/d3lphin/servicemenus/
 *  ~/.kde/share/apps/dolphin/servicemenus/

Lub globalnie (wymaga uprawnień administratora):
 *  /usr/share/apps/konqueror/servicemenus/
 *  /usr/share/apps/d3lphin/servicemenus/
 *  /usr/share/apps/dolphin/servicemenus/

Dla KDE4:

* ~/.kde4/share/kde4/services/ServiceMenus/

Globalnie dla KDE4:
 * /usr/lib/kde4/share/kde4/services/ServiceMenus/

Powyższe katalogi mogą być nieco inne w Twojej dystrybucji, w zależności od tego, gdzie
zostało zainstalowane KDE.

Nautilus
--------
UWAGA: Od wersji 0.1.5 QNapi powinien automatycznie integrować sie z Nautilusem po
zainstalowaniu pakietu qnapi-gnome, wlaściwego dla Twojej dystrybucji oraz po
ponownym uruchomieniu systemu (lub przeładowaniu demona gconfd).

Jeśli automatyczna integracja nie zadziała, bądź nie chcesz (nie możesz) zainstalować
dodatkowego pakietu, możesz skorzystać z alternatywnego spososbu na integrację QNapi
z Nautilusem:

Potrzebny bedzie zainstalowany pakiet nautilus-actions oraz plik qnapi-download.schemas,
który znajdziesz w katalogu /usr/share/doc/qnapi lub na stronie domowej programu
(http://krzemin.iglu.cz/qnapi).

Wystarczy skopiować plik qnapi-download.schemas do katalogu /usr/share/gconf/schemas/
i wykonać polecenie:

$ sudo gconf-schemas --register /usr/share/gconf/schemas/qnapi-download.schemas

Jeśli to by nie zadziałało, można spróbować zaimportować plik qnapi-download.schemas
narzędziem nautilus-actions-config.

Thunar
------
Integracja z Thunarem wymaga uruchomienia programu i wybrania opcji
Edycja -> Konfiguruj akcje...

Na zakładce "Podstawowe" należy ustawić:

Nazwa: "Pobierz napisy do filmu z QNapi"
Opis: (dowolny)
Polecenie: qnapi %F

Można wybrać ikonę z pliku graficznego (np. /usr/share/icons/qnapi-48.png).
Na zakładce "Warunki pokazywania" należy zaznaczyć tylko "Filmy wideo".
Potem OK i gotowe.


Raportowanie błędów
====================
Wszelkie znalezione błędy można raportować za pomocą bugtrackera na sourceforge.net:
http://sourceforge.net/tracker/?group_id=218384&atid=1043757

Kontakt
=======

Jeśli chcesz przygotować pakiet dla swojej dystrybucji bądź masz inne
uwagi/propozycje - napisz do mnie na pkrzemin@o2.pl
