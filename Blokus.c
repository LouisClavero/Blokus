#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <time.h>
#include <conio.h>

//definition avant compilation des macros qui definissent les valeurs des bords utilises pour notre jeu

#define TOPLEFT_CORNER  201
#define TOPRIGHT_CORNER  187
#define BOTTOMLEFT_CORNER  200
#define BOTTOMRIGHT_CORNER  188
#define HORIZONTAL_BORDER  205
#define SIDE_BORDER  186
#define SIZE_FIGURE 5

/** definition de la structure de la figure
**  le tableau jouee indique si un joueur a déjà joué cette pièce
**  size est la taille de la figure (en nombre de carrés)
**/
typedef struct figure
{
    char jouee[4];
    char lettre;
    char fig[SIZE_FIGURE][SIZE_FIGURE];
    int size;
} t_figure;

/** Definition de la structure joueur
**  si perdu vaut 1, le joueur a perdu.
**  pieces_jouees représente le nombre de pièces jouees.
**/
typedef struct joueur
{
    int num;
    int couleur_texte;
    int couleur_fond;
    int perdu;
    int pieces_jouees;
} t_joueur;

//prototypes des fonctions utilisées
void color(int texte, int fond);
void gotoligcol(int lig, int col);
void print_menu();
void print_menu2(int n);
int get_nombre_joueurs();
int get_nombre_humains(int n);
void print_regles();
void print_indexes();
void fill_game();
void fill_guide(char** fig, int couleur_texte, int couleur_fond);
void clear_guide();
void print_available(char available[12][19], t_joueur* j, t_figure liste_figures[21]);
void print_borners(int min_line, int min_column, int max_line, int max_column);
void rotate_droite(char** figure);
void rotate_gauche(char** figure);
void rotate_miroir(char** figure);
void initialise_jeu(t_figure liste_figures[21]);
void initialise_joueurs(t_joueur liste_joueurs[4]);
int verif (int game[20][20], char** fig, int lig, int col, int n, t_joueur* joueur_actuel, int tour);
void placer(int game[20][20], char** fig, int lig, int col, int n, t_joueur* joueur_actuel);

/***************************************************************
 Procedure: color
 Arguments: les couleurs du texte et du fond en int
 Retour:
 Auteur:Louis CLAVERO, Benjamin FORNACIARI, Ivan MAULINE
 Commentaire: permet de définir la couleur du futur texte à
              afficher dans la console
****************************************************************/
void color(int texte, int fond)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),fond*16+texte);
}

/***************************************************************
 Procedure: gotoligcol
 Arguments: la position en ligne et en colonne (deux int)
 Retour:
 Auteur:Louis CLAVERO, Benjamin FORNACIARI, Ivan MAULINE
 Commentaire: copiée du cours, permet de placer le curseur
              pour l'affichage sans effet stromboscopique
****************************************************************/
void gotoligcol(int lig, int col)
{
    COORD mycoord;
    mycoord.X = col;
    mycoord.Y = lig;
    SetConsoleCursorPosition( GetStdHandle( STD_OUTPUT_HANDLE ), mycoord );
}

/***************************************************************
 Procédure: printmenu
 Arguments:
 Retour:
 Auteur: Louis CLAVERO, Benjamin FORNACIARI, Ivan MAULINE
 Commentaire: permet d'afficher le premier menu pour selectionner
              le nombre de joueurs
****************************************************************/
void print_menu()
{
    gotoligcol(0,0);
    printf("    MENU:   ");
    gotoligcol(2,0);
    printf("(humains et machines confondus)");
    gotoligcol(4,0);
    printf("2. Mode 2 joueurs");
    gotoligcol(6,0);
    printf("3. Mode 3 joueurs");
    gotoligcol(8,0);
    printf("4. Mode 4 joueurs");
}

/***************************************************************
 Procédure: print_menu2
 Arguments: le nombre de joueurs répondu au menu précédent
 Retour:
 Auteur: Louis CLAVERO, Benjamin FORNACIARI, Ivan MAULINE
 Commentaire: permet d'afficher le deuxième menu pour selectionner
              le nombre de joueurs humains
****************************************************************/
void print_menu2(int n)
{
    int i;
    gotoligcol(0,0);
    printf("    MENU:   ");
    for(i=0;i<n+1;i++){
        gotoligcol(4+2*i,0);
        printf("%d. Mode %d joueur(s) humain(s)", i, i);
    }
}
/***************************************************************
 Fonction: get_nombre_joueurs
 Arguments:
 Retour: le nombre de joueurs (humains+machines)
 Auteur: Louis CLAVERO, Benjamin FORNACIARI, Ivan MAULINE
 Commentaire: permet de récupérer depuis la console le nombre
              entré par l'utilisateur
****************************************************************/
int get_nombre_joueurs()
{
    //Declaration des variables
    char choice[4];
    //boucle tant que la saisie n'est pas correcte
    do
    {
        gotoligcol(10, 0);
        printf("Entrer votre choix (2-4):              ");
        gotoligcol(10, 26);
        fgets(choice, 4, stdin);
        fflush(stdin);
        choice[strlen(choice)-1]='\0';
    }
    //blindage effectué grâce à une chaine de caracteres, tant qu'il y a
    //plus d'un caractere et qu'il n'est pas 2, 3 ou 4
    while(strlen(choice)!=1 || choice[0]<'2' || choice[0]>'4');
    return atoi(choice);
}

