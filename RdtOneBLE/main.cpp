/*
 * RdtOneBLE.cpp
 *
 * Created: 2019-03-26 15:18:36
 * Author : kong
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "system/Util.h"
#include "system/ConsoleUART.h"
#include "system/BluetoothUART.h"
#include "system/RotationINTR.h"
#include "system/DistanceADC.h"
#include "system/TickTIMER.h"
#include "profile/BLEProperty.h"
#include "profile/BLETypedCharacteristics.h"
#include "profile/BLEService.h"
#include "peripheral/LocalDevice.h"
#include "main.h"

void help();
void toggleShowup();
void toggleManualMode();

bool x_showup;
bool x_manual_mode;

BLEService g_xService("180F");
BLEUnsignedCharCharacteristic g_xLevelChar("2A19", BPP_READ | BPP_NOTIPY);

int main(void)
{
    x_showup = false;
    x_manual_mode = false;
    
    x_console.begin(BAUDRATE_CONSOLE);
    x_bluetooth.begin(BAUDRATE_BLUETOOTH);
    x_tick.begin(TICK_INTERVAL);
    x_rotation.begin();
    x_distance.begin();
    
    sei();
    
    x_console.write("\nhello world!\n");
    x_bluetooth.write("\nhello world!\n");
    help();
    
    char c;
    
    /*
    g_bleDevice.setLocalName("xService");
    g_bleDevice.setAdvertisedService(g_xService);
    g_xService.addCharacteristic(g_xLevelChar);
    g_bleDevice.addService(g_xService);
    g_xLevelChar.writeValue(0);
    g_bleDevice.advertise();
    */
    
    while (1)
    {
        c = (char)(x_console.read() & 0xff);
        switch (c) {
            case '!': help(); break;
            case '@': toggleShowup(); break;
            case '#': toggleManualMode(); break;
            case '+': x_tick.incInterval(); break;
            case '-': x_tick.decInterval(); break;
            default:  break;
        }
    }
}

void help()
{
    x_console.write("\n");
    x_console.write("! : help message\n");
    x_console.write("@ : show-up sensing activity on/off\n");
    x_console.write("# : manual event mode on/off\n");
    x_console.write("+ : increase interval\n");
    x_console.write("- : decrease interval\n");
}

void toggleShowup()
{
    if (x_showup) {
        x_showup = false;
        x_console.write("\nshow-up disabled\n");
    }
    else {
        x_showup = true;
        x_console.write("\nshow-up enabled\n");
    }
}

void toggleManualMode()
{
    if (x_manual_mode) {
        x_manual_mode = false;
        x_console.write("\nexit manual mode\n");
    }
    else {
        x_manual_mode = true;
        x_console.write("\nenter manual mode\n");
    }
}

/* EOF */