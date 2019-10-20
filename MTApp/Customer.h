#pragma once
#include <mutex>
#include <deque>


using namespace std;

struct Winda;



class Customer
{
public:
	Customer(int);
	~Customer();

	int id;
	bool pietro;


	void introduce();

	void wejdz_do_kolejki_recepcja(deque<int> &, mutex &, mutex &);
	void wyjdz_z_kolejki_recepcja(deque<int> &, mutex &, mutex &);

	void potrzeba_spotkanie(deque <int> &, deque <bool> &, mutex &, mutex &,mutex &,mutex &, int &, int &, Winda &, deque <int> &, deque <int> &, deque <bool> &, deque <bool> &, mutex &);

	int zajmij_recepcjoniste(deque <bool> & , mutex & );

	void wezwij_pracownika(int&, deque <bool> &, mutex &, mutex &);

	void wejdz_do_kolejki_winda_dol(deque <int> &, Winda &, mutex &, mutex &);
	void wyjdz_z_kolejki_winda_dol(deque <int> &, Winda &, mutex &, mutex &);

	void wejdz_do_kolejki_winda_gora(deque <int> &, Winda &, mutex &, mutex &);
	void wyjdz_z_kolejki_winda_gora(deque <int> &, Winda &, mutex &, mutex &);
};

