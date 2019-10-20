//#include "stdafx.h"
#include <ncurses.h>
#include "Worker.h"
#include <cstdlib>
#include <iostream>
#include <thread>
#include <mutex>
#include <deque>
#include <cmath>
#include <algorithm>
//#include <Windows.h>
#include <vector>
//#include "Licznik.h"
#include <random>
#include <unistd.h>
#include <time.h>


using namespace std;



Worker::Worker(int id)
{
	this->id = id;
	this->pietro = true;
}

Worker::~Worker()
{
}


void Worker::introduce()
{
	cout << "worker, id = " << this->id << endl;
}

void Worker::wejdz_do_kolejki_toalety(deque <int> &kolejka_toalety, mutex & m, mutex & m_wc)
{
	m_wc.lock();
	kolejka_toalety.push_back(this->id);
	m_wc.unlock();
	
	m.lock();
	//cout << this->id << " w kolejce" << endl;
    mvprintw(this->id,0,"pracownik     %d w kolejce do wc\n", this->id);
	refresh();
	m.unlock();
}

void Worker::wyjdz_z_kolejki_toalety(deque <int> &kolejka_toalety, mutex & m, mutex & m_wc)
{
	m_wc.lock();
	kolejka_toalety.pop_front();
	m_wc.unlock();

	m.lock();
	//cout << this->id << " opuscil kolejke" << endl;
    mvprintw(this->id,0,"pracownik     %d opuscil kolejke do wc\n", this->id);
	refresh();
	m.unlock();
}

int Worker::zajmij_toalete(deque <bool> & toalety, mutex & m_wc, mutex & m)
{
	int wybrane_wc = -1;
	m_wc.lock();
	for (int i = 0; i < toalety.size(); i++)
	{
		if (toalety[i] == true)
		{
			toalety[i] = false;
        
            m.lock();
            mvprintw(14,0,"toalety   ");
            for(int i=0; i<toalety.size(); i++)
            {
	            if(toalety[i]) mvprintw(14,11+i,"0");
                else mvprintw(14,11+i,"1"); 
            }
            refresh();
            m.unlock();            
    
			wybrane_wc = i;
			break;
		}
	}
	m_wc.unlock();

	return wybrane_wc;
}

void Worker::skorzystaj_z_toalety(deque <bool> & toalety, int numer, mutex &m, mutex &m_wc)
{
	m.lock();
	//cout << "worker " << this->id << " zajal wc " << numer << endl;
    mvprintw(this->id,0,"pracownik     %d zajal wc %d\n", this->id, numer);
	refresh();
	m.unlock();

    int time2 = 4000000;//(int(dist(mt)));
	vector <char> loading2;
	int part_time2 = (int)(time2/10);
	for(int i=0; i<10; i++)
	{
		loading2.push_back('W');
		for(int j=0; j<loading2.size(); j++)
		{
			m.lock();
			mvprintw(this->id,50+j,"%c", loading2[j]);
			mvprintw(this->id,60,"%c", '|');
            refresh();
	    	m.unlock();
			
		}
	usleep(part_time2);
	}
	loading2.clear();

	//usleep(3000000);
	m.lock();
	mvprintw(this->id,0,"pracownik     %d zwolnil wc %d\n", this->id, numer);
	refresh();
	m.unlock();

	m_wc.lock();
    m.lock();
	toalety[numer] = true;
    
    mvprintw(14,0,"toalety   ");
    for(int i=0; i<toalety.size(); i++)
    {
	    if(toalety[i]) mvprintw(14,11+i,"0");
        else mvprintw(14,11+i,"1"); 
	    
    }
    refresh();
    m.unlock();
	m_wc.unlock();
}

void Worker::potrzeba_toaleta(deque <int> &kolejka_toalety, deque <bool> & toalety, mutex &m, mutex &m_wc)
{
	this->wejdz_do_kolejki_toalety(kolejka_toalety, m, m_wc);

	bool czy_pierwszy_w_kolejce = false;
	
	while (czy_pierwszy_w_kolejce == false)
	{
		m_wc.lock();
		if (kolejka_toalety[0] == this->id)
		{
			czy_pierwszy_w_kolejce = true;
			//cout << this->id << " jestem pierwszy!" << endl;
		}
		m_wc.unlock();
		usleep(10000);
	}	

	
	if (czy_pierwszy_w_kolejce)
	{
		int wc = this->zajmij_toalete(toalety, m_wc, m);
		while (wc < 0)
		{
			//cout << this->id << " czekam" << endl;
			usleep(10000);
			wc = this->zajmij_toalete(toalety, m_wc, m);
		}
		this->wyjdz_z_kolejki_toalety(kolejka_toalety,m,  m_wc);
		this->skorzystaj_z_toalety(toalety, wc, m, m_wc);
	}
	czy_pierwszy_w_kolejce = false;

}

