/*
Author: Fabian Kainka


*/

#include "Arduino.h"
#include "NanoESP_MQTT.h"

const byte MQTT_CON = 1 << 4;   //Connect
const byte MQTT_CONACK = 2 << 4;
const byte MQTT_PUB = 3 << 4;   //Publish
const byte MQTT_PUBACK = 4 << 4;
const byte MQTT_PUBREC = 5 << 4;
const byte MQTT_PUBREL = (6 << 4) | 2;
const byte MQTT_PUVCOMP = 7 << 4;
const byte MQTT_SUB = 8 << 4 | 2; ; //Subscribe
const byte MQTT_SUBACK = 9 << 4;
const byte MQTT_UNSUB = 10 << 4 | 2; ;
const byte MQTT_USUBACK = 11 << 4;
const byte MQTT_PING = 12 << 4; //Ping
const byte MQTT_PINGACK = 13 << 4;
const byte MQTT_DISCON = 14 << 4; //Disconnect

bool cleanSession = true;
//this->keepAliveTime = 120; //If no Message is sent from Client to Server within this time the connection will be terminated
unsigned long previousMillisMQTTsend = 0;        // will store last time a msg was send
//this->interval = (keepAliveTime*1000)/2;           // interval a ping is send to stay connected


NanoESP_MQTT::NanoESP_MQTT(NanoESP& nanoesp):m_nanoesp(&nanoesp){

}


bool NanoESP_MQTT::connect(int id, const String& broker, unsigned int port, const String& deviceId, mqtt_msg * lastWill){
	return this->connect(id, broker, port, deviceId, cleanSession, keepAliveTime, lastWill, "","");
}

bool NanoESP_MQTT::connect(int id, const String& broker, unsigned int port, const String& deviceId, const String& userName , const String& password){
	mqtt_msg theLastWillDefault = {"", "", 0, 0};	
	return this->connect(id, broker, port, deviceId, cleanSession, keepAliveTime, &theLastWillDefault, userName,password);
}

bool NanoESP_MQTT::connect(int id, const String& broker, unsigned int port, const String& deviceId, bool cleanSession, byte keepAliveTime){
	mqtt_msg theLastWillDefault = {"", "", 0, 0};	
	return this->connect(id, broker, port, deviceId, cleanSession, keepAliveTime, &theLastWillDefault, "","");
	
}
bool NanoESP_MQTT::connect(int id, const String& broker, unsigned int port, const String& deviceId, mqtt_msg * lastWill, const String& userName, const String& password){
	return this->connect(id, broker, port, deviceId, cleanSession, keepAliveTime, lastWill, userName,password);

}

