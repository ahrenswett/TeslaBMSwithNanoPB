#include "config.h"
#include "BMSModuleManager.h"
#include "BMSUtil.h"
#include "Logger.h"
#include "TestModuleData.h"
#include <iostream>
using namespace std;


extern EEPROMSettings settings;

BMSModuleManager::BMSModuleManager()
{
    for (int i = 1; i <= MAX_MODULE_ADDR; i++) {
        modules[i].setExists(false);
        modules[i].setAddress(i);
    }

    lowestPackVolt = 1000.0f;
    highestPackVolt = 0.0f;
    lowestPackTemp = 200.0f;
    highestPackTemp = -100.0f;
    isFaulted = false;
}


/*
 * Setup the 
 * 
 */

void BMSModuleManager::setupBoards()
{
    // allow user to define how many modules
    
    // used to setup cells of each module in order to keep all modules at a ballance voltage as well

    Logger::debug("00 found");
    //look for a free address to use
    for (int y = 1; y < 63; y++) 
    {
        //set the module as existing and increase modules found
        if (!modules[y].isExisting())
        {
            modules[y].setExists(true);
            numFoundModules++;
            Logger::debug("Address assigned");
        }
    }
}


void BMSModuleManager::renumberBoardIDs()
{
    // uint8_t payload[3];
    // uint8_t buff[8];
    // int attempts = 1;

    for (int y = 1; y < 63; y++) 
    {
        modules[y].setExists(false);
        numFoundModules = 0;
    }

    setupBoards();
}



/*
Puts all boards on the bus into a Sleep state, very good to use when the vehicle is a rest state. 
Pulling the boards out of sleep only to check voltage decay and temperature when the contactors are open.
*/

void BMSModuleManager::sleepBoards()
{
    
}

/*
Wakes all the boards up and clears thier SLEEP state bit in the Alert Status Registery
*/

void BMSModuleManager::wakeBoards()
{
    uint8_t payload[3];
    uint8_t buff[8];
    payload[0] = 0x7F; //broadcast
    payload[1] = REG_IO_CTRL;//IO ctrl start
    payload[2] = 0x00;//write sleep bit
    BMSUtil::sendData(payload, 3, true);
    delay(2);
    BMSUtil::getReply(buff, 8);
  
    payload[0] = 0x7F; //broadcast
    payload[1] = REG_ALERT_STATUS;//Fault Status
    payload[2] = 0x04;//data to cause a reset
    BMSUtil::sendData(payload, 3, true);
    delay(2);
    BMSUtil::getReply(buff, 8);
    payload[0] = 0x7F; //broadcast
    payload[2] = 0x00;//data to clear
    BMSUtil::sendData(payload, 3, true);
    delay(2);
    BMSUtil::getReply(buff, 8);
}

void BMSModuleManager::getAllVoltTemp()
{
    packVolt = 0.0f;
    for (int x = 1; x <= MAX_MODULE_ADDR; x++)
    {
        if (modules[x].isExisting()) 
        {
            Logger::debug("");
            Logger::debug("Module %i exists. Reading voltage and temperature values", x);
            modules[x].readModuleValues();
            Logger::debug("Module voltage: %f", modules[x].getModuleVoltage());
            Logger::debug("Lowest Cell V: %f     Highest Cell V: %f", modules[x].getLowCellV(), modules[x].getHighCellV());
            Logger::debug("Temp1: %f       Temp2: %f", modules[x].getTemperature(0), modules[x].getTemperature(1));
            packVolt += modules[x].getModuleVoltage();
            if (modules[x].getLowTemp() < lowestPackTemp) lowestPackTemp = modules[x].getLowTemp();
            if (modules[x].getHighTemp() > highestPackTemp) highestPackTemp = modules[x].getHighTemp();            
        }
    }

    if (packVolt > highestPackVolt) highestPackVolt = packVolt;
    if (packVolt < lowestPackVolt) lowestPackVolt = packVolt;

    if (digitalRead(13) == LOW) {
        if (!isFaulted) Logger::error("One or more BMS modules have entered the fault state!");
        isFaulted = true;
    }
    else
    {
        if (isFaulted) Logger::info("All modules have exited a faulted state");
        isFaulted = false;
    }
}

