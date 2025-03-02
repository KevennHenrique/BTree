#ifndef BLOCKINTERLEAVING_H_INCLUDED
#define BLOCKINTERLEAVING_H_INCLUDED

#include <iostream>
#include <vector>
#include <fstream>
#include <queue>

using namespace std;

template <const unsigned int min_degree>
class blockInterleaving {
public:
    blockInterleaving() { this->blockSize = (2 * min_degree) - 1; }
    void intercalacaoPolifasica(const vector<string>& tempFiles, const string& outputFileName);

private:
    int blockSize;
};

template <const unsigned int min_degree>
void blockInterleaving<min_degree>::intercalacaoPolifasica(const vector<string>& tempFiles, const string& outputFileName) {
    vector<ifstream> files(tempFiles.size());

    for (size_t i = 0; i < tempFiles.size(); ++i) {
        files[i].open(tempFiles[i], ios::binary);
        if (!files[i]) {
            cout << "Erro ao abrir o arquivo temporario de nome: " << tempFiles[i] << endl;
            return;
        }
    }
    ofstream output(outputFileName, ios::binary);
    if (!output) {
        cout << "Erro ao abrir o arquivo de saída" << endl;
        return;
    }

    // Instancia uma fila de prioridade pq
    auto cmp = [](const pair<int, size_t>& a, const pair<int, size_t>& b) {
        return a.first > b.first;
    };
    priority_queue<pair<int, size_t>, vector<pair<int, size_t>>, decltype(cmp)> pq(cmp);

    vector<int> current_elements(files.size(), 0); // Armazena os elementos atuais lidos de cada arquivo
    vector<int> count(files.size(), 0);            // Conta quantos números foram extraídos de cada arquivo
    bool endFile = false;

    while (!endFile) {
        endFile = true; // Assume que chegou ao final de todos os arquivos, a menos que prove o contrário

        // Lê um elemento de cada arquivo se não atingiu o tamanho do bloco
        for (size_t i = 0; i < files.size(); ++i) {
            if (count[i] < this->blockSize && files[i].read(reinterpret_cast<char*>(&current_elements[i]), sizeof(int))) {
                pq.push({current_elements[i], i});
                count[i]++;
                endFile = false; // Ainda há números para processar
            }
        }

        // Intercala os elementos da fila
        while (!pq.empty()) {
            auto [value, file_index] = pq.top();
            pq.pop();

            output.write(reinterpret_cast<char*>(&value), sizeof(int));

            // Se ainda houver números a serem lidos do arquivo correspondente
            if (count[file_index] < this->blockSize && files[file_index].read(reinterpret_cast<char*>(&current_elements[file_index]), sizeof(int))) {
                pq.push({current_elements[file_index], file_index});
                count[file_index]++;
                endFile = false; // Continua o processamento
            }
        }

        // Reinicia a contagem para o próximo bloco
        fill(count.begin(), count.end(), 0);
    }

    for (ifstream& file : files) {
        file.close();
    }

    output.close();
    cout << "Intercalação concluída. Dados ordenados gravados no arquivo final." << endl;

    //#####################################################################
    this->blockSize = tempFiles.size() * this->blockSize;

    ifstream input(outputFileName, ios::binary);
    vector<ofstream> tempFilesOut(tempFiles.size());
    vector<int> block(this->blockSize);
    size_t toggleBlock = 0;

    // Abrir os arquivos temporários para escrita
    for (size_t i = 0; i < tempFiles.size(); ++i) {
        tempFilesOut[i].open(tempFiles[i], ios::out | ios::trunc | ios::binary);
        if (!tempFilesOut[i]) {
            cout << "Erro ao abrir o arquivo temporário para escrita: " << tempFiles[i] << endl;
            return;
        }
    }
    // Ler o arquivo de saída e dividir em blocos
    while (true) {
        input.read(reinterpret_cast<char*>(&block[0]), this->blockSize * sizeof(int));
        size_t bytesRead = input.gcount(); // Quantidade de bytes efetivamente lidos

        if (bytesRead == 0) {
            break; // Fim do arquivo
        }

        size_t elementsRead = bytesRead / sizeof(int); // Número de inteiros lidos
        size_t fileIndex = toggleBlock % tempFiles.size();

        tempFilesOut[fileIndex].write(reinterpret_cast<char*>(&block[0]), bytesRead);
        toggleBlock++;

        // Se leu menos do que o tamanho completo do bloco, significa que terminou
        if (bytesRead < this->blockSize * sizeof(int)) {
            break;
        }
    }

    for (size_t i = 0; i < tempFiles.size(); ++i) {
        tempFilesOut[i].close();
    }

    input.close();

    bool continueIntercalacao = false; // Evite chamada recursiva desnecessária

    // Verifique se é possível dividir o arquivo de saída novamente
    ifstream checkFile(outputFileName, ios::binary);
    checkFile.seekg(0, ios::end);
    size_t fileSize = checkFile.tellg();
    checkFile.close();

    // Se o tamanho do arquivo for maior que o tamanho do bloco, continue a intercalação
    continueIntercalacao = (fileSize > this->blockSize * sizeof(int));

    if(continueIntercalacao){
        intercalacaoPolifasica(tempFiles, outputFileName);
    }


    cout << "Intercalação concluída. Dados ordenados gravados no arquivo final." << endl;
}

#endif // BLOCKINTERLEAVING_H_INCLUDED

