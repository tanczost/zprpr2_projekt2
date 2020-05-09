// projekt2_Tanczos.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
/*
* autor - Tomas Tanczos
* datum - 09.05.2020
* program - praca so spajanym zoznamom
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUBOR "filmy.txt"
#define MAXZNAK 100
#define PAMATERR { printf("Malo pamati\n"); exit(-1); } //makro pre chybove hlasenie

typedef struct meno
{
	char krstne[MAXZNAK];
	char priezvisko[MAXZNAK];
}MENO;

typedef struct herec
{
	MENO meno;
	int rokNarodenia;
	struct herec* herci;
}HEREC;

typedef struct film
{
	char nazov[MAXZNAK];
	int rokVyroby;
	MENO reziser; //meno rezisera
	struct herec* herci;
	struct film* dalsiFilm;
}FILM;

/*funkcie*/
FILM* nacitaj(void);
FILM* nacitajFilm(FILE* fr);
HEREC* nacitajHerca(FILE* fr);
void vypis(FILM* kino);
FILM* uvolni(FILM* kino);
void pridaj(FILM** kino);
FILM* vymaz(FILM* kino);
void filmy(FILM* kino);
void herci(FILM* kino);
void rok(FILM* kino);
HEREC* zoznamRok(HEREC* zac, HEREC zdroj);
int porovnaj(const HEREC a, const HEREC b);


int main()
{
	FILM* kino = NULL;
	char prikaz[MAXZNAK];
	do {
		scanf("%s", prikaz);
		if (!strcmp(prikaz, "vypis")) vypis(kino); //vypis zoznam filmov
		else if (!strcmp(prikaz, "nacitaj")) //pamat sa uvolni a nacita sa znova zoznam filmov
		{
			kino = uvolni(kino);
			kino = nacitaj();
		}
		else if (!strcmp(prikaz, "pridaj")) pridaj(&kino);
		else if (!strcmp(prikaz, "vymaz")) kino = vymaz(kino);
		else if (!strcmp(prikaz, "filmy")) filmy(kino);
		else if (!strcmp(prikaz, "herci")) herci(kino);
		else if (!strcmp(prikaz, "rok")) rok(kino);
	} while (strcmp(prikaz, "koniec"));


	uvolni(kino); //uvolni sa pamat

	return 0;
}

FILM* nacitajFilm(FILE* fr) //fr moze byt bud subor alebo stdin
{
	FILM* novy = (FILM*)malloc(sizeof(FILM));//alokacia pamati pre novy prvok
	if (novy == NULL) PAMATERR
		/*nacitanie dat*/
		fgets(novy->nazov, MAXZNAK, fr);
	novy->nazov[strlen(novy->nazov) - 1] = '\0';
	fscanf(fr, "%d", &novy->rokVyroby);
	fscanf(fr, "%s %s", novy->reziser.krstne, novy->reziser.priezvisko);

	if (fr != stdin) fscanf(fr, "\n");
	else while (getchar() != '\n');
	novy->herci = NULL;
	novy->dalsiFilm = NULL;

	return novy;
}

HEREC* nacitajHerca(FILE* fr)
{
	HEREC* novy = (HEREC*)malloc(sizeof(HEREC)); //alokacia pamati pre novy prvok
	if (novy == NULL) PAMATERR
		fscanf(fr, "%s %s %d", novy->meno.krstne, novy->meno.priezvisko, &novy->rokNarodenia); //nacitanie dat
	if (fr != stdin) fscanf(fr, "\n");
	else while (getchar() != '\n');
	novy->herci = NULL;
	return novy;
}

FILM* nacitaj(void)
{
	/*premenne*/
	FILE* fr;
	char hviezda;
	int c;
	/*********/

	if ((fr = fopen(SUBOR, "r")) == NULL) { printf("Subor sa nenasiel\n"); exit(-1); } //kontrola otvorenia

	if ((c = getc(fr)) == EOF) { printf("Prazdny subor\n"); return NULL; } //subor je prazdny, vratime NULL
	else ungetc(c, fr); //musime vratit znak

	FILM* zac = nacitajFilm(fr); //vytvorenie prveho zaznamu
	FILM* kino = zac;


	if ((hviezda = getc(fr)) == '*') kino->herci = nacitajHerca(fr); //prvy herec v zozname
	else ungetc(hviezda, fr);
	HEREC* temp = kino->herci;

	while ((hviezda = getc(fr)) == '*') //kym sa tam nachadza hviezda pridavame novych
	{
		temp->herci = nacitajHerca(fr);
		temp = temp->herci;
	}
	ungetc(hviezda, fr);

	while (!feof(fr))
	{
		kino->dalsiFilm = nacitajFilm(fr);
		kino = kino->dalsiFilm;
		if ((hviezda = getc(fr)) == '*') kino->herci = nacitajHerca(fr); //prvy herec
		else ungetc(hviezda, fr);
		HEREC* temp = kino->herci;

		while ((hviezda = getc(fr)) == '*') //nacitanie dalsich kolegov
		{
			temp->herci = nacitajHerca(fr);
			temp = temp->herci;
		}
		ungetc(hviezda, fr);
	}

	if (fclose(fr) == EOF) { printf("Subor sa nepodarilo zatvorit\n"); exit(-1); } //kontrola zatvorenia

	return zac; //vratime zac
}

