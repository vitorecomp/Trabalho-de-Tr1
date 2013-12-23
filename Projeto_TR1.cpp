#include <iostream>
#include <string.h>
#include <cstdlib>
#include <vector>
#include <cmath>
// #define _OPEN_SYS
// #include <signal.h>
// #include <stdio.h>
#include "Projeto_TR1.h"


using namespace std;

// void sigalrm_handler(int sig);

int main ()
{
	int protocolo;
	//vector<int> dados;
	bool encerrar = false;

	//Variaveis temporarias
	int tamanho;
	char *temp = NULL;

	do
	{
		Sender *sender = new Sender ();
		Receiver *receiver = new Receiver ();
		Simulador *simulador = new Simulador ();

		cout << "Selecione o protocolo que deseja utilizar:" << endl << "1) Stop and Wait ARQ" << endl << "2) GoBack-N ARQ" << endl << "3) Selective Repeat ARQ" << endl << "Protocolo (para sair, digite '-1'): ";
		cin >> protocolo;

		switch (protocolo)
		{
			case 1:
				cout << "===============Protocolo Stop and Wait ARQ===============" << endl << endl;

				sender->getDados ();

				cout << "Dados para serem transmitidos: ";

				sender->showDados ();
				cout << endl;

				simulador->transmitir_SNW (sender, receiver);

				cout << endl << endl;

				break;
			case 2:
				cout << "===============GoBack-N ARQ===============" << endl << endl;

				sender->getDados ();

				cout << "Dados para serem transmitidos: ";

				sender->showDados ();
				cout << endl;

				simulador->transmitir_GBN (sender, receiver);

				cout << endl << endl;

				break;
			case 3:
				cout << "===============Selective Repeat ARQ===============" << endl << endl;

				sender->getDados ();

				cout << "Dados para serem transmitidos: ";

				sender->showDados ();
				cout << endl;

				simulador->transmitir_SR (sender, receiver);

				cout << endl << endl;

				break;
			case -1:
				encerrar = true;
				break;
			default:
				cout << "Opcao invalida!" << endl << endl;
				break;
		}

		delete (sender);
		delete (receiver);
		delete (simulador);
	}
	while (encerrar == false);

	return 0;
}

/* Sender */
void Sender::getDados ()
{
	int valor;

	for (int i = 0; i < quantidade_de_bits_gerados; i++)
	{
		valor  = (rand()%2);
    	
    	dados.push_back(valor);
	}
}

void Sender::showDados ()
{
	for (int i = 0; i < quantidade_de_bits_gerados; i++)
	{
		cout << dados[i];
	}
}

Pacote * Sender::getDadosPacote ()
{
	Pacote *pacote = (Pacote *) malloc ((bits_em_um_pacote) * sizeof (Pacote));

	for (int i = 0; i < bits_em_um_pacote; i++)
	{
		pacote->pkg[i] = dados[i];
	}

	//Retira os dados do vetor
	dados.erase (dados.begin (), dados.begin () + bits_em_um_pacote);

	return pacote;
}

/* Receiver */
int Receiver::receberPacote (Pacote pacote)
{
	//Checa se o pacote é repetido
	for (int i = 0; i < dadosRecebidos.size (); i++)
	{
		if (dadosRecebidos[i].id == pacote.id)
		{
			cout << "Pacote numero " << pacote.id << " (dados: ";
			for (int j = 0; j < bits_em_um_pacote; j++)
			{
				cout << pacote.pkg[j];
			}
			cout << ") foi descartado, pois ele eh redundante." << endl;

			return 1; //Indica que aquele pacote já havia sido recebido com sucesso, permitindo o envio do próximo e evitando a duplicação dele no receptor.
		}
	}

	//Checa se é o pacote esperado
	if (dadosRecebidos.size () != pacote.id)
	{
		cout << "Pacote numero " << pacote.id << " (dados: ";
		for (int i = 0; i < bits_em_um_pacote; i++)
		{
			cout << pacote.pkg[i];
		}
		cout << ") foi descartado por estar fora da ordem esperada e sera reenviado no futuro." << endl;

		return 2;
	}

	#ifdef ERRO
		int resultado = rand ()%100; //A primeira parte dos possiveis valores indica erro; o resto, que não há erro.

		if (resultado >= 0 && resultado < (int) 100*prob_de_erro)
		{
			int tipo_de_erro = rand ()%2;

			//Quadro recebido com erros
			if (tipo_de_erro == 0)
			{
				cout << "Pacote numero " << pacote.id << " (dados: ";
				for (int i = 0; i < bits_em_um_pacote; i++)
				{
					cout << pacote.pkg[i];
				}
				cout << ") sera reenviado assim que possivel, pois o Receiver retornou um NACK." << endl;

				return 2;
			}
			//Quadro não recebido, ou seja, o timeout será atingido
			else
			{
				cout << "Pacote numero " << pacote.id << " (dados: ";
				for (int i = 0; i < bits_em_um_pacote; i++)
				{
					cout << pacote.pkg[i];
				}
				cout << ") sera reenviado assim que possivel, pois o timeout foi atingido." << endl;

				return 3;
			}
		}
		else
		{
			dadosRecebidos.push_back(pacote);

			cout << "Pacote numero " << pacote.id << " (dados: ";
			for (int i = 0; i < bits_em_um_pacote; i++)
			{
				cout << pacote.pkg[i];
			}
			cout << ") enviado com sucesso! O Receiver retornou um ACK." << endl;

			return 1;
		}
	#else
		dadosRecebidos.push_back(pacote);

		cout << "Pacote numero " << pacote.id << " (dados: ";
		for (int i = 0; i < bits_em_um_pacote; i++)
		{
			cout << pacote.pkg[i];
		}
		cout << ") enviado com sucesso! O Receiver retornou um ACK." << endl;

		return 1;
	#endif
}

