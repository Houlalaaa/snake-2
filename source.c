/**
*
* \brief jeu de snake dans le terminal
*
* \author TESSIER leo
*
* \version 4.0
*
* \date 25 Novembre 2024
*
* Cette mise a jour ajoute des portails dans lesquelles le serpent peut passer pour se retrouver a l'opposé de lécran
* elle ajoute egalement des pommes qui, lors de la récupération augmente la taille et la vitesse du serpent
*
*/

// ajout des bibliotheques

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <time.h>


/*****************************************************
 *             DECLARATION CONSTANTES                *
 *****************************************************/

/**
*
* \def CARACTER_FIN
*
* \brief Touche pour arreter le jeu.
*
*/
#define CARACTER_FIN 'a'                

/**
*
* \def DROITE
*
* \brief Touche pour aller a  droite.
*
*/
#define DROITE 'd'

/**
*
* \def GAUCHE
*
* \brief Touche pour aller a gauche.
*
*/
#define GAUCHE 'q'

/**
*
* \def BAS
*
* \brief Touche pour aller en bas.
*
*/
#define BAS 's'

/**
*
* \def HAUT
*
* \brief Touche pour aller en haut.
*
*/
#define HAUT 'z'

/**
*
* \def COORDONNE_HORIZONTALE_DEPART
*
* \brief Position initiale X du serpent.
*
*/
#define COORDONNE_HORIZONTALE_DEPART 40

/**
*
* \def COORDONNE_VERTICALE_DEPART
*
* \brief Position initiale Y du serpent.
*
*/
#define COORDONNE_VERTICALE_DEPART 20

/**
*
* \def CORPS
*
* \brief Caractere pour le corps du serpent.
*
*/
#define CORPS 'X'

/**
*
* \def TETE
*
* \brief Caractere pour la tete du serpent.
*
*/
#define TETE 'O'

/**
*
* \def HAUTEUR_PLAT
*
* \brief Hauteur du plateau.
*
*/
#define HAUTEUR_PLAT 40

/**
*
* \def LARGEUR_PLAT
*
* \brief Largeur du plateau.
*
*/
#define LARGEUR_PLAT 80

/**
*
* \def TAILLE_MAX
*
* \brief taille max du serpent.
*
*/
#define TAILLE_MAX 20

/**
*
* \def CARACTERE_VIDE
*
* \brief caractere vide.
*
*/
#define CARACTERE_VIDE ' '

/**
*
* \def OBSTACLE
*
* \brief Caractere pour les obstacles.
*
*/
#define OBSTACLE '#'     

/**
*
* \def CHAR_POMME
*
* \brief caractère de la pomme.
*
*/
#define CHAR_POMME '6'

/*****************************************************
 *    INITIALISATION DES FONCTIONS ET PROCEDURES     *
 *****************************************************/

int kbhit();
void gotoXY(int x, int y);
void afficher(int x, int y, char c);
void effacer(int x , int y);
void dessinerSerpent(int lesX[], int lesY[]);
void progresser(int lesX[], int lesY[], char direction, char *ancienne_direction, bool *colision, bool *mange);
void disableEcho();
void enableEcho();
void initPlateau(char plateau[HAUTEUR_PLAT][LARGEUR_PLAT]);
void afficherPlateau(char plateau[HAUTEUR_PLAT][LARGEUR_PLAT]);
void ajoute_pomme();


/**
*
* \typedef typTab
*
* \brief type tableau a double entree
*
* Le type typTab sert de stockage pour les valeur du plateau.
*
*/
typedef char typtab[HAUTEUR_PLAT][LARGEUR_PLAT];   //crÃƒÂ©ation du type du plateau
typtab plateau;

// definition des variables globales

int TAILLE = 10;
int VITESSE = 200000;

/*****************************************************
 *              PROGRAMME PRINCIPAL                  *
 *****************************************************/