bool NanoESP_MQTT::connect(int id, const String& broker, unsigned int port, const String& deviceId, bool cleanSession, byte vkeepAliveTime, mqtt_msg * lastWill, const String& userName, const String& password)
{
  if (!m_nanoesp->newConnection(id, TCP, broker, port)) return false;

  keepAliveTime = vkeepAliveTime;
	
  bool boolLastWill = 0;
  if (lastWill->topic != "")boolLastWill = 1;

  byte connectFlag = 0;
  if (userName != "") connectFlag += 1;
  connectFlag = connectFlag << 1;

  if (password != "") connectFlag += 1;
  connectFlag = connectFlag << 1;

  if (boolLastWill == 1) {
    connectFlag += lastWill->retain;
    connectFlag = connectFlag << 1;

    connectFlag += bitRead(lastWill->qos, 1);
    connectFlag = connectFlag << 1;
    connectFlag += bitRead(lastWill->qos, 0);
    connectFlag = connectFlag << 1;

    connectFlag += boolLastWill;
    connectFlag = connectFlag << 1;
  } else {
    connectFlag = connectFlag << 4;
  }

  connectFlag += cleanSession;
  connectFlag = connectFlag << 1;

  unsigned char data[]  = {
    //Header (CONNECT BYTE, LEN)
    0x10, 0x00,
    //Variable Header
    0x00, 0x04, 'M', 'Q', 'T', 'T', //String LEN (2BYTE) + Protocol name
    0x04, //Protocol Level4
    connectFlag, //Connect Flag (User Name, Password, Will Retain, Will QoS, Will flag, Clean Session, X)
    0x00, keepAliveTime, //Keep Alive Timer
    //0x00, 0x00 //String Len UTF-8?
  };//CONNEC message initialization,
  //Then: ClientIdentifier, Will: Topic, Message, UserName, Password

  int lenMsg = sizeof(data);

  unsigned char vDeviceId[deviceId.length() + 2];
  this->utf8(deviceId, vDeviceId);

  lenMsg += sizeof(vDeviceId);

  unsigned char vTopic[lastWill->topic.length() + 2];
  unsigned char vValue[lastWill->value.length() + 2];

  if (boolLastWill) {
    this->utf8(lastWill->topic, vTopic);
    this->utf8(lastWill->value, vValue);
    lenMsg += sizeof(vTopic);
    lenMsg += sizeof(vValue);
  }

  unsigned char vUserName[userName.length() + 2];
  unsigned char vPassword[password.length() + 2];

  if (userName != "") {
    this->utf8(password, vPassword);
    this->utf8(userName, vUserName);
    lenMsg += sizeof(vUserName);
    lenMsg += sizeof(vPassword);
  }
  //len MSG
  data[1] = lenMsg - 2;

  unsigned char msg [lenMsg];
  int lenTemp = sizeof(data);
  this->charAdd(data, lenTemp, vDeviceId, sizeof(vDeviceId), msg);
  lenTemp += sizeof(vDeviceId);

  if (boolLastWill) {
    this->charAdd(msg, lenTemp, vTopic, sizeof(vTopic), msg);
    lenTemp += sizeof(vTopic);
    this->charAdd(msg, lenTemp, vValue, sizeof(vValue), msg);
    lenTemp += sizeof(vValue);
  }

  if (userName != "") {
    this->charAdd(msg, lenTemp, vUserName, sizeof(vUserName), msg);
    lenTemp += sizeof(vUserName);
    this->charAdd(msg, lenTemp, vPassword, sizeof(vPassword), msg);
    lenTemp += sizeof(vPassword);
  }

  if (this->send(id, msg, sizeof(msg), MQTT_CONACK)){
	char buffer[2];
    !m_nanoesp->readBytes(buffer, 2); //After Header for ConnAck
	if (buffer[1] != 0) {
		Serial.print(F("Connection refused. Code: "));
		Serial.println(buffer[1], HEX);
	}
	else return true;
  }   

  return false;
}


bool NanoESP_MQTT::connect(int id, const String& broker,unsigned  int port, const String& deviceId) {
  mqtt_msg theLastWillDefault = {"", "", 0, 0};	
  return this->connect(id, broker, port, deviceId, cleanSession, keepAliveTime, &theLastWillDefault, "","");
}

bool NanoESP_MQTT::disconnect(int id) {

  // Fixed Head,MS,LSB,    ,M,   Q       ?       ?   ?     ?  ,Ver X?,Conec,Kepp Alive tim
  unsigned char data[]  = {
    //Header
    MQTT_DISCON, 0x00, //Publish
  };

  if (send(id, data, sizeof(data))) return true; else return false;
}


bool NanoESP_MQTT::publish(int id, const String& topic, const String& value, byte qos, bool retain){
	mqtt_msg msg = {topic, value, qos, retain};	
	return this->publish(id, &msg);
}

bool NanoESP_MQTT::publish(int id, const String& topic, const String& value) {
	mqtt_msg msg = {topic, value, 0, 0};	
	return this->publish(id, &msg);
}

