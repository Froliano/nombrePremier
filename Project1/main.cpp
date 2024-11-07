#include <iostream>
#include <math.h>
#include <future>
#include <stdio.h>
#include <chrono>
#include <mutex>
#include <fstream>
#include <string>


/*
* Basil 
* Mathis
* Frejus
* patrick
*/
std::mutex mt;
bool isPremier(int value)
{
	if (value == 2)
	{
		return true;
	}
	else if(value == 1 || value % 2 == 0)
	{
		return false;
	}
	for (int i = 3; i <= round(sqrt(value)); i += 2)
	{
		if (value % i == 0)
		{
			return false;
		}
	}
	return true;
}

void premiers(int debut, int fin, int pas, std::shared_ptr<std::vector<int>> listePremier)
{
	//auto begin = std::chrono::high_resolution_clock::now();
	for (int i = debut; i < fin; i += pas)
	{
		if (isPremier(i)) 
		{
			mt.lock();
			listePremier->push_back(i);
			mt.unlock();
		}
	}
	/*auto end = std::chrono::high_resolution_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
	mt.lock();
	std::cout << elapsed.count() << " microsecondes " << std::endl;
	mt.unlock();*/
}

void calculNombre(int threadsUsed, int fin)
{
	std::vector<std::thread> listeAsync;
	std::shared_ptr<std::vector<int>> listePremiers = std::make_shared<std::vector<int>>();

	auto begin = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < threadsUsed; i++)
	{
		listeAsync.push_back(std::thread(premiers, i, fin, threadsUsed, listePremiers));
	}

	for (int i = 0; i < threadsUsed; i++)
	{
		listeAsync[i].join();

	}

	std::ofstream fichier("data" + std::to_string(threadsUsed) + "Thread" + std::to_string(fin) + "Value.txt");
	if (fichier)
	{
		fichier.clear();
		for (auto& value : *listePremiers)
		{
			fichier << value << std::endl;
		}
	}

	auto end = std::chrono::high_resolution_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);

	std::cout << "Temps pour " << threadsUsed << " Thread\t     pour  " << fin << " nombres \t: " << elapsed.count() << " microsecondes\n";
}

int main()
{
	unsigned int numThreads = std::thread::hardware_concurrency();

	calculNombre(1, 10000);
	calculNombre(numThreads, 10000);
	calculNombre(1, 100000);
	calculNombre(numThreads, 100000);
	calculNombre(1, 1000000);
	calculNombre(numThreads, 1000000);

	return 0;
}