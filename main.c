/*
  1- use the esp32 as an access point and connect client's device to it.
  2- use a web page to connect to the client's router
  3- blink...
*/

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#define LED_BUILTIN 2

// Set these to your desired credentials.
const char *ssid = "Smart_Method";
const char *password = "RoBot2021";

String html = "<!DOCTYPE html> <html> <head> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\"> <title>Robot X</title> <link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/css/bootstrap.min.css\" integrity=\"sha384-Gn5384xqQ1aoWXA+058RXPxPg6fy4IWvTNh0E263XmFcJlSAwiGgFAW/dAiS6JXm\" crossorigin=\"anonymous\"> <link rel=\"shortcut icon\" href=\"https://cdn.pixabay.com/photo/2017/10/24/00/39/bot-icon-2883144_1280.png\"> <style> html { display: inline-block; margin: 0px auto; text-align: center; } .configure { margin: 5% auto 0px auto; width: 70%; } .headcenter { text-align: center; } </style> </head> <body>";
String html_e = "</body></html>";

String router_ssid = "";
String router_pass = "";

WiFiServer server(80);

// Certificate Authority that signed the server certifcate for the demo server https://jigsaw.w3.org
const char *rootCACertificate =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIF6TCCA9GgAwIBAgIQBeTcO5Q4qzuFl8umoZshQ4zANBgkqhkiG9w0BAQwFADCB\n"
    "iDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0pl\n"
    "cnNleSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNV\n"
    "BAMTJVVTRVJUcnVzdCBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTQw\n"
    "OTEyMDAwMDAwWhcNMjQwOTExMjM1OTU5WjBfMQswCQYDVQQGEwJGUjEOMAwGA1UE\n"
    "CBMFUGFyaXMxDjAMBgNVBAcTBVBhcmlzMQ4wDAYDVQQKEwVHYW5kaTEgMB4GA1UE\n"
    "AxMXR2FuZGkgU3RhbmRhcmQgU1NMIENBIDIwggEiMA0GCSqGSIb3DQEBAQUAA4IB\n"
    "DwAwggEKAoIBAQCUBC2meZV0/9UAPPWu2JSxKXzAjwsLibmCg5duNyj1ohrP0pIL\n"
    "m6jTh5RzhBCf3DXLwi2SrCG5yzv8QMHBgyHwv/j2nPqcghDA0I5O5Q1MsJFckLSk\n"
    "QFEW2uSEEi0FXKEfFxkkUap66uEHG4aNAXLy59SDIzme4OFMH2sio7QQZrDtgpbX\n"
    "bmq08j+1QvzdirWrui0dOnWbMdw+naxb00ENbLAb9Tr1eeohovj0M1JLJC0epJmx\n"
    "bUi8uBL+cnB89/sCdfSN3tbawKAyGlLfOGsuRTg/PwSWAP2h9KK71RfWJ3wbWFmV\n"
    "XooS/ZyrgT5SKEhRhWvzkbKGPym1bgNi7tYFAgMBAAGjggF1MIIBcTAfBgNVHSME\n"
    "GDAWgBRTeb9aqitKz1SA4dibwJ3ysgNmyzAdBgNVHQ4EFgQUs5Cn2MmvTs1hPJ98\n"
    "rV1/Qf1pMOowDgYDVR0PAQH/BAQDAgGGMBIGA1UdEwEB/wQIMAYBAf8CAQAwHQYD\n"
    "VR0lBBYwFAYIKwYBBQUHAwEGCCsGAQUFBwMCMCIGA1UdIAQbMBkwDQYLKwYBBAGy\n"
    "MQECAhowCAYGZ4EMAQIBMFAGA1UdHwRJMEcwRaBDoEGGP2h0dHA6Ly9jcmwudXNl\n"
    "cnRydXN0LmNvbS9VU0VSVHJ1c3RSU0FDZXJ0aWZpY2F0aW9uQXV0aG9yaXR5LmNy\n"
    "bDB2BggrBgEFBQcBAQRqMGgwPwYIKwYBBQUHMAKGM2h0dHA6Ly9jcnQudXNlcnRy\n"
    "dXN0LmNvbS9VU0VSVHJ1c3RSU0FBZGRUcnVzdENBLmNydDAlBggrBgEFBQcwAYYZ\n"
    "aHR0cDovL29jc3AudXNlcnRydXN0LmNvbTANBgkqhkiG9w0BAQwFAAOCAgEAWGf9\n"
    "crJq13xhlhl+2UNG0SZ9yFP6ZrBrLafTqlb3OojQO3LJUP33WbKqaPWMcwO7lWUX\n"
    "zi8c3ZgTopHJ7qFAbjyY1lzzsiI8Le4bpOHeICQW8owRc5E69vrOJAKHypPstLbI\n"
    "FhfFcvwnQPYT/pOmnVHvPCvYd1ebjGU6NSU2t7WKY28HJ5OxYI2A25bUeo8tqxyI\n"
    "yW5+1mUfr13KFj8oRtygNeX56eXVlogMT8a3d2dIhCe2H7Bo26y/d7CQuKLJHDJd\n"
    "ArolQ4FCR7vY4Y8MDEZf7kYzawMUgtN+zY+vkNaOJH1AQrRqahfGlZfh8jjNp+20\n"
    "J0CT33KpuMZmYzc4ZCIwojvxuch7yPspOqsactIGEk72gtQjbz7Dk+XYtsDe3CMW\n"
    "1hMwt6CaDixVBgBwAc/qOR2A24j3pSC4W/0xJmmPLQphgzpHphNULB7j7UTKvGof\n"
    "KA5R2d4On3XNDgOVyvnFqSot/kGkoUeuDcL5OWYzSlvhhChZbH2UF3bkRYKtcCD9\n"
    "0m9jqNf6oDP6N8v3smWe2lBvP+Sn845dWDKXcCMu5/3EFZucJ48y7RetWIExKREa\n"
    "m9T8bJUox04FB6b9HbwZ4ui3uRGKLXASUoWNjDNKD/yZkuBjcNqllEdjB+dYxzFf\n"
    "BT02Vf6Dsuimrdfp5gJ0iHRc2jTbkNJtUQoj1iM=\n"
    "-----END CERTIFICATE-----\n";

