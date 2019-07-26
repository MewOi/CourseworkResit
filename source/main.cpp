#include "MicroBit.h"
#include "MicroBitUARTService.h"

MicroBit uBit;
MicroBitUARTService *uart;

//Variable to define if the connection has been made to the mobile devices
int IsConnected = 0;


//URL of the eddystone beacon directing to the UWE homepage
char URL[] = "https://www.uwe.ac.uk/";

//Values of the eddystone message field
const int8_t CALIBRATED_POWERS[] = {-49, -37, -33, -28, -25, -20, -15, -10};


uint8_t advertising = 0;
uint8_t tx_power_level = 6;

void startAdvertising() {
//This line controls whether or not the microbit be connected too, its set to true to allow the rest of the functions to work
    uBit.bleManager.advertiseEddystoneUrl(URL, CALIBRATED_POWERS[tx_power_level-1], true);
    uBit.bleManager.setTransmitPower(tx_power_level);
    //uBit.display.scroll("Transmitting");
    advertising = 1;
}

void onConnected(MicroBitEvent)
{
//Function is called when a connection is made to a device
    uBit.display.scroll("Connected");
    
    //Sets the IsConntected variable to 1 
    IsConnected = 1;

    //The connected device will send strings using ASCII which will terminat with a "."
    ManagedString finalchar(".");

    while(IsConnected == 1) {
        ManagedString msg = uart->readUntil(finalchar); //utilises the UART libary to handle data sent by any devices connected to the mircobit
        uBit.display.scroll(msg);
	
    }

}


//Function to notify if the connection has been lost
void onDisconnected(MicroBitEvent)
{
    uBit.display.scroll("Disconnected");
    IsConnected = 0;
}


//Function to allow the user to check if a connetion is currently established
void onButtonA(MicroBitEvent)
{//On a button press the statement will check if the IsConnected varible is 1 or 0,
 //If 0 it will return "No"
    if (IsConnected == 0) {
	uBit.display.scroll("No");        
	return;
    }
 //If it is not 0 then it will return "Yes" and sent a confirmation to the connected device
    uart->send(ManagedString("Connected"));
    uBit.display.scroll("Yes");
}



//function to stop transmitting the eddystone beacon
void onButtonB(MicroBitEvent) {
    uBit.bleManager.stopAdvertising();
    uBit.display.scroll("OFF");
    advertising = 0; 
}


int main()
{
    // Initialise the micro:bit runtime.
    uBit.init();

	//Tells the system to look for a connection to prompt the function to run
	uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_CONNECTED, onConnected);
	//Tells the system to look for a disconnection to prompt the function to run
	uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_DISCONNECTED, onDisconnected);
	//Tells the system to wait for button A to be pressed before calling the appropriate function
    	uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, onButtonA);
	//Tells the system to wait for button B to be pressed before calling the appropriate function
	uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, onButtonB);
	//Calls the startAdvertising function on starts
	startAdvertising();

    uart = new MicroBitUARTService(*uBit.ble, 32, 32);
    //On start notifcation to show device has turned on
    uBit.display.scroll("Go");
	
	release_fiber();
}
