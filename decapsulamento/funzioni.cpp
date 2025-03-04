#include "funzioni.h"
#include <iostream>   
#include <string> 
#include <fstream>   
#include <bitset>
using namespace std;  

IpHeader header;
ethernetHeader frameHeader;

// Funzione per leggere un messaggio da un file
string lettura(){ 
    ifstream fin("msg.txt");
    if(!fin){
        cerr << "Errore nell'apertura del file in lettura" << endl;
        return ""; // Ritorna stringa vuota in caso di errore
    }
    string messaggio;
    getline(fin, messaggio); // Legge tutta la riga
    fin.close();
    return messaggio;
}


// Funzione per scrivere un messaggio in un file
void scrittura(string frameBinario){
    ofstream fout("frame.txt"); // Apertura del file "frame.txt" in modalità scrittura

    if(!fout){ // Controllo se il file è stato aperto correttamente
        cout<<"Errore nell'apertura del file in scrittura"<<endl;
    }

    fout<<frameBinario; // Scrittura del messaggio nel file
    fout.close(); // Chiusura del file
}

// Funzione per creare un datagramma IP 
void creaDatagram(IpHeader &header){
    header.version = "0100";                                                 //Versione del protocollo IP
    header.HLEN = "0101";                                                    //Lunghezza dell'header in parole
    header.TOS = "01100110";                                                 //Type of service
    header.totalLength = "0000000000111100";                                 //Lunghezza totale del datagramma
    header.identification = "1101010011110001";                              //Numero di identificazione
    header.flag = "010";
    header.fragmentOffset = "0000000000000";
    header.TTL = "01000000";                                                 //Time to live
    header.protocol = "00000110";                                            //Protocollo del livello superiore
    header.headerChecksum = "0000000000000000";                              //Checksum impostato a tutti 0
    header.sourceAddress = "11000000101010000000000100001010";               //Indirizzo IP del mittente
    header.destinationAddress = "00001000000010000000000000001000";          //Indirizzo IP del destinatario
    header.data = stringToBin(lettura());                          //Contenuto del messaggio in binario
    header.headerChecksum = ChecksumIp(header);
}

string datagramInBinario(const IpHeader &header){
    return header.version +
           header.HLEN +
           header.TOS +
           header.totalLength +
           header.identification +
           header.flag +
           header.fragmentOffset +
           header.TTL +
           header.protocol +
           header.headerChecksum +
           header.sourceAddress +
           header.destinationAddress +
           header.data;
}



void creaFrame(ethernetHeader &header, const IpHeader &headerIP){
    header.preamble = "10101010101010101010101010101010101010101010101010101010";
    header.SFD = "11010101";
    header.destinationAddress = "101011111010111110101111101011111010111110101111";
    header.sourceAddress = "101011001010111110101101101011001010011010101110";
    header.type = "1000000000000000";
    header.ipHeader = headerIP;
    header.FCS = calcolaCRC(frameInBinario(header));                         
}


string frameInBinario(const ethernetHeader &header){
    return header.preamble +
           header.SFD +
           header.destinationAddress +
           header.sourceAddress +
           header.type +
           datagramInBinario(header.ipHeader) +
           header.FCS;
}

// Funzione per convertire una stringa in una stringa binaria
string stringToBin(string testo){
    string risultato; // Stringa per memorizzare la rappresentazione binaria della stringa
    for(char c : testo){ // Itera su ogni carattere della stringa
        for(int i = 7; i >= 0; i--){ // Ciclo per esaminare ogni bit del carattere (8 bit)
            if(c & (1 << i)){ // Controllo se il bit i-esimo è impostato a 1
                risultato += '1'; // Se sì, aggiungi '1' alla stringa binaria
            }else{
                risultato += '0'; // Altrimenti, aggiungi '0'
            }
        }
    }
    return risultato; // Restituisce la stringa binaria
}

string calcolaCRC(const string &messaggio) {  
    unsigned short crc = 0xFFFF;
    const unsigned short POLY = 0x1021;
    for (unsigned char c : messaggio) {
        crc ^= (c << 8);
        for (int i = 0; i < 8; i++) {
            if (crc & 0x8000) crc = (crc << 1) ^ POLY;
            else crc <<= 1;
        }
    }
    char crcStr[5];
    snprintf(crcStr, sizeof(crcStr), "%04X", crc);
    return string(crcStr);
}


string ChecksumIp(const IpHeader &header) {
    string headerBits = header.version + header.HLEN + header.TOS +
                        header.totalLength + header.identification +
                        header.flag + header.fragmentOffset +
                        header.TTL + header.protocol +
                        header.sourceAddress + header.destinationAddress;

    while (headerBits.length() % 16 != 0) {
        headerBits += "0"; // Padding per rendere la lunghezza multiplo di 16
    }

    unsigned int somma = 0;
    for (size_t i = 0; i < headerBits.length(); i += 16) {
        unsigned int blocco = stoi(headerBits.substr(i, 16), nullptr, 2);
        somma += blocco;
    }

    while (somma > 0xFFFF) {
        somma = (somma & 0xFFFF) + (somma >> 16);
    }

    somma = ~somma & 0xFFFF;
    return bitset<16>(somma).to_string();
}

void frame(){
	string messaggio = lettura();
	 if (messaggio.empty()){
        cerr << "Errore: messaggio vuoto, impossibile generare il frame." << endl;
        return;
    }
	cout<<"Messaggio letto: "<<messaggio<<endl;
    creaDatagram(header);
	creaFrame(frameHeader, header);
	string frameBinario = frameInBinario(frameHeader);
	scrittura(frameBinario);
	cout<<"Frame scritto con successo sul file frame.txt"<<endl;
} 