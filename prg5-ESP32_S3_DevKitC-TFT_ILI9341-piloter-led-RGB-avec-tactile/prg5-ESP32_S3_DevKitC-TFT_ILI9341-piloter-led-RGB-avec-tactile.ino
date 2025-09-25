/*
   ______               _                  _///_ _           _                   _
  /   _  \             (_)                |  ___| |         | |                 (_)
  |  [_|  |__  ___  ___ _  ___  _ __      | |__ | | ___  ___| |_ _ __ ___  _ __  _  ___  _   _  ___
  |   ___/ _ \| __|| __| |/ _ \| '_ \_____|  __|| |/ _ \/  _|  _| '__/   \| '_ \| |/   \| | | |/ _ \
  |  |  | ( ) |__ ||__ | | ( ) | | | |____| |__ | |  __/| (_| |_| | | (_) | | | | | (_) | |_| |  __/
  \__|   \__,_|___||___|_|\___/|_| [_|    \____/|_|\___|\____\__\_|  \___/|_| |_|_|\__  |\__,_|\___|
                                                                                      | |
                                                                                      \_|
  Fichier :       prg5-ESP32_S3_DevKitC-TFT_ILI9341-piloter-led-RGB-avec-tactile.ino
  
  Description :   Programme permettant de piloter la NeoPixel (LED RGB) embarquée sur un ESP32-S3-DevKitC,
                  à partir d'un écran TFT (contrôleur ILI9341) tactile (contrôleur XPT2046)

  Licence :       BY-NC-ND 4.0 CC (https://creativecommons.org/licenses/by-nc-nd/4.0/deed.fr)

  Remarques :     - jumper J1 "soudé/fait", pour que l'écran TFT fonctionne entièrement en 3,3V
                  - les broches utilisées ici (ESP32 ↔ écran TFT) sont définies dans le fichier "User_Setup.h" (dans le répertoire TFT_eSPI)
  
  Auteur :        Jérôme TOMSKI (https://passionelectronique.fr/)
  Créé le :       11.09.2025
  
*/

// Inclusion des librairies dont nous aurons besoin ici
#include <TFT_eSPI.h>
#include <Adafruit_NeoPixel.h>

// Constantes
#define BROCHE_NEOPIXEL   48    // Broche GPIO48 pour la WS2812 embarquée sur l'ESP32-S3-DevKitC
#define NBRE_NEOPIXELS     1    // Nombre de LEDs (1 pour une seule, car c'est le cas sur l'ESP32-S3-DevKitC)

// Instanciation de l'objet "TFT_eSPI" (écran TFT, à contrôleur ILI9341)
TFT_eSPI tft = TFT_eSPI();

// Instanciation de l'objet "Adafruit_NeoPixel" (NeoPixel présente sur l'ESP32-S3-DevKitC)
Adafruit_NeoPixel neopixels(NBRE_NEOPIXELS, BROCHE_NEOPIXEL, NEO_GRB + NEO_KHZ800);

// Variables
uint8_t valeur_couleur_rouge;
uint8_t valeur_couleur_verte;
uint8_t valeur_couleur_bleue;
uint16_t tactile_min_X;
uint16_t tactile_min_Y;
uint16_t tactile_max_X;
uint16_t tactile_max_Y;

// ========================
// Initialisation programme
// ========================
void setup() {

  // Initialisation de l'écran TFT
  tft.init();
  tft.setRotation(0);           // 0 = affichage mode "portrait"

  // Calibration de la partie tactile de l'écran TFT
  calibrer_partie_tactile_de_l_ecran_TFT();

  // Efface tout ce qui est affiché sur l'écran TFT
  tft.fillScreen(TFT_BLACK);
 
  // Initialisation des variables
  valeur_couleur_rouge = 255;
  valeur_couleur_verte = 210;
  valeur_couleur_bleue = 20;

  // Initialisation de la NeoPixel présente sur l'ESP32-S3-DevKitC
  neopixels.begin();
  neopixels.setBrightness(10);      // Réglage de la luminosité (0-255, réglée sur 10 pour éviter de trop tirer sur l'alimentation)

  // Tests
  afficher_elements_graphiques_statiques();
  afficher_elements_graphiques_dynamiques();
  piloter_ESP32_neopixel();

}