/***************************************************************
 Fonction: get_nombre_humains
 Arguments: le nombre de joueurs répondu au menu précédent
 Retour: le nombre de joueur (humains+machines)
 Auteur: Louis CLAVERO, Benjamin FORNACIARI, Ivan MAULINE
 Commentaire: permet de récupérer depuis la console le nombre
              entré par l'utilisateur
****************************************************************/
int get_nombre_humains(int n){
    //declaration des variables
    char choice[4];
    //boucle tant que la saisie n'est pas correcte
    do
    {
        gotoligcol(14, 0);
        printf("Entrer le nombre de joueurs humains (0-%d):              ", n);
        gotoligcol(14, 42);
        fgets(choice, 4, stdin);
        fflush(stdin);
        choice[strlen(choice)-1]='\0';
    }
    //Même blindage qu'à la fonction précédente.
    while(strlen(choice)!=1 || choice[0]<'0' || choice[0]>(char)('0'+n));
    return atoi(choice);
}

/***************************************************************
 Procedure: print_regles
 Arguments:
 Retour:
 Auteur: Louis CLAVERO, Benjamin FORNACIARI, Ivan MAULINE
 Commentaire: permet d'afficher les règles après les 2 menus
****************************************************************/
void print_regles()
{
    gotoligcol(0,0);
    printf("Le jeu se compose d un plateau de 400 cases dans lesquelles encastrer des pieces de differentes formes.");
    printf("\nChacune des 21 pieces est de forme differentes, un peu comme au Tetris.");
    printf("\nLe but de chaque joueur est de placer ses 21 pieces sur le plateau, tout en essayant de bloquer ses adversaires.");
    printf("\nIl est possible de jouer a 2, 3 ou 4 personnes (4 couleurs differentes de pieces).");
    printf("\nLe premier joueur pose la piece de son choix sur le plateau de telle sorte que celle-ci recouvre une case d angle du plateau.");
    printf("\nLes autres joueurs jouent a tour de role et placent leurs pieces de la meme maniere.");
    printf("\nPour les tours suivants, chaque nouvelle piece posee doit toucher une piece de la meme couleur par un ou plusieurs coins et jamais par les cotes.");
    printf("\nEn revanche, les pieces de couleur differente peuvent se toucher par les cotes.");
    printf("\nIci,Lorsque l'on choisit la piece a poser,on donne la coordonnee de la case la plus en haut a gauche de la piece");
    printf("\nLe joueur 1 est en haut a gauche et est de couleur rouge");
    printf("\nLe joueur 2 est en haut a droite et est de couleur bleu");
    printf("\nLe joueur 3 est en bas a gauche et est de couleur vert");
    printf("\nLe joueur 4 est en bas a droite et est de couleur jaune");
    printf("\n\n                                                   GOOD LUCK !");
    printf("\n\n                                          Appuyez sur ESPACE pour jouer");

}


/***************************************************************
 Procédure: print_indexes
 Arguments:
 Retour:
 Auteur: Louis CLAVERO, Benjamin FORNACIARI, Ivan MAULINE
 Derniere modification: 20/10/2017
 Commentaire: permet d'afficher les indices des cases du jeu
              de A à T et de 1 à 20
****************************************************************/
void print_indexes()
{
    //Declaration des variables
    int i;
    char top_index = 'A';
    //Affichage des lettres (on part de i=3 pour sauter les bordures et indices verticaux
    for(i=3; i<43; i++)
    {
        //on saute un espace entre chaque indice des colonnes pour que le jeu ressemble à un carré
        if(i%2 != 0)
        {
            gotoligcol(0, i);
            printf("%c", top_index);
            //Permet de passer à la lettre suivante pour la boucle d'après
            top_index++;
        }
    }
    //Affichage des chiffres
    for(i=1; i<21; i++)
    {
        gotoligcol(i+1, 0);
        printf("%d", i);
    }
}

/***************************************************************
 Procédure: fill_game
 Arguments:
 Retour:
 Auteur: Louis CLAVERO, Benjamin FORNACIARI, Ivan MAULINE
 Commentaire: permet d'afficher des '.' dans la matrice de jeu
****************************************************************/
void fill_game()
{
    //Declaration des variables
    int i, j;
    for(i=0; i<20; i++)
    {
        for(j=0; j<40; j++)
        {
            //Saut d'une case pour que les zones de jeu ressemblent à des carrés
            if(j%2 == 0)
            {
                gotoligcol(i+2,j+3);
                printf("%c", '.');
            }
        }
    }
}

/***************************************************************
 Procédure: fill_guide
 Arguments: tableau 5x5 de la figure à afficher, la
            couleur du texte et la couleur du fond
            (pour color(int, int))
 Retour:
 Auteur: Louis CLAVERO, Benjamin FORNACIARI, Ivan MAULINE
 Commentaire: permet d'afficher dans la zone de guide la figure
              passée en paramètre avec ses couleurs
****************************************************************/
void fill_guide(char** fig, int couleur_texte, int couleur_fond)
{
    //Declaration des variables
    int i, j;

    for(i=0; i<SIZE_FIGURE; i++)
    {
        for(j=0; j<SIZE_FIGURE*2; j++)
        {
            //Saut d'une case pour que les zones de jeu ressemblent à des carrés
            if(j%2 == 0)
            {
                gotoligcol(i+17,j+46);
                //Si c'est une case de la figure, on définit sa couleur
                if(fig[i][j/2]!='.') color(couleur_texte, couleur_fond);
                printf("%c", fig[i][j/2]);
                //Puis on remet la couleur par défaut (blanc sur noir)
                if(fig[i][j/2]!='.') color(15, 0);
            }
        }
    }
}

/***************************************************************
 Procédure: clear_guide
 Arguments:
 Retour:
 Auteur: Louis CLAVERO, Benjamin FORNACIARI, Ivan MAULINE
 Commentaire: permet de "clear" l'affichage de la zone de guide
              (réafficher des espaces)
****************************************************************/
void clear_guide(){
    int i;
    for(i=0; i<SIZE_FIGURE; i++){
        gotoligcol(i+17, 46);
        printf("          ");
    }
}