float BMSModuleManager::getPackVoltage()
{
    return packVolt;
}

float BMSModuleManager::getAvgTemperature()
{
    float avg = 0.0f;    
    for (int x = 1; x <= MAX_MODULE_ADDR; x++)
    {
        if (modules[x].isExisting()) avg += modules[x].getAvgTemp();
    }
    avg = avg / (float)numFoundModules;

    return avg;
}

float BMSModuleManager::getAvgCellVolt()
{
    float avg = 0.0f;    
    for (int x = 1; x <= MAX_MODULE_ADDR; x++)
    {
        if (modules[x].isExisting()) avg += modules[x].getAverageV();
    }
    avg = avg / (float)numFoundModules;

    return avg;
}

// ??????????????? Could this be refactored ????????????????
// ???????????????to use Protocol Buffers YESSSS!!!!!  ??????????????

void BMSModuleManager::printPackSummary()
{
    uint8_t faults;
    uint8_t alerts;
    uint8_t COV;
    uint8_t CUV;

    Logger::console("");
    Logger::console("");
    Logger::console("");
    Logger::console("                                     Pack Status:");
    if (isFaulted) Logger::console("                                       FAULTED!");
    else Logger::console("                                   All systems go!");
    Logger::console("Modules: %i    Voltage: %fV   Avg Cell Voltage: %fV     Avg Temp: %fC ", numFoundModules, 
                    getPackVoltage(),getAvgCellVolt(), getAvgTemperature());
    Logger::console("");
    for (int y = 1; y < 63; y++)
    {
        if (modules[y].isExisting())
        {
            faults = modules[y].getFaults();
            alerts = modules[y].getAlerts();
            COV = modules[y].getCOVCells();
            CUV = modules[y].getCUVCells();

            Logger::console("                               Module #%i", y);

            Logger::console("  Voltage: %fV   (%fV-%fV)     Temperatures: (%fC-%fC)", modules[y].getModuleVoltage(), 
                            modules[y].getLowCellV(), modules[y].getHighCellV(), modules[y].getLowTemp(), modules[y].getHighTemp());

            SerialUSB.print("  Currently balancing cells: ");
            for (int i = 0; i < 6; i++)
            {                
                if (modules[y].getBalancingState(i) == 1) 
                {                    
                    SerialUSB.print(i);
                    SerialUSB.print(" ");
                }
            }
            SerialUSB.println();

            if (faults > 0)
            {
                Logger::console("  MODULE IS FAULTED:");
                if (faults & 1)
                {
                    SerialUSB.print("    Overvoltage Cell Numbers (1-6): ");
                    for (int i = 0; i < 6; i++)
                    {
                        if (COV & (1 << i)) 
                        {
                            SerialUSB.print(i+1);
                            SerialUSB.print(" ");
                        }
                    }
                    SerialUSB.println();
                }
                if (faults & 2)
                {
                    SerialUSB.print("    Undervoltage Cell Numbers (1-6): ");
                    for (int i = 0; i < 6; i++)
                    {
                        if (CUV & (1 << i)) 
                        {
                            SerialUSB.print(i+1);
                            SerialUSB.print(" ");
                        }
                    }
                    SerialUSB.println();
                }
                if (faults & 4)
                {
                    Logger::console("    CRC error in received packet");
                }
                if (faults & 8)
                {
                    Logger::console("    Power on reset has occurred");
                }
                if (faults & 0x10)
                {
                    Logger::console("    Test fault active");
                }
                if (faults & 0x20)
                {
                    Logger::console("    Internal registers inconsistent");
                }
            }
            if (alerts > 0)
            {
                Logger::console("  MODULE HAS ALERTS:");
                if (alerts & 1)
                {
                    Logger::console("    Over temperature on TS1");
                }
                if (alerts & 2)
                {
                    Logger::console("    Over temperature on TS2");
                }
                if (alerts & 4)
                {
                    Logger::console("    Sleep mode active");
                }
                if (alerts & 8)
                {
                    Logger::console("    Thermal shutdown active");
                }
                if (alerts & 0x10)
                {
                    Logger::console("    Test Alert");
                }
                if (alerts & 0x20)
                {
                    Logger::console("    OTP EPROM Uncorrectable Error");
                }
                if (alerts & 0x40)
                {
                    Logger::console("    GROUP3 Regs Invalid");
                }
                if (alerts & 0x80)
                {
                    Logger::console("    Address not registered");
                }
            }
            if (faults > 0 || alerts > 0) SerialUSB.println();
        }
    }
}

