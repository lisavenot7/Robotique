
// utilise la librairie SparkFun pour gérer le capteur de distance VL53L1X
// https://www.sparkfun.com/products/14722
// https://github.com/sparkfun/SparkFun_VL53L1X_Arduino_Library

// [[mga255][mda 255]]
// [[tjs 0]]
// [[tjs 500][mga 255][mda 255]]

// codes des commandes
// on reçoit les commandes dans une ligne de texte

#define ERREUR 0

#define NUM_SEQ_CMD 1		// numéro de la séquence de commandes

#define MOTEUR_G_AV 2		// moteur gauche vers l'avant
#define MOTEUR_G_AR 3		// moteur gauche vers l'arrière
#define MOTEUR_D_AV 4		// moteur droit vers l'avant
#define MOTEUR_D_AR 5		// moteur droit vers l'arrière
#define MOTEURS_STOP 6		// arrêt des moteurs
#define MOTEURS_TIMEOUT 7	// timeout pour arrêt des moteurs
#define TIMEOUT_JSON 8	// timeout pour arrêt des moteurs
#define BOITE 9 // mission de tour d'une boîte

void init_buf_in();


void moteurs_init();
void moteur_gauche_avant(int);
void moteur_droit_avant(int);
void moteur_gauche_arriere(int);
void moteur_droit_arriere(int);
void moteurs_stop();

void vl53l1x_init();
void vl53l1x_read();
void vl53l1x_json();

void boite();
void boite_init(int);
int mode_boite=0;//défini si on cherche la boite ou non.

void software_serial_init();
void software_serial_read_vl53l1x();

#define BUF_IN 200	// taille max du buffer de lecture
#define LCMD 50		// taille max du tableau de la liste de commandes
#define TIMEOUT 1000	// durée max en ms d'une séquence de commandes

char buf_in[BUF_IN];	// buffer de lecture
int buf_in_count = 0;	// taille courante du buffer

int lcmd[LCMD]; // tableau de la liste des commandes à exécuter (2 entiers par commande)
int lcmd_count = 0;	// taille courante du tableau de la liste des commandes
int lcmd_pos = 0;	// position de la commande courante en cours d'exécution

// données d'exécution

int exec_en_cours = 0;	// à 1 si exécution d'une séquence de commandes en cours

unsigned long time = 0;	// on arrête l'exécution si time + timeout > millis()
unsigned long timeout = TIMEOUT;	// durée max en ms d'une séquence de commandes
// pour arrêter l'exécution d'une séquence de commandes on met timeout à 0

int num_seq_cmd = -1;	// numéro de la séquence de commandes

unsigned long time_json = 0;	
unsigned long timeout_json = 500;	// envoi de données JSON toutes les 500 ms

unsigned long time_vl53l1x_2 = 0;		// vl53l1x n°2
unsigned long timeout_vl53l1x_2 = 500;	// envoi de données JSON toutes les 500 ms


// [[ns 3][mga 255][mda 255][t 1000]]

void setup() {

	init_buf_in();

	Serial.begin(9600);

	vl53l1x_init();

	software_serial_init();

	delay(2000);

	Serial.println("OK Arduino");

	
}

int c = -1;		// caractère courant lu
int c_prec = -1;	// caractère précédent lu

void loop() {

	if (0 == 1) return;

// delay(1000);

// Serial.println(time_json);


	if (millis() > time_vl53l1x_2 + timeout_vl53l1x_2) {
		software_serial_read_vl53l1x();
	}

	if (time_json != 0) {
		if (millis() > time_json + timeout_json) {

			Serial.print("{");
			vl53l1x_json();
			Serial.print("}");

			time_json = millis();

		}
	}

	vl53l1x_read();
  boite();//appel systématique de la boite
	if (Serial.available()) {
		c_prec = c;
		c = Serial.read();
		if ((c == '[') && (c_prec == '[')) {

			// nouvelle lecture d'une séquence de commandes

			init_buf_in();
			buf_in[buf_in_count++] = '[';
		}
		else if ((c == ']') && (c_prec == ']')) {
			// fin lecture, nouvelle séquence de commandes reçue
			// Serial.print("lu = "); Serial.println(buf_in);
			decoder_seq_commandes();

			init_donness_execution();
	
			exec_en_cours = 1;
		}
		else if (c == '\n') {
		}
		else {
			buf_in[buf_in_count++] = c;
		}
	}

	if (exec_en_cours == 1) {
		if (time + timeout > millis()) {
			exec_cmd();
		}
		else {
			moteurs_stop();
			// on produit le résultat JSON

			// numéro de la séquence exécutée
			Serial.print("{\"ns\":");
			Serial.print(num_seq_cmd);

			// distances et fiabilités des distances
			Serial.print(",");
			vl53l1x_json();

			Serial.println("}");

			exec_en_cours = 0;
		}

	}


}




