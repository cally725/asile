#include <wiringPi.h>
#include <stdio.h>
#include <time.h>
#include "scales.h"

/*
Game    Room                Device                  WiringPi    Pi Pin  I/O

Asile   Chambre             Phone Box Switch	    0	        11	    In
                            Phone Magnet     	    1	        12	    Out
                            Phone Magnet Bypass	    1	        12	    Out
                            Puce Door Bypass	    11	        26      Out
                            Balance	                2	        13	    Out
                            Balance Bypass	        2	        13	    Out
					
        Salle electrique	Breaker switch	        3	        15	    In 
                            Breaker Switch Bypass	4	        16	    Out
                            Breaker Door	        4	        16	    Out
					
        Exterieur	        Camera Switch       	5	        18	    In 
                            Camera Switch Bypass	6	        22	    Out
                            Camera Door	            6	        22	    Out
                            Rocks Sensors	        7	        7	    In 
                            Rocks Sensors Bypass	10	        24	    Out
                            Rocks Trap	            10	        24	    Out    
*/

/*
 * Constant definition
 */

#define PHONE_BOX_SWITCH        0
#define PHONE_BOX_MAGNET        1
#define BYPASS_PHONE_BOX_SWITCH 1
#define BALANCE_DOOR            2
#define BYPASS_BALANCE          2
#define BREAKER_SWITCH          3
#define BYPASS_BREAKER_SWITCH   4
#define BREAKER_DOOR            4
#define CAMERA_SWITCHES         5
#define BYPASS_CAMERA_SWITCHED  6
#define CAMERA_DOOR             6 
#define ROCKS_SENSORS           7
#define BYPASS_ROCKS_SENSORS    10
#define ROCKS_TRAP              10
#define BYPASS_PUCE_DOOR        11
#define PILS_WEIGHT_MIN         80.0
#define PILS_WEIGHT_MAX         100.0

/*
 * Variable definition
 */
 
char phoneBoxSwitchBypass[30] = {"BYPASS_PHONE_BOX_SWITCH"};
char gacheModuleBypass[30] = {"BYPASS_GACHE_MODULE"};
time_t gacheButtonBypassTimer = 0;
char puceDoorBypass[30] = {"BYPASS_PUCE_DOOR"};
time_t puceDoorBypassTimer = 0;
char balanceBypass[30] = {"BYPASS_BALANCE"};
time_t balanceDoorTimer = 0;
time_t balanceStableTimer = 0;
time_t balanceBypassTimer = 0;
char breakerSwitchBypass[30] = {"BYPASS_BREAKER_SWITCH"};
time_t breakerSwitchTimer = 0;
time_t breakerSwitchBypassTimer = 0;
char cameraSwitchBypass[30] = {"BYPASS_CAMERA_SWITCHE"};
time_t cameraSwitchBypassTimer = 0;
char rocksSensorBypass[20] = {"BYPASS_ROCKS_SENSORS"};
time_t rocksSensorBypassTimer = 0;
time_t noTimer = -1;
char stopAsile[20] = {"STOP_ASILE"};

/*
 * Function :   checkBypass
 * Description: Check if the corresponding bypass file is present in the current directory
 *              if it is it means that we need to bypass the correcponding switch
 * 
 * Parameters:  file        Name of the file to check
 *              pin         Pin to drive for the bypass
 *              state       State of the pin to set
 *              startTime   Needed when a 10 second delay is required before restoring initial value
 *
 * Return       No return value
 * 
 */
void checkBypass(char *file, int pin, int state, time_t *startTime)
{
	FILE *file1;
    

	file1 = fopen(file, "rb");
	if (file1)
	{
        if (*startTime == noTimer)
        {
            digitalWrite(pin, state);
        }
        else if (*startTime == 0)
        {
            *startTime = time(NULL);
            digitalWrite(pin, state);
        }
        else
        {
            time_t curTime = time(NULL);
            if ((curTime - *startTime) > 10)
                {
                    if (state == LOW)
                        digitalWrite(pin, HIGH);
                    else
                        digitalWrite(pin, LOW);

                }
        }
	}
}