bool Worker::sprawdz_tablice_wezwan(mutex & m_tablica, deque<bool> & tablica_wezwan)
{
	bool wezwanie = false;
	m_tablica.lock();
	if (tablica_wezwan[this->id] == true)
	{
		wezwanie = true;
	}
	m_tablica.unlock();

	if (wezwanie == true)
	{
		//this->idz_na_spotkanie();
		m_tablica.lock();
		tablica_wezwan[this->id] = false;
		m_tablica.unlock();

		return true;
	}
	return false;
}

void Worker::idz_na_spotkanie(mutex &m, mutex &m_tablica, deque <bool> &tablica_spotkan)
{
	m.lock();
	//cout <<"pracownik"<< this->id << " idzie na spotkani" << endl;
    mvprintw(this->id,0,"pracownik     %d idzie na spotkanie\n", this->id);
	refresh();
	m.unlock();

	bool czy_dali_znak = false;

	while (!czy_dali_znak)
	{
		m_tablica.lock();
		if (tablica_spotkan[this->id] == true)
		{
			czy_dali_znak = true;
		}
		m_tablica.unlock();
		usleep(10000);
	}
	czy_dali_znak = false;

    m.lock();
	//cout <<"pracownik"<< this->id << " idzie na spotkani" << endl;
    mvprintw(this->id,0,"pracownik     %d na spotkaniu\n", this->id);
	refresh();
	m.unlock();

	while (!czy_dali_znak) //tutaj pracownik spedza czas na spotkaniu
	{
		m_tablica.lock();
		if (tablica_spotkan[this->id] == false)
		{
			czy_dali_znak = true;
		}
		m_tablica.unlock();
		usleep(100000);
	}

	//koniec spotkania

}

void Worker::wejdz_do_kolejki_komputery(deque <int> &kolejka_komputery, mutex & m, mutex & m_komp)
{
	m_komp.lock();
	kolejka_komputery.push_back(this->id);
	m_komp.unlock();

	m.lock();
	mvprintw(this->id,0,"pracownik     %d w kolejce do komputera\n", this->id);
	refresh();
	m.unlock();
}

void Worker::wyjdz_z_kolejki_komputery(deque <int> &kolejka_komputery, mutex & m, mutex & m_komp)
{
	m_komp.lock();
	kolejka_komputery.pop_front();
	m_komp.unlock();

	m.lock();
	mvprintw(this->id,0,"pracownik     %d opuscil kolejke do komputera\n", this->id);
	refresh();
	m.unlock();
}

int Worker::zajmij_komputer(deque <bool> & komputery, mutex & m_komp, mutex & m)
{
	int wybrany_komp = -1;
	m_komp.lock();
	for (int i = 0; i < komputery.size(); i++)
	{
		if (komputery[i] == true)
		{
            m.lock();    
            mvprintw(15,0,"komputery   ");
            for(int i=0; i<komputery.size(); i++)
            {
	            if(komputery[i]) mvprintw(15,11+i,"0");
                else mvprintw(15,11+i,"1"); 
            }
            refresh();
            m.unlock();
            
			komputery[i] = false;

            m.lock();    
            mvprintw(15,0,"komputery   ");
            for(int i=0; i<komputery.size(); i++)
            {
	            if(komputery[i]) mvprintw(15,11+i,"0");
                else mvprintw(15,11+i,"1"); 
            }
            refresh();
            m.unlock();

			wybrany_komp = i;
			break;
		}
	}
	m_komp.unlock();

	return wybrany_komp;
}