void vypis(FILM* kino)
{
	while (kino != NULL)
	{
		printf("%s (%d) %s %s\n\t", kino->nazov, kino->rokVyroby, kino->reziser.krstne, kino->reziser.priezvisko);
		HEREC* ucinkujuci = kino->herci;
		if (ucinkujuci == NULL) printf("Ziadny slavny herec ");
		else printf("Hraju:");
		while (ucinkujuci != NULL)
		{
			printf(" %s %s (%d)", ucinkujuci->meno.krstne, ucinkujuci->meno.priezvisko, ucinkujuci->rokNarodenia);
			ucinkujuci = ucinkujuci->herci;
			if (ucinkujuci != NULL) printf(", ");
		}

		putchar('\n');
		kino = kino->dalsiFilm;
	}
}

FILM* uvolni(FILM* kino)
{
	/*premenne*/
	/*********/


	FILM* pom;
	HEREC* pomH;
	while (kino != NULL)
	{

		while (kino->herci != NULL)
		{
			pomH = kino->herci;
			kino->herci = kino->herci->herci;
			free(pomH); //uvolni sa herec
		}

		pom = kino;
		kino = kino->dalsiFilm;
		free(pom); //uvolni sa film
	}
	return NULL;
}

void pridaj(FILM** kino)
{
	/*premenne*/
	char hviezda;
	HEREC* pom = NULL;
	FILM* temp = (*kino);
	while (getchar() != '\n');
	/*********/
	if (*kino == NULL) //vstupujeme ked zoznam je prazdny
	{
		(*kino) = nacitajFilm(stdin); //nacitame prvy film
		temp = (*kino);
	}
	else //vstupujeme ked uz v zozname su filmy
	{
		while (temp->dalsiFilm != NULL) //najdeme posledny
			temp = temp->dalsiFilm;
		temp = temp->dalsiFilm = nacitajFilm(stdin); //nacitame posledny film a nastavime nanho ukazovatel
	}

	if ((hviezda = getchar()) != '*')
	{
		ungetc(hviezda, stdin);
		temp->herci = nacitajHerca(stdin); //zadavaju sa herci
	}
	else
	{
		ungetc(hviezda, stdin);
		temp->herci = NULL;  //nezadavaju sa herci
	}

	pom = temp->herci;
	while ((hviezda = getchar()) != '*') //zadavanie dalsich hercov
	{
		ungetc(hviezda, stdin);
		pom->herci = nacitajHerca(stdin);
		pom = pom->herci;
	}
}

FILM* vymaz(FILM* kino)
{
	/*premenne*/
	char nazov[MAXZNAK];
	FILM* pred = NULL, * zac = kino;
	HEREC* pom;
	int counter = 1;
	while (getchar() != '\n');
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
	else pred->dalsiFilm = kino->dalsiFilm; //preskocime prvok kino aby sme ho vedeli uvolnit

	while (kino->herci != NULL)
	{
		pom = kino->herci;
		kino->herci = kino->herci->herci;
		free(pom);
	}
	free(kino);
	return zac;
}

void filmy(FILM* kino)
{
	char meno1[MAXZNAK], meno2[MAXZNAK];
	scanf("%s %s", meno1, meno2);
	while (kino != NULL)
	{
		HEREC* pom = kino->herci;
		while (pom != NULL)
		{
			if (!strcmp(meno1, pom->meno.krstne) && !strcmp(meno2, pom->meno.priezvisko)) break;
			pom = pom->herci;
		}
		if (pom != NULL) printf("%s (%d)\n", kino->nazov, kino->rokVyroby); //nasli sme herca lebo pom neukazuje na NULL
		kino = kino->dalsiFilm;
	}


}

