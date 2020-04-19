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
	struct MENO;
	int rokNarodenia;
	struct herec* kolega;
}HEREC;

typedef struct film
{
	char nazov[MAXZNAK];
	int rokVyroby;
	struct MENO; //meno rezisera
	struct HEREC* herci;
	struct film* dalsiFilm;
}FILM;


int main()
{

	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started:
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