void Worker::skorzystaj_z_komputera(deque <bool> & komputery, int numer, mutex &m, mutex &m_komp, mutex &m_tablica, deque <bool> &tablica_wezwan)
{
	m.lock();
	//cout << "worker " << this->id << " zajal komp " << numer << endl;
    mvprintw(this->id,0,"pracownik     %d zajal komputer %d\n", this->id, numer);
	refresh();
	m.unlock();

    int time2 = 3000000;//(int(dist(mt)));
	vector <char> loading2;
	int part_time2 = (int)(time2/10);
	for(int i=0; i<10; i++)
	{
		loading2.push_back('O');
		for(int j=0; j<loading2.size(); j++)
		{
			m.lock();
			mvprintw(this->id,50+j,"%c", loading2[j]);
			mvprintw(this->id,60,"%c", '|');
            refresh();
	    	m.unlock();
			
		}
		usleep(part_time2);
	}
		loading2.clear();
	//usleep(3000000);

	if (this->sprawdz_tablice_wezwan(m_tablica, tablica_wezwan))
	{
		m.lock();
		mvprintw(this->id,0,"pracownik     %d zwolnil komputer %d\n", this->id, numer);
	    refresh();
		m.unlock();

		m_komp.lock();

        m.lock();    
            mvprintw(15,0,"komputery   ");
            for(int i=0; i<komputery.size(); i++)
            {
	            if(komputery[i]) mvprintw(15,11+i,"0");
                else mvprintw(15,11+i,"1"); 
            }
            refresh();
            m.unlock();

		komputery[numer] = true;

        m.lock();
    
        mvprintw(15,0,"komputery   ");
        for(int i=0; i<komputery.size(); i++)
        {
	        if(komputery[i]) mvprintw(15,11+i,"0");
            else mvprintw(15,11+i,"1"); 
        }
        refresh();
        m.unlock();

		m_komp.unlock();
		this->idz_na_spotkanie(m, m_tablica, tablica_wezwan);
		return;
	}
	else
	{
        time2 = 3000000;//(int(dist(mt)));
		//vector <char> loading2;
		part_time2 = (int)(time2/10);
		for(int i=0; i<10; i++)
		{
			loading2.push_back('O');
			for(int j=0; j<loading2.size(); j++)
			{
				m.lock();
				mvprintw(this->id,50+j,"%c", loading2[j]);
				mvprintw(this->id,60,"%s", "|xxx");
                refresh();
				m.unlock();
				
			}
			usleep(part_time2);
		}
		loading2.clear();

		//usleep(3000000);
	}

	m.lock();
	mvprintw(this->id,0,"pracownik     %d zwolnil komputer %d\n", this->id, numer);
	refresh();
	m.unlock();

	m_komp.lock();

    m.lock();    
            mvprintw(15,0,"komputery   ");
            for(int i=0; i<komputery.size(); i++)
            {
	            if(komputery[i]) mvprintw(15,11+i,"0");
                else mvprintw(15,11+i,"1"); 
            }
            refresh();
            m.unlock();

	komputery[numer] = true;

    m.lock();
    
    mvprintw(15,0,"komputery   ");
    for(int i=0; i<komputery.size(); i++)
    {
	    if(komputery[i]) mvprintw(15,11+i,"0");
        else mvprintw(15,11+i,"1"); 
    }
    refresh();
    m.unlock();

	m_komp.unlock();
}

void Worker::potrzeba_komputer(deque <int> &kolejka_komputery, deque <bool> & komputery, mutex &m, mutex &m_komp, mutex &m_tablica, deque <bool> & tablica_wezwan)
{
	this->wejdz_do_kolejki_komputery(kolejka_komputery, m, m_komp);

	bool czy_pierwszy_w_kolejce = false;

	while (czy_pierwszy_w_kolejce == false)
	{
		m_komp.lock();
		if (kolejka_komputery[0] == this->id)
		{
			czy_pierwszy_w_kolejce = true;
			//cout << this->id << " jestem pierwszy!" << endl;
		}
		m_komp.unlock();
		usleep(100000);
	}


	if (czy_pierwszy_w_kolejce)
	{
		int wc = this->zajmij_komputer(komputery, m_komp, m);
		while (wc < 0)
		{
			//cout << this->id << " czekam" << endl;
			usleep(10000);
			wc = this->zajmij_komputer(komputery, m_komp, m);
		}
		this->wyjdz_z_kolejki_komputery(kolejka_komputery, m, m_komp);
		this->skorzystaj_z_komputera(komputery, wc, m, m_komp, m_tablica, tablica_wezwan);
	}
	czy_pierwszy_w_kolejce = false;

}
