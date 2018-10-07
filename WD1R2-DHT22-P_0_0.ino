/**Sketch for reading temperature and humidity
 * using the DHT22 and WEMOS D1 R2 with ESP8266
 * Version: WD1R2-DHT22-P_0_0 07 October 2018
 * Production Code - Release 0 version 0
 * Updates include: New DB on Pi3 aka Pecan (Pecan_DB)
*/

//Include Libraries to be used in Sketch
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <DHT.h>

//Define DHT Parameters such as type, the pin used on the Wemos D1 R2 with ESP8266
#define DHTPIN D5     // what digital pin the DHT22 is conected to
#define DHTTYPE DHT22   // there are multiple kinds of DHT sensors
DHT dht(DHTPIN, DHTTYPE);

// MySQL Setup and Parameters
   //IP Address of Pecan (Raspberry Pi3)
IPAddress server_addr(192, 168, 1, 104);

char user[] = "OldJelly";
char password[] = "JellyFish69";

// MySQL/MariaDB Insert string setup with DB Name.Table Name followed by Columns and then Values to be inserted into Table.
char INSERT_DATA[] = "INSERT INTO Pecan_PRD.RmtDHT_01 (SensorInfo, SensorTemp, SensorHumid, SensorHI) VALUES ('Pecan Sensor',%s,%s,%s)";

//Create buffers for storing data to be inserted into te database.
char query[128];
char temp2[10];
char hum2[10];
char heat2[10];

// EZP8266 WiFi Credentials
char ssid[] = "Pretzel_Shark"; // SSID NAME
char pass[] = "190497PrB"; // SSID PASSWORD

WiFiClient client;
MySQL_Connection conn((Client *)&client);


void setup() {
  Serial.begin(115200);
  Serial.setTimeout(10000);

  // Wait for serial to initialize.
  while (!Serial) { }

  //Establish WiFi connection to Home Network
  WiFi.begin(ssid, pass);
  delay(1000);


  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
  Serial.println ( "" );
  Serial.print ( "Connected to " );
  Serial.println ( ssid );
  Serial.print ( "IP address: " );
  Serial.println ( WiFi.localIP() );


  Serial.println("Device Started");
  Serial.println("-------------------------------------");
  Serial.println("Running DHT!");
  Serial.println("-------------------------------------");

}

int timeSinceLastRead = 0;
void loop() {
  // Report every 20 or so seconds.
  if (timeSinceLastRead > 20000) {

    //Establish MySQL Database Connection and wait till connection is available.
    Serial.println("DB - Connecting...");
    while (conn.connect(server_addr, 3306, user, password) != true) {
      delay(500);
      Serial.print ( "." );
    }

    //Call Functions to read DHT22 Sensor and Input Data into MySQL DB
    doMyTempies();
    delay(10000);
    doSQLInputy();

    //    timeSinceLastRead = 0;
  }

  // close the database connection
  conn.close();
  delay(100);
  timeSinceLastRead += 100;
//  }
}

//Function to Read DHT Sensor and Format for Input into MySQL DB
void doMyTempies() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();


  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    timeSinceLastRead = 0;
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

//Convert Temperature, Humidity and Heat Index Float into a String.
  dtostrf(t, 2, 2, temp2);
  dtostrf(h, 2, 2, hum2);
  dtostrf(hic, 2, 2, heat2);


//Just a bit of Serial Print of Temperature, Humidity and Heat Index for Debugging and Validation all is working when plugged into Puter/Raspberry Pi and Arduino.
  Serial.print("Temperature   ");
  Serial.print(temp2);
  Serial.print("   Humidity: ");
  Serial.print(hum2);
  Serial.print(" %\t");
  Serial.print(" *C ");
  Serial.print("   Heat index: ");
  Serial.print(heat2);
  Serial.println(" *C ");
  Serial.println(" * * * * * * * * ");

}

// Input Data into MySQL Database
void doSQLInputy() {
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  sprintf(query, INSERT_DATA, temp2, hum2, heat2);
  delay(5000);
  cur_mem->execute(query);
  Serial.println(query);
//Free up memory by deleting data stored MySQL cursor insert.
  delete cur_mem;

  delay(5000);

}
