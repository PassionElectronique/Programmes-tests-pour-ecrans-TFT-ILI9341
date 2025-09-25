/*
   ______               _                  _///_ _           _                   _
  /   _  \             (_)                |  ___| |         | |                 (_)
  |  [_|  |__  ___  ___ _  ___  _ __      | |__ | | ___  ___| |_ _ __ ___  _ __  _  ___  _   _  ___
  |   ___/ _ \| __|| __| |/ _ \| '_ \_____|  __|| |/ _ \/  _|  _| '__/   \| '_ \| |/   \| | | |/ _ \
  |  |  | ( ) |__ ||__ | | ( ) | | | |____| |__ | |  __/| (_| |_| | | (_) | | | | | (_) | |_| |  __/
  \__|   \__,_|___||___|_|\___/|_| [_|    \____/|_|\___|\____\__\_|  \___/|_| |_|_|\__  |\__,_|\___|
                                                                                      | |
                                                                                      \_|
  Fichier :       prg3-ArduinoUno-TFT_ILI9341-dessiner-a-la-main.ino
  
  Description :   Programme permettant de dessiner à la main, en choisissant l'épaisseur et la couleur de traçage
                  (projet mettant en oeuvre un Arduino Uno + écran TFT 2,8" SPI ILI9341)

  Licence :       BY-NC-ND 4.0 CC (https://creativecommons.org/licenses/by-nc-nd/4.0/deed.fr)

  Remarques :     Version utilisant un convertisseur 5V / 3,3V (TXS0108E)
  
  Auteur :        Jérôme TOMSKI (https://passionelectronique.fr/)
  Créé le :       01.09.2025
  
*/

// Inclusion des librairies dont nous aurons besoin ici
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>

// Définition des broches utilisées ici (hors pins SPI)
#define TFT_CS    10            // Pin D10 de l'Arduino -> broche CS de l'écran TFT
#define TFT_RST   9             // Pin D9  de l'Arduino -> broche RST de l'écran TFT
#define TFT_DC    8             // Pin D8  de l'Arduino -> broche DC de l'écran TFT
#define TOUCH_CS  7             // Pin D7  de l'Arduino -> broche CS de la partie tactile (touch) de l'écran TFT

// Initialisation de l’écran TFT (contrôleur ILI9341)
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);     // Le reste = SPI matériel (hardware spi), donc il ne faut pas le renseigner ;
                                                                      // sinon on passerait en émulation logicielle (software spi)
// Initialisation de la partie tactile de l'écran
XPT2046_Touchscreen ts = XPT2046_Touchscreen(TOUCH_CS);

// Tableau de couleurs sélectionnables ici, pour dessiner
uint16_t couleursSelectionnables[] = {ILI9341_LIGHTGREY, ILI9341_YELLOW, ILI9341_GREEN, ILI9341_BLUE, ILI9341_MAGENTA, ILI9341_RED};

// Variables
uint8_t tailleCrayonDeDessinSelectionne;
uint8_t couleurDuCrayonSelectionnee;
uint16_t tactile_min_X;
uint16_t tactile_min_Y;
uint16_t tactile_max_X;
uint16_t tactile_max_Y;
uint16_t point_touche_x;
uint16_t point_touche_y;
uint16_t point_touche_z;


// ========================
// Initialisation programme
// ========================
void setup() {

  // Initialisation de l’écran
  tft.begin();
  tft.setRotation(0);                   // Mode portrait (vertical)
  tft.fillScreen(ILI9341_BLACK);        // Fond noir

  // Initialisation du tactile
  ts.begin();
  ts.setRotation(0);                    // Synchronisation avec l’écran

  // Initialisation des variables de démarrage
  tailleCrayonDeDessinSelectionne = 1;
  couleurDuCrayonSelectionnee = 0;

  // Calibration de l'écran tactile (actions manuelles, demandées à l'utilisateur, à même l'écran TFT)
  calibration_ecran_tactile();
  
  // Affichage initial
  affichage_barre_du_haut();            // Affiche les boutons de commande (différentes tailles et bouton "effacer")
  affichage_barre_du_bas();             // Affiche les boutons de commande (différentes couleurs sélectionnables)

}