int main() {
//definintion des variables
    int tabX[TAILLE_MAX], tabY[TAILLE_MAX]; 
    char caractere = DROITE, ancienne_direction = DROITE;
    bool colision = false , mange = false ;
//initialisation des aprametre de bae du jeu
    srand(time(NULL));
    system("clear");

    for (int i = 0; i < TAILLE; i++) {
        tabX[i] = COORDONNE_HORIZONTALE_DEPART - i;
        tabY[i] = COORDONNE_VERTICALE_DEPART;
    }

    initPlateau(plateau);
    system("clear");
    afficherPlateau(plateau);
    dessinerSerpent(tabX, tabY);
    ajoute_pomme();
    disableEcho();

// Boucle principale du jeu
    while (caractere != CARACTER_FIN) {
        progresser(tabX, tabY, caractere, &ancienne_direction , &colision , &mange);
        usleep(VITESSE);  

    //verification de pressage des touches
        if (kbhit()) {  
            char touche = getchar();

    //empeche l'utilisateur d'utiliser des touches non autorisees
        if (touche == DROITE || touche == GAUCHE || touche == BAS || touche == HAUT || touche == CARACTER_FIN) {
                caractere = touche;
            }
        }

    //verifie si le serpent touche un obstacle
        if (colision == true){
            caractere=CARACTER_FIN ;
        }

    //verifie quand une pomme est mangee
        if (mange == true){

        //verifie su la taille max est atteinte
            if(TAILLE >=TAILLE_MAX){
                caractere=CARACTER_FIN ;
            }

            ajoute_pomme();
            VITESSE = VITESSE -10000 ;
            mange = false;
        }
    }
//parametre de fin du jeu
    system("clear");
//verifie su la taille max est atteinte ou non
    if(TAILLE >=TAILLE_MAX){
        printf("Victoire !");
    }
    else{
        printf("Fin du jeu\n");
    }
    enableEcho();
    return EXIT_SUCCESS;
}



/*****************************************************
 *              FONCTIONS ET PROCEDURES              *
 *****************************************************/


/**
*
* \fn initPlateau(char plateau[HAUTEUR_PLAT][LARGEUR_PLAT])
*
* \brief initialise le plateau de jeu.
*
* \param plateau[HAUTEUR_PLAT][LARGEUR_PLAT] : plateau a initialiser.
*
* cree un plateau avec des bordure qui ont des portail au milieu et des bloc qui apparaisent aleatoirement.
*
*/
void initPlateau(char plateau[HAUTEUR_PLAT][LARGEUR_PLAT]) {
    // Initialisation des bords du plateau
    for (int i = 0; i < LARGEUR_PLAT; i++) {
        plateau[0][i] = OBSTACLE;           // Bord supÃƒÂ©rieur
        plateau[HAUTEUR_PLAT - 1][i] = OBSTACLE;  // Bord infÃƒÂ©rieur
    }

    for (int i = 0; i < HAUTEUR_PLAT; i++) {
        plateau[i][0] = OBSTACLE;           // Bord gauche
        plateau[i][LARGEUR_PLAT - 1] = OBSTACLE;  // Bord droit
    }
    
    // Initialisation de l'intÃƒÂ©rieur du plateau (vide)
    for (int i = 1; i < HAUTEUR_PLAT - 1; i++) {
        for (int j = 1; j < LARGEUR_PLAT - 1; j++) {
            if(plateau[i][j] != OBSTACLE){   //empeche le programme de faire apparÃƒÂ¢itre des bloc sur la zone prÃƒÂ©vue pour le serpent
            plateau[i][j] = CARACTERE_VIDE;
            }
        }
    }

    // Placement des obstacles
    for (int k = 0; k < 4; k++) {
        //genere des coordonÃƒÂ©es alÃƒÂ©atoire dans le plateau
        int y_carre = (rand() % (LARGEUR_PLAT - 7) + 2);
        int x_carre = (rand() % (HAUTEUR_PLAT - 7) + 2);
 
        while (plateau[x_carre + 5][y_carre + 5] == OBSTACLE || (x_carre >=25 && x_carre <= 45 && y_carre >= 14 && y_carre <=20)) { //verifie que les blocs n'apparaisent pas l'un sur autre
            y_carre = (rand() % (LARGEUR_PLAT - 7) + 2);
            x_carre = (rand() % (HAUTEUR_PLAT - 7) + 2);
        }
        
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                plateau[i + x_carre][j + y_carre] = OBSTACLE;
            }
        }
    }
    //initialisation des portail
    plateau[0][LARGEUR_PLAT / 2] = CARACTERE_VIDE;
	plateau[HAUTEUR_PLAT - 1][LARGEUR_PLAT / 2] = CARACTERE_VIDE;

	plateau[HAUTEUR_PLAT / 2][0] = CARACTERE_VIDE;
	plateau[HAUTEUR_PLAT / 2][LARGEUR_PLAT - 1] = CARACTERE_VIDE;
    
}

