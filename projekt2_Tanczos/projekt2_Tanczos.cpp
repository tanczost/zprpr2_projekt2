// projekt2_Tanczos.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUBOR "filmy.txt"
#define MAXZNAK 100

typedef struct meno
{
	char krstne[MAXZNAK];
	char priezvisko[MAXZNAK];
}MENO;

typedef struct herec
{
	MENO meno;
	int rokNarodenia;
	struct herec* kolega;
}HEREC;

typedef struct film
{
	char nazov[MAXZNAK];
	int rokVyroby;
	MENO reziser; //meno rezisera
	HEREC* herci;
	film* dalsiFilm;
}FILM;

/*funkcie*/
FILM* nacitaj(void);
void vypis(FILM* kino);
FILM* uvolni(FILM* kino);
void pridaj(FILM** kino);
FILM* vymaz(FILM* kino);

int main()
{
	FILM* kino = NULL;
	while (1)
	{
		char riadok[MAXZNAK];
		char prikaz[MAXZNAK];
		fgets(riadok, MAXZNAK - 1, stdin);
		sscanf(riadok, "%s", prikaz);
		if (!strcmp(prikaz, "koniec")) //uvolni sa pamat a konci program
		{
			uvolni(kino);
			break;
		}
		else if (!strcmp(prikaz, "vypis")) vypis(kino); //vypis zoznam filmov
		else if (!strcmp(prikaz, "nacitaj")) //pamat sa uvolni a nacita sa znova zoznam filmov
		{
			kino = uvolni(kino);
			kino = nacitaj();
		}
		else if(!strcmp(prikaz, "pridaj")) pridaj(&kino);
		else if(!strcmp(prikaz, "vymaz")) kino = vymaz(kino);

	}
	return 0;
}

FILM* nacitaj(void)
{
	FILE* fr;
	if ((fr = fopen(SUBOR, "r")) == NULL)
	{
		printf("Subor sa nenasiel\n"); exit(-1);
	}

	int c;

	FILM *zac = (FILM*)malloc(sizeof(FILM)); //vytvorenie zaciatku zoznamu
	if (zac == NULL) { printf("Malo pamati\n"); exit(-1);}
	FILM* kino = zac;

	while ((c = getc(fr)) != EOF)
	{
		ungetc(c, fr);

		if (kino->dalsiFilm == NULL)
		{
			kino->dalsiFilm = (FILM*)malloc(sizeof(FILM)); //pridanie do zoznamu , !!urobit este kontrolu na null!!
			if(kino ->dalsiFilm == NULL) { printf("Malo pamati\n"); exit(-1); }
			kino = kino->dalsiFilm;
		}
		fgets(kino->nazov, MAXZNAK, fr);
		kino->nazov[strlen(kino->nazov) - 1] = '\0';
		fscanf(fr, "%d", &kino->rokVyroby);
		fscanf(fr, "%s %s\n", kino->reziser.krstne, kino->reziser.priezvisko);

		char hviezda; HEREC* ucinkujuci = kino->herci;

		if ((hviezda = getc(fr)) == '*') //ked su herci tak sa vytvori prvy prvok pre herca
		{
			kino->herci = (HEREC*)malloc(sizeof(HEREC));
			if(kino->herci == NULL){ printf("Malo pamati\n"); exit(-1); }
			ucinkujuci = kino->herci;
		}
		else kino->herci = NULL; //nie su herci
		ungetc(hviezda, fr);

		while ((hviezda = getc(fr)) == '*')
		{
			if (ucinkujuci->kolega == NULL)
			{
				ucinkujuci->kolega = (HEREC*)malloc(sizeof(HEREC)); //!!kontrola na null!!
				if(ucinkujuci->kolega == NULL) { printf("Malo pamati\n"); exit(-1); }
				ucinkujuci = ucinkujuci->kolega;
			}
			fscanf(fr, "%s %s %d\n", ucinkujuci->meno.krstne, ucinkujuci->meno.priezvisko, &ucinkujuci->rokNarodenia);

			ucinkujuci->kolega = NULL;
		}
		ungetc(hviezda, fr);

		kino->dalsiFilm = NULL;
	}

	//if (fclose(fr) == EOF)	printf("Subor sa nepodarilo zatvorit\n"); exit(-1);
	return zac;
}