// =================
// Boucle principale
// =================
void loop() {

  // Variables temporaires, qui contiendront les coordonnées du "point" touché, et dire si une couleur a été modifiée
  uint16_t touche_x = 0, touche_y = 0;
  bool couleur_modifiee = false;

  // Teste sur une touche "valide" a été détectée sur l'écran
  bool touche_detectee = tft.getTouch(&touche_x, &touche_y);

  // Si une touche valide a été détectée, alors on la traite
  if (touche_detectee) {

    // On regarde si on a touché dans la zone de la barre rouge (élargissement 5 pixels en dessus et en dessous)
    if(touche_y > 75 && touche_y < 105) {
      valeur_couleur_rouge = map(touche_x, 0, 239, 0, 255);
      couleur_modifiee = true;
    }

    // On regarde si on a touché dans la zone de la barre verte (élargissement 5 pixels en dessus et en dessous)
    if(touche_y > 145 && touche_y < 175) {
      valeur_couleur_verte = map(touche_x, 0, 239, 0, 255);
      couleur_modifiee = true;
    }

    // On regarde si on a touché dans la zone de la barre bleue (élargissement 5 pixels en dessus et en dessous)
    if(touche_y > 205 && touche_y < 235) {
      valeur_couleur_bleue = map(touche_x, 0, 239, 0, 255);
      couleur_modifiee = true;
    }

    // Si une couleur a été modifiée, alors on met à jour les éléments graphiques dynamiques à l'écran, et la neopixel de l'ESP32
    if(couleur_modifiee) {
      afficher_elements_graphiques_dynamiques();
      piloter_ESP32_neopixel();
    }

  }

}


// =================================================
// Fonction : afficher éléments graphiques statiques
// =================================================
void afficher_elements_graphiques_statiques() {

  // Définition de la couleur des textes
  tft.setTextColor(TFT_WHITE);


  // -------------------
  // Titre et sous-titre
  // -------------------

  // Affichage du titre, en haut
  tft.setTextSize(2);
  tft.setCursor(0, 0);
  tft.print("Pilotage LED RGB");

  // Affichage du sous-titre
  tft.setTextSize(1);
  tft.setCursor(0, 20);
  tft.print("ESP32-S3-DevKitC");

  // Soulignage du sous-titre
  tft.drawLine(0, 35, 239, 35, TFT_WHITE);


  // -------------------
  // Barre pour le rouge
  // -------------------

  // Texte pour le rouge
  tft.setTextSize(2);
  tft.setCursor(0, 60);
  tft.print("Rouge");

  // Barre pour le rouge
  tft.drawRect(0, 80, 240, 20, TFT_LIGHTGREY);


  // ------------------
  // Barre pour le vert
  // ------------------

  // Texte pour le vert
  tft.setTextSize(2);
  tft.setCursor(0, 130);
  tft.print("Vert");

  // Barre pour le vert
  tft.drawRect(0, 150, 240, 20, TFT_LIGHTGREY);


  // ------------------
  // Barre pour le bleu
  // ------------------

  // Texte pour le bleu
  tft.setTextSize(2);
  tft.setCursor(0, 190);
  tft.print("Bleu");

  // Barre pour le bleu
  tft.drawRect(0, 210, 240, 20, TFT_LIGHTGREY);


  // ------------
  // Partie basse
  // ------------

  // Texte "Résultat"
  tft.setTextSize(2);
  tft.setCursor(0, 284);
  tft.print("Resultat >>");

  // Cadre pour le résultat
  tft.drawRect(150, 260, 90, 60, TFT_LIGHTGREY);

}


