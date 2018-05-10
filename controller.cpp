#include "controller.hpp"

Controller::Controller(Client *c, MainWindow *w){ // connect différents listener
  client = c;
  main_window = w;
  
  //connex entre vue et client

  QObject::connect(main_window->getDisconnectBtn(), SIGNAL(clicked()), client,
      SLOT(Disconnect()));
  QObject::connect(main_window->getSendBtn(), SIGNAL(clicked()), this,
      SLOT(SendMessage()));
  QObject::connect(client, SIGNAL(newMessage(QString)), main_window,
      SLOT(UpdateChat(QString)));

}

Controller::~Controller(){ //
}

void Controller::SendMessage(){ // à partir du controller, récupérer le msg envoyé par l'user 
  client->SendMessage(main_window->getUserInput()->text()); //prendre le user 
  main_window->getUserInput()->setText(""); // j'efface l'entrée pour que l'user puisse avoir une zone de saisie nettoyée
}
