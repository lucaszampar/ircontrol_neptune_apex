/*
 * ircontrol_neptune_apex.ino
 * -------------------------------
 * Arduino program to control Air Conditioner using IR Signal.
 * Used mainly to cool aquarium water using Air Conditioner and Neptune Apex controller.
 * On power outlet the board send the programed ON IR Code and on shutdown the outled, the board send the OFF IR code.
 *
 * Lucas Zampar Bernardi
 * March, 2017
 *
 * */

#include <IRremote.h>
#include <Button.h>
#include "EEPROMAnything.h"


// Pins definitions
#define PIN_IR_RX		11
#define PIN_LED_PROG	2
#define PIN_LED_IR		3
#define PIN_LED_ONBOARD	13
#define PIN_PROG_ON		4
#define PIN_PROG_OFF	5
#define PIN_AC_INPUT	6

// Memory address definitions
#define ADDR_EEPROM_CODE_ON		0x00
#define ADDR_EEPROM_CODE_OFF	0xF0

// Send IR Options
#define IR_SEND_ATTEMPT			5
#define IR_SEND_DELAY			300


// create structures and vars
IRrecv ir_rx(PIN_IR_RX);
IRsend ir_tx;

// Define buttons
Button btn_prog_on(PIN_PROG_ON);
Button btn_prog_off(PIN_PROG_OFF);
Button btn_ac_input(PIN_AC_INPUT);

// Structure with IR Codes
decode_results lIRCodeOn;
decode_results lIRCodeOff;

// Prototypes functions
void  encoding (decode_results *results);
void  dumpInfo (decode_results *results);
void  ircode (decode_results *results);
void sendIRCommand(decode_results *results);


void  setup ( )
{

	// Configure all pins
	pinMode(PIN_IR_RX, INPUT);
	pinMode(PIN_LED_PROG, OUTPUT);
	pinMode(PIN_LED_IR, OUTPUT);
	pinMode(PIN_LED_ONBOARD, OUTPUT);
	pinMode(PIN_PROG_ON, INPUT);
	pinMode(PIN_PROG_OFF, INPUT);
	pinMode(PIN_AC_INPUT, INPUT);


	// Configure serial
	Serial.begin(115200);
	Serial.println("--- Starting APP");

	// Read EEPROM data

	EEPROM_readAnything(ADDR_EEPROM_CODE_ON, lIRCodeOn);
	EEPROM_readAnything(ADDR_EEPROM_CODE_OFF, lIRCodeOff);

	Serial.println("--- EEPROM Registered codes:");
	Serial.print("IR ON => ");
	dumpInfo(&lIRCodeOn);
	Serial.print("IR OFF => ");
	dumpInfo(&lIRCodeOff);
	Serial.println("----");

	// Init buttons
	btn_prog_on.begin();
	btn_prog_off.begin();


	// Wait for cap fully charge
	delay(2000);

	Serial.println("Starting ON command. Sending IR command");
	sendIRCommand(&lIRCodeOn);



}

void  loop ( )
{

	decode_results  result;



	// Verify prog on button
	if (btn_prog_on.released())
	{
		digitalWrite(PIN_LED_PROG, HIGH);
		digitalWrite(PIN_LED_ONBOARD, LOW);

		Serial.println("PROG ON pressed. Waiting IR command");

		ir_rx.enableIRIn();
		delay(5000);
		if (ir_rx.decode(&result))
		{
			dumpInfo(&result);           // Output the results
			if ((result.decode_type == UNKNOWN) || (result.decode_type == SANYO) || (result.decode_type == MITSUBISHI))
			{
				Serial.println("Unsupported IR Code!");
				digitalWrite(PIN_LED_ONBOARD, HIGH);
			}
			else
			{
				EEPROM_writeAnything(ADDR_EEPROM_CODE_ON, result);
				lIRCodeOn = result;
			}

			ir_rx.resume();
		}
		else
		{
			Serial.println("No IR command received!");
			digitalWrite(PIN_LED_ONBOARD, HIGH);
		}

		digitalWrite(PIN_LED_PROG, LOW);
	}

	// Verify prog off button
	if (btn_prog_off.released())
	{
		digitalWrite(PIN_LED_PROG, HIGH);
		digitalWrite(PIN_LED_ONBOARD, LOW);

		Serial.println("PROG OFF pressed. Waiting IR command");

		ir_rx.enableIRIn();
		delay(5000);
		if (ir_rx.decode(&result))
		{
			dumpInfo(&result);           // Output the results
			if ((result.decode_type == UNKNOWN) || (result.decode_type == SANYO) || (result.decode_type == MITSUBISHI))
			{
				Serial.println("Unsupported IR Code!");
				digitalWrite(PIN_LED_ONBOARD, HIGH);
			}
			else
			{
				EEPROM_writeAnything(ADDR_EEPROM_CODE_OFF, result);
				lIRCodeOff = result;
			}

			ir_rx.resume();
		}
		else
		{
			Serial.println("No IR command received!");
			digitalWrite(PIN_LED_ONBOARD, HIGH);
		}

		digitalWrite(PIN_LED_PROG, LOW);

	}

	// Verify AC LOSS
	if (btn_ac_input.pressed())
	{
		Serial.println("AC energy loss detected. Sending IR command");
		sendIRCommand(&lIRCodeOff);
	}

	// Verify AC RECOVER
	if (btn_ac_input.released())
	{
		Serial.println("AC energy recover detected. Sending IR command");
		sendIRCommand(&lIRCodeOn);
	}

}