/**
*
* \fn afficherPlateau(char plateau[HAUTEUR_PLAT][LARGEUR_PLAT])
*
* \brief affiche le plateau.
*
* \param plateau[HAUTEUR_PLAT][LARGEUR_PLAT] : plateau a initialiser.
*
* Consiste à afficher le plateau.
*
*/

void afficherPlateau(char plateau[HAUTEUR_PLAT][LARGEUR_PLAT]) {
    for (int i = 0; i < HAUTEUR_PLAT; i++) {
        for (int j = 0; j < LARGEUR_PLAT; j++) {
            printf("%c", plateau[i][j]);
        }
        printf("\n");
    }
}

/**
*
* \fn progresser(int lesX[], int lesY[], char direction, char *ancienne_direction, bool *colision, bool *mange)
*
* \brief fait avancer le serpent.
*
* \param lesX :tableau de coordonees verticales du serpent.
* \param lesY :tableau de coordonees horizontales du serpent.
* \param direction : direction du serpent dan le plateau
* \param ancienne_direction : ancienne direction du serpent
* \param colision : sert a determiner si le serpent est en colision
* \param mange : sert a savoir si le serpent est sur une pomme

* Consiste à faire avance le serpent en prenant compte des direction et en detectant les obstacles,murs et pommes.
*
*/
void progresser(int lesX[], int lesY[], char direction, char *ancienne_direction, bool *colision, bool *mange) {
    effacer(lesX[TAILLE-1],lesY[TAILLE-1]); //enleve le dernier bout du serpent
    plateau[lesY[TAILLE-1]-1][lesX[TAILLE-1]-1]=' ';//enleve le dernier bout du serpent dans le tableau

    if (plateau[lesY[0]][lesX[0]] == '6' ) {//detection de la pomme
        *mange = true;
        TAILLE +=1 ;
    }
    //modifie les coordonÃƒÂ©es du serpent
    for (int i = TAILLE - 1; i > 0; i--) {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
        plateau[lesY[i]-1][lesX[i]-1]=OBSTACLE;
    }

    // Ignorer les mouvements opposÃƒÂ©s                  
    if ((direction == DROITE && *ancienne_direction == GAUCHE) ||
        (direction == GAUCHE && *ancienne_direction == DROITE) ||
        (direction == HAUT && *ancienne_direction == BAS) ||
        (direction == BAS && *ancienne_direction == HAUT)) {
        direction = *ancienne_direction;
    } else {
        *ancienne_direction = direction;
    }
    //modifie la position de la tÃƒÂªte
    if (direction == HAUT) {
        if(lesY[0]==1){
            lesY[0]=lesY[0]+39;
        }
        else{
            lesY[0]--;
        }
    }
    //detecion des direction et des portail et ajutement de la position de la tete
    if (direction == BAS) {
        if(lesY[0]==40){
            lesY[0]=lesY[0]-39;

        }
        else{
            lesY[0]++;
        }
    }
    if (direction == DROITE) {
        if(lesX[0]==80){
            lesX[0]=lesX[0]-79;

        }
        else{
            lesX[0]++;
        }
    }
    if (direction == GAUCHE) {
        if(lesX[0]==1){
            lesX[0]=lesX[0]+79;

        }
        else{
            lesX[0]--;
        }
    }


    //verifie si le serpent entre en colision
    if (plateau[lesY[0]-1][lesX[0]-1] == OBSTACLE ) {
        *colision = true;
    } else {
        dessinerSerpent(lesX, lesY);
    }
}

