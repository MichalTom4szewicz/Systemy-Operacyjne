//#include "stdafx.h"
#include <ncurses.h>
#include "Customer.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <deque>
#include <cmath>
#include <algorithm>
//#include <Windows.h>
#include <vector>
#include <random>
#include <unistd.h>
#include <time.h>


using namespace std;

struct Winda
{
	bool pietro; 
	bool zajeta;
};

Customer::Customer(int id)
{
	this->id = id;
	this->pietro = false;
}

Customer::~Customer()
{
}

void Customer::introduce()
{
	cout << "customer, id = " << this->id << endl;
}


void Customer::wejdz_do_kolejki_recepcja(deque <int> &kolejka_recepcja, mutex & m, mutex & m_rec)
{
	m_rec.lock();
	kolejka_recepcja.push_back(this->id);
	m_rec.unlock();

	m.lock();
	mvprintw(17+this->id,0,"klient     %d w kolejce na recepcje\n", this->id);
	refresh();
	m.unlock();
}

void Customer::wyjdz_z_kolejki_recepcja(deque <int> &kolejka_recepcja, mutex & m, mutex & m_rec)
{
	m_rec.lock();
	kolejka_recepcja.pop_front();
	m_rec.unlock();

	m.lock();
	mvprintw(17+this->id,0,"klient     %d opuscil kolejke na recepcje\n", this->id);
	refresh();
	m.unlock();
}

void Customer::wejdz_do_kolejki_winda_dol(deque <int> &kolejka_winda_dol, Winda &w, mutex & m, mutex & m_winda)
{
	m_winda.lock();
	kolejka_winda_dol.push_back(this->id);
	m_winda.unlock();

	m.lock();
	//cout << this->id << " klient w kolejce do windy dol" << endl;
    mvprintw(17+this->id,0,"klient     %d w kolejce do windy na dole\n", this->id);
	refresh();
	m.unlock();
}

void Customer::wejdz_do_kolejki_winda_gora(deque <int> &kolejka_winda_gora, Winda &w, mutex & m, mutex & m_winda)
{
	m_winda.lock();
	kolejka_winda_gora.push_back(this->id);
	m_winda.unlock();

	m.lock();
	mvprintw(17+this->id,0,"klient     %d w kolejce do windy na gorze\n", this->id);
	refresh();
	m.unlock();
}

void Customer::wyjdz_z_kolejki_winda_dol(deque <int> &kolejka_winda_dol, Winda &w, mutex & m, mutex & m_winda)
{
	m_winda.lock();
	kolejka_winda_dol.pop_front();
	m_winda.unlock();

	m.lock();
	mvprintw(17+this->id,0,"klient     %d opuscil kolejke windy na dole\n", this->id);
	refresh();
	m.unlock();
}

void Customer::wyjdz_z_kolejki_winda_gora(deque <int> &kolejka_winda_gora, Winda &w, mutex & m, mutex & m_winda)
{
	m_winda.lock();
	kolejka_winda_gora.pop_front();
	m_winda.unlock();

	m.lock();
	mvprintw(17+this->id,0,"klient     %d opuscil kolejke windy na gorze\n", this->id);
	refresh();
	m.unlock();
}

