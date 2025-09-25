/*
   ______               _                  _///_ _           _                   _
  /   _  \             (_)                |  ___| |         | |                 (_)
  |  [_|  |__  ___  ___ _  ___  _ __      | |__ | | ___  ___| |_ _ __ ___  _ __  _  ___  _   _  ___
  |   ___/ _ \| __|| __| |/ _ \| '_ \_____|  __|| |/ _ \/  _|  _| '__/   \| '_ \| |/   \| | | |/ _ \
  |  |  | ( ) |__ ||__ | | ( ) | | | |____| |__ | |  __/| (_| |_| | | (_) | | | | | (_) | |_| |  __/
  \__|   \__,_|___||___|_|\___/|_| [_|    \____/|_|\___|\____\__\_|  \___/|_| |_|_|\__  |\__,_|\___|
                                                                                      | |
                                                                                      \_|
  Fichier :       prg4-ESP32_S3_DevKitC-TFT_ILI9341-afficher-des-images.ino
  
  Description :   Programme permettant d'afficher des images sur un écran TFT (contrôleur ILI9341),
                  en utilisant un ESP32-S3-DevKitC

  Licence :       BY-NC-ND 4.0 CC (https://creativecommons.org/licenses/by-nc-nd/4.0/deed.fr)

  Remarques :     - jumper J1 "soudé/fait", pour que l'écran TFT fonctionne entièrement en 3,3V
                  - les broches utilisées ici (ESP32 ↔ écran TFT) sont définies dans le fichier "User_Setup.h" (dans le répertoire TFT_eSPI)
  
  Auteur :        Jérôme TOMSKI (https://passionelectronique.fr/)
  Créé le :       10.09.2025
  
*/

// Inclusion des librairies dont nous aurons besoin ici
#include <TFT_eSPI.h>

// Inclusion des images
#include "1_printemps.h"
#include "2_ete.h"
#include "3_automne.h"
#include "4_hiver.h"

// Constantes
#define pause_entre_chaque_image  3000            // 3000 ms (soit 3 sec) de pause, entre chaque image affichée sur l'écran TFT

// Création d'une instance de l'objet TFT_eSPI
TFT_eSPI tft = TFT_eSPI();


// ========================
// Initialisation programme
// ========================
void setup() {

  // Initialiser l'écran
  tft.init();
  tft.setSwapBytes(true);   // Corrige un "bug" de décalage de couleurs
  
  // Définir l'orientation de l'écran (0 à 3 pour rotation)
  tft.setRotation(1); // Ajustez selon l'orientation souhaitée (1 = paysage)

  // Remplir l'écran avec une couleur de fond (noir)
  tft.fillScreen(TFT_BLACK);

}


// =================
// Boucle principale
// =================
void loop() {

  // Affichage de la 1ère image
  afficher_image_avec_texte_accompagnant(printemps, "Printemps", TFT_WHITE);
  delay(pause_entre_chaque_image);

  // Affichage de la 2ème image
  afficher_image_avec_texte_accompagnant(ete, "Ete", TFT_WHITE);
  delay(pause_entre_chaque_image);

  // Affichage de la 3ème image
  afficher_image_avec_texte_accompagnant(automne, "Automne", TFT_WHITE);
  delay(pause_entre_chaque_image);

  // Affichage de la 4ème image
  afficher_image_avec_texte_accompagnant(hiver, "Hiver", TFT_WHITE);
  delay(pause_entre_chaque_image);

}

// =================================================
// Fonction : afficher image avec texte accompagnant
// =================================================
void afficher_image_avec_texte_accompagnant(const uint16_t *image, const char *texte_accompagnant, uint16_t couleur_du_texte) {

  // Afficher l’image
  tft.pushImage(0, 0, 320, 240, image);   // posX, posY, largeur, hauteur, lien_image

  // Afficher le texte accompagnant
  tft.setTextColor(couleur_du_texte);
  tft.setTextSize(2);
  tft.setCursor(20, 20);
  tft.print(texte_accompagnant);

  // Traçage d'une ligne horizontale, sous le texte
  tft.drawLine(20, 40, 300, 40, couleur_du_texte);

}