// =================
// Boucle principale
// =================
void loop() {

  // Attend qu'un endroit soit touché
  if (ts.touched()) {

    // Récupération des "coordonnées" du point touché
    TS_Point p = ts.getPoint();
    point_touche_x = p.x;
    point_touche_y = p.y;

    // Rognage des points éventuellement "sortant"
    if(point_touche_x < tactile_min_X) point_touche_x = tactile_min_X;
    if(point_touche_x > tactile_max_X) point_touche_x = tactile_max_X;
    if(point_touche_y < tactile_min_Y) point_touche_y = tactile_min_Y;
    if(point_touche_y > tactile_max_Y) point_touche_y = tactile_max_Y;

    // Adaptation des coordonnées (les positions sur l'écran tactile allant de 200 à 3700 environ, alors que nous, l'écran fait 240 x 320 px)
    point_touche_x = map(point_touche_x, tactile_min_X, tactile_max_X, 0, 240);
    point_touche_y = map(point_touche_y, tactile_min_Y, tactile_max_Y, 0, 320);

    // Teste si l'un des boutons du haut ou du bas a été appuyé
    teste_si_bouton_appuye_en_haut(point_touche_x, point_touche_y);
    teste_si_bouton_appuye_en_bas(point_touche_x, point_touche_y);

    // Dessine un cercle à l'endroit visé
    if(point_touche_y > 40 && point_touche_y < 280) {
      tft.fillCircle(point_touche_x, point_touche_y, tailleCrayonDeDessinSelectionne * 4, couleursSelectionnables[couleurDuCrayonSelectionnee]);
    }
    
  }

  // Petit délai de rebouclage, histoire de réduire la charge CPU
  delay(10);

}


// ====================================
// Fonction : calibration_ecran_tactile
// ====================================
void calibration_ecran_tactile() {

  // --------------------------------------------------------------------------------
  // Pointage du coin haut/gauche (pour récupérer le minX et minY de l'écran tactile)
  // --------------------------------------------------------------------------------

  // Dessin de la flèche montrant où appuyer sur l'écran, pour initialiser ce 1er point
  tft.drawLine(1, 1, 11,  1, ILI9341_GREEN);
  tft.drawLine(1, 1,  1, 11, ILI9341_GREEN);
  tft.drawLine(1, 1, 21, 21, ILI9341_GREEN);
  delay(300);

  // Affichage du texte correspondant
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.setCursor(10, 40);    tft.println("Appuyez ici !");
  tft.setTextColor(ILI9341_LIGHTGREY);
  tft.setTextSize(1);
  tft.setCursor(10, 60);    tft.println("(au bout de la fleche,");
  tft.setCursor(10, 70);    tft.println("avec un stylet)");

  // Attend que ce coin haut/gauche soit touché
  while (!ts.touched()) {}

  // Récupération des infos du point touché
  TS_Point premier_point = ts.getPoint();

  // Enregistrement des données
  tactile_min_X = premier_point.x;
  tactile_min_Y = premier_point.y;

  // Effacement de l'écran pour passer à la suite
  tft.fillScreen(ILI9341_BLACK);

  // ------------------------------------------------------------------------------
  // Pointage du coin bas/droit (pour récupérer le maxX et maxY de l'écran tactile)
  // ------------------------------------------------------------------------------

  // Dessin de la flèche montrant où appuyer sur l'écran, pour initialiser ce 2ème point
  tft.drawLine(239, 319, 239, 309, ILI9341_GREEN);
  tft.drawLine(239, 319, 229, 319, ILI9341_GREEN);
  tft.drawLine(239, 319, 219, 299, ILI9341_GREEN);
  delay(300);

  // Affichage du texte correspondant
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.setCursor(50, 220);    tft.println("Et appuyez la !");
  tft.setTextColor(ILI9341_LIGHTGREY);
  tft.setTextSize(1);
  tft.setCursor(50, 240);    tft.println("(au bout de la fleche,");
  tft.setCursor(50, 250);    tft.println("avec un stylet)");

  // Attend que ce coin bas/droit soit touché
  while (!ts.touched()) {}

  // Récupération des infos du point touché
  TS_Point second_point = ts.getPoint();

  // Enregistrement des données
  tactile_max_X = second_point.x;
  tactile_max_Y = second_point.y;

  // Et nettoyage de l'écran, enfin, avant de passer à la suite !
  tft.fillScreen(ILI9341_BLACK);
  delay(300);

}