void Customer::potrzeba_spotkanie(deque<int>& kolejka_recepcja, deque <bool> & tablica_wezwan, mutex & m, mutex & m_rec, mutex & m_winda, mutex &m_spotkanie, int & wybor_r_0, int & wybor_r_1, Winda &w, deque <int> & kolejka_winda_dol, deque <int> & kolejka_winda_gora, deque <bool> &pokoje_spotkan, deque <bool> & recepcjonisci, mutex & m_tablica)
{
	//this->introduce();

	this->wejdz_do_kolejki_recepcja(kolejka_recepcja, m, m_rec);

	bool czy_pierwszy_w_kolejce = false;

	while (czy_pierwszy_w_kolejce == false)
	{
		m_rec.lock();
		if (kolejka_recepcja[0] == this->id)
		{
			czy_pierwszy_w_kolejce = true;
			//cout << this->id << " pierwszy" << endl;
		}
		m_rec.unlock();
		usleep(10000);
	}

	if (czy_pierwszy_w_kolejce)
	{
		int recepcjonista = this->zajmij_recepcjoniste(recepcjonisci, m_rec);/////////////////////////
		while (recepcjonista < 0)
		{			
			usleep(10000);
			recepcjonista = this->zajmij_recepcjoniste(recepcjonisci, m_rec);///////////////////////
		}
		int pracownik;
		if (recepcjonista == 0)
		{
			this->wezwij_pracownika(wybor_r_0, tablica_wezwan, m, m_tablica);
			pracownik = wybor_r_0;
			wybor_r_0++;
			if (wybor_r_0 == 6) wybor_r_0 = 0;
		}
		else
		{
			this->wezwij_pracownika(wybor_r_1, tablica_wezwan, m, m_tablica);
			pracownik = wybor_r_1;
			wybor_r_1++;
			if (wybor_r_1 == 12) wybor_r_1 = 6;
		}
		//cout << this->id << " wybral recepcjoniste , delegowany pracownik ma nr: " << pracownik << endl;
        m.lock();
        mvprintw(17+this->id,0,"klient     %d wybral recepcjoniste i pracownika %d\n", this->id, pracownik);
	    refresh();        
        m.unlock();
		//trzeba ogarnac winde

		this->wejdz_do_kolejki_winda_dol(kolejka_winda_dol, w, m, m_winda);
		this->wyjdz_z_kolejki_recepcja(kolejka_recepcja, m, m_rec);

		bool czy_pierwszy_w_kolejce_w = false;
		
		while (czy_pierwszy_w_kolejce_w == false)
		{
			m_winda.lock();
			if (kolejka_winda_dol[0] == this->id)
			{
				czy_pierwszy_w_kolejce_w = true;
			}
			m_winda.unlock();
			usleep(10000);
		}
		czy_pierwszy_w_kolejce_w = false;
		//////////////////////

		bool winda_zajeta = true;

		while (winda_zajeta) //zajmowanie windy
		{
			m_winda.lock();
			if (w.zajeta == false)
			{
				winda_zajeta = false;
				w.zajeta = true;

                m.lock();
                mvprintw(23,0,"winda ");
                if(w.zajeta) mvprintw(23, 10, "zajeta");
                else mvprintw(23,10,"wolna"); 
                m.unlock();
			}
			m_winda.unlock();
			usleep(20000);
		}
		//zajeto winde
		winda_zajeta = true;

        int time1;
        vector <char> loading1;
        int part_time1;
        
		m_winda.lock();
		if (w.pietro == true)
		{
			m.lock();
			//cout << "winda na gorze" << endl;
            mvprintw(22,0,"winda na gorze\n");
	        refresh();
			m.unlock();
        
            time1 = 6000000;//(int(dist(mt)));		    
		    part_time1 = (int)(time1/10);
		    for(int i=0; i<10; i++)
		    {
			    loading1.push_back('O');
			    for(int j=0; j<loading1.size(); j++)
			    {
				    m.lock();
			       	mvprintw(22,20+j,"%c", loading1[j]);
				    mvprintw(22,30,"%c", '|');
			    	refresh();
                    m.unlock();
				    
			    }
			    usleep(part_time1);
		    }
		    loading1.clear();
                
			//usleep(1000000);
			w.pietro = false;//zjechala na dol
			m.lock();
			mvprintw(22,0,"winda na dole\n");
	        refresh();
			m.unlock();

			time1 = 6000000;//(int(dist(mt)));		    
		    part_time1 = (int)(time1/10);
		    for(int i=0; i<10; i++)
		    {
			    loading1.push_back('O');
			    for(int j=0; j<loading1.size(); j++)
			    {
				    m.lock();
			       	mvprintw(22,20+j,"%c", loading1[j]);
				    mvprintw(22,30,"%c", '|');
			    	refresh();
                    m.unlock();
				    
			    }
			    usleep(part_time1);
		    }
		    loading1.clear();

			w.pietro = true; //jedziemy na gore
			m.lock();
			//cout << "winda na gorze" << endl;
			//cout << "winda na gorze, " << this->id << " na gorze idzie na spotkanie" << endl;
            mvprintw(17+this->id,0,"klient     %d idzie na spotknie\n", this->id);
            mvprintw(22,0,"winda na gorze\n");
	        refresh();
			m.unlock();
		}
		else
		{
			m.lock();
			//cout << "winda na dole" << endl;
            mvprintw(22,0,"winda na dolee\n");
	        refresh();
			m.unlock();

			time1 = 6000000;//(int(dist(mt)));		    
		    part_time1 = (int)(time1/10);
		    for(int i=0; i<10; i++)
		    {
			    loading1.push_back('O');
			    for(int j=0; j<loading1.size(); j++)
			    {
				    m.lock();
			       	mvprintw(22,20+j,"%c", loading1[j]);
				    mvprintw(22,30,"%c", '|');
                    refresh();
			    	m.unlock();
				    
			    }
			    usleep(part_time1);
		    }
		    loading1.clear();

			w.pietro = true; //jedziemy na gore
			m.lock();
			//cout << "winda na gorze" << endl;
			//cout <<"winda na gorze, "<< this->id << " na gorze idzie na spotkanie" << endl;
            mvprintw(17+this->id,0,"klient     %d idzie na spotknie\n", this->id);
            mvprintw(22,0,"winda na gorze\n");
	        refresh();
			m.unlock();
		}
        w.zajeta = false;
        m.lock();
                mvprintw(23,0,"winda ");
                if(w.zajeta) mvprintw(23, 10, "zajeta");
                else mvprintw(23,10,"wolna"); 
                m.unlock();
		m_winda.unlock();//jestesmy na gorze

		this->wyjdz_z_kolejki_winda_dol(kolejka_winda_dol, w, m, m_winda);

		//czekamy na pracownika

		bool pracownik_gotowy = false;

		while (pracownik_gotowy == false)
		{
			if (tablica_wezwan[pracownik] == false) // pracownik ustawia false gdy jest gotowy na spotkanie
			{
				pracownik_gotowy = true;
			}
			usleep(10000);
			//cout << this->id << " czeka na pracownika" << endl;
		}
		//to wyzej nie musi byc w SK bo jest 2 recepcjonistow i kazdy ma grupe pracownikow ktorych wzywa 

		//pracownik jest gotow

		

		//zajmujemy pokoj
		m_spotkanie.lock();
		if (pokoje_spotkan[0] == true)
		{
			pokoje_spotkan[0] = false;
		}
		else
		{
			pokoje_spotkan[1] = false;
		}
		m_spotkanie.unlock();

		m_tablica.lock();
		tablica_wezwan[pracownik] = true; ///znak dla pracownikaz ze ma byc na spotkaniu /sleep
		m_tablica.unlock();

		m.lock();
		//cout << "spotkanie klienta " << this->id << " i pracownika " << pracownik << " rozpoczelo sie" << endl;
        mvprintw(17+this->id,0,"klient     %d ma spotknie z pracownikiem %d\n", this->id, pracownik);
        refresh();
		m.unlock();

        int time2 = 5000000;//(int(dist(mt)));
		vector <char> loading2;
		int part_time2 = (int)(time2/10);
		for(int i=0; i<10; i++)
		{
			loading2.push_back('O');
			for(int j=0; j<loading2.size(); j++)
			{
				m.lock();
				mvprintw(17+this->id,50+j,"%c", loading2[j]);
				mvprintw(17+this->id,60,"%c", '|');
                refresh();
				m.unlock();
				
			}
			usleep(part_time2);
		}
		loading2.clear();
		//usleep(5000000); //spotkanie trwa

		m.lock();
		//cout << "spotkanie klienta " << this->id << " i pracownika " << pracownik << " dobiego konca" << endl;
        mvprintw(17+this->id,0,"klient     %d skonczyl spotkanie\n", this->id);
        refresh();
		m.unlock();

		m_tablica.lock();
		tablica_wezwan[pracownik] = false; //znak dla pracownika ze skonczyl spotkanie
		m_tablica.unlock();

		//koniec spotkania, trzeba wezwac winde na gore i pojechac na dol

		this->wejdz_do_kolejki_winda_gora(kolejka_winda_gora, w, m, m_winda);

		while (czy_pierwszy_w_kolejce_w == false)
		{
			m_winda.lock();
			if (kolejka_winda_gora[0] == this->id)
			{
				czy_pierwszy_w_kolejce_w = true;
			}
			m_winda.unlock();
			usleep(10000);
		}
		czy_pierwszy_w_kolejce_w = false;

        winda_zajeta = true;
		while (winda_zajeta) //zajmowanie windy
		{
			m_winda.lock();
			if (w.zajeta == false)
			{
				winda_zajeta = false;
				w.zajeta = true;
                m.lock();
                mvprintw(23,0,"winda ");
                if(w.zajeta) mvprintw(23, 10, "zajeta");
                else mvprintw(23,10,"wolna"); 
                m.unlock();
			}
			m_winda.unlock();
			usleep(20000);
		}
		//zajeto winde

		m_winda.lock();
		if (w.pietro == true)
		{
			m.lock();
			mvprintw(22,0,"winda na gorze\n");
	        refresh();
			m.unlock();

			time1 = 6000000;//(int(dist(mt)));		    
		    part_time1 = (int)(time1/10);
		    for(int i=0; i<10; i++)
		    {
			    loading1.push_back('O');
			    for(int j=0; j<loading1.size(); j++)
			    {
				    m.lock();
			       	mvprintw(22,20+j,"%c", loading1[j]);
				    mvprintw(22,30,"%c", '|');
                    refresh();
			    	m.unlock();
				    
			    }
			    usleep(part_time1);
		    }
		    loading1.clear();

			w.pietro = false;//zjezdza na dol
			m.lock();
			//cout << "winda na dole, klient " << this->id << " odchodzi" << endl;
            mvprintw(17+this->id,0,"klient     %d na dole, odchodzi\n", this->id);
            mvprintw(22,0,"winda na dolee\n");
            refresh();
			m.unlock();
		}
		else
		{
			m.lock();
			mvprintw(22,0,"winda na dolee\n");
            refresh();
			m.unlock();

			time1 = 6000000;//(int(dist(mt)));		    
		    part_time1 = (int)(time1/10);
		    for(int i=0; i<10; i++)
		    {
			    loading1.push_back('O');
			    for(int j=0; j<loading1.size(); j++)
			    {
				    m.lock();
			       	mvprintw(22,20+j,"%c", loading1[j]);
				    mvprintw(22,30,"%c", '|');
                    refresh();
			    	m.unlock();
				    
			    }
			    usleep(part_time1);
		    }
		    loading1.clear();

			w.pietro = true; //wina jedzie na gore na gore
			m.lock();
			//cout << "winda na gorze" << endl;
            mvprintw(22,0,"winda na gorze\n");
            refresh();
			m.unlock();

			time1 = 6000000;//(int(dist(mt)));		    
		    part_time1 = (int)(time1/10);
		    for(int i=0; i<10; i++)
		    {
			    loading1.push_back('O');
			    for(int j=0; j<loading1.size(); j++)
			    {
				    m.lock();
			       	mvprintw(22,20+j,"%c", loading1[j]);
				    mvprintw(22,30,"%c", '|');
                    refresh();
			    	m.unlock();
				    
			    }
			    usleep(part_time1);
		    }
		    loading1.clear();

			w.pietro = true; //jedziemy na dol
			m.lock();
			mvprintw(17+this->id,0,"klient     %d na dole, odchodzi\n", this->id);
            mvprintw(22,0,"winda na dolee\n");
            refresh();
			m.unlock();
		}
		m_winda.unlock();//jestesmy na dole

		m_winda.lock();
		w.zajeta = false;
        m.lock();
                mvprintw(23,0,"winda ");
                if(w.zajeta) mvprintw(23, 10, "zajeta");
                else mvprintw(23,10,"wolna"); 
                m.unlock();
		m_winda.unlock();

		this->wyjdz_z_kolejki_winda_gora(kolejka_winda_gora, w, m, m_winda);

		m_rec.lock();
		recepcjonisci[recepcjonista] = true;
		m_rec.unlock();


	}
}

int Customer::zajmij_recepcjoniste(deque <bool> & recepcjonisci, mutex & m_rec)
{
	int wybrany_rec = -1;
	m_rec.lock();
	for (int i = 0; i < recepcjonisci.size(); i++)
	{
		if (recepcjonisci[i] == true)
		{
			recepcjonisci[i] = false;
			wybrany_rec = i;
			break;
		}
	}
	m_rec.unlock();

	return wybrany_rec;
}

void Customer::wezwij_pracownika(int & wybor_r, deque <bool> & tablica_wezwan, mutex & m, mutex & m_tablica)
{
	m_tablica.lock();
	tablica_wezwan[wybor_r] = true;
	m_tablica.unlock();
}
