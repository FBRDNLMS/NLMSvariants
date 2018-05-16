# Aufgabenstellung

Aus der PDF entnommen mit der Auswahl an PVL-Themen

### "Vergleich von linearen Prädiktionsstrategien (NLMSvariants)"

Das Voraussagen von Signalwerten ist ein Schlüsselelement in modernen Kompressionssystemen.
Aber auch in anderen Anwendungsgebieten ist die Prädiktion von Ereignissen erforderlich.
Aufgabe ist es, ein Matlab/SciLab/Octave/C-Programm zu schreiben, das Varianten eines bestimmten
Prädiktionsverfahrens miteinander vergleicht.
Adaptive Prädiktionsfilter passen sich der Charakteristik der Signale an und gehören somit in den
Bereich des maschinellen Lernens. So genannte Least-Mean-Square-Filter (LMS) sind adaptive
Filter mit einer vergleichsweisen geringen Komplexität. Im Wesentlichen werden M vorangegangene
Signalwerte gewichtet überlagert, um einen Schätzwert für die aktuelle Position zu generieren.

Xˉ [𝑛] = �𝑎𝑗[𝑛] ∙ 𝑥[𝑛 − 𝑗]

Der Prädiktionsfehler lautet: e[𝑛] = 𝑥[𝑛] − 𝑥'[𝑛]. Um die Prädiktionsfehlerenergie zu minimieren
müssen die Filterkoeffizienten nachgeführt (aktualisiert) werden

𝑎𝑗[𝑛 + 1] = 𝑎𝑗[𝑛] + 𝜇 * 𝑒[𝑛] * 𝑥[𝑛−𝑗]
 
||𝐱[𝑛]||
2 mit ||𝐱[𝑛]||
2 = ∑ (𝑥[𝑛 − 𝑗]) 𝑀 2
𝑗=1

während 0 < 𝜇 ≤ 1 die Lernrate ist.
Leider funktioniert das nur gut, wenn der Mittelwert von x[n] gleich Null ist. Für Bilder und auch
teilweise für Sprachsignale ist das nicht gegeben. Als Lösung kommen drei Varianten in Frage,
welche die obigen Formeln leicht abwandeln:

1. lokalen Mittelwert abziehen
 
 𝑥
�[𝑛] = 𝑥̅[𝑛] + ∑ 𝑎𝑗[𝑛] ∙ (𝑥[𝑛 − 𝑗] − 𝑥̅[𝑛]) 𝑀
𝑗=1 mit 𝑥̅[𝑛] = 1
𝑀 ∑ 𝑥[𝑛 − 𝑗] 𝑀
𝑗=1
und
 𝑎𝑗[𝑛 + 1] = 𝑎𝑗[𝑛] + 𝜇 ∙ 𝑒[𝑛] ∙
𝑥[𝑛−𝑗]−𝑥̅[𝑛]
||𝐱[𝑛]||
2 mit ||𝐱[𝑛]||
2 = ∑ (𝑥[𝑛 − 𝑗] − 𝑥�[𝑛]) 𝑀 2
𝑗=1

2. Bezug auf direkten Vorgänger nehmen

𝑥
�[𝑛] = 𝑥[𝑛 − 1] + ∑ 𝑎𝑗[𝑛] ∙ (𝑥[𝑛 − 1] − 𝑥[𝑛 − 𝑗 − 1]) 𝑀
𝑗=1
und
𝑎𝑗[𝑛 + 1] = 𝑎𝑗[𝑛] + 𝜇 ∙ 𝑒[𝑛] ∙
𝑥[𝑛−1]−𝑥[𝑛−𝑗−1]
||𝐱[𝑛]||
2 mit ||𝐱[𝑛]||
2 = ∑ (𝑥[𝑛 − 1] − 𝑥[𝑛 − 𝑗 − 1]) 𝑀 2
𝑗=1

3. differentiellen Bezug auf Vorgänger nehmen

𝑥
�[𝑛] = 𝑥[𝑛 − 1] + ∑ 𝑎𝑗[𝑛] ∙ (𝑥[𝑛 − 𝑗] − 𝑥[𝑛 − 𝑗 − 1]) 𝑀
𝑗=1
und
𝑎𝑗[𝑛 + 1] = 𝑎𝑗[𝑛] + 𝜇 ∙ 𝑒[𝑛] ∙
𝑥[𝑛−𝑗]−𝑥[𝑛−𝑗−1]
||𝐱[𝑛]||
2 mit ||𝐱[𝑛]||
2 = ∑ (𝑥[𝑛 − 𝑗] − 𝑥[𝑛 − 𝑗 − 1]) 𝑀 2
𝑗=1

Das originale Verfahren und die drei Varianten sind zu implementieren und mit verschiedenen
Testsignalen (synthetisierte und reale, N>= 500) und verschiedene M zu prüfen. Als Gütekriterium
ist die mittlere Energie des Schätzfehlers 𝐸 = 1
𝑁

� ∑ (𝑒[𝑛]) 𝑁 2 𝑛=1 heranzuziehen. Bei selbstgenerierten
Signalen könnte auch die Konvergenz der Filterkoeffizienten zu den richtigen Werten
untersucht werden.

Weitere Unterstützung wird bei Bedarf gegeben. Alle Untersuchungen sind schriftlich zu dokumentieren.
Neben der schriftlichen Arbeit sind alle Quellen (Programmcode, Texte, Testsignale)
und Tools abzugeben, damit eine Reproduktion der Ergebnisse möglich ist.

Teilaufgaben:
* Koordination
* Recherche
* Programmierung
* Dokumentation (Grundlagen, Methode, Änderungen am Quellcode, Kompressionsergebnisse)


## E-Mail Ergänzung zur Aufgabenstellung:

Sehr geehrte Studierende,

danke für die Wahl des Themas.

Zur Themenstellung möchte ich Folgendes ergänzen:
Die drei Varianten sind jeweils in einer Schleife über alle 
verfügbaren Signalwerte abzuarbeiten, wobei typischer Weise mit 
Filterkoeffizienten a_j = 0 angefangen wird.
Abweichend davon ist es evtl. sinnvoll, a_1=1 zusetzen.

Testsignale können Sie sich selbst generieren, wobei wir uns 
hierüber noch abstimmen sollten.
Man könnte verschiedene Zeilen aus verschiedenen Bildern nehmen, 
Sprachsignale aufzeichnen oder Signale synthetisch generieren:

z.B.
x[0] = x[1] = x[2] =..= x[M] = 100;
for n = M+1: N-1
   x[n] = a_1 * x[n-1] + a_2 * x[n-2] + ..+ a_M *x[n-M] + 
randn(1,1)*5;
end     

Die Koeffizienten des LMS-Filters müssten dann gegen die bei der 
Synthese verwendeten Koeffizienten konvergieren
Achtung: bei Matlab beginnt das Indizieren mit 1 (statt 0).

Bitte fangen Sie zeitnah an.
Falls es Unklarheiten zum Ablauf oder der Verfahrensweise gibt, 
bitte per Email bei mir melden.
Da es gewisse Freiheitsgrade in der Bearbeitung gibt, wäre es sehr 
sinnvoll, wenn Sie mir regelmäßig über den Stand der PVL berichten 
und wir uns ggf. austauschen können.

Mit freundlichen Grüßen
T.Strutz



Test:
---
$$ V=\pi \cdot r^{2} \cdot h $$
