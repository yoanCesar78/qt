#include "main_window.hpp"


MainWindow::MainWindow(){
  CreateLayouts(); // conteneur de placement 
  CreateMainArea(); // différentes places d'affichage
  CreateButtons();
  show();
}

void MainWindow::CreateLayouts(){
  main_layout = new QVBoxLayout; // grille
  central_widget = new QWidget;
  central_widget->setLayout(main_layout);
  setCentralWidget(central_widget);
}

void MainWindow::CreateMainArea(){
  display_chat = new QTextEdit; // zone d'affichage texte
  display_chat->setReadOnly(true); // non modifiable

  user_input = new QLineEdit; // rentre les messages que le user veut envoyer
  
  main_layout->addWidget(display_chat);
  main_layout->addWidget(user_input);
}

void MainWindow::CreateButtons(){
  send_btn = new QPushButton("Envoyer");
  disconnect_btn = new QPushButton("Deconnexion"); // arret de l'application

  QHBoxLayout *layout = new QHBoxLayout; // contenant

  layout->addWidget(disconnect_btn);
  layout->addWidget(send_btn);

  main_layout->addLayout(layout);
}

void MainWindow::UpdateChat(QString msg){ // cette methode ne peut pas etre appelée si la display_chat n'est pas initialisé
	// ajouter à la zone d'affichage 
  display_chat->setPlainText(display_chat->toPlainText() + msg +"\n");
}