void Receiver::showDadosRecebidos ()
{
	for (int i = 0; i < dadosRecebidos.size (); i++)
	{
		for (int j = 0; j < bits_em_um_pacote; j++)
		{
			cout << dadosRecebidos[i].pkg[j];
		}
	}
}

int Receiver::receberPacote_SR (Pacote pacote)
{
	//Checa se o pacote é repetido
	for (int i = 0; i < dadosRecebidos.size (); i++)
	{
		if (dadosRecebidos[i].id == pacote.id)
		{
			cout << "Pacote numero " << pacote.id << " (dados: ";
			for (int j = 0; j < bits_em_um_pacote; j++)
			{
				cout << pacote.pkg[j];
			}
			cout << ") foi descartado, pois ele eh redundante." << endl;

			return 1; //Indica que aquele pacote já havia sido recebido com sucesso, permitindo o envio do próximo e evitando a duplicação dele no receptor.
		}
	}

	//Checa se é o pacote esperado
	if (dadosRecebidos.size () != pacote.id)
	{
		cout << "Pacote numero " << pacote.id << " (dados: ";
		for (int i = 0; i < bits_em_um_pacote; i++)
		{
			cout << pacote.pkg[i];
		}
		cout << ") foi descartado por estar fora da ordem esperada e sera reenviado no futuro." << endl;

		return 2;
	}

	#ifdef ERRO
		int resultado = rand ()%100; //A primeira parte dos possiveis valores indica erro; o resto, que não há erro.

		if (resultado >= 0 && resultado < (int) 100*prob_de_erro)
		{
			int tipo_de_erro = rand ()%2;

			//Quadro recebido com erros
			if (tipo_de_erro == 0)
			{
				cout << "Pacote numero " << pacote.id << " (dados: ";
				for (int i = 0; i < bits_em_um_pacote; i++)
				{
					cout << pacote.pkg[i];
				}
				cout << ") sera reenviado assim que possivel, pois o Receiver retornou um NACK." << endl;

				return 2;
			}
			//Quadro não recebido, ou seja, o timeout será atingido
			else
			{
				cout << "Pacote numero " << pacote.id << " (dados: ";
				for (int i = 0; i < bits_em_um_pacote; i++)
				{
					cout << pacote.pkg[i];
				}
				cout << ") sera reenviado assim que possivel, pois o timeout foi atingido." << endl;

				return 3;
			}
		}
		else
		{
			dadosRecebidos.push_back(pacote);

			cout << "Pacote numero " << pacote.id << " (dados: ";
			for (int i = 0; i < bits_em_um_pacote; i++)
			{
				cout << pacote.pkg[i];
			}
			cout << ") enviado com sucesso! O Receiver retornou um ACK." << endl;

			return 1;
		}
	#else
		dadosRecebidos.push_back(pacote);

		cout << "Pacote numero " << pacote.id << " (dados: ";
		for (int i = 0; i < bits_em_um_pacote; i++)
		{
			cout << pacote.pkg[i];
		}
		cout << ") enviado com sucesso! O Receiver retornou um ACK." << endl;

		return 1;
	#endif
}