// ==================================
// Fonction : affichage_barre_du_haut
// ==================================
void affichage_barre_du_haut() {

  // Fond de la barre du haut
  tft.fillRect(  0, 0,240, 40, ILI9341_LIGHTGREY);

  // Quatre options de la barre du haut
  tft.drawRect(  0, 0, 60, 40, ILI9341_BLACK);
  tft.drawRect( 60, 0, 60, 40, ILI9341_BLACK);
  tft.drawRect(120, 0, 60, 40, ILI9341_BLACK);
  tft.drawRect(180, 0, 63, 40, ILI9341_BLACK);

  // Trois traits, pour les 3 premières options (choix grosseur de crayon, en fait)
  tft.fillRect( 15, 19, 30,  2, ILI9341_BLACK);
  tft.fillRect( 75, 18, 30,  4, ILI9341_BLACK);
  tft.fillRect(134, 16, 30,  8, ILI9341_BLACK);

  // Texte de la 4ème option
  tft.setCursor(188, 16);
  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(1);
  tft.print("EFFACER");

  // Entourage de la taille choisie ("2 pixels" de large)
  tft.drawRect(60*(tailleCrayonDeDessinSelectionne - 1), 0, 60, 40, ILI9341_WHITE);
  tft.drawRect(60*(tailleCrayonDeDessinSelectionne - 1) + 1, 1, 58, 38, ILI9341_WHITE);

}

// =================================
// Fonction : affichage_barre_du_bas
// =================================
void affichage_barre_du_bas() {

  // Affichage des 6 couleurs possibles en bas (blocs de 40x40 pixels, la hauteur max du TFT faisant 320px)
  tft.fillRect(  0, 280, 40, 40, couleursSelectionnables[0]);
  tft.fillRect( 40, 280, 40, 40, couleursSelectionnables[1]);
  tft.fillRect( 80, 280, 40, 40, couleursSelectionnables[2]);
  tft.fillRect(120, 280, 40, 40, couleursSelectionnables[3]);
  tft.fillRect(160, 280, 40, 40, couleursSelectionnables[4]);
  tft.fillRect(200, 280, 40, 40, couleursSelectionnables[5]);

  // Entourage de la couleur choisie ("2 pixels" de large)
  tft.drawRect(40*couleurDuCrayonSelectionnee, 280, 40, 40, ILI9341_WHITE);
  tft.drawRect(40*couleurDuCrayonSelectionnee + 1, 281, 38, 38, ILI9341_WHITE);

}


// ==========================================
// Fonction : teste_si_bouton_appuye_en_haut
// ==========================================
void teste_si_bouton_appuye_en_haut(uint16_t x, uint16_t y) {

  // On sort, si on n'a pas appuyé dans cette zone
  if(y > 40) {
    return;
  }

  // Test si 1er choix du haut touché
  if(x >= 10 && x < 50 && y >= 0 && y < 40) {
    tailleCrayonDeDessinSelectionne = 1;
  }

  // Test si 2ème choix du haut touché
  if(x > 70 && x < 110 && y >= 0 && y < 40) {
    tailleCrayonDeDessinSelectionne = 2;
  }

  // Test si 3ème choix du haut touché
  if(x > 130 && x < 170 && y >= 0 && y < 40) {
    tailleCrayonDeDessinSelectionne = 3;
  }

  // Test si 4ème choix du haut touché (bouton "effacer")
  if(x >= 180 && x < 240 && y >= 0 && y < 40) {
    tft.fillScreen(ILI9341_BLACK);
    affichage_barre_du_bas();
  }

  // Rafraîchissement partie haute/basse
  affichage_barre_du_haut();

}

// ========================================
// Fonction : teste_si_bouton_appuye_en_bas
// ========================================
void teste_si_bouton_appuye_en_bas(uint16_t x, uint16_t y) {

  // Teste si on est bien dans la zone du bas
  if(x >= 0 && x < 240 && y >= 280 && y < 320) {
    // Récupère la couleur sélectionnée (chaque "couleur" faisant 40px de large)
    couleurDuCrayonSelectionnee = x / 40;

    // Rafraîchissement partie haute/basse
    affichage_barre_du_bas();
  }


}