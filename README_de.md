
PiSiDo
------
PiSiDo wurde entwickelt, um pisi-Pakete zu erstellen, die der Paketmanager des PisiLinux System ist.
Ziel von PiSiDo ist es, ein Startpunkt, der für das PISI-Paketsystem neu ist.
Und auch ein einfaches Programm, um Pakete aus Quellcode für Endbenutzer zu machen.
PiSiDo verwendet keine PISI-Funktionen mit voller Kapazität. Es sammelt notwendige Informationen
Um Basispakete zu erstellen.


Eigenschaften
--------

    * Beim Bauen :
        * Schreibt die GUI-Felder in die Datei pspec.xml.
        * Macht eine fertige Vorlage, highlihting und autocomplete in der actions.py.
        * Macht patches.
        * Kann Dateien und Verzeichnisse beschriften, die nach dem Erstellen beschriftet werden.
        * Sie können nur bis zu definierten Build-Schritten arbeiten.
    * Beim Importieren :
        * Liest die pspec.xml aus dem definierten Arbeitsbereichverzeichnis. Nach dem Importieren bleiben die Dateien in der XML-Datei unberührt, die im GUI nicht übereinstimmen.
        * Sie können die actions.py editieren.
        * Sie können die History des Paketes sehen.
    * Kreieren des Menu Icons :
        * Erstellt ein Beispiel einer .desktop and icon Datei.
    * Sie können die Ansicht mit beweglichen und verankerbaren Fenstern ändern.
    * Sie können sehen was während des Bauvorgangs passiert, durch das integrierte Terminal Fenster.


Lizenz
-------
Dieses Programm ist freie Software; Können Sie es neu verteilen und / oder ändern unter
den Bedingungen der letzten Version der GNU General Public License.
Bitte lesen Sie die LIZENZ-Datei für die Lizenz.


Verwendung
-----
Bitte lesen Sie die help/help_de_DE.pdf oder die Hilfe in der Anwendung.


Installieren
-------
Um PiSiDo zu installieren benötigen Sie die, Qt Entwicklungstools und Pakete, QTermWidget und QScintilla2 Entwicklungs-Dateien (development/.dev) müssen auf Ihrem System installiert sein.

Erforderliche Pakete um PiSiDo erfolgreich in PisiLinux zu installieren:

    sudo pisi it -c system.devel
    sudo pisi it qt-creator
    sudo pisi it qscintilla2-devel
    sudo pisi it qtermwidget5-devel

Um das Paket zu kompilieren und zu starten benutzen Sie folgende Befehle :

    cd pisido_source_code_directory
    qmake pisido.pro
    make
    ./pisido

Wenn Sie es installieren möchten:

    make install

Auch Sie können ein Pisi-Paket mit PiSiDo selbst bauen und installieren :)


Sprach Dateien
--------------
Wenn Sie eine neue Sprach Datei erstellen möchten, können Sie ein pisi paket erstellen welches installiert werden kann.
pisido_*.qm Datei nach /usr/share/pisido/translations/ und help_*.pdf Datei nach
/usr/share/pisido/help/. PiSiDo sucht nach pisido_*.qm Dateien für die Sprachauswahl.