/*******************************************************************************/
/********************* décodage de la séquence de commandes ********************/
/*******************************************************************************/

void decoder_seq_commandes() {
	// analyse de la séquence de commandes reçue dans buf_in
	// résultat dans lcmd (2 entiers par commande, numéro commande et paramètre)
	
	for (int i=0 ; i<LCMD ; i++) {
		lcmd[i] = ERREUR; // init lcmd à 0
	}
	lcmd_count = 0;

	int pos = 0;
	int p1;
	for (;;) { // décodage d'une commande à chaque itération
		if (pos >= buf_in_count) break;

		if (buf_in[pos] == '[') {
			pos++;
			if ((buf_in[pos] == 'n') && (buf_in[pos+1] == 's')) {
				lcmd[lcmd_count++] = NUM_SEQ_CMD;
				pos += 2;
			}
			else if ((buf_in[pos] == 'm') && (buf_in[pos+1] == 'g') && (buf_in[pos+2] == 'a')) {
				lcmd[lcmd_count++] = MOTEUR_G_AV;
				pos += 3;
			}
			else if ((buf_in[pos] == 'm') && (buf_in[pos+1] == 'g') && (buf_in[pos+2] == 'r')) {
				lcmd[lcmd_count++] = MOTEUR_G_AR;
				pos += 3;
			}
			else if ((buf_in[pos] == 'm') && (buf_in[pos+1] == 'd') && (buf_in[pos+2] == 'a')) {
				lcmd[lcmd_count++] = MOTEUR_D_AV;
				pos += 3;
			}
			else if ((buf_in[pos] == 'm') && (buf_in[pos+1] == 'd') && (buf_in[pos+2] == 'r')) {
				lcmd[lcmd_count++] = MOTEUR_D_AR;
				pos += 3;
			}
			else if ((buf_in[pos] == 'm') && (buf_in[pos+1] == 's') && (buf_in[pos+2] == 't')) {
				lcmd[lcmd_count++] = MOTEURS_STOP;
				pos += 3;
			}
			else if ((buf_in[pos] == 't') && (buf_in[pos+1] == 'j') && (buf_in[pos+2] == 's')) {
				lcmd[lcmd_count++] = TIMEOUT_JSON;
				pos += 3;
			}
      else if ((buf_in[pos] == 'b') && (buf_in[pos+1] == 'o')) {//mission de la boite
        lcmd[lcmd_count++] = BOITE;
        pos += 2;
      }
			else if (buf_in[pos] == 't') {
				lcmd[lcmd_count++] = MOTEURS_TIMEOUT;
				pos++;
			}
			else {
				lcmd[lcmd_count++] = ERREUR;
				break;
			}

			if (buf_in[pos] != ' ') {
				lcmd[lcmd_count++] = ERREUR;
				break;
			}
			pos++;
			if ((buf_in[pos] < '0') || (buf_in[pos] > '9')) {
				lcmd[lcmd_count++] = ERREUR;
				break;
			}
			p1 = pos; // début du paramètre (entier)
			for (;;) {
				pos++;
				if ((buf_in[pos] < '0') || (buf_in[pos] > '9')) {
					break;
				}
			}
			if (buf_in[pos] == ']') {
				buf_in[pos] = 0;
				lcmd[lcmd_count++] = atoi(buf_in+p1);
				pos++;
			}
			else {
				lcmd[lcmd_count-1] = ERREUR;
				break;
			}
			


		}
		else {
			lcmd[lcmd_count++] = ERREUR;
			break;
		}
	}



	// affichage du tableau des commandes
	// for (int i=0 ; i<lcmd_count ; i+=2) {
	// 	Serial.print("cmd = ");
	// 	Serial.print(lcmd[i]);
	// 	Serial.print(" ");
	// 	Serial.print(lcmd[i+1]);
	// 	Serial.println();
	// }

}


