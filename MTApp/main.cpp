#include <cstdlib>
#include <iostream>
#include <thread>
#include <mutex>
#include <deque>
#include <cmath>
#include <algorithm>
#include <ncurses.h>
#include "Worker.h"
//#include <Windows.h>
#include <vector>
//#include "Licznik.h"
#include "Customer.h"
//#include <conio.h>
#include <random>
#include <unistd.h>
#include <time.h>
#include <ctime>

using namespace std;


/*
g++ -o prog etap_3.cpp Worker.cpp Customer.cpp -lpthread -lncurses -std=c++11
*/

const int ilosc_toalet = 4;
const int ilosc_recepcjonistow = 2;
const int ilosc_pokojow_spotkan = 2;
const int ilosc_komputerow = 8;

const int ilosc_pracownikow = 12;
const int ilosc_klientow = 4;

bool zyja_klienci = true;
bool zyja_pracownicy = true;
int pozostalych_klientow = ilosc_klientow;

int wybor_r_0 = 0;
int wybor_r_1 = ilosc_pracownikow/2;

class Worker;
class Customer;

Worker **pracownicy;
Customer **klienci;

deque <bool> toalety;
deque <int> kolejka_toalety;

deque <bool> komputery;
deque <int> kolejka_komputery;

deque <bool> recepcjonisci;
deque <int> kolejka_recepcja;

deque <bool> tablica_wezwan;

mutex m_wc;
mutex m_rec;
mutex m_winda;
mutex m_spotkanie;
mutex m;
mutex m_tablica;
mutex m_komp;
mutex m_kill;

struct Winda
{
	bool pietro = false;
	bool zajeta = false;
};

Winda w;
deque <int> kolejka_winda_dol;
deque <int> kolejka_winda_gora;

deque <bool> pokoje_spotkan;


void zycie_pracownika(Worker * w)
{
	while(zyja_pracownicy)//for (int i = 0; i < 3; i++)//while(true)//for (int i = 0; i < 3; i++)
	{
		if(rand()%100 > 50) w->potrzeba_toaleta(kolejka_toalety, toalety, m, m_wc);
		w->potrzeba_komputer(kolejka_komputery, komputery, m, m_komp, m_tablica, tablica_wezwan);
		if (w->sprawdz_tablice_wezwan(m_tablica, tablica_wezwan)) w->idz_na_spotkanie(m, m_tablica, tablica_wezwan);
		//Sleep(1000);
	}

	m.lock();
	mvprintw(w->id,0,"pracownik     %d umarl\n", w->id);
	refresh();
	m.unlock();
}

void zycie_klienta(Customer *c)
{
	while(zyja_klienci)//for (int i = 0; i < 1; i++)
	{
		c->potrzeba_spotkanie(kolejka_recepcja,tablica_wezwan, m, m_rec, m_winda, m_spotkanie, wybor_r_0, wybor_r_1, w, kolejka_winda_dol, kolejka_winda_gora, pokoje_spotkan, recepcjonisci, m_tablica);
	}

	m_kill.lock();
	pozostalych_klientow -= 1;
	m_kill.unlock();

	m.lock();
	mvprintw(17+c->id,0,"klient     %d umarl\n", c->id);
	refresh();
	m.unlock();
}
void koncz()
{
	bool dziala = true;
	char x;
	while(dziala)
	{
		x = getch();

		if (x == 'q')
		{
			

			zyja_klienci = false;
			bool zamykac_pracownikow = true;
			while (zamykac_pracownikow)
			{
				m_kill.lock();
				if (pozostalych_klientow == 0) zamykac_pracownikow = false;
				m_kill.unlock();
				
				usleep(15000);
			}

			zyja_pracownicy = false;
			dziala = false;
		}

		
	}
}

int main()
{
    srand(time(NULL));
    initscr();
	for (int i = 0; i <ilosc_pracownikow; i++)
	{
		tablica_wezwan.push_back(false); //false = nie zostal wezwany //true - zostal wezwany i musi isc...
	}

	for (int i = 0; i <ilosc_toalet; i++)
	{
		toalety.push_back(true); //true czyli wolne
	}
	for (int i = 0; i <ilosc_komputerow; i++)
	{
		komputery.push_back(true); //true czyli wolne
	}
	

	for (int i = 0; i <ilosc_pokojow_spotkan; i++)
	{
		pokoje_spotkan.push_back(true); //true czyli wolne
	}

	for (int i = 0; i <ilosc_recepcjonistow; i++)
	{
		recepcjonisci.push_back(true); //true czyli wolne
	}

	/////////////////////////////////////                                      start

	thread workers_thr[ilosc_pracownikow];
	thread clients_thr[ilosc_klientow];
	thread zakonczenie;

	pracownicy = new Worker*[ilosc_pracownikow];
	for (int i = 0; i < ilosc_pracownikow; i++)
	{
		pracownicy[i] = new Worker(i);
	}

	klienci = new Customer*[ilosc_klientow];
	for (int i = 0; i < ilosc_klientow; i++)
	{
		klienci[i] = new Customer(i);
	}

	for (int i = 0; i < ilosc_pracownikow; i++)
	{
		workers_thr[i] = thread(zycie_pracownika, pracownicy[i]);
	}
    for(int i = 0; i < ilosc_klientow; i++)
	{
		clients_thr[i] = thread(zycie_klienta, klienci[i]);
	}

	zakonczenie = thread(koncz);
////////////////////////////////////////////////////////////////////joinowanie
	zakonczenie.join();
	for (int i = 0; i < ilosc_pracownikow; i++)
	{
		workers_thr[i].join();
	}
	for (int i = 0; i < ilosc_klientow; i++)
	{
		clients_thr[i].join();
	}
    refresh();
	endwin();
    return 0;
}