/* Simulador */
void Simulador::transmitir_SNW (Sender *sender, Receiver *receiver)
{
	int resultado;
	int total_tentativas = 0;
	unsigned int quantidade_pacotes = quantidade_de_bits_gerados/bits_em_um_pacote;

	cout << "Quantidade de pacotes: " << quantidade_pacotes << endl << endl;

	cout << "Pacotes montados e log de envios: " << endl;

	//Transmitindo os dados em pacotes
	for (int i = 0; i < quantidade_pacotes; i++)
	{
		//Primeiro, montamos o pacote
		Pacote pacote;

		pacote = *(sender->getDadosPacote ());
		pacote.id = i;

		do
		{
			//Enviando ao receptor
			resultado = receiver->receberPacote (pacote);

			//Independente de receber um ACK ou um NACK, o tempo transcorrido será o mesmo
			if (resultado == 1 || resultado == 2)
			{
				if (resultado == 2)
				{
					NACKs++;
				}
				else if (resultado == 1)
				{
					T_de_trabalho += bits_em_um_pacote*Ttrans + Tproc;
				}

				Ttotal += 2*(Tprop + Tproc) + bits_em_um_pacote*Ttrans + 2*Ttrans;
			}
			//Se o timeout é atingido, computa-se simplesmente o tempo do timeout
			else
			{
				Timeouts++;
				Ttotal += timeout + bits_em_um_pacote*Ttrans + Ttrans;
			}

			total_tentativas++;
		} while (resultado != 1);
	}

	cout <<"\n\nDados recebidos: ";

	receiver->showDadosRecebidos ();

	cout << endl;

	float porcentagem = (float) ((float)(NACKs + Timeouts)/total_tentativas)*100;
	float eficiencia = ((float) T_de_trabalho/Ttotal)*100;

	cout << "=====Resultados=====" << endl;
	#ifdef ERRO
		cout << "Com o parametro de probabilidade de erros com valor " << prob_de_erro*100 << "% obteve-se " << porcentagem << "% de erros no teste pratico." << endl;
	#endif
	cout << "Eficiencia obtida: " << eficiencia << "%" << endl; //Eficiencia é o tempo em que se trabalhou / tempo total gasto
	cout << "Erros por recebimento de NACKs: " << NACKs << endl;
	cout << "Timeouts: " << Timeouts << endl;
	cout << "Tempo total transcorrido: " << Ttotal << " ms." << endl;
}

void Simulador::transmitir_GBN (Sender *sender, Receiver *receiver)
{
	int total_tentativas = 0;
	unsigned int quantidade_pacotes = quantidade_de_bits_gerados/bits_em_um_pacote;
	Pacote pacote[quantidade_pacotes];

	cout << "Quantidade de pacotes: " << quantidade_pacotes << endl;
	cout << "Tamanho selecionado para a janela de envio: " << Ws << endl << endl;

	//Primeiro, vamos montar todos os pacotes
	for (int i = 0; i < quantidade_pacotes; i++)
	{
		pacote[i] = *(sender->getDadosPacote ());
		pacote[i].id = i;
	}

	// //Setando a interrupção
	// signal(SIGALRM, &sigalrm_handler);
	// alarm(2*Tprop + 2*Tproc + Ttrans);

	cout << "Pacotes montados e log de envios:" << endl;
	int id_prox_pacote;
	int j = -1;
	vector<int> resultado;
	vector<int> id_suporte;

	for (int i = 0; i < quantidade_pacotes; i++)
	{
		j++;

		id_suporte.push_back(pacote[i].id);
		resultado.push_back(receiver->receberPacote(pacote[i]));

		Ttotal += bits_em_um_pacote*(Ttrans + Tproc);

		//Quando a janela é exaurida, os ACKs são checados
		if (j == Ws - 1 || i == quantidade_pacotes - 1)
		{
			j = -1;
			bool sair = false;

			for (int z = 0; z < resultado.size () && sair == false; z++)
			{
				if (resultado[z] != 1)
				{
					id_prox_pacote = id_suporte[z];
					sair = true;
				}

				//Foi um timeout
				if (resultado[z] == 3)
				{
					Ttotal += timeout - (Ws - z)*Tproc; //Durante o tempo do timeout, ainda utilizamos 'z' vezes o canal para transmitir dados
				}
			}

			if (sair == true)
			{
				//Já que a variavel 'i' sera incrementada, decrementamos o seu valor para obter o valor correto
				id_prox_pacote--;
				i = id_prox_pacote;

				cout << "Reenviando a partir do pacote de numero " << i + 1 << " ..." << endl;
			}
			else
			{
				T_de_trabalho += 2*(bits_em_um_pacote)*(Ttrans + Tproc);
			}

			id_suporte.clear ();
			resultado.clear ();
		}
	}

	// alarm(0); //Desativa a interrupção por tempo

	Ttotal += timeout;

	cout <<"\n\nDados recebidos: ";

	receiver->showDadosRecebidos ();

	cout << endl;

	float porcentagem = (float) ((float)(NACKs + Timeouts)/total_tentativas)*100;
	float eficiencia = ((float) T_de_trabalho/Ttotal)*100;

	cout << "=====Resultados=====" << endl;
	#ifdef ERRO
		cout << "Com o parametro de probabilidade de erros com valor " << prob_de_erro*100 << "% obteve-se " << porcentagem << "% de erros no teste pratico." << endl;
	#endif
	cout << "Eficiencia obtida: " << eficiencia << "%" << endl; //Eficiencia é o tempo em que se trabalhou / tempo total gasto
	cout << "Erros por recebimento de NACKs: " << NACKs << endl;
	cout << "Timeouts: " << Timeouts << endl;
	cout << "Tempo total transcorrido: " << Ttotal << " ms." << endl;
}