/***************************************************************
 Procédure: print_available
 Arguments: tableau 12x19 de la zone de figures disponibles,
 liste de figures pour verifier que la piece a deja ete jouee,
 un pointeur sur un joueur
 Retour:
 Auteur: Louis CLAVERO, Benjamin FORNACIARI, Ivan MAULINE
 Commentaire: permet d'afficher la liste des pieces jouables par
              chaque joueur, grisees si déja jouees
****************************************************************/
void print_available(char available[12][19], t_joueur* jo, t_figure liste_figures[21])
{
    //Declaration des variables
    int i, j;
    for(i=0; i<12; i++)
    {
        //Saut d'une case pour que les zones de jeu ressemblent à des carrés
        for(j=0; j<37; j+=2)
        {
            gotoligcol(i+2,j+46);
            //S'il s'agit d'une case figure
            if(available[i][j/2]!='.'){
                //Si la pièce a deja ete jouee on l'affiche grisee
                if(liste_figures[(int)(available[i][j/2]-'A')].jouee[jo->num-1] == 1)
                {
                    color(jo->couleur_texte,8);
                    printf("%c", available[i][j/2]);
                    color(15, 0);
                }
                //Sinon on l'affiche dans sa couleur
                else
                {
                    color(jo->couleur_texte, jo->couleur_fond);
                    printf("%c", available[i][j/2]);
                }
            //Sinon on affiche le point en blanc sur noir
            } else{
                color(15, 0);
                printf("%c", available[i][j/2]);
            }
        }
    }
}

/***************************************************************
 Procédure: print_borders
 Arguments: les indices minimum et maximum des lignes et colonnes
            des bordures a afficher
 Retour:
 Auteur: Louis CLAVERO, Benjamin FORNACIARI, Ivan MAULINE
 Commentaire: permet d'afficher les bordures pour chacune des
              zones du jeu (tableau de jeu, guide, zone de
              disponibilite)
****************************************************************/
void print_borders(int min_line, int min_column, int max_line, int max_column)
{
    //Declaration des variables
    int i;
    //Affichage des coins
    gotoligcol(min_line, min_column);
    color(12,0);
    printf("%c", TOPLEFT_CORNER);
    gotoligcol(min_line, max_column);
    color(3, 0);
    printf("%c", TOPRIGHT_CORNER);
    gotoligcol(max_line, min_column);
    color(2, 0);
    printf("%c", BOTTOMLEFT_CORNER);
    gotoligcol(max_line, max_column);
    color(12|2, 0);
    printf("%c", BOTTOMRIGHT_CORNER);
    color(15,0);
    //Affichage des bords supérieur et inférieur
    for(i=min_column+1; i<max_column; i++)
    {
        gotoligcol(min_line, i);
        printf("%c", HORIZONTAL_BORDER);
        gotoligcol(max_line, i);
        printf("%c", HORIZONTAL_BORDER);
    }
    //Affichage des bords gauche et droit
    for(i=min_line+1; i<max_line; i++)
    {
        gotoligcol(i, min_column);
        printf("%c", SIDE_BORDER);
        gotoligcol(i, max_column);
        printf("%c", SIDE_BORDER);
    }
}

/***************************************************************
 Procédure: rotate_gauche
 Arguments: tableau de la figure a tourner
 Retour:
 Auteur: Louis CLAVERO, Benjamin FORNACIARI, Ivan MAULINE
 Commentaire: permet d'effectuer la rotation vers la gauche de la
              figure (sens anti-horaire)
 ****************************************************************/
void rotate_gauche(char** figure)
{
    //Declaration des variables
    int i;
    int j;
    char temporaire[SIZE_FIGURE][SIZE_FIGURE];
    //Copie de la figure dans une autre figure temporaire
    for (i=0; i<SIZE_FIGURE; i++)
    {
        for (j=0; j<SIZE_FIGURE; j++)
        {
            temporaire[i][j] = figure[i][j];
        }
    }
    //Mise à jour de la figure en arguments par rapport à temporaire.
    for (i=0; i<SIZE_FIGURE; i++)
    {
        for (j=0; j<SIZE_FIGURE; j++)
        {
            figure[i][j] = temporaire[j][SIZE_FIGURE-1-i];
        }
    }
}

/***************************************************************
 Procédure: rotate_droite
 Arguments: tableau de la figure a tourner
 Retour:
 Auteur: Louis CLAVERO, Benjamin FORNACIARI, Ivan MAULINE
 Commentaire: permet d'effectuer la rotation vers la droite de la
              figure (sens horaire)
 ****************************************************************/
void rotate_droite(char** figure)
{
    //Declaration des variables
    int i;
    int j;
    char temporaire[SIZE_FIGURE][SIZE_FIGURE];
    //Copie de la figure dans une autre figure temporaire
    for (i=0; i<SIZE_FIGURE; i++)
    {
        for (j=0; j<SIZE_FIGURE; j++)
        {
            temporaire[i][j] = figure[i][j];
        }
    }
    //Mise à jour de la figure en arguments par rapport à temporaire.
    for (i=0; i<SIZE_FIGURE; i++)
    {
        for (j=0; j<SIZE_FIGURE; j++)
        {
            figure[j][SIZE_FIGURE-1-i] = temporaire[i][j];
        }
    }
}

/***************************************************************
 Procédure: rotate_miroir
 Arguments: tableau de la figure a inverser
 Retour:
 Auteur: Louis CLAVERO, Benjamin FORNACIARI, Ivan MAULINE
 Commentaire: permet d'effectuer la rotation miroir (inverse) de
              la figure
 ****************************************************************/