// =================================================================================================
// Fonction : conversion de composantes rouge/vert/bleu 8 bits en valeur résultante 16 bits (RGB565)
// =================================================================================================
uint16_t convertir_RGB_8bits_vers_RGB565(uint8_t rouge, uint8_t vert, uint8_t bleu) {
  // Réduire les valeurs 8 bits à 5/6/5 bits, pour coller au "standard" RGB 565
  uint16_t r = (rouge >> 3);    // 8 bits -> 5 bits (0-31)
  uint16_t g = (vert >> 2);     // 8 bits -> 6 bits (0-63)
  uint16_t b = (bleu >> 3);     // 8 bits -> 5 bits (0-31)
  
  // Combiner en une valeur 16 bits (RGB565)
  return (r << 11) | (g << 5) | b;
}


// ==================================================
// Fonction : afficher éléments graphiques dynamiques
// ==================================================
void afficher_elements_graphiques_dynamiques() {

  // Contrainte des différentes valeurs
  uint8_t largeur_barre_rouge = map(valeur_couleur_rouge, 0, 255, 1, 238);
  uint8_t largeur_barre_verte = map(valeur_couleur_verte, 0, 255, 1, 238);
  uint8_t largeur_barre_bleue = map(valeur_couleur_bleue, 0, 255, 1, 238);

  // Remplissage des barres de couleur
  tft.fillRect(1, 81, largeur_barre_rouge, 18, TFT_RED);
  tft.fillRect(1, 151, largeur_barre_verte, 18, TFT_GREEN);
  tft.fillRect(1, 211, largeur_barre_bleue, 18, TFT_BLUE);

  // Remplissage des parties non remplies, au niveau des barres de couleur
  tft.fillRect(largeur_barre_rouge + 1, 81, 238 - largeur_barre_rouge, 18, TFT_BLACK);
  tft.fillRect(largeur_barre_verte + 1, 151, 238 - largeur_barre_verte, 18, TFT_BLACK);
  tft.fillRect(largeur_barre_bleue + 1, 211, 238 - largeur_barre_bleue, 18, TFT_BLACK);

  // Détermination de la couleur résultante (et passage au format 16 bits / RGB565)
  uint16_t couleur_resultante = convertir_RGB_8bits_vers_RGB565(valeur_couleur_rouge, valeur_couleur_verte, valeur_couleur_bleue);

  // Remplissage de la couleur résultante
  tft.fillRect(151, 261, 88, 58, couleur_resultante);

}


// ===================================================
// Fonction : piloter la NeoPixel présente sur l'ESP32
// ===================================================
void piloter_ESP32_neopixel() {

  // Définition des composants R(ouge), G(reen), B(lue), pour la neopixel embarquée sur l'ESP32-S3-DevKitC
  neopixels.setPixelColor(0, neopixels.Color(valeur_couleur_rouge, valeur_couleur_verte, valeur_couleur_bleue));

  // Mise à jour de la NeoPixel
  neopixels.show();

}


// ====================================================
// Fonction : calibrer la partie tactile de l'écran TFT
// ====================================================
void calibrer_partie_tactile_de_l_ecran_TFT()
{

  // Variable temporaire
  uint16_t donnees_de_calibration[5];

  // Début de calibration
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);

  tft.setCursor(10, 40);
  tft.println("Touchez les coins,");
  tft.setCursor(30, 65);
  tft.println("comme indique !");

  tft.calibrateTouch(donnees_de_calibration, TFT_WHITE, TFT_BLACK, 15);

  // Calibration terminée
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(40, 60);
  tft.println("Calibration");
  tft.setCursor(40, 85);
  tft.println("terminee !");
  tft.setCursor(40, 160);
  tft.println("Demarrage...");

  // Petite pause, avant de passer à la suite
  delay(2000);

}