bool NanoESP_MQTT::publish(int id, mqtt_msg *msg) {

  // Fixed Head
  unsigned char data[]  = {
    //Header
    0x30, 0x00 //Publish
  };
  //MQTT_PUB .3 = DUP, .2.1 = QOS, .0 =RETAIN
  //bool dup = 0;

  byte qos = msg->qos;

  data[0] += qos << 1;
  data[0] += msg->retain;
 
  unsigned char vTopic[ msg->topic.length() + 2];
  this->utf8(msg->topic, vTopic);


  int lenMsg  = sizeof(data) + sizeof(vTopic) + msg->value.length();
  if (qos > 0) lenMsg += 2;

  unsigned char vmsg [lenMsg];

  int lenTemp = sizeof(data);
  this->charAdd(data, lenTemp, vTopic, sizeof(vTopic), vmsg);
  lenTemp += sizeof(vTopic);

  if (qos > 0) {
    vmsg[lenTemp] = 10; //Packet identifier if QOS > 0
    lenTemp += 2;
  }

  for (int i = 0; i < msg->value.length() ; i ++) {
    vmsg[i + lenTemp] = msg->value[i];
  }

  vmsg[1] = lenMsg-2;
  
  bool sendOk ;

  const byte MQTT_PUBACK = 4 << 4;
  const byte MQTT_PUBREC = 5 << 4;

  if (qos == 0) sendOk = this->send(id, vmsg, sizeof(vmsg));
  else if (qos == 1) sendOk = this->send(id, vmsg, sizeof(vmsg), MQTT_PUBACK) ; //+2 for PacketIdentifier
  else if (qos == 2) sendOk = this->send(id, vmsg, sizeof(vmsg), MQTT_PUBREC) ;	//+2 for PacketIdentifier

  if (qos > 0){
    char buffer[2];
     !m_nanoesp->readBytes(buffer, 2); //After Header for ConnAck
  }
  
  if (sendOk) return true; else return false;
}

bool NanoESP_MQTT::recvMQTT(int &id, String &topic, String &value) {
  if (m_nanoesp->available()) {
    if (m_nanoesp->findUntil("+IPD,", "\n")) {
      int id = m_nanoesp->parseInt();
      m_nanoesp->find(",");
      int len = m_nanoesp->parseInt();
      char buffer[len];

      m_nanoesp->find(":");
      m_nanoesp->readBytes(buffer, len);

      if ((buffer[0] & 11110000) == MQTT_PUB) {
        byte lenTopic = buffer[3];

        for (int i = 0; i < lenTopic; i++) {
          topic += buffer[i + 4];
        }
        for (int i = 4 + lenTopic; i < len; i++) {
          value += buffer[i];
        }
        return true;
      }
    }
  }
  return false;
}


bool NanoESP_MQTT::subscribe(int id, const String& topic, byte qos, void (*g)(const String& value)) 
{
		if (this->subscribe(id,topic,qos)){
			for (int i = 0; i<maxEvents; i++){
				if (events[i].topic == ""){
					events[i] = {topic, (*g)};
					return true;
				}  // nur bei send OK?
			}
			Serial.println(F("Max subscribe handler reached"));
		
		}
  return false;
}

bool NanoESP_MQTT::subscribe(int id, const String& topic, byte qos) {

  // Fixed Head,MS,LSB,    ,M,   Q       ?       ?   ?     ?  ,Ver X?,Conec,Kepp Alive tim
  unsigned char data[]  = {
    //Header
    MQTT_SUB, 0x00, //Publish
    0x00, 0x0A,      //packet identifier ??
  };

  unsigned char vTopic[topic.length() + 2];
  this->utf8(topic, vTopic);

  int lenMsg  = sizeof(data) + sizeof(vTopic) + 1;  //+QOS Byte
 
  unsigned char vmsg [lenMsg];

  int lenTemp = sizeof(data);
  this->charAdd(data, lenTemp, vTopic, sizeof(vTopic), vmsg);
  lenTemp += sizeof(vTopic);
  
  vmsg[lenTemp] = qos;
  vmsg[1] = lenMsg-2; //Len without header
  
  bool sendOk = send(id, vmsg, sizeof(vmsg),MQTT_SUBACK);
   char buffer[3];
     !m_nanoesp->readBytes(buffer, 3); //PacketIdentifier + payload Success MSG
  if (sendOk) return true; else return false;
}


