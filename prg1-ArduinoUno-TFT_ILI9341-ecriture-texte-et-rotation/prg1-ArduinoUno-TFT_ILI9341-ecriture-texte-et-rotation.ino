/*
   ______               _                  _///_ _           _                   _
  /   _  \             (_)                |  ___| |         | |                 (_)
  |  [_|  |__  ___  ___ _  ___  _ __      | |__ | | ___  ___| |_ _ __ ___  _ __  _  ___  _   _  ___
  |   ___/ _ \| __|| __| |/ _ \| '_ \_____|  __|| |/ _ \/  _|  _| '__/   \| '_ \| |/   \| | | |/ _ \
  |  |  | ( ) |__ ||__ | | ( ) | | | |____| |__ | |  __/| (_| |_| | | (_) | | | | | (_) | |_| |  __/
  \__|   \__,_|___||___|_|\___/|_| [_|    \____/|_|\___|\____\__\_|  \___/|_| |_|_|\__  |\__,_|\___|
                                                                                      | |
                                                                                      \_|
  Fichier :       prg1-ArduinoUno-TFT_ILI9341-ecriture-texte-et-rotation.ino
  
  Description :   Programme permettant d'afficher du texte et de faire des rotations d'écran,
                  à partir d'un Arduino Uno, sur un écran TFT de 2,8" à contrôleur ILI9341

  Licence :       BY-NC-ND 4.0 CC (https://creativecommons.org/licenses/by-nc-nd/4.0/deed.fr)

  Remarques :     Version avec ponts diviseurs de tension résistifs (1K/2K, pour passer du 5V au 3,3V environ)
  
  Auteur :        Jérôme TOMSKI (https://passionelectronique.fr/)
  Créé le :       27.08.2025
  
*/

// Inclusion des librairies dont nous aurons besoin ici
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// Définition des broches utilisées ici (hors pins SPI)
#define TFT_CS    10            // Pin D10 de l'Arduino -> broche CS de l'écran TFT
#define TFT_RST   9             // Pin D9  de l'Arduino -> broche RESET de l'écran TFT (aussi nommée RST, dans certains cas)
#define TFT_DC    8             // Pin D8  de l'Arduino -> broche DC de l'écran TFT

// Définition des couleurs utilisées ici
#define NOIR      0x0000
#define BLEU      0x001F
#define VERT      0x07E0
#define CYAN      0x07FF
#define ROUGE     0xF800
#define MAGENTA   0xF81F
#define ORANGE    0xFBA0
#define JAUNE     0xFFE0
#define BLANC     0xFFFF

// Initialisation de l’écran TFT (contrôleur ILI9341)
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);     // Le reste = SPI matériel (hardware spi), donc il ne faut pas le renseigner ;
                                                                      // sinon on passerait en émulation logicielle (software spi)

// Tableau de couleurs possibles ici, à personnaliser/compléter selon ses envies (avec son index correspondant, pour les faire afficher l'une après l'autres)
uint16_t couleurs[] = {BLEU, VERT, CYAN, ROUGE, MAGENTA, ORANGE, JAUNE, BLANC};
uint8_t indexCouleur = 0;
uint8_t nombreDeCouleursPossibles;


// ========================
// Initialisation programme
// ========================
void setup() {

  // Initialisation de l’écran
  tft.begin();

  // Calcul du nombre de différentes couleurs à gérer
  nombreDeCouleursPossibles = sizeof(couleurs) / sizeof(uint16_t);      // Pour avoir le nombre d'éléments présents dans notre "tableau" de couleurs, il faut
                                                                        // diviser la taille totale de ce tableau, par la taille de chaque élément de ce tableau
}


// =================
// Boucle principale
// =================
void loop() {

  // Parcours des 4 orientations d'écran possible (rotation de 90° à chaque fois, donc)
  for (int rotation = 0; rotation < 4; rotation++) {
    // Nota : démarrage portrait, puis tourne de 90 en 90°, en sens horaire

    // Fixation du sens de rotation souhaité
    tft.setRotation(rotation);
    
    // Effacement de l'écran
    tft.fillScreen(NOIR);

    // Affichage de texte en mode "normal" (écriture selon couleur choisie, sur fond noir, par exemple)
    tft.setCursor(0, 30);
    tft.setTextColor(couleurs[indexCouleur], NOIR);   // Nota : on aurait pu simplement écrire "tft.setTextColor(couleurs[indexCouleur])", sans préciser la couleur du fond,
    tft.setTextSize(1);                               //        car celui-ci est déjà noir (cf. fonction setup plus haut) ; ainsi, on écrirait en "transparence", au niveau
    tft.println("Texte normal (taille 1)");           //        de l'arrière plan du texte (très utile, si vous souhaitez un jour écrire du texte sur une image, par exemple)

    // Affichage de texte en mode inversé (écriture noire, sur fond de la couleur choisie)
    tft.setCursor(0, 80);
    tft.setTextSize(2);
    tft.setTextColor(NOIR, couleurs[indexCouleur]); // Texte noir sur fond blanc
    tft.println("Texte inverse");
    tft.println("(taille 2)");

    // Et... un affichage en taille 3, histoire de montrer ce que ça donne !
    tft.setCursor(0, 150);
    tft.setTextColor(couleurs[indexCouleur], NOIR);
    tft.setTextSize(3);
    tft.println("Taille 3 !");
    
    // Attente d'une seconde, avant de reboucler
    delay(1000);

    // Et changement de couleur, pour le prochain tour !
    indexCouleur++;
    if(indexCouleur >= nombreDeCouleursPossibles) indexCouleur = 0;
  }

}