// Not sure if WiFiClientSecure checks the validity date of the certificate.
// Setting clock just to be sure...
void setClock()
{
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2)
  {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }
  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
}
void get_name()
{
  for (int i = 0; i < server.args(); i++)
  {
    if (server.argName(i) == "ssid")
    {
      router_ssid += server.arg(i);
    }
    else if (server.argName(i) == "password")
    {
      router_pass += server.arg(i);
    }
  }
}

WiFiMulti WiFiMulti;

void setup()
{

  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  pinMode(LED_BUILTIN, OUTPUT);

  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

  Serial.println("Server started");

  // listen for incoming clients
  WiFiClient client = server.available();

  server.on("/", get_name);

  // if you get a client
  if (client)
  {

    Serial.println("New Client.");

    // loop while the client's connected and print web page
    while (client.connected())
    {
      String login = html;
      login += "<form action=\"index.html\" class=\"configure\" method=\"GET\"> <center><img src=\"https://cdn.pixabay.com/photo/2017/10/24/00/39/bot-icon-2883144_1280.png\" width=\"200\" alt=\"\"> </center> <p class=\"h1 headcenter\">Configuration Router</p> <div class=\"form-group\"> <label for=\"SSID\">Router SSID</label> <input type=\"text\" class=\"form-control\" id=\"SSID\" onfocus=\"this.placeholder = ''\" onblur=\"this.placeholder = 'Enter Your Router\\'s SSID'\" placeholder=\"Enter Your Router's SSID\" name=\"ssid\" required> </div> <div class=\"form-group\"> <label for=\"Password\">Password</label> <input type=\"password\" class=\"form-control\" id=\"Password\" onfocus=\"this.placeholder = ''\" onblur=\"this.placeholder = 'EnterYour Router\\'s Password'\" placeholder=\"Enter Your Router's Password\" name=\"password\" required> </div> <button type=\"submit\" class=\"btn btn-primary\">Configure</button> </form>";
      login += html_e;

      // if there's bytes to read from the client,
      if (client.available())
      {

        client.print(html)

            WiFi.mode(WIFI_STA);

        // user's route info
        WiFiMulti.addAP(router_ssid, router_pass);

        // wait for WiFi connection
        Serial.print("Waiting for WiFi to connect...");
        while ((WiFiMulti.run() != WL_CONNECTED))
        {
          Serial.print(".");
        }

        Serial.println(" connected");
        setClock();

        WiFiClientSecure *client = new WiFiClientSecure;

        if (client)
        {
          client->setCACert(rootCACertificate);

          {
            // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
            char c = client.read(); // read a byte, then
            Serial.write(c);        // print it out the serial monitor
            if (c == '\n')
            { // if the byte is a newline character

              // if the current line is blank, you got two newline characters in a row.
              // that's the end of the client HTTP request, so send a response:
              if (currentLine.length() == 0)
              {
                String cpanel = html;
                // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                // and a content-type so the client knows what's coming, then a blank line:
                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/html");
                client.println();

                // the content of the HTTP response follows the header:
                client.print("Click <a href=\"/H\">here</a> to turn ON the LED.<br>");
                client.print("Click <a href=\"/L\">here</a> to turn OFF the LED.<br>");
                cpanel += html_e;

                // The HTTP response ends with another blank line:
                client.println();
                // break out of the while loop:
                break;
              }
              else
              { // if you got a newline, then clear currentLine:
                currentLine = "";
              }
            }
            else if (c != '\r')
            {                   // if you got anything else but a carriage return character,
              currentLine += c; // add it to the end of the currentLine
            }

            // Check to see if the client request was "GET /H" or "GET /L":
            if (currentLine.endsWith("GET /H"))
            {
              digitalWrite(LED_BUILTIN, HIGH); // GET /H turns the LED on
            }
            if (currentLine.endsWith("GET /L"))
            {
              digitalWrite(LED_BUILTIN, LOW); // GET /L turns the LED off
            }

          } // End extra scoping block
          delete client;
        }
        else
        {
          Serial.println("Unable to create client");
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}

void loop() {}