#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// Configurações - variáveis editáveis
const char* default_SSID = "Wokwi-GUEST"; // Nome da rede Wi-Fi
const char* default_PASSWORD = ""; // Senha da rede Wi-Fi
const char* default_BROKER_MQTT = "23.22.163.175"; // IP do Broker MQTT
const int default_BROKER_PORT = 1883; // Porta do Broker MQTT
const char* default_TOPICO_SUBSCRIBE = "/TEF/gaco001/cmd"; // Tópico MQTT que escuta
const char* default_TOPICO_PUBLISH_1 = "/TEF/gaco001/attrs"; // Tópico MQTT de envio
const char* default_TOPICO_PUBLISH_2 = "/TEF/gaco001/attrs/t"; // Tópico MQTT - temperatura
const char* default_TOPICO_PUBLISH_3 = "/TEF/gaco001/attrs/h"; // Tópico MQTT - umidade
const char* default_TOPICO_PUBLISH_4 = "/TEF/gaco001/attrs/p"; // Tópico MQTT - proximidade
const char* default_TOPICO_PUBLISH_5 = "/TEF/gaco001/attrs/l"; // Tópico MQTT - luminosidade
const char* default_ID_MQTT = "fiware_001"; // ID MQTT
const int default_D4 = 2; // Pino do LED onboard

// Configurações dos sensores
#define DHTPIN 16           
#define DHTTYPE DHT22       
#define TRIGGER_PIN 17      
#define ECHO_PIN 18         
const int ldrPin = 34;     

// Configurações dos LEDs
const int led_white = 12;   // LED branco (eólica)
const int led_orange = 14; // LED laranja (solar)
const int led_cyan = 27;    // LED ciano (umidificador)
const int led_yellow = 26; // LED amarelo (lâmpada)
const int led_green = 25;  // LED verde (ar-condicionado)

// Variáveis para configurações
char* SSID = const_cast<char*>(default_SSID);
char* PASSWORD = const_cast<char*>(default_PASSWORD);
char* BROKER_MQTT = const_cast<char*>(default_BROKER_MQTT);
int BROKER_PORT = default_BROKER_PORT;
char* TOPICO_SUBSCRIBE = const_cast<char*>(default_TOPICO_SUBSCRIBE);
char* TOPICO_PUBLISH_1 = const_cast<char*>(default_TOPICO_PUBLISH_1);
char* TOPICO_PUBLISH_2 = const_cast<char*>(default_TOPICO_PUBLISH_2);
char* TOPICO_PUBLISH_3 = const_cast<char*>(default_TOPICO_PUBLISH_3);
char* TOPICO_PUBLISH_4 = const_cast<char*>(default_TOPICO_PUBLISH_4);
char* TOPICO_PUBLISH_5 = const_cast<char*>(default_TOPICO_PUBLISH_5);
char* ID_MQTT = const_cast<char*>(default_ID_MQTT);
int D4 = default_D4;

WiFiClient espClient;
PubSubClient MQTT(espClient);
DHT dht(DHTPIN, DHTTYPE);

char EstadoSaida = '0';
int luminosidade = 0;
int temperatura = 0;
int umidade = 0;
int proximidade = 0;

void initSerial() {
    Serial.begin(115200);
}

void initWiFi() {
    delay(10);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(default_SSID);
    reconectWiFi();
}

void initMQTT() {
    MQTT.setServer(default_BROKER_MQTT, default_BROKER_PORT);
    MQTT.setCallback(mqtt_callback);
}

void InitOutput() {
    pinMode(D4, OUTPUT);
    digitalWrite(D4, HIGH);
    for (int i = 0; i <= 10; i++) {
        digitalWrite(D4, !digitalRead(D4));
        delay(200);
    }
}

void setup() {
    // Inicializa os LEDs
    pinMode(led_white, OUTPUT);
    pinMode(led_orange, OUTPUT);
    pinMode(led_cyan, OUTPUT);
    pinMode(led_yellow, OUTPUT);
    pinMode(led_green, OUTPUT);
    pinMode(D4, OUTPUT);

    // Configurações dos sensores
    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    // Desliga todos os LEDs no início
    digitalWrite(led_white, LOW);
    digitalWrite(led_orange, LOW);
    digitalWrite(led_cyan, LOW);
    digitalWrite(led_yellow, LOW);
    digitalWrite(led_green, LOW);
    digitalWrite(D4, HIGH);

    InitOutput();
    initSerial();
    initWiFi();
    initMQTT();
    dht.begin();

    delay(5000);
    MQTT.publish(default_TOPICO_PUBLISH_1, "s|on");
}

void loop() {
    VerificaConexoesWiFIEMQTT();
    handleSensors();
    getProximity();
    handleLuminosity();
    EnviaEstadoOutputMQTT();
    MQTT.loop();
}

