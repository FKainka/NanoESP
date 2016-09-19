/*



*/
#include "Arduino.h"
#include <NanoESP.h>

#ifndef NanoESP_MQTT_H
#define NanoESP_MQTT_H


  typedef struct{
	String topic;
	String value;
	byte qos;
	bool retain;
  } mqtt_msg;
 
 typedef struct {
  String topic;
  void (*g)(String value);
} mqtt_event;

class NanoESP_MQTT { 

  public:
	
    NanoESP_MQTT(NanoESP& nanoesp);
    //NanoESP* getNanoESP() {return m_nanoesp;}
    
    bool disconnect(int id) ;
   
    bool recvMQTT(int &id, String &topic, String &value) ;  //Obsolet!
    bool subscribe(int id, String topic) ;
    bool unsubscribe(int id, String topic) ;
    bool ping(int id);
    bool send(int id, unsigned char data[], int LenChar);
    bool send(int id, unsigned char data[], int LenChar, char ack);
	
	//New
	bool recvMQTT(int id, int len, String &topic, String &value);
	void stayConnected(int id, long interval);
    void stayConnected(int id);
		
	void utf8(String input,unsigned char* output);
    void charAdd(unsigned char* inputA, int lenA,unsigned char* inputB, int lenB, unsigned char* output);
	
	bool connect(int id, String broker, unsigned int port, String deviceId, bool cleanSession, byte keepAliveTime, mqtt_msg * lastWill , String userName , String password);
	bool connect(int id, String broker, unsigned int port, String deviceId);
	bool connect(int id, String broker, unsigned int port, String deviceId, mqtt_msg * lastWill);
	bool connect(int id, String broker, unsigned int port, String deviceId, bool cleanSession, byte keepAliveTime, mqtt_msg * lastWill);
	bool connect(int id, String broker, unsigned int port, String deviceId, String userName , String password);
	bool connect(int id, String broker, unsigned int port, String deviceId, bool cleanSession, byte keepAliveTime);
	bool connect(int id, String broker, unsigned int port, String deviceId, mqtt_msg * lastWill , String userName , String password);
	//static mqtt_msg theLastWillDefault = {"", "", 0, 0};	
	//bool connect(int id, String broker, int port, String deviceId, bool cleanSession = true, byte keepAliveTime = 120, mqtt_msg *lastWill = &theLastWillDefault, String userName = "", String password = "");
	
	
	bool publish(int id, mqtt_msg *msg);
	bool publish(int id, String topic, String value);
	bool publish(int id, String topic, String value, byte qos, bool retain);
	
	bool subscribe(int id, String topic, byte qos);
	bool subscribe(int id, String topic, byte qos, void (*g)(String value));
	
	bool recvMQTT(int id, int len);
	bool topicCompare(String topic1, String topic2);
	byte keepAliveTime = 120;
	
	
  private:
    static const byte maxEvents = 5;
	mqtt_event events[maxEvents];
	
    NanoESP* m_nanoesp;
	
		
};


#endif