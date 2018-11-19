/*
Author: Fabian Kainka


*/

#include "Arduino.h"
#include "NanoESP_HTTP.h"


NanoESP_HTTP::NanoESP_HTTP(NanoESP& nanoesp):m_nanoesp(&nanoesp){

}

//-------------------------------------------------New RESTapi & Webserver Functions-----------------------------------------

bool NanoESP_HTTP::sendFromFlash(int client, const char *website, int len) {
  
  if (m_nanoesp->sendCom(at+cip+"SEND=" + String(client) + "," + String(len), ">")) {
    for (int i = 0; i <= len; i++)
    {
      m_nanoesp->print(char(pgm_read_byte_near(website + i)));
    }
  }
  else {
    return false;
  }
  return m_nanoesp->find("OK\r\n");
}

bool NanoESP_HTTP::recvRequest(int &id, String &method, String &ressource, String &parameter) {
  if (m_nanoesp->available()) {
    if (m_nanoesp->findUntil("+IPD,", "\n") ) {
      id = m_nanoesp->parseInt();
      //debug(String(id));
      if (id >= 0) {
        m_nanoesp->findUntil(":", "\n");               //+IPD, 0, 407: GET / HTTP / 1.1

        method = m_nanoesp->readStringUntil(' ');
        String url = m_nanoesp->readStringUntil(' ');
        m_nanoesp->find("\r\n\r\n");

        int x = url.indexOf('?');     //192.168.178.41/?rgb=%2300ffff

        parameter = url.substring(x);
        ressource = url.substring(0, x);

        if (method == "POST") {
			//if (m_nanoesp->find("\r\n\r\n")) {
            m_nanoesp->setTimeout(300);
            parameter = m_nanoesp->readString(); //Until(' ');          //+IPD,0,13:rgb=%230000ff
            m_nanoesp->setTimeout(1000);
          
        }
      }
      return true;
    }
  }
  return false;
}


bool NanoESP_HTTP::recvHTTP(int id, int len, String &method, String &ressource, String &parameter) {
	if (len>=11){
	 if( m_nanoesp->findUntil(":", "\n")){               //+IPD, 0, 407: GET / HTTP / 1.1

        method = m_nanoesp->readStringUntil(' ');
        String url = m_nanoesp->readStringUntil(' ');
        m_nanoesp->find("\r\n\r\n");

        int x = url.indexOf('?');     //192.168.178.41/?rgb=%2300ffff

        parameter = url.substring(x);
        ressource = url.substring(0, x);

        if (method == "POST") {
			//if (m_nanoesp->find("\r\n\r\n")) {
            m_nanoesp->setTimeout(300);
            parameter = m_nanoesp->readString(); //Until(' ');          //+IPD,0,13:rgb=%230000ff
            m_nanoesp->setTimeout(1000);
          
        }
      return true;
	 }
	}
  return false;
}


bool NanoESP_HTTP::sendStreamHeader(int connectionId) {
  this->sendFromFlash(connectionId, serverStreamResponse, sizeof(serverStreamResponse) - 1);
}

bool NanoESP_HTTP::sendRequest(int id, char method[5], const String& address, const String& parameter) {
  String host = address.substring(0, address.indexOf('/'));
  String ressource = address.substring(address.indexOf('/'));

  if ((method == "GET") && (parameter != "")) {
    ressource = ressource + "?" + parameter;
  }

  String getRequest = String(method) + " " + ressource + " HTTP/1.1\r\nHost:" + host + "\r\n";

  if ((method == "POST") && (parameter != "")) {
    getRequest = getRequest + "Content-Length: " + String(parameter.length()) + "\r\n\r\n";
    getRequest = getRequest + parameter;
  }
  else {
    getRequest = getRequest + "\r\n";
  }

  if (m_nanoesp->newConnection(id, TCP, host, 80)) {
		return m_nanoesp->sendData(id, getRequest);
  }
  return false;
}

bool NanoESP_HTTP::sendRequest(int id, char method[5], const String& address) {
  return sendRequest( id,  method,  address, "");
}











