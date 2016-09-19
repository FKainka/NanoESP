
/*
  NanoESP.c - Library for the IoT WiFi-Board NanoESP (aka Pretzelboard)
  Created by Fabian Kainka, f.kainka@gmx.de, 29.09.2015
  www.iot.fkainka.de
  
  Updates: 30.03.2016:
  timeout WLAN connect higer (30s)
  
  Update 14.04.2016:
  wifiConnected() //test if Wifi is connected (by checking IP != 0.0.0.0)
*/

#include "NanoESP.h"

NanoESP::NanoESP():
  SoftwareSerial(11, 12)
{
}

//----------------------------------Basics-----------------------------------------
char ok[] = "OK\r\n";

boolean NanoESP::init(boolean vDebug)
{
  debug = vDebug;	
  boolean success = true;
  this->begin(19200);
  success &= reset();
  success &= setTransferMode() ;
  success &= setMultipleConnections();
  return success;
}

boolean NanoESP::setMultipleConnections() {
  boolean success = true;
  success &= sendCom(at+cip+"MUX=1", ok); //Allways multiple connections
  return success;
}

boolean NanoESP::setTransferMode() {
  boolean success = true;
  success &= sendCom(at+cip+"MODE=0", ok); //Normal trasfer mode
  return success;
}

boolean NanoESP::reset() {
  boolean success = true;
  this->setTimeout(5000);
  success &= sendCom(at+"RST", "ready");
  this->setTimeout(1000);
  return success;
}

boolean NanoESP::sendCom(String command, char respond[])
{	
  this->println(command);
  //if (debug){Serial.println(command);}
  if (this->findUntil(respond, "ERROR"))
  {
    return true;
  }
  else
  {
	if(debug){Serial.println ("Err: " + command);}
    //debug("ESP SEND ERROR: " + command);
    return false;
  }
}

String NanoESP::sendCom(String command)
{
  this->println(command);
  return this->readString();
}

//----------------------------------WiFi-Functions-----------------------------------------

boolean NanoESP::configWifi(int modus, String ssid, String password) 
{
  boolean success = true;
  switch (modus)
  {
    case 1:
      success &= configWifiMode(1);
      success &= configWifiStation(ssid, password);
      return success;
      break;

    case 2:
      success &= configWifiMode(2);
      success &= configWifiAP(ssid, password);
      return success;
      break;

    case 3:
      success &= configWifiMode(3);
      success &= configWifiStation(ssid, password);
      return success;
      break;
  }
}

boolean NanoESP::configWifiMode(int modus)
{
  return (sendCom(at+cw+"MODE=" + String(modus), ok));
}


boolean NanoESP::configWifiStation(String ssid, String password)
{
  boolean success = true;
  this->setTimeout(30000);
  success &= (sendCom(at+cw+"JAP=\"" + String(ssid) + "\",\"" + String(password) + "\"", ok));
  this->setTimeout(1000);
  return success;
}

boolean NanoESP::configWifiAP( String ssid, String password)
{
  boolean success = true;
  this->setTimeout(20000);
  if (password == "") success &= (sendCom(at+cw+"SAP=\"" + String(ssid) + "\",\"\",5,0", ok));
  else  success &= (sendCom(at+cw+"SAP=\"" + String(ssid) + "\",\"" + String(password) + "\",5,4", ok));
  this->setTimeout(1000);
  return success;
}


boolean NanoESP::configWifiAP( String ssid, String password, int channel, int crypt)
{
  boolean success = true;
  this->setTimeout(20000);
  success &= (sendCom(at+cw+"SAP=\"" + String(ssid) + "\",\"" + String(password) + "\"," + String(channel) + "," + String(crypt), ok));
  this->setTimeout(1000);
  return success;
}

boolean NanoESP::disconnectWifi()
{
  boolean success = true;
  success &= (sendCom(at+cw+"WAP", ok));
  return success;
}

String NanoESP::getIp()
{
  return sendCom(at+"CIFSR");
}


//----------------------------------Connection-Functions-----------------------------------------

