#include "WebServer.h"

#include "IPAddress.h"

#include "Hash.h"
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <Arduino.h>

static AsyncWebServer gServer(80);
static IPAddress gIp(0,0,0,0);

WebServer::WebServer(StateManager* state_manager ) : mServer( &gServer ), mIp( &gIp ), mStateManager( state_manager )
{
    //called from global construction, not from setup function.
}

void WebServer::Init()
{
    mServer->on("/", HTTP_GET, [this](AsyncWebServerRequest *x){this->OnIndex(x);});

    mServer->on("/change_device_type", HTTP_GET, [this](AsyncWebServerRequest *x){this->OnSetDeviceType(x);});
    mServer->on("/change_led_count", HTTP_GET, [this](AsyncWebServerRequest *x){this->OnSetLedCount(x);});
    mServer->on("/change_brightness", HTTP_GET, [this](AsyncWebServerRequest *x){this->OnSetBrightness(x);});
    mServer->on("/change_group", HTTP_GET, [this](AsyncWebServerRequest *x){this->OnSetGroup(x);});
    mServer->on("/change_name", HTTP_GET, [this](AsyncWebServerRequest *x){this->OnSetName(x);});

    mServer->begin();
}

void WebServer::Update()
{
    if( mNewDeviceTypeFlag )
    {
        //this calls update from the Loop context.
        mStateManager->SetDeviceType( mNewDeviceType );
        mNewDeviceTypeFlag = false;
    }  

    if( mNewLedCountFlag )
    {
        mStateManager->SetLedCount( mNewLedCount );
        mNewLedCountFlag = false;
    }   

    if( mNewBrightnessFlag )
    {
        mStateManager->SetBrightness( mNewBrightness );
        mNewBrightnessFlag = false;
    }  

    if( mNewGroupFlag )
    {
        mStateManager->SetGroup( mNewGroup );
        mNewGroupFlag = false;
    }  

    if( mNewNameFlag )
    {
        const char* new_name = mNewName.c_str();
        mStateManager->SetDeviceName( mNewName.c_str() );
        mNewNameFlag = false;
    }  
}

void WebServer::OnIndex(AsyncWebServerRequest* request)
{
    Serial.println("\n\nrequest received");

    const State* current_state = mStateManager->GetState();

    String contents = "";

    TableHelper helper;
    helper.AddRow("Device Type", current_state->mDeviceType == device_type_t::DIMMER ? "Dimmer" : "LED");
    helper.AddRow("LED Count", String(current_state->mLedCount));
    helper.AddRow("Brightness", String(current_state->mBrightness));
    helper.AddRow("Group", String(current_state->mGroup));
    helper.AddRow("Name", String(current_state->mDeviceName));
    helper.AddRow("Local State", String(current_state->mLocalState));
    helper.AddRow("Network State", String(current_state->mNetworkState[current_state->mGroup]));
    helper.AddRow("Auto Advance", String(current_state->mAutoAdvance ? "True" : "False"));
    helper.AddRow("Auto Advance Time", String(current_state->mAutoAdvanceTime));
    helper.AddRow("Next State", String(current_state->mNextState));
    helper.AddRow("Auto Disable On Next State", String(current_state->mAutoDisableOnNextState ? "True" : "False"));
    switch(current_state->mMode)
    {
        case device_mode_t::NETWORK:
            helper.AddRow("Mode", "Network");
            break;
        case device_mode_t::INDIVIDUAL:
            helper.AddRow("Mode", "Individual");
            break;
        case device_mode_t::OPENPIXEL:
            helper.AddRow("Mode", "Open Pixel Control");
            break;
    }

    helper.AddRow("Enabled", String(current_state->mEnabled ? "True" : "False"));
    helper.AddRow("Broadcast Required", String(current_state->mNetworkBroadcastRequired ? "True" : "False"));

    contents += helper.ToString();
    helper.Clear();

    FormHelper form;
    form.AddLabel("Change Device Type");
    form.AddRatio("mDeviceType", "DIMMER", "Dimmer", (current_state->mDeviceType == device_type_t::DIMMER));
    form.AddRatio("mDeviceType", "LED", "LED", (current_state->mDeviceType == device_type_t::LED));
    form.AddSubmitButton();
    contents += form.ToString("change_device_type");
    form.Clear();

    form.AddLabel("Change LED Count");
    form.AddNumberInput("mLedCount", 0, 100, (int)current_state->mLedCount);
    form.AddSubmitButton();
    contents += form.ToString("change_led_count");
    form.Clear();

    form.AddLabel("Change Brightness");
    form.AddNumberInput("mBrightness", 0, 255, (int)current_state->mBrightness);
    form.AddSubmitButton();
    contents += form.ToString("change_brightness");
    form.Clear();

    form.AddLabel("Change Group");
    form.AddNumberInput("mGroup", 0, 7, (int)current_state->mGroup);
    form.AddSubmitButton();
    contents += form.ToString("change_group");
    form.Clear();

    form.AddLabel("Change Name");
    form.AddTextBox("mDeviceName", String(current_state->mDeviceName));
    form.AddSubmitButton();
    contents += form.ToString("change_name");
    form.Clear();

    contents = "<html>\n<head>\n<title>Mesh Device</title></head>\n<body>\n" + contents + "</body>\n</html>\n";

    request->send(200, "text/html", contents);
    //print all state variables.

    //request->send(200, "text/html", "<form>Text to Broadcast<br><input type='text' name='BROADCAST'><br><br><input type='submit' value='Submit'></form>");
}