bool NanoESP_MQTT::subscribe(int id, const String& topic) {

  // Fixed Head,MS,LSB,    ,M,   Q       ?       ?   ?     ?  ,Ver X?,Conec,Kepp Alive tim
  unsigned char data[]  = {
    //Header
    MQTT_SUB, 0x00, //Publish
    0x00, 0x0A,      //packet identifier ??
    0x00, 0x00,      //Length MSV, LSB (of Topic)
    //Here comes topic
    0x02  //Qos Maximum
  };

  byte lenChar = sizeof(data);
  byte lenTopic = topic.length();

  data[lenChar - 2] = lenTopic;

  unsigned char msg [lenChar + lenTopic];

  for (int i = 0; i < lenChar - 1 ; i ++) {
    msg[i] = data[i];
  }
  for (int i = 0; i < lenTopic ; i ++) {
    msg[i + lenChar - 1] = topic[i];
  }
  msg[lenChar + lenTopic - 1] = data[lenChar - 1];

  if (send(id, msg, sizeof(msg))) return true; else return false;
}

bool NanoESP_MQTT::unsubscribe(int id, const String& topic) {

  // Fixed Head,MS,LSB,    ,M,   Q       ?       ?   ?     ?  ,Ver X?,Conec,Kepp Alive tim
  unsigned char data[]  = {
    //Header
    MQTT_UNSUB, 0x00, //Publish
    0x00, 0x0A,      //packet identifier ??
      //Here comes topic
  };
  
  unsigned char vTopic[topic.length() + 2];
  this->utf8(topic, vTopic);

  int lenMsg  = sizeof(data) + sizeof(vTopic);  //+QOS Byte
 
  unsigned char vmsg [lenMsg];
  this->charAdd(data, sizeof(data), vTopic, sizeof(vTopic), vmsg);
  vmsg[1] = lenMsg-2; //Len without header
  
  bool sendOk = send(id, vmsg, sizeof(vmsg),MQTT_USUBACK);
  char buffer[2];
  !m_nanoesp->readBytes(buffer, 2); //PacketIdentifier + payload Success MSG
  
  	for (int i = 0; i<maxEvents; i++){
			if (events[i].topic == topic) events[i].topic = ""; // nur bei send OK?
	}
  
  if (sendOk) return true; else return false;
}

bool NanoESP_MQTT::ping(int id) {
  // Fixed Head
  unsigned char data[]  = {
    //Header
    MQTT_PING, 0x00, //Publish
  };
  if (send(id, data, sizeof(data),MQTT_PINGACK)) return true; else return false; //get ack!!!
}

bool NanoESP_MQTT::send(int id, unsigned char data[], int LenChar) {
  data[1] = LenChar - 2; //Header Len

  m_nanoesp->println("AT+CIPSEND=" + String(id) + "," + String(LenChar));
  if (m_nanoesp->find(">")) {
    for (int i = 0; i < LenChar; i++) {
      m_nanoesp->write(data[i]);
    }
    if   (m_nanoesp->find("OK\r\n")) {
		previousMillisMQTTsend = millis(); //Reset time till ping
		return true; //Short it
	}
  }
  return false;
}

bool NanoESP_MQTT::send(int id, unsigned char data[], int LenChar, char ack) {
  data[1] = LenChar - 2; //Header Len

  m_nanoesp->println("AT+CIPSEND=" + String(id) + "," + String(LenChar));
  if (m_nanoesp->find(">")) {
    for (int i = 0; i < LenChar; i++) {
      m_nanoesp->write(data[i]);
    }
    if (m_nanoesp->find("OK\r\n")) {
	  previousMillisMQTTsend = millis(); //Reset time till ping
      m_nanoesp->find("+IPD");
      m_nanoesp->find(":");

      //Get ACK Header
      char buffer[2] ; //Header should be 2!!! 
      m_nanoesp->readBytes(buffer, 2);
      if (buffer[0] == ack) {
        return true;
      }
    }
  }
  return false;
}