/*******************************************************************/
/********************* exécution d'une commande ********************/
/*******************************************************************/

void exec_cmd() {
	if (lcmd_pos >= lcmd_count) return;

	int cmd = lcmd[lcmd_pos++];
	int param = lcmd[lcmd_pos++];

	// Serial.print("cmd = "); Serial.print(cmd); Serial.print(" param = "); Serial.println(param);

	if (cmd == NUM_SEQ_CMD) {
		num_seq_cmd = param;
	}
	else if (cmd == MOTEUR_G_AV) moteur_gauche_avant(param);
	else if (cmd == MOTEUR_D_AV) moteur_droit_avant(param);
	else if (cmd == MOTEUR_G_AR) moteur_gauche_arriere(param);
	else if (cmd == MOTEUR_D_AR) moteur_droit_arriere(param);
	else if (cmd == MOTEURS_STOP) moteurs_stop();
	else if (cmd == MOTEURS_TIMEOUT) timeout = param;
  else if (cmd == BOITE) {
    boite_init(param);
  }//appel de l'initialisation de la boite->exécution de la fonction boite.
	else if (cmd == TIMEOUT_JSON) {
		timeout_json = param;
		if (param == 0) time_json = 0; else time_json = millis();
	}
}

/*******************************************************************/
/************************** init_buf_in ****************************/
/*******************************************************************/

void init_buf_in() {
	for (int i=0 ; i<BUF_IN ; i++) {
		buf_in[i] = 0;
	}
	buf_in_count = 0;
}

/*******************************************************************/
/************************** init_donness_execution *****************/
/*******************************************************************/

void init_donness_execution() {
	lcmd_pos = 0;	// commande courante

	num_seq_cmd = -1;

	time = millis();	// temps courant en ms
	timeout = TIMEOUT;	// durée de l'exécution d'une séquence de commandes

}

/*******************************************************************/
/********************* commande des moteurs ************************/
/*******************************************************************/

// Attention : suivant les cartes, on utilise l'une série de constantes ou l'autre

// https://www.gotronic.fr/art-shield-motor-2-x-2-a-dri0009-19345.htm
int E1 = 5;	// D5 vitesse moteur 1
int M1 = 4;	// D4 sens moteur 1
int E2 = 6;	// D6 vitesse moteur 2
int M2 = 7;	// D7 sens moteur 2

// https://fr.aliexpress.com/item/32786304281.html
// int E1 = 10;	// vitesse moteur 1
// int M1 = 12;	// sens moteur 1
// int E2 = 11;	// vitesse moteur 2
// int M2 = 13;	// sens moteur 2

int vitesse_gauche = 0;	// valeur de 0 à 255 (en fait de 100 ou 150 à 255), positif si vers l'avant, négatif si vers l'arrière
int vitesse_droit = 0;	// valeur de 0 à 255 (en fait de 100 ou 150 à 255)

void moteurs_init() {
	pinMode(M1, OUTPUT);   
	pinMode(M2, OUTPUT); 
	moteurs_stop();
}

void moteur_gauche_avant(int speed) {
  vitesse_gauche = speed;
	digitalWrite(M1,LOW);
	analogWrite(E1, speed);   //PWM Speed Control
}

void moteur_gauche_arriere(int speed) {
	vitesse_gauche = -speed;
	digitalWrite(M1,HIGH);
	analogWrite(E1, speed);   //PWM Speed Control
}

void moteur_droit_avant(int speed) {
	vitesse_droit = speed;
 	digitalWrite(M2,LOW);
	analogWrite(E2, speed);   //PWM Speed Control
}

void moteur_droit_arriere(int speed) {
	vitesse_droit = -speed;
	digitalWrite(M2,HIGH);
	analogWrite(E2, speed);   //PWM Speed Control
}

void moteurs_stop() {
	analogWrite(E1, 0);   //PWM Speed Control
	analogWrite(E2, 0);   //PWM Speed Control
	vitesse_gauche = 0;
	vitesse_droit = 0;
}



