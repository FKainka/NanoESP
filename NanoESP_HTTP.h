/*



*/
#include "Arduino.h"
#include <NanoESP.h>
#include "NanoESP_Flash.h"

#ifndef NanoESP_HTTP_H
#define NanoESP_HTTP_H

class NanoESP_HTTP { 
  public:
  
    NanoESP_HTTP(NanoESP& nanoesp);
    //NanoESP* getNanoESP() {return m_nanoesp;}
    
	bool sendFromFlash(int client, const char *website, int len);
	bool recvRequest(int &id, String &method, String &ressource, String &parameter);   //Müsste recvHTTP heißen //OBSOLET?!?
	bool sendStreamHeader(int connectionId);		
	bool sendRequest(int id, char method[5], const String& address, const String& parameter);
	bool sendRequest(int id, char method[5], const String& address);
	
	//New
	bool recvHTTP(int id, int len, String &method, String &ressource, String &parameter);

  private:
    NanoESP* m_nanoesp;
	
	String at = "AT+";
	String cip = "CIP";
	String cw = "CW";
	
};


#endif