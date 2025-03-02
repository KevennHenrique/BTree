#ifndef RANDOMNUMBERGENERATOR_H_INCLUDED
#define RANDOMNUMBERGENERATOR_H_INCLUDED

#include <iostream>
#include <cstdlib>  // Para rand()
#include <ctime>    // Para time()
#include "BTree.h"

using namespace std;

template <class T, const unsigned int min_degree>
class randomNumberGenerator{
private:
    int amount;       // Quantidade de números a serem gerados (3000)
    int max_value;   // Valor máximo dos números gerados  (3000)

public:
    randomNumberGenerator(int amount = 3000, int max_value = 3000);
    //randomNumberGenerator(int amount = 3000, int max_value = 3000) : amount(amount), max_value(max_value){}  //poderia ser feito assim tambem

    void generateAndInsert(const string & fileName);

};

template <class T, const unsigned int min_degree>
randomNumberGenerator<T, min_degree>::randomNumberGenerator(int amount, int max_value) {
    this->amount = amount;
    this->max_value = max_value;
}

template <class T, const unsigned int min_degree>
void randomNumberGenerator<T, min_degree>::generateAndInsert(const string & fileName) {
        cout << "Gerando " << this->amount << " números aleatórios" << endl;

        srand(time(0));  // Inicializando o gerador de números aleatórios

        T key;

        fstream file(fileName, ios::in | ios::out | ios::trunc | ios::binary);

        if (!file.is_open())   // first run will create the file
        {
            file.open(fileName, ios::out);
            file.close();
            file.open(fileName, ios::in | ios::out | ios::binary);
        }
        if (file.is_open())
        {
            for (int i = 0; i < amount; i++) {
                int n = rand() % (max_value + 1);  // Gera número aleatório de 0 até max_valor

                file.write(reinterpret_cast<char*>(&n), sizeof(int));
                cout<< "numero aleatorio: "<< n <<endl;
            }
            file.close();
            cout << this->amount<< " números aleatórios foram escritos no arquivo com sucesso.\n" << endl;

        }
        else
        {
            cerr << "O arquivo nao pode ser aberto para escrever numeros aleatorios!\n" << endl;
        }
}

#endif // RANDOMNUMBERGENERATOR_H_INCLUDED
