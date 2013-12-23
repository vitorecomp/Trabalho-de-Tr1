/* Define se erros serão simulados ou não */
#define ERRO

#ifdef ERRO
	/* Probabilidade de erro */
	//Valor de 0 até 1. Por favor, usar no máximo duas casas decimais.
	const float prob_de_erro = 0.30;
#endif

/* Tempos */
//Considere que os tempos estão em segundos
const int Ttrans = 1; //Tempo necessário para se colocar 1 bit na rede
const int Tprop = 2;
const int Tproc = 1;
const int timeout = 2*Tprop + 2*Ttrans + Tproc + 4;

/* Janela de transmissão para o transmissor */
const int m = 2;
const int Ws = pow (2, m)/2; //Potência de 2. Com 'm' bits, devemos escolhe Ws = 2^m/2 = 2^(m - 1)

/* Dados gerados e transmitidos */
/****************************************
* IMPORTANTE! Assume-se que a quantidade *
* de bits gerados é um multiplo inteiro  *
* da quantidade de bits em um pacote!    *
*****************************************/
const int bits_em_um_pacote = 5;
const int quantidade_de_bits_gerados = 100;


class Pacote
{
public:
	int pkg[bits_em_um_pacote];
	int id;
};

class Sender
{
public:
	/**
	* \brief Construtor.
	*
	*/
	Sender (){}

	/**
	* \brief Destrutor.
	*
	*/
	~Sender (){dados.clear ();}

	/**
	* \brief Gera bits de maneira aleatoria para serem transmitidos.
	*
	*/
	void getDados ();

	/**
	* \brief Imprime os bits que devem ser transmitidos.
	*
	*/
	void showDados ();

	/**
	* \brief Retorna um conjunto de bits do tamanho de um pacote, removendo-os dos dados a serem enviados.
	*
	* \return Retorna o pacote.
	*
	*/
	Pacote *getDadosPacote ();

private:
	std::vector<int> dados;

};

class Receiver
{
public:
	/**
	* \brief Construtor.
	*
	*/
	Receiver (){for (int i = 0; i < Ws; i++){pacotesEsperados[Ws] = i;}}

	/**
	* \brief Destrutor.
	*
	*/
	~Receiver (){dadosRecebidos.clear ();}

	/**
	* \brief Recebe e, se estiver tudo correto, armazena o pacote.
	*
	* \param pacote O pacote recebido.
	*
	* \return Retorna 1, se o pacote foi recebido sem erros (ACK); 2, se houve erro (NACK); 3, se for ocorrer um timeout (quadro não recebido).
	*
	*/
	int receberPacote (Pacote pacote);

	/**
	* \brief Imprime os bits recebidos.
	*
	*/
	void showDadosRecebidos ();

	/**
	* \brief Recebe e, se estiver tudo correto, armazena o pacote. Utilize este método para utilizar o protocolo Selective Repeat ARQ. É igual ao receberPacote, porém com adição de um vetor que representa a janela de recepção.
	*
	* \param pacote O pacote recebido.
	*
	* \return Retorna 1, se o pacote foi recebido sem erros (ACK); 2, se houve erro (NACK); 3, se for ocorrer um timeout (quadro não recebido).
	*
	*/
	int receberPacote_SR (Pacote pacote);

private:
	std::vector<Pacote> dadosRecebidos;
	int pacotesEsperados[2]; //Wr = Ws
};

class Simulador
{
public:
	/**
	* \brief Construtor.
	*
	*/
	Simulador () {Ttotal = 0; T_de_trabalho = 0; NACKs = 0; Timeouts = 0;}

	/**
	* \brief Realiza a transmissao dos dados entre dois nós de utilizando o protocolo Stop and Wait ARQ.
	*
	* \param sender Nó que envia dados.
	* \param receiver Nó que recebe dados.
	* 
	*/
	void transmitir_SNW (Sender *sender, Receiver *receiver);

	/**
	* \brief Realiza a transmissao dos dados entre dois nós de utilizando o protocolo GoBack-N ARQ.
	*
	* \param sender Nó que envia dados.
	* \param receiver Nó que recebe dados.
	* 
	*/
	void transmitir_GBN (Sender *sender, Receiver *receiver);

	/**
	* \brief Realiza a transmissao dos dados entre dois nós de utilizando o protocolo Selective Repeat ARQ.
	*
	* \param sender Nó que envia dados.
	* \param receiver Nó que recebe dados.
	* 
	*/
	void transmitir_SR (Sender *sender, Receiver *receiver);


private:
	unsigned int Ttotal;
	unsigned int T_de_trabalho;
	unsigned int NACKs;
	unsigned int Timeouts;
};