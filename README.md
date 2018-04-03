# NLMSvariants


## Aufgabenstellung

Aus der PDF entnommen mit der Auswahl an PVL-Themen

### "Vergleich von linearen Prädiktionsstrategien (NLMSvariants)"

Das Voraussagen von Signalwerten ist ein Schlüsselelement in modernen Kompressionssystemen.
Aber auch in anderen Anwendungsgebieten ist die Prädiktion von Ereignissen erforderlich.
Aufgabe ist es, ein Matlab/SciLab/Octave/C-Programm zu schreiben, das Varianten eines bestimmten
Prädiktionsverfahrens miteinander vergleicht.
Adaptive Prädiktionsfilter passen sich der Charakteristik der Signale an und gehören somit in den
Bereich des maschinellen Lernens. So genannte Least-Mean-Square-Filter (LMS) sind adaptive
Filter mit einer vergleichsweisen geringen Komplexität. Im Wesentlichen werden M vorangegangene
Signalwerte gewichtet überlagert, um einen Schätzwert für die aktuelle Position zu generieren

Xˉ [𝑛] = �𝑎𝑗[𝑛] ∙ 𝑥[𝑛 − 𝑗]
𝑀
𝑗=1
Der Prädiktionsfehler lautet: e[𝑛] = 𝑥[𝑛] − 𝑥�[𝑛]. Um die Prädiktionsfehlerenergie zu minimieren
müssen die Filterkoeffizienten nachgeführt (aktualisiert) werden
 𝑎𝑗[𝑛 + 1] = 𝑎𝑗[𝑛] + 𝜇 ∙ 𝑒[𝑛] ∙ 𝑥[𝑛−𝑗]
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