/*
 * Function :   checkEndRequest
 * Description: Check if the end file is present
 *              if it is it means that we need to stop the program
 * 
 * Parameters:  file        Name of the file to check
 *
 * Return       No return value
 * 
 */
int checkEndRequest(char *file)
{
	FILE *file1;
    

	file1 = fopen(file, "rb");
	if (file1)
	{
        return 1;
	}
    else
    {
        return 0;
    }
}

/*
 * Function :   TimedActivate
 * Description: Activate n IO pin for 10 seconds1
 * 
 * Parameters:  pin         Pin to drive for the bypass
 *              state       State of the pin to set
 *              startTime   Needed when a 10 second delay is required before restoring initial value
 *
 * Return       No return value
 * 
 */
void TimedActivate(int pin, int state, time_t *startTime)
{

    if (*startTime == 0)
    {
        *startTime = time(NULL);
        digitalWrite(pin, state);
    }
    else
    {
        time_t curTime = time(NULL);
        if ((curTime - *startTime) > 10)
            {
                if (state == LOW)
                    digitalWrite(pin, HIGH);
                else
                    digitalWrite(pin, LOW);
            }
    }
}

void cleanUpFiles()
{
    remove("BYPASS_PHONE_BOX_SWITCH");
    remove("BYPASS_GACHE_MODULE");
    remove("BYPASS_PUCE_DOOR");
    remove("BYPASS_BALANCE");
    remove("BYPASS_BREAKER_SWITCH");
    remove("BYPASS_CAMERA_SWITCHE");
    remove("BYPASS_ROCKS_SENSORS");  
    remove("STOP_ASILE");
}
 
/*
 * Function :   main
 * Description: Program for the Asile
 * 
 * Parameters:  None
 * 
 * Return       int but it never returns
 * 
 */  