void rotate_miroir(char** figure)
{
    //Declaration des variables
    int i;
    int j;
    char temporaire[SIZE_FIGURE][SIZE_FIGURE];
    //Copie de la figure dans une autre figure temporaire
    for (i=0; i<SIZE_FIGURE; i++)
    {
        for (j=0; j<SIZE_FIGURE; j++)
        {
            temporaire[i][j] = figure[i][j];
        }
    }
    //Mise à jour de la figure en arguments par rapport à temporaire.
    for (i=0; i<SIZE_FIGURE; i++)
    {
        for (j=0; j<SIZE_FIGURE; j++)
        {
            figure[i][SIZE_FIGURE-j-1] = temporaire [i][j];
        }
    }
}
/***************************************************************
 Procedure: initialise_jeu
 Arguments: La liste des 21 figures initiales
 Retour:
 Auteur: Louis CLAVERO, Benjamin FORNACIARI, Ivan MAULINE
 Commentaire: Permet d'initialiser les 21 figures avec leurs formes,
              tailles, le fait qu'elles n'aient pas ete jouees par
              les joueurs
 ****************************************************************/
void initialise_jeu(t_figure liste_figures[21])
{
    //Declaration des variables
    int i,j,k;
    for(i=0; i<21; i++)
    {
        //On rentre dans le champs lettre le char correspondant à la figure
        liste_figures[i].lettre = i+'A';
        //Au debut, aucun joueur n'a joue de figures
        for(j=0; j<4; j++)
        {
            liste_figures[i].jouee[j] = 0;
        }
        //On stocke dans size les tailles de chaque figure
        if(i+'A' == 'A') liste_figures[i].size = 1;
        else if(i+'A' == 'B') liste_figures[i].size = 2;
        else if(i+'A' == 'C' || i+'A' == 'D') liste_figures[i].size = 3;
        else if(i+'A' >='E' && i+'A'<='J' && i+'A' !='F') liste_figures[i].size = 4;
        else liste_figures[i].size = 5;
    }
    //On remplit de base par des '.' toutes les figures
    for(i=0; i<21; i++)
    {
        for(j=0; j<SIZE_FIGURE; j++)
        {
            for(k=0; k<SIZE_FIGURE; k++)
            {
                liste_figures[i].fig[j][k] = '.';
            }
        }
    }
    //Manuellement, on définit chaque figure avec sa lettre
    liste_figures[0].fig[2][2] = 'A';

    liste_figures[1].fig[2][2] = 'B';
    liste_figures[1].fig[2][3] = 'B';

    liste_figures[2].fig [2][1] = 'C';
    liste_figures[2].fig [2][2] = 'C';
    liste_figures[2].fig [2][3] = 'C';

    liste_figures[3].fig [2][2] = 'D';
    liste_figures[3].fig [2][3] = 'D';
    liste_figures[3].fig [3][2] = 'D';

    liste_figures[4].fig [2][0] = 'E';
    liste_figures[4].fig [2][1] = 'E';
    liste_figures[4].fig [2][2] = 'E';
    liste_figures[4].fig [2][3] = 'E';

    liste_figures[5].fig [0][2] = 'F';
    liste_figures[5].fig [1][2] = 'F';
    liste_figures[5].fig [2][2] = 'F';
    liste_figures[5].fig [3][2] = 'F';
    liste_figures[5].fig [4][2] = 'F';

    liste_figures[6].fig [1][2] = 'G';
    liste_figures[6].fig [2][2] = 'G';
    liste_figures[6].fig [3][2] = 'G';
    liste_figures[6].fig [3][3] = 'G';

    liste_figures[7].fig [1][2] = 'H';
    liste_figures[7].fig [2][2] = 'H';
    liste_figures[7].fig [3][2] = 'H';
    liste_figures[7].fig [2][3] = 'H';

    liste_figures[8].fig [2][2] = 'I';
    liste_figures[8].fig [2][3] = 'I';
    liste_figures[8].fig [3][2] = 'I';
    liste_figures[8].fig [3][3] = 'I';

    liste_figures[9].fig [2][1] = 'J';
    liste_figures[9].fig [2][2] = 'J';
    liste_figures[9].fig [3][2] = 'J';
    liste_figures[9].fig [3][3] = 'J';

    liste_figures[10].fig [1][1] = 'K';
    liste_figures[10].fig [2][1] = 'K';
    liste_figures[10].fig [2][2] = 'K';
    liste_figures[10].fig [2][3] = 'K';
    liste_figures[10].fig [3][2] = 'K';

    liste_figures[11].fig [2][1] = 'L';
    liste_figures[11].fig [2][2] = 'L';
    liste_figures[11].fig [2][3] = 'L';
    liste_figures[11].fig [2][4] = 'L';
    liste_figures[11].fig [3][4] = 'L';

    liste_figures[12].fig [3][1] = 'M';
    liste_figures[12].fig [4][1] = 'M';
    liste_figures[12].fig [3][2] = 'M';
    liste_figures[12].fig [2][2] = 'M';
    liste_figures[12].fig [1][2] = 'M';

    liste_figures[13].fig [2][2] = 'N';
    liste_figures[13].fig [2][3] = 'N';
    liste_figures[13].fig [3][2] = 'N';
    liste_figures[13].fig [3][3] = 'N';
    liste_figures[13].fig [1][3] = 'N';

    liste_figures[14].fig [1][1] = 'O';
    liste_figures[14].fig [1][2] = 'O';
    liste_figures[14].fig [2][2] = 'O';
    liste_figures[14].fig [3][2] = 'O';
    liste_figures[14].fig [3][1] = 'O';

    liste_figures[15].fig [1][1] = 'P';
    liste_figures[15].fig [2][1] = 'P';
    liste_figures[15].fig [2][2] = 'P';
    liste_figures[15].fig [2][3] = 'P';
    liste_figures[15].fig [3][3] = 'P';

    liste_figures[16].fig [1][2] = 'Q';
    liste_figures[16].fig [2][2] = 'Q';
    liste_figures[16].fig [2][3] = 'Q';
    liste_figures[16].fig [3][2] = 'Q';
    liste_figures[16].fig [4][2] = 'Q';

    liste_figures[17].fig [1][2] = 'R';
    liste_figures[17].fig [2][2] = 'R';
    liste_figures[17].fig [3][2] = 'R';
    liste_figures[17].fig [3][1] = 'R';
    liste_figures[17].fig [3][3] = 'R';

    liste_figures[18].fig [1][1] = 'S';
    liste_figures[18].fig [2][1] = 'S';
    liste_figures[18].fig [3][1] = 'S';
    liste_figures[18].fig [3][2] = 'S';
    liste_figures[18].fig [3][3] = 'S';

    liste_figures[19].fig [1][1] = 'T';
    liste_figures[19].fig [1][2] = 'T';
    liste_figures[19].fig [2][2] = 'T';
    liste_figures[19].fig [2][3] = 'T';
    liste_figures[19].fig [3][3] = 'T';

    liste_figures[20].fig [1][2] = 'U';
    liste_figures[20].fig [2][2] = 'U';
    liste_figures[20].fig [2][3] = 'U';
    liste_figures[20].fig [2][1] = 'U';
    liste_figures[20].fig [3][2] = 'U';

}