void WebServer::OnSetDeviceType(AsyncWebServerRequest* request)
{
    const char* arg_name = "mDeviceType";
    if(!request->hasArg(arg_name))
    {
        request->send(200, "text/html", String("failed to detect parameter") +  String(arg_name));
        return;
    }

    String arg = request->arg(arg_name);


    device_type_t new_type;
    if( arg == "LED" )
    {
        new_type = device_type_t::LED;
    }
    else if( arg == "DIMMER")
    {
        new_type = device_type_t::DIMMER;
    }
    else
    {
        request->send(200, "text/html", "invalid arg.");
        return;
    }

    mNewDeviceType = new_type;
    mNewDeviceTypeFlag = true;
    request->redirect("/");

}
void WebServer::OnSetLedCount(AsyncWebServerRequest* request)
{
    const char* arg_name = "mLedCount";
    if(!request->hasArg(arg_name))
    {
        request->send(200, "text/html", String("failed to detect parameter") +  String(arg_name));
        return;
    }

    String arg = request->arg(arg_name);


    uint16_t new_value = arg.toInt();

    mNewLedCount = new_value;
    mNewLedCountFlag = true;
    request->redirect("/");
      
}
void WebServer::OnSetBrightness(AsyncWebServerRequest* request)
{

    const char* arg_name = "mBrightness";
    if(!request->hasArg(arg_name))
    {
        request->send(200, "text/html", String("failed to detect parameter") +  String(arg_name));
        return;
    }

    String arg = request->arg(arg_name);


    uint8_t new_value = arg.toInt();

    mNewBrightness = new_value;
    mNewBrightnessFlag = true;
    request->redirect("/");
}

void WebServer::OnSetGroup(AsyncWebServerRequest* request)
{
    const char* arg_name = "mGroup";
    if(!request->hasArg(arg_name))
    {
        request->send(200, "text/html", String("failed to detect parameter") +  String(arg_name));
        return;
    }

    String arg = request->arg(arg_name);


    uint8_t new_value = arg.toInt();

    mNewGroup = new_value;
    mNewGroupFlag = true;
    request->redirect("/");
}

void WebServer::OnSetName(AsyncWebServerRequest* request)
{
    const char* arg_name = "mDeviceName";
    if(!request->hasArg(arg_name))
    {
        request->send(200, "text/html", String("failed to detect parameter") +  String(arg_name));
        return;
    }

    String new_value = request->arg(arg_name);


    mNewName = new_value;
    mNewNameFlag = true;
    request->redirect("/");
}

TableHelper::TableHelper()
{
    mWorkingString = "";
}

void TableHelper::AddRow(const String& r1)
{
    mWorkingString += "<tr><td>" + r1 + "</td></tr>\n";
}

void TableHelper::AddRow(const String& r1, const String& r2)
{
    mWorkingString += "<tr><td>" + r1 + "</td><td>" + r2 + "</td></tr>\n";
}
String TableHelper::ToString()
{
    char* header = "<table>\n";
    char* footer = "</table>\n";
    return String(header) + mWorkingString + String(footer);
}

void TableHelper::Clear()
{
    mWorkingString = "";
}

FormHelper::FormHelper()
{
    mWorkingString = "";
}
void FormHelper::AddLabel(const String& label)
{
    mWorkingString += "<Label>" + label + "</Label><br>\n";
}
void FormHelper::AddTextBox(const String& name, const String& existing_value)
{
    //<input type='text' name='mDeviceName' value='existing name here'>
    mWorkingString += "<input type='text' name='" + name + "' value='" + existing_value + "'/><br>\n";
}
void FormHelper::AddRatio(const String& name, const String& value, const String& contents, bool checked)
{
    //<input type="radio" name="mDeviceType" value="LED" checked>LED<br>
    mWorkingString += "<input type='radio' name='" + name + "' value='" + value + "' " + (checked ? "checked" : "") + ">" + contents + "<br>\n";
}
void FormHelper::AddNumberInput(const String& name, int min_value, int max_value, int existing_value)
{
    //<input type='number' name='quantity' min='1' max='5'>
    mWorkingString += "<input type='number' name='" + name + "' min='" + String(min_value) +  "' max='" + String(max_value) + "' value='" + String(existing_value) + "'><br>\n";
}
void FormHelper::AddSubmitButton()
{
    mWorkingString += "<input type='submit' value='Submit'><br>\n";
}
String FormHelper::ToString(const String& action)
{
    return String("<form action=\"") + action + String("\">\n") + mWorkingString + String("</form><br>\n");
}
void FormHelper::Clear()
{
    mWorkingString = "";
}
