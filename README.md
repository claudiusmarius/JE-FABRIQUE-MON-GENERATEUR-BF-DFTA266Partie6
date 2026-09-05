# JE-FABRIQUE-MON-GENERATEUR-BF-DFTA266Partie6

Générateur BF DDS -- AD9833 / Seeeduino XIAO / ATtiny85

Présentation

Ce projet est la réalisation d'un générateur de fonctions basse
fréquence (BF) basé sur le circuit DDS AD9833, piloté par un
Seeeduino XIAO SAMD21 et associé à un ATtiny85.

L'appareil fournit trois formes d'onde :

Sinus

Triangle

Rectangle

La fréquence est sélectionnée par plages puis ajustée par appui sur deux boutons
poussoirs. Un afficheur OLED indique la fréquence et la forme d'onde
sélectionnée.

Le générateur dispose également de réglages de niveau, d'amplitude/gain
et d'offset, ainsi que d'un étage de sortie à transistors
complémentaires.

Architecture

Génération DDS

Le signal est produit par l'AD9833, commandé par le Seeeduino XIAO.

Le XIAO assure notamment : - la programmation de la fréquence ; - la
sélection de la forme d'onde ; - la gestion des boutons ; - la gestion
de l'afficheur OLED.

Gestion des modes

Un ATtiny85 intervient dans la gestion des modes, notamment pour
l'atténuation automatique appliquée en mode rectangle afin de rapprocher
son niveau de celui des signaux sinus et triangle.

Traitement du signal

Le signal DDS est traité sur une carte analogique comprenant notamment
des NE5532 et un LT1006.

Cette partie assure : - l'adaptation du niveau ; - le réglage de
l'amplitude ; - le réglage de l'offset ; - le traitement nécessaire
avant l'étage de sortie.

Étage de sortie

L'étage de sortie utilise une paire complémentaire BD139 / BD140 en
push-pull.

La contre-réaction est prélevée à la jonction des deux résistances
d'émetteur, avant la résistance série de sortie. Elle participe à la
correction du fonctionnement de l'étage et à la réduction de la
distorsion de croisement.

Alimentation

L'appareil est alimenté en 24 V DC.

Une carte dédiée permet de créer une alimentation symétrique : - +12 V -
0 V - −12 V

Une même conception de PCB est utilisée, avec une implantation
différente, pour l'alimentation symétrique et pour l'étage push-pull.

Face avant

La face avant regroupe :

bouton d'augmentation de fréquence ;

bouton de diminution de fréquence ;

sélection de plage ;

sélection de forme d'onde ;

réglage du niveau du signal DDS ;

réglage du gain/amplitude ;

réglage d'offset ;

LED d'indication des modes ;

afficheur OLED ;

bornes de sortie.

Alimentation logique en 3,3 V

Au cours du développement, l'AD9833, l'ATtiny85 et l'afficheur OLED ont
été utilisés en 3,3 V, afin d'homogénéiser le domaine logique avec
le XIAO.

Cette évolution a également fait partie des essais visant à améliorer la
fiabilité du système lors des mises sous tension répétées.

Essais et mise au point

Le projet a été développé progressivement à partir d'essais sur
breadboard, de simulations, de mesures à l'oscilloscope et de
réalisations de PCB.

Des essais de fonctionnement prolongé et des cycles répétés de mise sous
tension ont également été réalisés.

Plusieurs corrections ont été apportées au cours du développement.

ERRATUMS

Les erreurs identifiées dans les schémas ou les PCB sont regroupées dans
la documentation sous la forme :

ERRATUM N°1, ERRATUM N°2, etc.

Chaque erratum précise : - l'erreur constatée ; - sa conséquence
éventuelle ; - la correction à appliquer ; - la révision de PCB
concernée.

Consulter impérativement les erratums avant toute reproduction d'une
carte.

Documentation

La documentation technique présente notamment :

Introduction

Générateur DDS et réglages

Traitement du signal : amplitudes et offsets

Étage de sortie push-pull

Alimentation 24 V

Alimentation symétrique avec zéro virtuel

Erratums et corrections PCB

Elle est complétée par les schémas, fichiers PCB, photographies et
documents associés présents dans ce dépôt.

Vidéos

Le projet est documenté sur YouTube sous la référence DFTA266 --
Générateur BF DDS.

Les liens vers les différentes vidéos peuvent être ajoutés dans cette
section.

Logiciels

KiCad -- schémas et PCB

LTspice -- simulations

Arduino IDE -- programmation

Contenu du dépôt

Le dépôt peut notamment contenir :

schémas électroniques ;

fichiers PCB ;

fichiers Gerber ;

programmes du Seeeduino XIAO ;

programme de l'ATtiny85 ;

documentation technique ;

photographies ;

documents complémentaires ;

erratums et corrections.

Avertissement

Ce projet est une réalisation personnelle de laboratoire.

Les fichiers sont publiés pour documenter la conception et permettre à
d'autres passionnés d'étudier ou de reproduire le projet.

Avant toute reproduction, vérifier les schémas, les valeurs des
composants et les erratums correspondant à la révision du PCB
utilisée.

Auteur

Claude DUFOURMONT

Projet personnel -- Générateur BF DDS
