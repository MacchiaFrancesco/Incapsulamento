#ifndef FUNZIONI_H
#define FUNZIONI_H
#include <string>
#include <bitset>
#include <iostream>
using namespace std; //sconsigliato però non so farlo senza.

struct IpHeader{
    string version;
    string HLEN;
    string TOS;
    string totalLength;
    string identification;
    string flag;
    string fragmentOffset;
    string TTL;
    string protocol;
    string headerChecksum;
    string sourceAddress;
    string destinationAddress;
    string data;
};

struct ethernetHeader{
    string preamble;
    string SFD;
    string destinationAddress;
    string sourceAddress;
    string type;
    IpHeader ipHeader;              
    string FCS;
};

string lettura();
void scrittura(string frameBinario);	//prototipi delle funzioni che useremo
void creaDatagram(IpHeader &header);
string calcolaCRC(const string &messaggio);
string ChecksumIp(const IpHeader &header);
void frame();
void creaFrame(ethernetHeader &header, const IpHeader &headerIP);
string frameInBinario(const ethernetHeader &header);
string datagramInBinario(const IpHeader &header);
string stringToBin(string testo);

#endif