void herci(FILM* kino)
{
	while (getchar() != '\n');
	char nazov1[MAXZNAK], nazov2[MAXZNAK];
	fgets(nazov1, MAXZNAK, stdin);
	nazov1[strlen(nazov1) - 1] = '\0';

	fgets(nazov2, MAXZNAK, stdin);
	nazov2[strlen(nazov2) - 1] = '\0';

	HEREC* pole = NULL; //dynamicke pole hercov
	int pocetHercov = 0;
	while (kino != NULL)
	{
		if (strcmp(nazov1, kino->nazov) && strcmp(nazov2, kino->nazov)) { kino = kino->dalsiFilm; continue; }

		HEREC* pom = kino->herci;
		while (pom != NULL)
		{
			pole = (HEREC*)realloc(pole, (pocetHercov + 1) * sizeof(HEREC));
			if (pole == NULL) PAMATERR
				pole[pocetHercov] = *pom;
			pocetHercov++;
			pom = pom->herci;
		}
		kino = kino->dalsiFilm;

	}
	for (int i = 0; i < pocetHercov; i++)
	{
		for (int j = i + 1; j < pocetHercov; j++)
		{
			if (!strcmp(pole[i].meno.priezvisko, pole[j].meno.priezvisko))
			{
				printf("%s %s (%d), ", pole[i].meno.krstne, pole[i].meno.priezvisko, pole[i].rokNarodenia); //vypiseme ked sa meno nachadza v poli 2x
			}
		}
	}
	printf("\b\b ");
	putchar('\n');
	free(pole);
}

HEREC* zoznamRok(HEREC* zac, HEREC zdroj)
{
	if (zac == NULL) //vkladame prveho herca
	{
		zac = (HEREC*)malloc(sizeof(HEREC));
		if (zac == NULL) PAMATERR
			* zac = zdroj;
		zac->herci = NULL;
		return zac;
	}

	if (porovnaj(zdroj, *zac) < 0) //treba poeovnat zvlast prveho, lebo sa vracia iny ukazovatel
	{
		HEREC* novy = (HEREC*)malloc(sizeof(HEREC));
		if (novy == NULL) PAMATERR
			* novy = zdroj;
		novy->herci = zac;
		return novy;
	}

	HEREC* pom = zac;

	while (pom->herci != NULL && porovnaj(zdroj, *(pom->herci)) > 0) pom = pom->herci;
	if (pom->herci != NULL && porovnaj(zdroj, *(pom->herci)) == 0) return zac; //ked sme koncili v loope kvoli 0 mame toho isteho herca

	HEREC* novy = (HEREC*)malloc(sizeof(HEREC)); //koncili sme kvoli -1
	if (novy == NULL) PAMATERR;
	*novy = zdroj;
	novy->herci = pom->herci;
	pom->herci = novy;

	return zac;
}

void rok(FILM* kino)
{
	int rok;
	scanf("%d", &rok);
	HEREC* zac = NULL;

	while (kino != NULL)
	{
		if (kino->rokVyroby == rok) //kontrola ci sa rok zhoduje
		{
			HEREC* temp = kino->herci;
			while (temp != NULL) //ideme cez zoznam hercov a vkladame ich do noveho zoznamu
			{
				zac = zoznamRok(zac, *temp);
				temp = temp->herci;
			}
		}
		kino = kino->dalsiFilm;
	}

	while (zac != NULL) //vypisujeme a postupne sa uvolnuje pamat
	{
		printf("%s %s %d", zac->meno.krstne, zac->meno.priezvisko, zac->rokNarodenia);
		HEREC* pom = zac;
		zac = zac->herci;
		if (zac != NULL) printf(", ");
		free(pom);
	}
	putchar('\n');
}

int porovnaj(const HEREC a, const HEREC b)
{
	if (strcmp(a.meno.priezvisko, b.meno.priezvisko)) //zoradit podla priezviska
		return(strcmp(a.meno.priezvisko, b.meno.priezvisko) < 0 ? -1 : 1);

	else if (strcmp(a.meno.krstne, b.meno.krstne)) //zoradit podla krstneho mena
		return(strcmp(a.meno.krstne, b.meno.krstne) < 0 ? -1 : 1);

	else if (a.rokNarodenia != b.rokNarodenia) //zoradit podla roku narodenia
		return(a.rokNarodenia < b.rokNarodenia ? -1 : 1);

	return 0; //uplne rovnaky prvok
}