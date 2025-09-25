/*
   ______               _                  _///_ _           _                   _
  /   _  \             (_)                |  ___| |         | |                 (_)
  |  [_|  |__  ___  ___ _  ___  _ __      | |__ | | ___  ___| |_ _ __ ___  _ __  _  ___  _   _  ___
  |   ___/ _ \| __|| __| |/ _ \| '_ \_____|  __|| |/ _ \/  _|  _| '__/   \| '_ \| |/   \| | | |/ _ \
  |  |  | ( ) |__ ||__ | | ( ) | | | |____| |__ | |  __/| (_| |_| | | (_) | | | | | (_) | |_| |  __/
  \__|   \__,_|___||___|_|\___/|_| [_|    \____/|_|\___|\____\__\_|  \___/|_| |_|_|\__  |\__,_|\___|
                                                                                      | |
                                                                                      \_|
  Fichier :       prg2-ArduinoUno-TFT_ILI9341-generation-formes-aleatoires.ino
  
  Description :   Programme permettant de générer formes aléatoires colorées,
                  via un Arduino Uno, sur un écran TFT 2,8" SPI (contrôleur ILI9341)

  Licence :       BY-NC-ND 4.0 CC (https://creativecommons.org/licenses/by-nc-nd/4.0/deed.fr)

  Remarques :     Version avec convertisseur 5V / 3,3V (TXS0108E)
  
  Auteur :        Jérôme TOMSKI (https://passionelectronique.fr/)
  Créé le :       01.09.2025
  
*/

// Inclusion des librairies dont nous aurons besoin ici
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// Définition des broches utilisées ici (hors pins SPI)
#define TFT_CS    10            // Pin D10 de l'Arduino Uno -> broche CS de l'écran TFT
#define TFT_RST   9             // Pin D9  de l'Arduino Uno -> broche RST de l'écran TFT
#define TFT_DC    8             // Pin D8  de l'Arduino Uno -> broche DC de l'écran TFT

// Initialisation de l’écran TFT (contrôleur ILI9341)
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);     // Le reste = SPI matériel (hardware spi), donc il ne faut pas le renseigner ;
                                                                      // sinon on passerait en émulation logicielle (software spi)

// ========================
// Initialisation programme
// ========================
void setup() {

  // Initialisation de l’écran
  tft.begin();
  tft.setRotation(0);                   // Mode "portrait" (affichage vertical, soit 240 pixels en largeur et 320px en hauteur)
  tft.fillScreen(ILI9341_BLACK);        // Fond en "noir"

  // Initialisation du générateur aléatoire
  randomSeed(analogRead(0));            // On lit l'entrée A0, qui n'est reliée à rien ; cela permet d'avoir un nombre de base aléatoire, du fait du bruit

}


// =================
// Boucle principale
// =================
void loop() {

  // ------------------------------------------------------------------------------------------------
  // Nota : à chaque passage dans cette boucle loop, une nouvelle forme sera dessinée sur l'écran TFT
  // ------------------------------------------------------------------------------------------------

  // Compteur de formes (en fait, toutes les X formes dessinées, on effacera l'écran pour faire de la place, et y voir plus clair !)
  // Remarque : le mot "static" permet de mettre à 0 cette variable UNIQUEMENT lors du 1er appel de cette boucle loop
  static uint16_t nbre_de_formes_dessinees = 0;

  // -------------------------------------------------------------------------------
  // Génération d'une forme à la forme, couleur, remplissage, et position aléatoires
  // -------------------------------------------------------------------------------
  uint8_t forme = random(6);              // 0: pixel, 1: ligne, 2: rectangle, 3: cercle, 4: rectangle arrondi, 5: triangle
  bool rempli = random(2);                // TRUE: forme remplie, FALSE: contour uniquement
  uint16_t couleur = random(0xFFFF);      // Couleur aléatoire (sur 2 octets, soit 16 bits, pour coller au RGB565 → 5 bits de rouge, 6 de vert, et 5 de bleu)
  uint16_t x = random(240);               // Position X aléatoire (position sur la largeur)
  uint16_t y = random(320);               // Position Y aléatoire (position sur la hauteur)

  // ---------------------
  // Dessinage de la forme
  // ---------------------
  switch(forme) {
    case 0:   // Pixel        (posX, posY, couleur)
      tft.drawPixel(x, y, couleur);                                                                                 // Pixel coloré
      break;
    case 1:   // Ligne        (posX_depart, posY_depart, posX_arrivee, posY_arrivee, couleur)
      tft.drawLine(x, y, x + random(50), y + random(50), couleur);                                                  // Ligne colorée
      break;
    case 2:   // Rectangle    (posX, posY, largeur, hauteur, couleur)
      if (rempli) tft.fillRect(x, y, random(20, 60), random(20, 60), couleur);                                      // Rectangle rempli
      else tft.drawRect(x, y, random(20, 60), random(20, 60), couleur);                                             // Contour rectangle uniquement
      break;
    case 3:   // Cercle       (posX, posY, rayon, couleur)
      if (rempli) tft.fillCircle(x, y, random(10, 30), couleur);                                                    // Disque (cercle rempli)
      else tft.drawCircle(x, y, random(10, 30), couleur);                                                           // Cercle (contour uniquement dessiné)
      break;
    case 4:   // Rectangle arrondi    (posX, posY, largeur, hauteur, rayon, couleur)
      if (rempli) tft.fillRoundRect(x, y, random(20, 60), random(20, 60), random(2, 10), couleur);                  // Rectangle arrondi rempli
      else tft.drawRoundRect(x, y, random(20, 60), random(20, 60), random(2, 10), couleur);                         // Contour rectangle arrondi uniquement
      break;
    case 5:   // Triangle     (posX0, posY0, posX1, posY1, posX2, posY2, couleur)
      if (rempli) tft.fillTriangle(x, y, x + random(50), y + random(50), x + random(50), y - random(50), couleur);  // Triangle rempli
      else tft.drawTriangle(x, y, x + random(50), y + random(50), x + random(50), y - random(50), couleur);         // Contour triangle uniquement
      break;
    default:
      break;
  }

  // -------------------------------------------
  // Mise à jour du compteur de formes dessinées
  // -------------------------------------------
  nbre_de_formes_dessinees++;

  // ----------------------------------------------------------------------------------------------------------
  // Effacement de l'écran toutes les "n" formes dessinées, histoire d'y voir plus clair, à intervalle régulier
  // ----------------------------------------------------------------------------------------------------------
  if (nbre_de_formes_dessinees >= 150) {
    tft.fillScreen(ILI9341_BLACK);          // Effacement de l’écran (remplissage en noir, si vous préférez)
    nbre_de_formes_dessinees = 0;
  }

  // ----------------------------------------------------------------
  // Petite pause avant de reboucler, pour dessiner la forme suivante
  // ----------------------------------------------------------------
  delay(100);

}