//NEW
bool NanoESP_MQTT::recvMQTT(int id, int len, String &topic, String &value) {
	if (len>0){
		if( m_nanoesp->findUntil(":", "\n")){               //+IPD, 0, 407: GET / HTTP / 1.1
			char buffer[len];
			m_nanoesp->readBytes(buffer, len);

			if ((buffer[0] & 11110000) == MQTT_PUB) {
				byte qos = buffer[0] & 00000110;
				bool retain = bitRead(buffer[0],0);			
				
				byte lenTopic = buffer[3];

				for (int i = 0; i < lenTopic; i++) {
					topic += buffer[i + 4];
				}
				
				byte startPos = 4;
				if (qos !=0){
					 startPos += 2;
				} //with packet identifier
				
				
				for (int i = startPos + lenTopic; i < len; i++) {
					value += buffer[i];
				}
				return true;
			}
		}
	}
  return false;
}

bool NanoESP_MQTT::recvMQTT(int id, int len) {
	String topic;
	String value;
	
	if (this->recvMQTT(id, len, topic, value)){
		for (int i = 0; i<maxEvents; i++){
			if (this->topicCompare(events[i].topic,topic)) events[i].g(value);
		}
		return true;
	}
return false;
}


bool NanoESP_MQTT::topicCompare( const String& topic1, const String& topic2) {
//Serial.println(topic1 +" - "+topic2);
	
  if (topic1 == "") return false;

  if (topic1.indexOf('+') > 0) {
    int pos1 = topic1.indexOf('/');
    int pos2 = topic1.indexOf('/');

    String restTopic1 = topic1;
    String restTopic2 = topic2;

    while (pos1> 0) {
      String cTopic1 = restTopic1.substring(0, pos1);
      String cTopic2 = restTopic2.substring(0, pos2);

      restTopic1 = restTopic1.substring(pos1+1);
      restTopic2 = restTopic2.substring(pos2+1);

      pos1 = restTopic1.indexOf('/');
	  pos2 = restTopic2.indexOf('/');
		  
      if (cTopic1 == "+") continue;
      if (cTopic1 == "#") return true;
      if (cTopic1 != cTopic2) return false;
    }
	//Serial.println(restTopic1 +" - "+restTopic2);
    if (restTopic1 == restTopic2) return true; else return false;

  }
  else if (topic1.indexOf('#') > 0) {
    int pos = topic1.indexOf("#");
    if ( topic1.substring(0, pos) == topic2.substring(0, pos)) return true; else return false;
  }
  else {
    if (topic1 == topic2) return true; else return false;
  }
}


void NanoESP_MQTT::stayConnected(int id, long interval){
	if (millis() - previousMillisMQTTsend >= interval) {
		this->ping(id);
  }
}

void NanoESP_MQTT::stayConnected(int id){
	long interval = keepAliveTime/2;
	interval = interval *1000;
	
	this->stayConnected(id, interval);
}

void NanoESP_MQTT::utf8(const String& input, unsigned char* output) {
  byte len = input.length();

  output[0] = 0;
  output[1] = len;
  for (int i = 0; i < len ; i++) {
    output[i + 2] = input[i];
  }
}

void NanoESP_MQTT::charAdd(unsigned char* inputA, int lenA, unsigned char* inputB, int lenB, unsigned char* output) {

  for (int i = 0; i < lenA; i++) {
    output[i] = inputA[i];
  }
  for (int i = 0; i < lenB; i++) {
    output[i + lenA] = inputB[i];
  }
}