void Simulador::transmitir_SR (Sender *sender, Receiver *receiver)
{
	int total_tentativas = 0;
	unsigned int quantidade_pacotes = quantidade_de_bits_gerados/bits_em_um_pacote;
	Pacote pacote[quantidade_pacotes];

	cout << "Quantidade de pacotes: " << quantidade_pacotes << endl;
	cout << "Tamanho selecionado para a janela de envio: " << Ws << endl << endl;

	//Primeiro, vamos montar todos os pacotes
	for (int i = 0; i < quantidade_pacotes; i++)
	{
		pacote[i] = *(sender->getDadosPacote ());
		pacote[i].id = i;
	}

	cout << "Pacotes montados e log de envios:" << endl;
	int id_prox_pacote;
	int j = -1;
	vector<int> resultado;
	vector<int> id_suporte;

	for (int i = 0; i < quantidade_pacotes; i++)
	{
		j++;

		id_suporte.push_back(pacote[i].id);
		resultado.push_back(receiver->receberPacote(pacote[i]));

		Ttotal += bits_em_um_pacote*(Ttrans + Tproc);

		//Quando a janela é exaurida, os ACKs são checados
		if (j == Ws - 1 || i == quantidade_pacotes - 1)
		{
			j = -1;
			bool sair = false;

			for (int z = 0; z < resultado.size () && sair == false; z++)
			{
				if (resultado[z] != 1)
				{
					id_prox_pacote = id_suporte[z];
					sair = true;
				}

				//Foi um timeout
				if (resultado[z] == 3)
				{
					Ttotal += timeout - (Ws - z)*Tproc; //Durante o tempo do timeout, ainda utilizamos 'z' vezes o canal para transmitir dados
				}
			}

			if (sair == true)
			{
				//Já que a variavel 'i' sera incrementada, decrementamos o seu valor para obter o valor correto
				id_prox_pacote--;
				i = id_prox_pacote;

				cout << "Reenviando a partir do pacote de numero " << i + 1 << " ..." << endl;
			}
			else
			{
				T_de_trabalho += 2*(bits_em_um_pacote)*(Ttrans + Tproc);
			}

			id_suporte.clear ();
			resultado.clear ();
		}
	}

	Ttotal += timeout;

	cout <<"\n\nDados recebidos: ";

	receiver->showDadosRecebidos ();

	cout << endl;

	float porcentagem = (float) ((float)(NACKs + Timeouts)/total_tentativas)*100;
	float eficiencia = ((float) T_de_trabalho/Ttotal)*100;

	cout << "=====Resultados=====" << endl;
	#ifdef ERRO
		cout << "Com o parametro de probabilidade de erros com valor " << prob_de_erro*100 << "% obteve-se " << porcentagem << "% de erros no teste pratico." << endl;
	#endif
	cout << "Eficiencia obtida: " << eficiencia << "%" << endl; //Eficiencia é o tempo em que se trabalhou / tempo total gasto
	cout << "Erros por recebimento de NACKs: " << NACKs << endl;
	cout << "Timeouts: " << Timeouts << endl;
	cout << "Tempo total transcorrido: " << Ttotal << " ms." << endl;
}

// /* Timer */
// void sigalrm_handler(int sig)
// {
// 	cout << "DEU UM TIMER!!!" << endl;
// 	alarm(2*Tprop + 2*Tproc + Ttrans);
// }