/***************************************************************
 Procedure: initialise_joueurs
 Arguments: La liste des 4 joueurs
 Retour:
 Auteur: Louis CLAVERO, Benjamin FORNACIARI, Ivan MAULINE
 Commentaire: Permet d'initialiser les 4 joueurs en rentrant leur
              numéro, leurs couleurs d'affichage, leur nombre de
              pieces jouees et le fait qu'ils n'aient pas encore
              perdu
 ****************************************************************/
void initialise_joueurs(t_joueur liste_joueurs[4]){
    //on remplit chacun des joueurs manuellement avec ses données
    liste_joueurs[0].num = 1;
    liste_joueurs[0].couleur_texte = 15;
    liste_joueurs[0].couleur_fond = 12;
    liste_joueurs[0].perdu = 0;
    liste_joueurs[0].pieces_jouees = 0;
    liste_joueurs[1].num = 2;
    liste_joueurs[1].couleur_texte = 15;
    liste_joueurs[1].couleur_fond = 3;
    liste_joueurs[1].perdu = 0;
    liste_joueurs[1].pieces_jouees = 0;
    liste_joueurs[2].num = 3;
    liste_joueurs[2].couleur_texte = 15;
    liste_joueurs[2].couleur_fond = 2;
    liste_joueurs[2].perdu = 0;
    liste_joueurs[2].pieces_jouees = 0;
    liste_joueurs[3].num = 4;
    liste_joueurs[3].couleur_texte = 0;
    liste_joueurs[3].couleur_fond = 12 | 2;
    liste_joueurs[3].perdu = 0;
    liste_joueurs[3].pieces_jouees = 0;
}

/***************************************************************
 Procedure: verif
 Arguments: le tableau de jeu, la figure, la ligne et la colonne
            ou placer la figure, la taille de la figure, le joueur
            voulant placer la figure et le tour actuel
 Retour: 1 s'il est possible de placer la figure, 0 sinon
 Auteur: Louis CLAVERO, Benjamin FORNACIARI, Ivan MAULINE
 Commentaire: permet de verifier que la piece peut etre jouee a
              l'emplacement (lig, col)
 ****************************************************************/
int verif(int game[20][20], char** fig, int lig, int col, int n, t_joueur* joueur_actuel, int tour)
{
    //Declaration des variables
    int i, j, posx, posy;
    int a = 0;
    int b = 1;
    int c = 0;
    //Tableau pour stocker les positions des carrés de chaque figure
    int tab[5][2] = { {0,0},
                      {0,0},
                      {0,0},
                      {0,0},
                      {0,0}
                      };
    //Tant qu'on a pas trouvé tous les carrés, on cherche les positions
    //dans la figure passée en argument
    for (i=0;i<5&&a!=n;i++)
    {
        for(j=0;j<5&&a!=n;j++)
        {
            if (fig[i][j] != '.')
            {
                    tab[a][0] = i;
                    tab[a][1] = j;
                    a=a+1;
            }
        }
    }
    //On vérifie pour chaque coordonnées que son positionnement est autorise
    for (i=0;i<n;i++)
    {
        //positions relatives des carrés dans la matrice de jeu
        posx = lig+tab[i][0]-tab[0][0];
        posy = col+tab[i][1]-tab[0][1];
        //si ce n'est pas sur le jeu, b=0
        if(posx<0 || posx>19 || posy<0 || posy>19)
        {
                b=0;
        }
        else{
            //Si une piece est deja pose a cet endroit, b=0
            if(game[posx][posy] != 0)
            {
                b=0;
            }
            //Si une piece du meme joueur a un coté en commun, b=0
            else {
                if(posx+1>-1 && posx+1<20){
                    if(game[posx+1][posy] == joueur_actuel->num) b=0;
                }
                if(posx-1>-1 && posx-1<20){
                    if(game[posx-1][posy] == joueur_actuel->num) b=0;
                }
                if(posy+1>-1 && posy+1<20){
                    if(game[posx][posy+1] == joueur_actuel->num) b=0;
                }
                if(posy-1>-1 && posy-1<20){
                    if(game[posx][posy-1] == joueur_actuel->num) b=0;
                }
            }
        }
        //Si le carré a dans sa diagonale une piece du meme joueur, c=1
        if(posx+1<20 && posx+1>-1 && posy+1<20 && posy+1>-1){
            if(game[posx+1][posy+1]== joueur_actuel->num) c=1;
        }
        if(posx-1<20 && posx-1>-1 && posy+1<20 && posy+1>-1){
            if(game[posx-1][posy+1] == joueur_actuel->num) c=1;
        }
        if(posx+1<20 && posx+1>-1 && posy-1<20 && posy-1>-1){
            if(game[posx+1][posy-1] == joueur_actuel->num) c=1;
        }
        if(posx-1<20 && posx-1>-1 && posy-1<20 && posy-1>-1){
            if(game[posx-1][posy-1] == joueur_actuel->num) c=1;
        }
        //Traitement des cas au début du jeu, lorsqu'aucune piece n'est posee
        //Il faut que le joueur joue dans son angle
        if (joueur_actuel->num == 1 && tour==0)
        {
            if (posx-1==-1 && posy-1==-1)
            {
                c=1;
            }
        }
        if (joueur_actuel->num == 2 && tour==0)
        {
            if (posx-1==-1 && posy+1==20)
            {
                c=1;
            }
        }
        if (joueur_actuel->num == 3 && tour==0)
        {
            if (posx+1==20 && posy-1==-1)
            {
                c=1;
            }
        }
        if (joueur_actuel->num == 4 && tour==0)
        {
            if (posx+1==20 && posy+1==20)
            {
                c=1;
            }
        }
    }
    //Si b vaut 1 et c vaut 1, on retourne 1, sinon on retourne 0
    return (b==1 && c==1)? 1:0;
}