/*******************************************************************/
/******************* VL53L1X Capteurs de distance ******************/
/*******************************************************************/


// utilise la librairie SparkFun pour gérer le capteur de distance VL53L1X
// https://www.sparkfun.com/products/14722
// https://github.com/sparkfun/SparkFun_VL53L1X_Arduino_Library

#include <Wire.h>
#include "SparkFun_VL53L1X.h" //Click here to get the library: http://librarymanager/All#SparkFun_VL53L1X

SFEVL53L1X distanceSensor;


unsigned long time_sensor = 0;		// si 0 : timeout d'arrêt des moteurs 

int vl53l1x_dist = -1;	// distance en mm
int vl53l1x_rate = -1;	// fiabilité
int vl53l1x_status = -1;	// statut good : 0 ; fail : >0 

void vl53l1x_init() {
	Wire.begin();
	if (distanceSensor.begin() != 0) { //Begin returns 0 on a good init
			Serial.println("Sensor failed to begin. Please check wiring. Freezing...");
	}
  	Serial.println("Sensor online!");

        time_sensor = millis();

}

void vl53l1x_read() {
	long startTime = millis();
	distanceSensor.startRanging(); //Write configuration block of 135 bytes to setup a measurement
	while (!distanceSensor.checkForDataReady())	{
		delay(1);
	}
	vl53l1x_dist = distanceSensor.getDistance(); //Get the result of the measurement from the sensor
	vl53l1x_rate = distanceSensor.getSignalRate();
	vl53l1x_status = distanceSensor.getRangeStatus();

	distanceSensor.clearInterrupt();
	distanceSensor.stopRanging();

}

void vl53l1x_json() {
    //Serial.print("{");

    Serial.print("\"d1\":");
    Serial.print(vl53l1x_dist);
    Serial.print(",\"r1\":");
    Serial.print(vl53l1x_rate);
    //Serial.print(", \"s\":");
    //Serial.print(vl53l1x_status);
 
    //Serial.println("}");
}

/*******************************************************************/
/**************************** SoftwareSerial ***********************/
/*******************************************************************/

#include <SoftwareSerial.h>   //Software Serial Port
#define RxD 2
#define TxD 3

SoftwareSerial softwareSerial(RxD,TxD);

int mode_serial = 0;

char buf_software_serial[80];
int count_software_serial = 0;

void software_serial_init() {
	softwareSerial.begin(9600); // default baud rate
}

void software_serial_read_vl53l1x() {
	softwareSerial.print("v");

	while(softwareSerial.available()) {
		char c = softwareSerial.read();
		Serial.print(c);
		if (c == '{') {
			mode_serial = 1;
			count_software_serial = 0;
		}
		if (mode_serial == 1) {
			buf_software_serial[count_software_serial] = c;
			count_software_serial++;
			if (c == '}') {
				buf_software_serial[count_software_serial] = 0;
				Serial.println(buf_software_serial);
				mode_serial = 0;
				break;
			}
		}
	}

}

/*Mission : longer une boite*/

#define BOITE_LONGER 10
#define BOITE_PAS 11
#define BOITE_TOURNER 12
#define BOITE_TROUVER 13
int mode = 0;
int dist = 0;
int v = 0;
int dt = 0;


void boite_init(int m){
  mode_boite = m;
  mode = BOITE_LONGER;
}

void boite() {
  if(mode_boite != 1) return;
  if (mode == BOITE_LONGER) {
    if (vl53l1x_rate>1000) {
      moteur_droit_avant(180);
      moteur_gauche_avant(180);
    }
    else {
      mode = BOITE_PAS;
    }
  }
  else if (mode == BOITE_PAS) {
    moteur_droit_avant(180);
    moteur_gauche_avant(180);
    delay(50);
    mode = BOITE_TOURNER;
  }
  else if (mode == BOITE_TOURNER) {
    moteur_droit_arriere (255);
    moteur_gauche_avant(255);
    delay(500);
    mode = BOITE_TROUVER;
  }
  else if (mode == BOITE_TROUVER) {
    if (vl53l1x_rate>1000) {
      mode = BOITE_LONGER;
    }
    else {
      moteur_droit_avant(180);
      moteur_gauche_avant(180);
    }
  }
  else {
    Serial.println("erreur longer boite");
  }

}
