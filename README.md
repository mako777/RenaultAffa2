# RenaultAffa2
Program do obsługi wyświetlacza AFFA2++ w samochodach Renault

Układ pozwalający na komunikację z wyświetlaczem w samochodzie, wysyła do wyświetlacza tekst do wyświetlenia i odbiera naciśnięcia przycisków na pilocie przy kierownicy.

Radio z wyświetlaczem komunikuje się przy użyciu szyny I2C z dodatkowym pinem MIRQ.

Całość bez problemu do zmontowana przy użyciu Arduino Nano i kilku elementów pasywnych.<br>
Do komunikacji z wyświetlaczem używamy sprzętowego I2C + 2 dodatkowe piny do MIRQ i włączenia wyświetlacza.<br>
Do wyzwalania naciśnięć pilota przy kierownicy radio musi mieć obsługę pilota na zasadzie zwierania kabla do masy przez różne oporniki odpowiadające różnym przyciskom, bo w tym projekcie użyty jest do tego celu lekko zmodyfikowany prosty 4 bitowy przetwornik DAC R2R (taka drabinka rezystorów).<br>

Podłączenie jest następujące, na początek wtyczka od wyświetlacza przy radiu:<br>
http://tlcdcemu.sourceforge.net/hardware.html<br>
C1  connector (żółta):<br>
 1   DTA   I2C-Data<br>
 2   CLK   I2C-Clock<br>
 3   MIRQ  trzeba go ściągnąć do masy przed wysyłaniem<br>
 4   n/c<br>
 5   Radio ON	podać 5V żeby się włączył wyświetlacz<br>
 6   GND<br>

Trzeba podłączyć wszystkie piny z tej wtyczki, domyślnie wartości w programie (Arduino Nano):<br>
 1   DTA → A4 (PC4/SDA)<br>
 2   CLK → A5 (PC5/SCL)<br>
 3   MIRQ → A3 (PC3)<br>
 5   Radio ON → A2 (PC2)<br>
 6   GND → GND<br>

Drabinka R2R do radia jest na wyjściach D4 (PD4), D5 (PD5), D6 (PD6), D7 (PD7)<br>
W sumie to najlepiej zobrazuje to poniższy obrazek :)<br>
![Schemat](/images/ArduinoBuild.png)

Dodatkowo układ zmontowany według powyższego schematu też będzie mierzył napięcie zasilania (12V) w samochodzie, żeby wyświetlić je na wyświetlaczu wystarczy nacisnąć dolny przycisk pilota.


Program by nie powstał gdyby nie informacje zamieszczone tutaj:<br>
https://www.elektroda.pl/rtvforum/topic778615.html<br>
Także dzięki głównie kolegom „szymtro” i „dj-raszit” za zamieszczone materiały, które były bardzo pomocne w opracowaniu działającej komunikacji z wyświetlaczem :)
