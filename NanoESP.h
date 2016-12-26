/*
  NanoESP.h - Library for the IoT WiFi-Board NanoESP (aka Pretzelboard)
  Created by Fabian Kainka, f.kainka@gmx.de, 29.09.2015
  www.iot.fkainka.de
*/


#define STATION 1
#define ACCESSPOINT 2
#define DUAL 3

#define TCP "TCP"
#define UDP "UDP"

#define GET "GET"
#define POST "PSOT"


#ifndef _NANOESP_H_
#define _NANOESP_H_

#include "Arduino.h"
#include <SoftwareSerial.h>

class NanoESP : public SoftwareSerial {
  public:
    NanoESP() ; 
    boolean init(boolean vDebug=false);
    boolean sendCom(const String& command, char respond[]);
    String sendCom(const String& command);
    boolean setMultipleConnections();
    boolean setTransferMode() ;
    boolean reset();

    boolean configWifi(int modus, const String& ssid, const String& password);
    boolean configWifiMode(int modus);
    boolean configWifiStation(const String& ssid, const String& password);
    boolean configWifiAP(const String& ssid, const String& password);
    boolean configWifiAP(const String& ssid, const String& password, int channel, int crypt);
    boolean disconnectWifi();
    String getIp();

    boolean newConnection(int id, const String& type, const String& ip , unsigned int port);
    boolean closeConnection(int id) ;
    boolean startUdpServer(int id, const String& ip , unsigned int port, unsigned int recvport, int mode=0);
    boolean endUdpServer(int id);
    boolean startTcpServer(unsigned int port) ;
    boolean endTcpServer();
    boolean sendData(int id, const String& msg);
    boolean sendDataClose(int id, const String& msg);
    int getId(); 		//Obsolet? !!!
    //int ping(String adress); 

	
	//----NEW----14.04.2016
	bool wifiConnected();	//changed 19.04.2016
	bool recvData(int &id,int &len);
	
	//----NEW----19.04.2016
	bool getIpMac(String &ip, String &mac);
	//----NEW----21.04.2016
	bool sendRaw(int id, unsigned char data[], int LenChar);
	
	//----NEW----28.04.2016
	 int ping(const String& adress); 
	//bool sendNTPpacket(int id, String address);
	//long getNTPpacket(int id, String address, int timeZone);
	//----TEST------
	
    void serialDebug();    
	
  private :
	String at = "AT+";
	String cip = "CIP";
	String cw = "CW";
	boolean debug = false;
};

#endif