int main() 
{    
    cleanUpFiles();
    
    int initScaleDone = 0;
    
    wiringPiSetup() ;
  
    pinMode(PHONE_BOX_SWITCH, INPUT);
    pinMode(PHONE_BOX_MAGNET, OUTPUT);
    pinMode(BALANCE_DOOR, OUTPUT);
    pinMode(BYPASS_BALANCE, OUTPUT);
    pinMode(BREAKER_SWITCH, INPUT);
    pinMode(BYPASS_BREAKER_SWITCH, OUTPUT);
    pinMode(BREAKER_DOOR, OUTPUT);
    pinMode(CAMERA_SWITCHES, INPUT);
    pinMode(BYPASS_CAMERA_SWITCHED, OUTPUT);
    pinMode(CAMERA_DOOR, OUTPUT);
    pinMode(ROCKS_SENSORS, INPUT);
    pinMode(BYPASS_ROCKS_SENSORS, OUTPUT);
    pinMode(ROCKS_TRAP, OUTPUT);
    pinMode(BYPASS_PUCE_DOOR, OUTPUT);
    
    pullUpDnControl(PHONE_BOX_SWITCH, PUD_UP);
    pullUpDnControl(PHONE_BOX_MAGNET, PUD_UP);
    digitalWrite(PHONE_BOX_MAGNET, LOW);
    pullUpDnControl(BALANCE_DOOR, PUD_UP);
    digitalWrite(BALANCE_DOOR, LOW);
    pullUpDnControl(BYPASS_BALANCE, PUD_UP);
    digitalWrite(BYPASS_BALANCE, LOW);
    pullUpDnControl(BREAKER_SWITCH, PUD_UP);
    pullUpDnControl(BYPASS_BREAKER_SWITCH, PUD_UP);
    digitalWrite(BYPASS_BREAKER_SWITCH, LOW);
    pullUpDnControl(BREAKER_DOOR, PUD_UP);
    digitalWrite(BREAKER_DOOR, LOW);
    pullUpDnControl(CAMERA_SWITCHES, PUD_UP);
    pullUpDnControl(BYPASS_CAMERA_SWITCHED, PUD_UP);
    digitalWrite(BYPASS_CAMERA_SWITCHED, LOW);
    pullUpDnControl(CAMERA_DOOR, PUD_UP);
    digitalWrite(CAMERA_DOOR, LOW);
    pullUpDnControl(ROCKS_SENSORS, PUD_UP);
    pullUpDnControl(BYPASS_ROCKS_SENSORS, PUD_UP);
    digitalWrite(BYPASS_ROCKS_SENSORS, LOW);
    pullUpDnControl(ROCKS_TRAP, PUD_UP);
    digitalWrite(ROCKS_TRAP, LOW);
    pullUpDnControl(BYPASS_PUCE_DOOR, OUTPUT);
    digitalWrite(BYPASS_PUCE_DOOR, LOW);
    
    while (!checkEndRequest(stopAsile))
    {
        // Scale management
        if (initScaleDone == 0)
        {
            int r = initScale(1);
            if (r == 0)
            {
                initScaleDone = 1;
            }
        }
        
        if (initScaleDone == 1)
        {
            double weight;
            weight = runScale();
            if (weight == -1.0)
                initScaleDone = 0;
            
            if ((weight > PILS_WEIGHT_MIN) && (weight < PILS_WEIGHT_MAX))
            {
                if (balanceStableTimer == 0)
                {
                    balanceStableTimer = time(NULL);
                }
                else
                {
                    time_t curTime = time(NULL);
                    if ((curTime - balanceStableTimer) > 10)
                    {

                        //TimedActivate(BALANCE_DOOR, HIGH, &balanceDoorTimer);
                        digitalWrite(BALANCE_DOOR, HIGH);
                    }
                }            
            }
            else
            {
                if (balanceStableTimer != 0)
                {
                    balanceStableTimer = 0;
                }
            }
            printf("weight = %lf Oz\n", weight);
        }
            //printf("toto\n");        
        // Phone box management
        // Check if switch was pressed (set to ground)
        // If closed then release the magnet
        if (digitalRead(PHONE_BOX_SWITCH) == 0){
            //TimedActivate(BREAKER_DOOR, LOW, &breakerSwitchTimer);
            digitalWrite(PHONE_BOX_MAGNET, HIGH);
            printf("toto");
        }

        // Breaker management
        // Check if braker swith is closed (set to ground)
        // If closed then open the door
        if (digitalRead(BREAKER_SWITCH) == 0)
            //TimedActivate(BREAKER_DOOR, LOW, &breakerSwitchTimer);
            digitalWrite(BREAKER_DOOR, HIGH);

        // Camera management
        // Check if camera swith is closed (set to ground)
        // If closed then open the door
        if (digitalRead(CAMERA_SWITCHES) == 0)
            digitalWrite(CAMERA_DOOR, HIGH);
            
        // Rocks management    
        // Check if rocks swith is closed (set to ground)
        // If closed then open the trsp
        if (digitalRead(ROCKS_SENSORS) == 0)
            digitalWrite(ROCKS_TRAP, HIGH);
            
        // Bypass management    
        // Check if any of the bypass file exist
        checkBypass(phoneBoxSwitchBypass, BYPASS_PHONE_BOX_SWITCH, HIGH, &noTimer);
        checkBypass(puceDoorBypass, BYPASS_PUCE_DOOR, HIGH, &gacheButtonBypassTimer);
        checkBypass(balanceBypass, BYPASS_BALANCE, HIGH, &noTimer);
        checkBypass(breakerSwitchBypass, BYPASS_BREAKER_SWITCH, HIGH, &noTimer);
        checkBypass(cameraSwitchBypass, BYPASS_CAMERA_SWITCHED, HIGH, &noTimer);
        checkBypass(rocksSensorBypass, BYPASS_ROCKS_SENSORS, HIGH, &noTimer);
        checkBypass(puceDoorBypass, BYPASS_PUCE_DOOR, HIGH, &noTimer);
        
    }
  
    //closeScale();
    
    digitalWrite(PHONE_BOX_MAGNET, HIGH);
    digitalWrite(BALANCE_DOOR, HIGH);
    digitalWrite(BREAKER_DOOR, HIGH);
    digitalWrite(CAMERA_DOOR, HIGH);
    digitalWrite(ROCKS_TRAP, HIGH);
    digitalWrite(BYPASS_PUCE_DOOR, HIGH);
    
    return 0;
}
