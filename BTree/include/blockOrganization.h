#ifndef BLOCKORGANIZATION_H_INCLUDED
#define BLOCKORGANIZATION_H_INCLUDED

#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;

template <const unsigned int min_degree>
class blockOrganization{
private:
    int blockSize;

public:
    blockOrganization();

    void sortAndWriteToTempFiles(const string & inputFileName);

};

template <const unsigned int min_degree>
blockOrganization<min_degree>::blockOrganization(){
    this->blockSize = (2 * min_degree) - 1;
}

template <const unsigned int min_degree>
void blockOrganization<min_degree>::sortAndWriteToTempFiles(const string & inputFileName){
    ifstream inputFile(inputFileName, ios::binary); //abre arquivo em modo leitura
    if(!inputFile){
        cout << "\nErro ao abrir o arquivo "<< inputFileName <<"\n";
        cout << "Provavelmente o arquivo com este nome nao existe.\n";
        return;
    }
    vector<int> block(blockSize);
    int toggleBlock = 0;

    ofstream outputFiles[3];

    outputFiles[0].open("temp0.dat", ios::binary);
    outputFiles[1].open("temp1.dat", ios::binary);
    outputFiles[2].open("temp2.dat", ios::binary);

    for (int i = 0; i < 3; ++i) {
        if (!outputFiles[i]) {
            cout << "Erro ao abrir o arquivo temporário temp" << (i) << ".dat\n";
            return;
        }
    }
    //lê um bloco
    while(inputFile.read(reinterpret_cast<char*>(&block[0]), blockSize * sizeof(int))){
        sort(block.begin(), block.end());

        //escreve o bloco lido no arquivo temporario
        outputFiles[toggleBlock % 3].write(reinterpret_cast<char*>(&block[0]), blockSize * sizeof(int));
        toggleBlock++;

    }

    //fecho os arquivos temporario
    for (int i = 0; i < 3; ++i) {
        outputFiles[i].close();
    }

    inputFile.close();
    cout << "Blocos ordenados e gravados nos arquivos temporários." << endl;
}


#endif // BLOCKORGANIZATION_H_INCLUDED
