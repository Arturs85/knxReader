#include "uicom.hpp"
#include <signal.h>

#include <iostream>
#include <thread>
#include <sstream>
#include <time.h>
const std::string UiCom::TAG = "[uicom] ";


void UiCom::sendCount(int count)
{
    std::string com1 = "curl -X PATCH -H \"Content-Type: application/json\" -d '{\"name\":\"simple2\",\"fields\":{\"singleField\":{\"integerValue\":";
    std::string com2 = "}}}' 'https://content-firestore.googleapis.com/v1beta1/projects/ceilingcounterui/databases/(default)/documents/containera/simple2?currentDocument.exists=true&alt=json'";
    std::string com = com1+std::to_string(count)+com2;
    system(com.data());
}

void UiCom::sendLocations(std::string locations)
{
    std::stringstream ss;

    std::string com1 = "curl -X PATCH -H \"Content-Type: application/json\" -d '{ \"name\": \"status\", \"fields\": {\"isStarted\": {\"booleanValue\": false},\"persons\": {\"arrayValue\": {\"values\": [";


    std::string com2 = "]}},\"lastImageTime\": {\"timestampValue\": \"";
    std::string time  = currentDateTime();
    std::string com3 = "\" }}}' 'https://content-firestore.googleapis.com/v1beta1/projects/ceilingcounterui/databases/(default)/documents/containera/status?currentDocument.exists=true&alt=json'";

    ss<<com1<<locations<<com2<<time<<com3;

    std::string com = ss.str();

    system(com.data());

}
void UiCom::sendAir(std::string value,std::string name)
{
    std::stringstream ss;

    std::string com1 = "curl -X PATCH -H \"Content-Type: application/json\" -d '{ \"name\": \""+name+"\", \"fields\": {\"value\": {\"doubleValue\":"; 

    std::string com2 = "},\"lastImageTime\": {\"timestampValue\": \"";
    std::string time  = currentDateTime();
    std::string com3 = "\" }}}' 'https://content-firestore.googleapis.com/v1beta1/projects/ceilingcounterui/databases/(default)/documents/containera/"+name+"?currentDocument.exists=true&alt=json'";

    ss<<com1<<value<<com2<<time<<com3;

    std::string com = ss.str();

    system(com.data());

}
// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const std::string UiCom::currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%dT%XZ", &tstruct);

    return buf;
}