void BMSModuleManager::printPackDetails()
{
    uint8_t faults;
    uint8_t alerts;
    uint8_t COV;
    uint8_t CUV;
    int cellNum = 0;

    Logger::console("");
    Logger::console("");
    Logger::console("");
    Logger::console("                                         Pack Status:");
    if (isFaulted) Logger::console("                                           FAULTED!");
    else Logger::console("                                      All systems go!");
    Logger::console("Modules: %i    Voltage: %fV   Avg Cell Voltage: %fV     Avg Temp: %fC ", numFoundModules, 
                    getPackVoltage(),getAvgCellVolt(), getAvgTemperature());
    Logger::console("");
    for (int y = 1; y < 63; y++)
    {
        if (modules[y].isExisting())
        {
            faults = modules[y].getFaults();
            alerts = modules[y].getAlerts();
            COV = modules[y].getCOVCells();
            CUV = modules[y].getCUVCells();

            SerialUSB.print("Module #");
            SerialUSB.print(y);
            if (y < 10) SerialUSB.print(" ");
            SerialUSB.print("  ");
            SerialUSB.print(modules[y].getModuleVoltage());
            SerialUSB.print("V");
            for (int i = 0; i < 6; i++)
            {
                if (cellNum < 10) SerialUSB.print(" ");
                SerialUSB.print("  Cell");
                SerialUSB.print(cellNum++);
                SerialUSB.print(": ");
                SerialUSB.print(modules[y].getCellVoltage(i));
                SerialUSB.print("V");
                if (modules[y].getBalancingState(i) == 1) SerialUSB.print("*");
                else SerialUSB.print(" ");
            }
            SerialUSB.print("  Neg Term Temp: ");
            SerialUSB.print(modules[y].getTemperature(0));
            SerialUSB.print("C  Pos Term Temp: ");
            SerialUSB.print(modules[y].getTemperature(1)); 
            SerialUSB.println("C");
        }
    }
}

void BMSModuleManager::processCANMsg(CAN_FRAME &frame)
{
    uint8_t battId = (frame.id >> 16) & 0xF;
    uint8_t moduleId = (frame.id >> 8) & 0xFF;
    uint8_t cellId = (frame.id) & 0xFF;
    
    if (moduleId = 0xFF)  //every module
    {
        if (cellId == 0xFF) sendBatterySummary();        
        else 
        {
            for (int i = 1; i <= MAX_MODULE_ADDR; i++) 
            {
                if (modules[i].isExisting()) 
                {
                    sendCellDetails(i, cellId);
                    delayMicroseconds(500);
                }
            }
        }
    }
    else //a specific module
    {
        if (cellId == 0xFF) sendModuleSummary(moduleId);
        else sendCellDetails(moduleId, cellId);
    }
}

// ??????????????? Could this be refactored ????????????????
// ???????????????to use Protocol Buffers YESSSS!!!!!  ??????????????

void BMSModuleManager::sendBatterySummary()
{
    CAN_FRAME outgoing;
    outgoing.id = (0x1BA00000ul) + ((settings.batteryID & 0xF) << 16) + 0xFFFF;
    outgoing.rtr = 0;
    outgoing.priority = 1;
    outgoing.extended = true;
    outgoing.length = 8;

    uint16_t battV = uint16_t(getPackVoltage() * 100.0f);
    outgoing.data.byte[0] = battV & 0xFF;
    outgoing.data.byte[1] = battV >> 8;
    outgoing.data.byte[2] = 0;  //instantaneous current. Not measured at this point
    outgoing.data.byte[3] = 0;
    outgoing.data.byte[4] = 50; //state of charge
    int avgTemp = (int)getAvgTemperature() + 40;
    if (avgTemp < 0) avgTemp = 0;
    outgoing.data.byte[5] = avgTemp;
    avgTemp = (int)lowestPackTemp + 40;
    if (avgTemp < 0) avgTemp = 0;    
    outgoing.data.byte[6] = avgTemp;
    avgTemp = (int)highestPackTemp + 40;
    if (avgTemp < 0) avgTemp = 0;
    outgoing.data.byte[7] = avgTemp;
    Can0.sendFrame(outgoing);
}