void  encoding (decode_results *results)
{
	switch (results->decode_type) {
	default:
	case UNKNOWN:      Serial.print("UNKNOWN");       break ;
	case NEC:          Serial.print("NEC");           break ;
	case SONY:         Serial.print("SONY");          break ;
	case RC5:          Serial.print("RC5");           break ;
	case RC6:          Serial.print("RC6");           break ;
	case DISH:         Serial.print("DISH");          break ;
	case SHARP:        Serial.print("SHARP");         break ;
	case JVC:          Serial.print("JVC");           break ;
	case SANYO:        Serial.print("SANYO");         break ;
	case MITSUBISHI:   Serial.print("MITSUBISHI");    break ;
	case SAMSUNG:      Serial.print("SAMSUNG");       break ;
	case LG:           Serial.print("LG");            break ;
	case WHYNTER:      Serial.print("WHYNTER");       break ;
	case AIWA_RC_T501: Serial.print("AIWA_RC_T501");  break ;
	case PANASONIC:    Serial.print("PANASONIC");     break ;
	case DENON:        Serial.print("Denon");         break ;
	}
}

void  ircode (decode_results *results)
{
	// Panasonic has an Address
	if (results->decode_type == PANASONIC) {
		Serial.print(results->address, HEX);
		Serial.print(":");
	}

	// Print Code
	Serial.print(results->value, HEX);
}


void  dumpInfo (decode_results *results)
{
	// Check if the buffer overflowed
	if (results->overflow) {
		Serial.println("IR code too long. Edit IRremoteInt.h and increase RAWLEN");
		return;
	}

	// Show Encoding standard
	Serial.print("Encoding : ");
	encoding(results);
	Serial.print(", Code : ");
	ircode(results);
	Serial.print(" (");
	Serial.print(results->bits, DEC);
	Serial.println(" bits)");
}

void sendIRCommand(decode_results *results)
{
	int i = 0;

	for (i = 0; i < IR_SEND_ATTEMPT; i++)
	{


		switch (results->decode_type) {
		default:
		case UNKNOWN:
			Serial.println("UNKNOWN Code! Did't send any IR Code!");
			break ;

		case NEC:
			ir_tx.sendNEC(results->value, results->bits);
			break ;

		case SONY:
			ir_tx.sendSony(results->value, results->bits);
			break ;

		case RC5:
			ir_tx.sendRC5(results->value, results->bits);
			break ;

		case RC6:
			ir_tx.sendRC6(results->value, results->bits);
			break ;

		case DISH:
			ir_tx.sendDISH(results->value, results->bits);
			break ;

		case SHARP:
			ir_tx.sendSharp(results->address, results->value);
			break ;

		case JVC:
			ir_tx.sendJVC(results->value, results->bits, false);
			break ;

		case SANYO:
			Serial.println("SANYO Protocol not Implemented! Did't send any IR Code!");
			break ;

		case MITSUBISHI:
			Serial.println("MITSUBISH Protocol not Implemented! Did't send any IR Code!");
			break ;

		case SAMSUNG:
			ir_tx.sendSAMSUNG(results->value, results->bits);
			break ;

		case LG:
			ir_tx.sendLG(results->value, results->bits);
			break ;

		case WHYNTER:
			ir_tx.sendWhynter(results->value, results->bits);
			break ;

		case AIWA_RC_T501:
			ir_tx.sendAiwaRCT501(results->value);
			break ;

		case PANASONIC:
			ir_tx.sendPanasonic(results->address, results->value);
			break ;

		case DENON:
			ir_tx.sendDenon(results->value, results->bits);
			break ;
		}
		delay(IR_SEND_DELAY);
	}

}
