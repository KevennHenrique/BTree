#include <iostream>
#include <clocale>
#include "header.h"
#include "record.h"
#include "typedfile.h"
#include "intserial.h"
#include "node.h"
#include "BTree.h"
#include "randomNumberGenerator.h"
#include "blockOrganization.h"
#include "blockInterleaving.h"

using namespace std;

constexpr unsigned int min_degree = 2;

int main()
{
    setlocale(LC_ALL, "Portuguese");

    int op, n;
    intSerial key;

    Btree<intSerial, min_degree> b("teste.dat", "TPG", 2);

    if (b.isOpen())
    {
        cout << "Arquivo aberto com sucesso!\n\n";

        do
        {
            cout << "\nSelecione uma opcao:\n\n"
                 << "1. Inserir uma chave\n"
                 << "2. Excluir uma chave\n"
                 << "3. Pesquisar uma chave\n"
                 << "4. Printar a arvore b\n"
                 << "5. Gerar, ordenar, gravar e intercalar (intercalacao polifasica)\n"
                 << "6. Ler nós de forma ordenada e intercalá-los\n"
                 << "7. Sair\n\n"
                 << "Sua opcao: ";
            cin >> op;
            switch (op)
            {
            case 1:{
                cout << "Digite um numero inteiro: ";
                cin >> n;
                key.setValue(n);
                b.bTreeInsert(key);
                cout << "Valor " << n << " inserido com sucesso.\n" << endl;
                break;
            }

            case 2:{
                cout << "Digite valor deseja remover do arquivo: ";
                cin >> n;
                key.setValue(n);
                b.bTreeDelete(key);
                break;
            }

            case 3:{
                cout << "Digite o valor a ser pesquisado: ";
                cin >> n;
                key.setValue(n);

                pair<node<intSerial, min_degree>,int> result;
                result = b.bTreeSearch(b.getRoot(), key);

                node<intSerial, min_degree> nodeResult = result.first;
                int indexResult = result.second;

                if (indexResult != -1) {
                    cout << "\nChave encontrada na árvore b!" << endl;
                    cout << "Valor da chave: " << nodeResult.getKeys(indexResult).getValue() <<"\n" <<endl;
                } else {
                    cout << "Chave não encontrada na árvore.\n" << endl;
                }
                break;
            }
            case 4:{
                cout << "Print da arvore b: " << endl;
                b.printTreeB();
                break;
            }
            case 5: {
                randomNumberGenerator<intSerial, min_degree> randonNumber;
                randonNumber.generateAndInsert("numerosAleatoriosOrdenados.dat");

                //Gera blocos de numeros aleatorios, ordena eles e insere nos arquivos temporarios
                blockOrganization<min_degree> blockOrg;
                blockOrg.sortAndWriteToTempFiles("numerosAleatoriosOrdenados.dat");

                //intercala blocos ordenados
                blockInterleaving<min_degree> interPolif;
                vector<string> temp_files = {"temp0.dat", "temp1.dat", "temp2.dat"};
                interPolif.intercalacaoPolifasica(temp_files, "sorted_output.dat");

                // Inserir dados na árvore B
                ifstream infile("sorted_output.dat", ios::binary);
                while (infile.read(reinterpret_cast<char*>(&n), sizeof(int))) {
                    key.setValue(n);
                    cout <<"Numero inserido: "<< n <<"\n";
                    b.bTreeInsert(key);
                }
                break;
            }
            case 6: {
                //Abre um arquivo binário para salvar os nós
                ofstream outFile("nodes_in_order.bin", ios::binary);
                if (!outFile.is_open()) {
                    cerr << "Erro ao abrir o arquivo para escrita!" << endl;

                    return 1;
                }
                b.writeNodesInOrderToBinaryFile(outFile);
                cout << "Nós escritos no arquivo binário 'nodes_in_order.bin'" << endl;
                outFile.close(); // Fecha o arquivo

                //Leitura dos nós in_order no arquivo
                ifstream inFileNodes("nodes_in_order.bin", ios::binary);
                if (!inFileNodes.is_open()) {
                    cerr << "Erro ao abrir o arquivo para leitura!" << endl;
                    return 1;
                }
                while(inFileNodes.read(reinterpret_cast<char*>(&n), sizeof(int))){
                    cout<<" "<< n << " ";
                }
                inFileNodes.close();

                //Gera blocos de numeros aleatorios, ordena eles e insere nos arquivos temporarios
                blockOrganization<min_degree> blockOrg;
                blockOrg.sortAndWriteToTempFiles("nodes_in_order.bin");

                //intercala blocos ordenados
                blockInterleaving<min_degree> interPolif;
                vector<string> temp_files = {"temp0.dat", "temp1.dat", "temp2.dat"};
                interPolif.intercalacaoPolifasica(temp_files, "sorted_output.dat");

                //leitura dos dados dos nós in_order
                ifstream inFile("sorted_output.dat", ios::binary);
                if (!inFile.is_open()) {
                    cerr << "Erro ao abrir o arquivo para leitura!" << endl;
                    return 1;
                }
                while(inFile.read(reinterpret_cast<char*>(&n), sizeof(int))){
                    cout<<" "<< n << " ";
                }
                inFile.close();
                break;
            }
            case 7:
                cout << "Encerrando o programa... ";
                b.close();
                cout << "concluido." << endl;
                return 0;
                break;
            default:
                cout << "opcao invalida! Tente novamente..." << endl;
                break;
            }
        }
        while (true);
    }
    else
    {
        cout << "Nao foi possivel abrir o arquivo!" << endl;
        return 1;
    }

    return 0;
}