// ??????????????? Could this be refactored ????????????????
// ???????????????to use Protocol Buffers YESSSS!!!!!  ??????????????

void BMSModuleManager::sendModuleSummary(int module)
{
    CAN_FRAME outgoing;
    outgoing.id = (0x1BA00000ul) + ((settings.batteryID & 0xF) << 16) + ((module & 0xFF) << 8) + 0xFF;
    outgoing.rtr = 0;
    outgoing.priority = 1;
    outgoing.extended = true;
    outgoing.length = 8;

    uint16_t battV = uint16_t(modules[module].getModuleVoltage() * 100.0f);
    outgoing.data.byte[0] = battV & 0xFF;
    outgoing.data.byte[1] = battV >> 8;
    outgoing.data.byte[2] = 0;  //instantaneous current. Not measured at this point
    outgoing.data.byte[3] = 0;
    outgoing.data.byte[4] = 50; //state of charge
    int avgTemp = (int)modules[module].getAvgTemp() + 40;
    if (avgTemp < 0) avgTemp = 0;
    outgoing.data.byte[5] = avgTemp;
    avgTemp = (int)modules[module].getLowestTemp() + 40;
    if (avgTemp < 0) avgTemp = 0;
    outgoing.data.byte[6] = avgTemp;
    avgTemp = (int)modules[module].getHighestTemp() + 40;
    if (avgTemp < 0) avgTemp = 0;
    outgoing.data.byte[7] = avgTemp;

    Can0.sendFrame(outgoing);
}



// ??????????????? Could this be refactored ????????????????
// ???????????????to use Protocol Buffers YESSSS!!!!!  ??????????????

void BMSModuleManager::sendCellDetails(int module, int cell)
{
    CAN_FRAME outgoing;
    outgoing.id = (0x1BA00000ul) + ((settings.batteryID & 0xF) << 16) + ((module & 0xFF) << 8) + (cell & 0xFF);
    outgoing.rtr = 0;
    outgoing.priority = 1;
    outgoing.extended = true;
    outgoing.length = 8;

    uint16_t battV = uint16_t(modules[module].getCellVoltage(cell) * 100.0f);
    outgoing.data.byte[0] = battV & 0xFF;
    outgoing.data.byte[1] = battV >> 8;
    battV = uint16_t(modules[module].getHighestCellVolt(cell) * 100.0f);
    outgoing.data.byte[2] = battV & 0xFF;
    outgoing.data.byte[3] = battV >> 8;
    battV = uint16_t(modules[module].getLowestCellVolt(cell) * 100.0f);
    outgoing.data.byte[4] = battV & 0xFF;
    outgoing.data.byte[5] = battV >> 8;
    int instTemp = modules[module].getHighTemp() + 40;
    outgoing.data.byte[6] = instTemp; // should be nearest temperature reading not highest but this works too.
    outgoing.data.byte[7] = 0; //Bit encoded fault data. No definitions for this yet.

    Can0.sendFrame(outgoing);
}

//The SerialConsole actually sets the battery ID to a specific value. We just have to set up the CAN filter here to
//match.
void BMSModuleManager::setBatteryID()
{
    //Setup filter for direct access to our registered battery ID
    uint32_t canID = (0xBAul << 20) + (((uint32_t)settings.batteryID & 0xF) << 16);
    Can0.setRXFilter(0, canID, 0x1FFF0000ul, true);
}

int32_t BMSModuleManager::getNumOfModules(){
   return numFoundModules;
}

BMSModule BMSModuleManager::getModules(int i){
    return modules[i];
}