/***************************************************************
 Procedure: placer
 Arguments: le tableau de jeu, la figure, la ligne et la colonne
            ou placer la figure, la taille de la figure, le joueur
            voulant placer la figure
 Retour:
 Auteur: Louis CLAVERO, Benjamin FORNACIARI, Ivan MAULINE
 Commentaire: permet de placer la piece a l'endroit donné du tableau
              (de l'afficher sur le jeu et de mettre à jour la matrice
               de jeu)
 ****************************************************************/
void placer(int game[20][20], char** fig, int lig, int col, int n, t_joueur* joueur_actuel){
    //Declaration des variables
    int tab[5][2] = {{0,0},
                     {0,0},
                     {0,0},
                     {0,0},
                     {0,0}
                    };
    int i, j, posx, posy;
    int a = 0;
    //De même que verif, on stocke toutes les positions des carrés de la figure
    for (i=0;i<5&&a!=n;i++)
    {
        for(j=0;j<5&&a!=n;j++)
        {
            if (fig[i][j] != '.')
            {
                    tab[a][0] = i;
                    tab[a][1] = j;
                    a++;
            }
        }
    }
    //pour chacun de ces carrés, on le place dans la matrice de jeu
    //et on affiche avec sa couleur au bon endroit à l'écran
    for(i=0; i<n; i++){
        //positions relatives dans la matrice de jeu
        posx = lig+tab[i][0]-tab[0][0];
        posy = col+tab[i][1]-tab[0][1];
        //mise à jour de la matrice de jeu
        game[posx][posy] = joueur_actuel->num;
        //Affichage de la pièce à l'ecran
        gotoligcol(posx+2, 2*posy+3);
        color(joueur_actuel->couleur_texte, joueur_actuel->couleur_fond);
        printf("%c", fig[tab[i][0]][tab[i][1]]);
        color(15, 0);
    }
}

/***************************************************************
 Fonction: main
 Arguments:
 Retour:
 Auteur: Louis CLAVERO, Benjamin FORNACIARI, Ivan MAULINE
 Commentaire: Fonction main qui gère tout le jeu
 ****************************************************************/
