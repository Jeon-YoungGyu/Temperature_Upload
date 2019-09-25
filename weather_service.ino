//기상청의 RSS로부터 단순히 현재 시각의 기온 정보만 받아오는 것이 아니라
//3시간 단위, 총 18개의 sequence로 나뉘어진 시간대별 기온 예측 정보를 모두 불러와
//기온의 변화 예상 추이를 모두 ThingSpeak로 전송합니다.
//따라서 주기의 꼭지점1을 기준으로 순서에 따라 +3시간째에 해당하는 기온 정보를 담고 있습니다.

#include <WiFi.h>

const char* ssid     = "iptime";
const char* password = "43722734";

const char* host1 = "www.kma.go.kr";
const char* host2 = "api.thingspeak.com";

void setup()
{
    Serial.begin(115200);   
    delay(10);

// We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    }

    int value = 0;

void loop()
{
    delay(5000);
    ++value;

    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    String tmp;
    String temperature;
    int index;
    
    Serial.print("connecting to ");
    Serial.println(host1);

    if (!client.connect(host1, httpPort)) {
    Serial.println("connection failed");
    return;
    }

    // We now create a URI for the request
    String url1 = "/wid/queryDFSRSS.jsp?zone=4117357600";

    Serial.print("Requesting URL: ");
    Serial.println(url1);

    // This will send the request to the server
    client.print(String("GET ") + url1 + " HTTP/1.1\r\n" +
                 "Host: " + host1 + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return;
            }
    }

    // Read all the lines of the reply from server and print them to Serial
    while(client.available()) {
        String line = client.readStringUntil('\n');
        index = line.indexOf("</temp>");
        if (index > 0) {
            tmp = "<temp>";
            tmp = line.substring(line.indexOf(tmp) + tmp.length(), index);
            Serial.println(String(tmp) + "ºC");
            temperature += tmp;
        }
    }
    client.stop();
    Serial.println("closing connection\r\n");



    Serial.print("connecting to ");
    Serial.println(host2);

    // We now create a URI for the request
    String url2 = "/update?api_key=1XUV6BG8I2O4OUEJ&field2=";
    index = 0;

    while (index < temperature.length()) {
        if (!client.connect(host2, httpPort)) {
          Serial.println("connection failed");
          return;
        }
        tmp = temperature.substring(index, index+4);
        url2 += tmp;
        Serial.print("Requesting URL: ");
        Serial.println(url2);

        // This will send the request to the server
        client.print(String("GET ") + url2 + " HTTP/1.1\r\n" +
                     "Host: " + host2 + "\r\n" +
                     "Connection: close\r\n\r\n");
        url2.remove(url2.length() - 4);
        index += 4;
        delay(15000);
        client.stop();
    }
    Serial.println("closing connection\r\n");
    delay(200000);
}
