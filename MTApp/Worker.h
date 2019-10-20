#pragma once
#include <deque>
#include <mutex>
#include <vector>

using namespace std;

class Worker
{
public:
	Worker(int);
	~Worker();

	int id;
	bool pietro;

	void introduce();

	int zajmij_toalete(deque <bool> &, mutex &, mutex &);

	void skorzystaj_z_toalety(deque <bool> &, int, mutex &, mutex &);

	void wejdz_do_kolejki_toalety(deque<int> &, mutex &, mutex &);
	void wyjdz_z_kolejki_toalety(deque<int> &, mutex &, mutex &);

	void potrzeba_toaleta(deque<int> &, deque<bool> &, mutex &, mutex &);

	bool sprawdz_tablice_wezwan(mutex &, deque <bool> &);

	void idz_na_spotkanie(mutex &, mutex &, deque <bool> &);

	int zajmij_komputer(deque <bool> &, mutex &, mutex &);

	void skorzystaj_z_komputera(deque <bool> &, int, mutex &, mutex &, mutex &, deque <bool> &);

	void wejdz_do_kolejki_komputery(deque<int> &, mutex &, mutex &);
	void wyjdz_z_kolejki_komputery(deque<int> &, mutex &, mutex &);

	void potrzeba_komputer(deque<int> &, deque<bool> &, mutex &, mutex &, mutex &, deque <bool> &);
};