void vypis(FILM* kino)
{
	while (kino != NULL)
	{
		printf("%s (%d) %s %s\n\tHraju:", kino->nazov, kino->rokVyroby, kino->reziser.krstne, kino->reziser.priezvisko);
		HEREC* ucinkujuci = kino->herci;
		while (ucinkujuci != NULL)
		{
			printf(" %s %s (%d),", ucinkujuci->meno.krstne, ucinkujuci->meno.priezvisko, ucinkujuci->rokNarodenia);
			ucinkujuci = ucinkujuci->kolega;
		}
		printf("\b \n");
		kino = kino->dalsiFilm;
	}
}

FILM* uvolni(FILM* kino)
{
	/*premenne*/
	FILM* pom1;
	HEREC *pom2;
	/*********/

	pom1 = kino;

	while (pom1 != NULL)
	{
		pom2 = pom1->herci;
		while (pom2 != NULL)
		{
			HEREC* pom = pom2;
			pom2 = pom2->kolega;
			free(pom); //uvolni sa herec
		}
		FILM* pom;
		pom = pom1;
		pom1 = pom1->dalsiFilm;
		free(pom); //uvolni sa film
	}
	return NULL;
}

void pridaj(FILM** kino)
{
	/*premenne*/
	char hviezda;
	HEREC* pom1 = NULL;
	FILM* pom = (FILM*)malloc(sizeof(FILM));
	/*********/
	if (*kino == NULL) //vstupujeme ked zoznam je prazdny
	{
		*kino = pom;
	}
	else //vstupujeme ked uz v zozname su filmy
	{
		while ((*kino)->dalsiFilm != NULL) //najdeme posledny
			{
				(*kino) = (*kino)->dalsiFilm;
			}
	}

	fgets(pom->nazov, MAXZNAK, stdin); //nacitanie novych nazvov, cisiel, ...
	pom->nazov[strlen(pom->nazov) - 1] = '\0';
	scanf("%d", &pom->rokVyroby);
	scanf("%s %s", pom->reziser.krstne, pom->reziser.priezvisko);
	pom->dalsiFilm = NULL;
	while (getchar() != '\n');

	if ((hviezda = getchar()) != '*') pom1 = pom->herci = (HEREC*)malloc(sizeof(HEREC)); //zadavaju sa herci
	else pom->herci = NULL;  //nezadavaju sa herci
	ungetc(hviezda, stdin);

	while((hviezda = getchar()) != '*')
	{
		ungetc(hviezda, stdin);
		if (pom1->kolega == NULL)
		{
			pom1->kolega = (HEREC*)malloc(sizeof(HEREC));
			pom1 = pom1->kolega;
		}
		scanf("%s %s %d", pom1->meno.krstne, pom1->meno.priezvisko, &pom1->rokNarodenia);
		pom1->kolega = NULL;
		while (getchar() != '\n');

	}

	while (getchar() != '\n');

	if (*kino == pom) (*kino)->dalsiFilm = NULL; //je to prvy film v zozname
	else (*kino)->dalsiFilm = pom; //pridali sme dalsi film do zoznamu
}

FILM* vymaz(FILM* kino)
{
	/*premenne*/
	char nazov[MAXZNAK];
	FILM* pred = NULL;
	FILM* zac = kino;
	HEREC* pom1;
	int counter = 1;
	/**********/

	fgets(nazov, MAXZNAK, stdin);
	nazov[strlen(nazov) - 1] = '\0';
	while (kino != NULL && strcmp(nazov, kino->nazov)) //hladanie filmu
	{
		pred = kino;
		kino = kino->dalsiFilm;
		counter++;
	}

	if (kino == NULL) return zac; //film sa nenasiel, alebo nemame zoznam

	if (counter == 1) zac = kino->dalsiFilm;//vymazava sa prvy film, vratime iny ukazovatel
	else pred->dalsiFilm = kino->dalsiFilm;

	pom1 = kino->herci;
	while (pom1 != NULL)
	{
		HEREC* pomHerec = pom1;
		pom1 = pom1->kolega;
		free(pomHerec);
	}
	free(kino);
	return zac;
}