int main()
{
    //Declaration des variables
    int i,j,k, pos_lettre, pos_chiffre;
    int joueur_actuel = 0;
    int tour = 0;
    char key;
    char jouable;
    int game[20][20];
    char lettre_fig, lettre_rot;
    int test = 0;
    int nombre_joueurs;
    int nombre_humains;
    int nombre_perdants;
    int joueur_non_perdant;
    t_figure liste_figures[21];
    t_joueur liste_joueurs[4];
    char get_letter[5] = {0};
    //Matrice des pieces disponibles initiales
    char available[12][19] =
    {
        {'A','.','B','B','.','C','C','C','.','D','D','.','E','E','E','E','.','.','F'},
        {'.','.','.','.','.','.','.','.','.','D','.','.','.','.','.','.','.','.','F'},
        {'G','.','.','H','.','.','I','I','.','.','.','.','.','.','K','.','.','.','F'},
        {'G','.','.','H','H','.','I','I','.','.','J','J','.','.','K','K','K','.','F'},
        {'G','G','.','H','.','.','.','.','.','.','.','J','J','.','.','K','.','.','F'},
        {'.','.','.','.','.','.','M','.','.','N','.','.','.','.','.','.','.','.','.'},
        {'L','L','L','L','.','.','M','.','N','N','.','.','.','O','O','.','P','.','.'},
        {'.','.','.','L','.','M','M','.','N','N','.','.','.','.','O','.','P','P','P'},
        {'Q','.','.','.','.','M','.','.','.','.','.','.','.','O','O','.','.','.','P'},
        {'Q','Q','.','R','.','.','.','S','.','.','T','T','.','.','.','.','U','.','.'},
        {'Q','.','.','R','.','.','.','S','.','.','.','T','T','.','.','U','U','U','.'},
        {'Q','.','R','R','R','.','.','S','S','S','.','.','T','.','.','.','U','.','.'}
    };
    int choisie_taille = 0;

    char** verification = calloc(5, sizeof(char*));
    char** choisie = calloc(5, sizeof(char*));
    for(i=0; i<5; i++)
    {
        choisie[i] = calloc(5, sizeof(char));
        verification[i] = calloc(5, sizeof(char));
    }
    //initialisation de la matrice de jeu
    for(i=0; i<20; i++){
        for(j=0; j<20; j++){
            game[i][j] = 0;
        }
    }
    //Initialisation de générateur de nombres aléatoires utilisé pour l'IA
    srand(time(NULL));
    initialise_jeu(liste_figures);
    initialise_joueurs(liste_joueurs);
    //Affichage du premier menu et obtention du nombre de joueurs total
    print_menu();
    nombre_joueurs = get_nombre_joueurs();
    system("cls");
    //Affichage du deuxième menu et obtention du nombre de joueurs humains
    print_menu2(nombre_joueurs);
    nombre_humains = get_nombre_humains(nombre_joueurs);
    system("cls");
    //Affichage des regles
    print_regles();
    while(key!= ' ')
    {
        key = getch();
    }
    system("cls");
    //Affichage des indexes du tableau de jeu (A-T, 1-20)
    print_indexes();
    //Affiche les bords du jeu
    print_borders(1, 2, 22, 43);
    //Affiche les bords du guide
    print_borders(16, 44, 22, 56);
    //Affiche les bords de la zone de pièces disponibles
    print_borders(1, 44, 14, 84);
    //affichage de '.' pour simuler un quadrillage du tableau de jeu
    fill_game();

    gotoligcol(15, 45);
    color(10, 0);
    printf("||BLOKUS||");
    color(15, 0);
    gotoligcol(18, 58);
    printf("Choisissez la piece a poser: ");
    gotoligcol(19, 58);
    printf("Rotation/Inversion (G-D-I / V pour valider) ");
    gotoligcol(20, 58);
    printf("Sur quelle colonne poser la piece (A-T): ");
    gotoligcol(21, 58);
    printf("Sur quelle ligne poser la piece (1-20): ");
    while(1)
    {
        //On va vérifier que chaque joueur est en mesure de jouer, par rapport à ses pieces disponibles
        jouable = 0;
        for(i=0; (i<21) && (jouable == 0);i++){
            if(liste_figures[i].jouee[joueur_actuel] == 0){
                for(j=0; j<SIZE_FIGURE; j++){
                    for(k=0;j<SIZE_FIGURE; j++){
                        verification[j][k]=liste_figures[i].fig[j][k];
                    }
                }
                //Verification de chaque piece et toutes ses rotations possibles
                for(j=0; (j<160) && (jouable == 0) ; j++){
                    if(j==20 || j==40 || j==60 || j==100 || j==120 || j==140){
                        rotate_droite(verification);
                    }
                    else if(j==80) {
                        rotate_miroir(verification);
                    }
                    for(k=0; (k<20) && (jouable==0); k++){
                        if((verif(game, verification, j%20, k, liste_figures[i].size, &liste_joueurs[joueur_actuel], tour) == 1)) {
                            //on affiche l'indice de la pièce jouable
                            gotoligcol(15, 58);
                            printf("Indice: piece %c jouable en (%c, %d)", liste_figures[i].lettre, 'A'+k, (j%20)+1 );
                            jouable = 1;
                        }
                    }
                }
            }
        }
        //Si le joueur ne peut pas jouer, on vérifie si les autres joueurs ont déja perdu
        if(jouable==0){
            for(i=0;i<nombre_joueurs;i++){
                if(liste_joueurs[i].perdu == 1 && i!=joueur_actuel){
                    nombre_perdants++;
                }
                else if(i!=joueur_actuel){
                    joueur_non_perdant = i;
                }
            }
            //Si c'est l'avant dernier des joueurs à perdre, le dernier a gagné et on sort de la boucle
            if(nombre_perdants==nombre_joueurs-2 && nombre_perdants>0){
                gotoligcol(23+joueur_actuel, 0);
                printf("Le joueur %d a perdu.", joueur_actuel+1);
                gotoligcol(27,0);
                color(liste_joueurs[joueur_non_perdant].couleur_fond, 0);
                printf("Le joueur %d a gagne!!!", joueur_non_perdant+1);
                color(15, 0);
                break;
            }
            //Sinon, on affiche seulement qu'il a perdu
            else{
                liste_joueurs[joueur_actuel].perdu = 1;
                gotoligcol(23+joueur_actuel, 0);
                printf("Le joueur %d a perdu.", joueur_actuel+1);
                joueur_actuel = (joueur_actuel+1)%nombre_joueurs;
            }
            nombre_perdants=0;
        }
        //Sinon, si une piece est jouable
        else{
            gotoligcol(17, 58);
            printf("Joueur %d, a vous de jouer!", joueur_actuel+1);
            print_available(available, &liste_joueurs[joueur_actuel], liste_figures);
            //S'il s'agit d'une IA, on choisit une lettre aléatoirement
            if(joueur_actuel>=nombre_humains){
                //Tant qu'on n'a pas trouvé une possibilité viable pour placer une piece
                do{
                    //On prend une piece encore disponible aleatoirement et on essaie de la placer
                    do{
                        lettre_fig = 'A'+rand()%21;
                    }while(liste_figures[(int)(lettre_fig - 'A')].jouee[joueur_actuel] == 1);
                    choisie_taille = liste_figures[(int)(lettre_fig -'A')].size;
                    for(i=0; i<SIZE_FIGURE; i++)
                    {
                        for(j=0; j<SIZE_FIGURE; j++)
                        {
                            choisie[i][j] = liste_figures[lettre_fig - 'A'].fig[i][j];
                        }
                    }
                    //On affiche dans la zone de guide la piece
                    fill_guide(choisie, liste_joueurs[joueur_actuel].couleur_texte, liste_joueurs[joueur_actuel].couleur_fond);
                    Sleep(1000);
                    test = 0;
                    //Puis on va essayer de la placer dès que possible à une position, en la tournant si necessaire
                    for(i=0; (i<160) && (test == 0); i++){
                        if(i==20 || i==40 || i==60 || i==100 || i==120 || i==140){
                            rotate_droite(choisie);
                            fill_guide(choisie, liste_joueurs[joueur_actuel].couleur_texte, liste_joueurs[joueur_actuel].couleur_fond);
                            Sleep(400);
                        }
                        else if(i==80) {
                            rotate_miroir(choisie);
                            fill_guide(choisie, liste_joueurs[joueur_actuel].couleur_texte, liste_joueurs[joueur_actuel].couleur_fond);
                            Sleep(400);
                        }
                        //test=1 si on a trouvé comment placer la piece
                        for(j=0; (j<20) && (test == 0); j++){
                            if(verif(game, choisie, i%20, j, choisie_taille, &liste_joueurs[joueur_actuel], tour) == 1){
                                pos_chiffre = i%20;
                                pos_lettre = j;
                                test=1;
                            }
                        }
                    }
                }while(test==0);

            }
            //Sinon, on demande à l'utilisateur de rentrer ses choix
            else{
                //On lui demande d'abord la piece qu'il souhaite placer
                do
                {
                    get_letter[0]=0;
                    get_letter[1]=0;
                    gotoligcol(18, 87);
                    printf("                  ");
                    gotoligcol(18, 87);
                    fgets(get_letter, 4, stdin);
                    fflush(stdin);
                    get_letter[strlen(get_letter)-1]='\0';
                    lettre_fig = get_letter[0];
                }
                while(lettre_fig < 'A' || lettre_fig > 'U' || liste_figures[(int)(lettre_fig - 'A')].jouee[joueur_actuel] == 1 || get_letter[1]!=0
                    || lettre_fig == 0);
                choisie_taille = liste_figures[lettre_fig -'A'].size;
                //On copie dans choisie la figure que l'utilisateur a choisi pour ensuite pouvoir la tourner si demandé
                for(i=0; i<SIZE_FIGURE; i++)
                {
                    for(j=0; j<SIZE_FIGURE; j++)
                    {
                        choisie[i][j] = liste_figures[lettre_fig - 'A'].fig[i][j];
                    }
                }
                //on l'affiche dans la zone de guide
                fill_guide(choisie, liste_joueurs[joueur_actuel].couleur_texte, liste_joueurs[joueur_actuel].couleur_fond);

                //on propose au joueur de tourner/inverser sa pièce tant qu'il n'a pas validé avec la touche 'V'
                while(lettre_rot!='V'){
                    gotoligcol(19, 102);
                    if(kbhit()){
                        lettre_rot = getch();
                        fflush(stdin);
                        switch(lettre_rot)
                        {
                        case 'G':
                            rotate_gauche(choisie);
                            fill_guide(choisie, liste_joueurs[joueur_actuel].couleur_texte, liste_joueurs[joueur_actuel].couleur_fond);
                            break;
                        case 'D':
                            rotate_droite(choisie);
                            fill_guide(choisie, liste_joueurs[joueur_actuel].couleur_texte, liste_joueurs[joueur_actuel].couleur_fond);
                            break;
                        case 'I':
                            rotate_miroir(choisie);
                            fill_guide(choisie, liste_joueurs[joueur_actuel].couleur_texte, liste_joueurs[joueur_actuel].couleur_fond);
                            break;
                        }
                    }
                }
                //On demande la position en colonne
                lettre_rot = 0;
                do
                {
                    get_letter[0]=0;
                    get_letter[1]=0;
                    gotoligcol(20, 99);
                    printf("               ");
                    gotoligcol(20, 99);
                    fgets(get_letter, 4, stdin);
                    fflush(stdin);
                    get_letter[strlen(get_letter)-1]='\0';
                }while(get_letter[0]<'A' || get_letter[0] > 'T' || get_letter[1]!=0 || get_letter[0] == 0);
                pos_lettre = (int) get_letter[0] - 'A';
                //On demande la position en ligne
                do{
                    get_letter[0]=0;
                    get_letter[1]=0;
                    get_letter[2]=0;
                    gotoligcol(21, 98);
                    printf("               ");
                    gotoligcol(21, 98);
                    fgets(get_letter, 5, stdin);
                    fflush(stdin);
                    get_letter[strlen(get_letter)-1]='\0';
                }while(get_letter[2] != 0 || (get_letter[0]=='2' && get_letter[1]!=0 && get_letter[1]!='0') ||
                       (get_letter[0]=='1' && get_letter[1]!=0 && (get_letter[1]<'0' || get_letter[1]>'9')) ||
                       (get_letter[0]<'1' || get_letter[0]>'9') || get_letter[0]==0);
                pos_chiffre = atoi(get_letter)-1;
            }
            //Ici, soit l'IA va pouvoir poser sa pièce, soit on va verifier que le joueur humain peut.
            if(verif(game, choisie, pos_chiffre, pos_lettre, choisie_taille, &liste_joueurs[joueur_actuel], tour) == 1){
                //Si c'est possible, on place la piece
                placer(game, choisie, pos_chiffre, pos_lettre, choisie_taille, &liste_joueurs[joueur_actuel]);
                liste_figures[(int)(lettre_fig - 'A')].jouee[joueur_actuel] = 1;
                liste_joueurs[joueur_actuel].pieces_jouees++;
                //Si un joueur a posé toutes ses pièces, il gagne et on sort de la boucle
                if(liste_joueurs[joueur_actuel].pieces_jouees==21){
                    gotoligcol(27, 0);
                    color(liste_joueurs[joueur_actuel].couleur_fond, 0);
                    printf("Le joueur %d a gagne!!!", joueur_actuel+1);
                    color(15, 0);
                    break;
                }
                //On passe donc au joueur suivant
                do{
                    joueur_actuel=(joueur_actuel+1)%nombre_joueurs;
                    if(joueur_actuel==0) {
                        tour++;
                    }
                }while(liste_joueurs[joueur_actuel].pieces_jouees==21 || liste_joueurs[joueur_actuel].perdu == 1);
            }
            //Si le joueur humain a choisi une position illégale pour la piece, on lui indique, et on lui permet de rechoisir une piece et ses positions
            //puisque l'on ne change pas de joueur (on répète la boucle pour le meme joueur)
            else{
                gotoligcol(22, 58);
                color(12, 0);
                printf("Impossible de placer la piece a cet endroit.");
                Sleep(1500);
                gotoligcol(22, 58);
                printf("                                            ");
                color(15, 0);
            }
            //On vide la zone de guide
            clear_guide();
            gotoligcol(15, 58);
            printf("                                     ");
        }
    }
    //On libère la mémoire dynamiquement allouée plus haut
    free(choisie);
    free(verification);
    printf("\n\n\n");
    return 0;
}