boolean NanoESP::newConnection(int id, String type, String ip ,unsigned int port) {  //single direction
  boolean success = true;
  this->setTimeout(10000);
  success &= sendCom(at+cip+"START=" + String(id) + ",\"" + String(type) + "\",\"" + String(ip) + "\"," + String(port), ok); 
  this->setTimeout(1000);
  return success;
}

boolean NanoESP::closeConnection(int id) {
  boolean success = true;
  success &= sendCom(at+cip+"CLOSE=" + String(id), ok);
  return success;
}

boolean NanoESP::startUdpServer(int id, String ip ,unsigned int port,unsigned int  recvport, int mode) { //dual direction
  boolean success = true;
  success &= sendCom(at+cip+"START=" + String(id) + ",\"UDP\",\"" + String(ip) + "\"," + String(port) + "," + String(recvport) + "," + String(mode), ok);
  return success;
}

boolean NanoESP::endUdpServer(int id) {
  return closeConnection(id);
}

boolean NanoESP::startTcpServer(unsigned int port) {
  boolean success = true;
  success &= sendCom(at+cip+"SERVER=1," + String(port), ok);
  return success;
}

boolean NanoESP::endTcpServer() {
  boolean success = true;
  success &= sendCom(at+cip+"SERVER=0", ok);
  success &= init(); //Restart?!?
  return success;
}

boolean NanoESP::sendData(int id, String msg) {
  boolean success = true;

  success &= sendCom(at+cip+"SEND=" + String(id) + "," + String(msg.length() + 2), ">");
  if (success)
  {
    success &= sendCom(msg, ok);
  }
  return success;
}

bool NanoESP::sendRaw(int id, unsigned char data[], int LenChar) {
  if(this->sendCom(at+cip+"SEND=" + String(id) + "," + String(LenChar)), ">"){
	for (int i = 0; i < LenChar; i++) {
      this->write(data[i]);
    }
    if  (this->find("OK\r\n")) return true;
	return false;
  }
}

boolean NanoESP::sendDataClose(int id, String msg) {
  boolean success = true;

  success &= sendCom(at+cip+"SEND=" + String(id) + "," + String(msg.length() + 2), ">");
  if (success)
  {
    success &= sendCom(msg, ok);
    success &= closeConnection(id);
  }
  return success;
}


int NanoESP::getId() { 
  if (this->available()) // check if the esp is sending a message
  {
    if (this->find("+IPD,"))
    {
      return this->parseInt();
    }
  }
  else return -1;
}

 int NanoESP::ping(String address) {
// boolean success = true;
  this->println(at+"PING=\"" + String(address)+"\"");
	this->find("+");
	int time = this->parseInt();
	this->find(ok);
  return time;
}

//New
bool NanoESP::wifiConnected() 
{
  int i = 20; //counter, max 30 tries (arround 30+ s)
  String ip = "0.0.0.0";
  String mac; 

  while (ip == "0.0.0.0") {
	getIpMac(ip,mac);
	delay(500);
	i--;
    if (i <= 0) return false;
  }
  return true;
}

//New
bool NanoESP::recvData(int &id,int &len) 
{
	if (this->available()) {
		if (this->findUntil("+IPD,", "\n") ) {
			id = this->parseInt();
			if (id >= 0) {
				this->find(",");
				len = this->parseInt();
				//this->find(":"); DONT!
				return true;
			}
		}
	}
  return false;
}

//New
bool NanoESP::getIpMac(String &ip, String &mac) { //LiB??? anstelle mit connected??
  this->println(F("AT+CIFSR"));
  if (this->find("+CIFSR:STAIP,\"")) ip = this->readStringUntil('\"');
  if (this->find("+CIFSR:STAMAC,\"")) mac = this->readStringUntil('\"');
  if (this->find(ok)) return true;
  return false; 
}

//bool sendNTPpacket(int id, String address);
//long getNTPpacket(int id, String address, int timeZone);
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message


//-------------------------------------------------Debug Functions------------------------------------------------------
void NanoESP::serialDebug() {
  while (true)
  {
    if (this->available())
      Serial.write(this->read());
    if (Serial.available())
      this->write(Serial.read());
  }
}
