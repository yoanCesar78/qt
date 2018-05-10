#include "client.hpp"

Client::Client(QString username, QString ip, int port){
  this->username = username; // différenciation de param et d'attribut
  user_id = -1; //fourni par le serveur après, besoin d'initialiser cette valeur -1 permet de ne pas avoir une valeur random
  server_ip = ip;
  server_port = port; //this->ip
  sock = new QTcpSocket();

  OpenSocket();
  ConnectSignals(); //connexion des signaux internes à la classe 
}

void Client::ConnectSignals(){
  connect(sock, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(RetryOpenSocket())); //indication erreur -> retry
  connect(sock, SIGNAL(connected()), this, SLOT(ConnectApp())); //déconnecté la fonction retry, connectApp -> envoi au niveau applicatif
  connect(sock, SIGNAL(readyRead()), this, SLOT(ReadPackets())); //inform si un msg est à lire dans la socket, readPackets -> lecture des paquets reçus

}
void Client::OpenSocket(){
  static char tryCount = 5;
  
  qDebug() << "Try connection " << tryCount; /*facultatif, permet de savoir qu'OpenSocket est bien lancé*/
  if (--tryCount <= 0)
    qApp->quit();
  
  sock->connectToHost(QString(server_ip), server_port); //class TCPSocket
}

void Client::SendPacket(int message_code, QString message){ //msg_code -> identifiant de msg
  struct app_proto packet; //création de paquet
  
  packet.msg_code = message_code; 
  packet.user_id = user_id; //affectation de val à user_id (à -1 initialement)
  strcpy(packet.data, message.toStdString().c_str()); // à savoir, objectif -> obtenir une chaines de caractères
  packet.data[message.length()] = '\0';
  
  sock->write((char*)&packet, 8+message.length()+1); 
  //envoi du paquet dans la socket -> on ne peut pas faire autrement fait par API
  // 8 taille de user_id
}

void Client::ConnectApp(){ //slot activé lorsque je reçois un signal de connexion
  disconnect(sock, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(RetryOpenSocket())); // déconnexion erreur
  SendPacket(CONNECT); // envoi de mesg applicatif
}

void Client::SendName(){
  SendPacket(NAME, username); // envoi un paquet  de type NAME-> se trouve l'enum 
}

void Client::SendMessage(QString msg){
  qDebug() << "Envoie d'un message: " << msg;
  SendPacket(MESSAGE, msg); // envoi un paquet  de type MESSAGE-> se trouve l'enum
}

void Client::Disconnect(){
  SendPacket(DISCONNECT, username); // envoi un paquet  de type DISCONNECT-> se trouve l'enum
}
void Client::RetryOpenSocket(){ // Réessayer de se connecter 
  OpenSocket();
}

void Client::ReadPackets(){ // gère les paquets reçus
  struct app_proto packet;
  while(sock->bytesAvailable()){
    qDebug() << sock->read((char*)&packet, sizeof(struct app_proto));

    switch(packet.msg_code){ //on fait la boucle pour lire tous les paquets en liste d'attente 
      case CONNECT_ACK:
        SendName();
        break;
      
      case NAME_ACK:
        qDebug() << packet.data << " vient de se connecter";
        if (packet.data == username) 
          user_id = packet.user_id; //id de l'user envoyé par le serveur
        users_list[packet.user_id] = packet.data;
        break;

      case MESSAGE_ACK:
        qDebug() << "Un message vient d'etre recu: " << packet.data;
        emit(newMessage(users_list[packet.user_id] + "> " + packet.data)); // permet d'envoyer le signnal du msg déclaré dans la classe pour inteface graphique
        break;
      
      case DISCONNECT_ACK:
        qDebug() << packet.data << " vient de se deconnecter";
        users_list.remove(packet.user_id); // si c'est pas moi, je l'enlève e la lsite des connectés
        if (packet.user_id == user_id){ // si c'est moi, j'arrete l'application
          sock->close();
          qApp->quit();
        }
    }
  }
}