// Função de reconexão Wi-Fi
void reconectWiFi() {
    if (WiFi.status() == WL_CONNECTED) return;
    WiFi.begin(default_SSID, default_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("Conectado com sucesso na rede.");
    Serial.print("IP obtido: ");
    Serial.println(WiFi.localIP());
}

// Função para reconectar ao broker MQTT
void VerificaConexoesWiFIEMQTT() {
    if (!MQTT.connected()) reconnectMQTT();
    reconectWiFi();
}

void reconnectMQTT() {
    while (!MQTT.connected()) {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(default_BROKER_MQTT);
        if (MQTT.connect(default_ID_MQTT)) {
            Serial.println("Conectado com sucesso ao broker MQTT!");
            if (MQTT.subscribe("/TEF/gaco001/attrs/t")) {
                Serial.println("Inscrito no tópico /TEF/gaco001/attrs/t");
            }
            if (MQTT.subscribe("/TEF/gaco001/attrs/h")) {
                Serial.println("Inscrito no tópico /TEF/gaco001/attrs/h");
            }
            if (MQTT.subscribe("/TEF/gaco001/attrs/l")) {
                Serial.println("Inscrito no tópico /TEF/gaco001/attrs/l");
            }
            if (MQTT.subscribe("/TEF/gaco001/attrs/p")) {
                Serial.println("Inscrito no tópico /TEF/gaco001/attrs/p");
            }
        } else {
            Serial.println("Falha ao reconectar. Tentativa em 2s.");
            delay(2000);
        }
    }
}

// Função de callback MQTT
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    String msg;
    for (int i = 0; i < length; i++) {
        msg += (char)payload[i];
    }
    Serial.print("- Mensagem recebida no tópico ");
    Serial.print(topic);
    Serial.print(": ");
    Serial.println(msg);
    // Verifica o tipo de mensagem recebida
    if (String(topic) == "/TEF/gaco001/attrs/t") {
        temperatura = msg.toFloat();
        controlaArCondicionado();
        delay(1500);
    } else if (String(topic) == "/TEF/gaco001/attrs/h") {
        umidade = msg.toFloat();
        controlaUmidificador();
        delay(1500);
    } else if (String(topic) == "/TEF/gaco001/attrs/p") {
        proximidade = msg.toFloat();
        controlaLampada();
        delay(1500);
    } else if (String(topic) == "/TEF/gaco001/attrs/l") {
        luminosidade = msg.toInt();
        controlaEnergia();
        delay(1500);
    } else {
        Serial.println("Mensagem desconhecida.");
    }
}

// Função para controlar o ar-condicionado
void controlaArCondicionado() {
    if (temperatura > 25) {
        digitalWrite(led_green, HIGH); 
        Serial.println("-> Ar-condicionado ligado (LED verde ON).");
    } else {
        digitalWrite(led_green, LOW); 
        Serial.println("-> Ar-condicionado desligado (LED verde OFF).");
    }
}

// Função para controlar o umidificador
void controlaUmidificador() {
    if (umidade < 40) {
        digitalWrite(led_cyan, HIGH); 
        Serial.println("-> Umidificador ligado (LED ciano ON).");
    } else {
        digitalWrite(led_cyan, LOW); 
        Serial.println("-> Umidificador desligado (LED ciano OFF).");
    }
}

// Função para controlar o tipo de energia
void controlaEnergia() {
    // Desliga todos os LEDs de energia
    digitalWrite(led_white, LOW);
    digitalWrite(led_orange, LOW);

    if (luminosidade > 70) {
        digitalWrite(led_orange, HIGH); 
        Serial.println("-> Energia solar ativada (LED laranja ON).");
    } else if (luminosidade > 40) {
        digitalWrite(led_white, HIGH);
        Serial.println("-> Energia eólica ativada (LED branco ON).");
    } else {
        Serial.println("-> Energia elétrica padrão ativada.");
    }
}

// Função para controlar a lâmpada
void controlaLampada() {
    if (proximidade < 60) {
        digitalWrite(led_yellow, HIGH); 
        Serial.println("-> Lâmpada ligada (LED amarelo ON).");
    } else {
        digitalWrite(led_yellow, LOW); 
        Serial.println("-> Lâmpada desligada (LED amarelo OFF).");
    }
}

// Função para lidar com sensores
void handleSensors() {
    // Temperatura e umidade
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    if (!isnan(temperature)) {
        Serial.print("Temperatura: ");
        Serial.println(temperature);
        MQTT.publish(TOPICO_PUBLISH_2, String(temperature).c_str());
    }
    if (!isnan(humidity)) {
        Serial.print("Umidade: ");
        Serial.println(humidity);
        MQTT.publish(TOPICO_PUBLISH_3, String(humidity).c_str());
    }
}

// Função de leitura de proximidade
void getProximity() {
    digitalWrite(TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH);
    proximidade = duration * 0.034 / 2;
    Serial.print("Proximidade: ");
    Serial.println(proximidade);
    MQTT.publish(TOPICO_PUBLISH_4, String(proximidade).c_str());
}

// Função de controle de luminosidade
void handleLuminosity() {
    int ldrValue = analogRead(ldrPin);
    luminosidade = map(ldrValue, 4095, 0, 0, 100);
    Serial.print("Luminosidade: ");
    Serial.println(luminosidade);
    MQTT.publish(TOPICO_PUBLISH_5, String(luminosidade).c_str());
}

// Envia o estado de saída via MQTT
void EnviaEstadoOutputMQTT() {
    String msg = "s|" + String(EstadoSaida);
    MQTT.publish(default_TOPICO_PUBLISH_1, msg.c_str());
}