/**
*
* \fn dessinerSerpent(int lesX[], int lesY[])
*
* \brief affiche le serpent.
*
* \param lesX[] :tableau de coordonees verticales du serpent.
* \param lesY[] :tableau de coordonees horizontales du serpent.
*
* affiche le serpent.
*
*/
void dessinerSerpent(int lesX[], int lesY[]) {
    for (int i = 0; i < TAILLE; i++) {
        if (i == 0) {
            afficher(lesX[i], lesY[i], TETE);  // La tÃƒÂªte du serpent
        } else {
            afficher(lesX[i], lesY[i], CORPS); // Le corps du serpent
        }
    }
}

/**
*
* \fn afficher(int x, int y, char c) 
*
* \brief affiche un caractere.
*
* \param x : coordonnee verticale.
* \param y : coordonnee horizontale.
* \param c : caractere a afficher.
*
* affiche un caractere.
*
*/
void afficher(int x, int y, char c) {
    gotoXY(x, y);
    printf("%c", c);
    fflush(stdout);
}

/**
*
* \fn effacer(int x, int y)
*
* \brief efface un caractere.
*
* \param x : coordonnee verticale.
* \param y : coordonnee horizontale.
*
* efface un caractere.
*
*/
void effacer(int x, int y) {
    afficher(x, y, ' ');
}

/**
*
* \fn gotoXY(int x, int y)
*
* \brief changer la position du curseur.
*
* \param x : coordonnee verticale.
* \param y : coordonnee horizontale.
*
* changer la position du curseur.
*
*/
void gotoXY(int x, int y) {
    printf("\033[%d;%df", y, x);
}

/**
*
* \fn disableEcho().
*
* \brief empeche decrire dans le terminal.
*
* empeche d'ecrire dans le terminal.
*
*/
void disableEcho() {
    struct termios tty;
    if (tcgetattr(STDIN_FILENO, &tty) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }
    tty.c_lflag &= ~ECHO;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

/**
*
* \fn enableEcho().
*
* \brief permet decrire dans le terminal.
*
* permet d'ecrire dans le terminal.
*
*/
void enableEcho() {
    struct termios tty;
    if (tcgetattr(STDIN_FILENO, &tty) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }
    tty.c_lflag |= ECHO;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

/**
*
* \fn int kbhit()
*
* \brief Fonction qui verifie si un touche est presse.
*
* \return 1 si oui 2 sinon.
*
* verifie si une touche est presse our indiquer le direction du serpent.
*
*/
int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

/**
*
* \fn ajoute_pomme()
*
* \brief ajoute une pomme.
*
* ajoute une pomme dans le plateau en dehors des carre genere et du serpent.
*
*/
void ajoute_pomme(){
    int x_carre = (rand() % (LARGEUR_PLAT - 2) + 2);
    int y_carre = (rand() % (HAUTEUR_PLAT - 2) + 2);
    while (plateau[y_carre-1][x_carre-1] != CARACTERE_VIDE ) { //verifie que les la pomme n'apparait pas sur le serpent et les bloc
        x_carre = (rand() % (LARGEUR_PLAT - 2) + 2);
        y_carre = (rand() % (HAUTEUR_PLAT - 2) + 2);
    }
    plateau[y_carre][x_carre] = CHAR_POMME;
    afficher(x_carre,y_carre,CHAR_POMME);


}