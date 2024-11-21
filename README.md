# GS 2 - Edge Computing

## Integrantes 👋
<ul>
    <li>João Marcelo Furtado Romero (RM555199)</li>
    <li>Kayky Silva Stiliano (RM555148)</li>
</ul>
 
<img src="path" alt="print do circuito ESP32/printscreen ESP32 circuit"/>

Link da simulação no <a href="path">Wokwi</a>

## Explicação do Projeto 📖

## Componentes ⚙️
<ul>
    <li>1x Placa ESP32</li>
    <li>2x Breadboard</li>
    <li>1x DHT11/22</li>
    <li>1x LDR</li>
    <li>5x LEDs</li>
    <li>1x Sensor de proximidade (HC-SR04)</li>
    <li>5x Resistores</li>
</ul>
 
## Dependências 📦
<ul>
    <li>WiFi</li>
    <li>PubSubClient</li>
    <li>DHT Sensor Library</li>
</ul>
 
## Funcionalidades 🛠️
<ul>
    <li>Controle de Energia: Determina o tipo de energia utilizado (solar, eólica ou elétrica) com base na luminosidade.</li>
    <li>Automação de Iluminação: Liga/desliga uma lâmpada com base na proximidade detectada.</li>
    <li>Controle de Temperatura: Liga/desliga o ar-condicionado com base na temperatura do ambiente.</li>
    <li>Controle de Umidade: Liga/desliga o umidificador com base na umidade do ambiente.</li>
    <li>Monitoramento em Tempo Real: Temperatura e umidade; Luminosidade; Proximidade</li> 
    <li>Conectividade MQTT: Publicação de dados dos sensores; Inscrição em tópicos para receber comandos.</li>
</ul>

## 🖥️⚙️ Configuração do Código

### Credenciais de Rede Wi-Fi

```cpp
const char* default_SSID = "Wokwi-GUEST";
const char* default_PASSWORD = "";
```

### Configurações do Broker MQTT

```cpp
const char* default_BROKER_MQTT = "23.22.163.175";
const int default_BROKER_PORT = 1883;
```

### Tópicos MQTT

| Tópico             | Função                                |
|--------------------|---------------------------------------|
| `/TEF/gaco001/cmd` | Comandos recebidos pelo dispositivo.  |
| `/TEF/gaco001/attrs` | Publicação de dados gerais.        |
| `/TEF/gaco001/attrs/t` | Publicação de temperatura.       |
| `/TEF/gaco001/attrs/h` | Publicação de umidade.           |
| `/TEF/gaco001/attrs/l` | Publicação de luminosidade.      |
| `/TEF/gaco001/attrs/p` | Publicação de proximidade.       |

---

### Pinagem do Projeto

| Componente         | Pino ESP32  |
|---------------------|-------------|
| DHT22 (dados)       | 16          |
| Ultrassônico (trigger) | 17      |
| Ultrassônico (echo) | 18          |
| LDR (entrada analógica) | 34      |
| LED Branco          | 12          |
| LED Laranja         | 14          |
| LED Amarelo         | 26          |
| LED Verde           | 25          |
| LED Ciano           | 27          |

---

## Explicando o <a href="path">Código</a> 🧑‍💻
```c
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
```
Descrição: Configura os pinos de entrada/saída, inicializa os sensores, conecta ao Wi-Fi e MQTT, e configura o sistema para começar o monitoramento.
<br>
Etapas principais:
<ul>
    <li>Configura todos os LEDs como saídas e os sensores como entradas.</li> 
    <li>Inicializa o LED onboard com piscadas para indicar o início do programa.</li> 
    <li>Estabelece conexão Wi-Fi e MQTT.</li> 
    <li>Publica uma mensagem inicial no tópico principal do MQTT (/TEF/gaco001/attrs).</li>
</ul>

<br>

```c
void initWiFi() {
    delay(10);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(default_SSID);
    reconectWiFi();
}
```
Descrição: Conecta o ESP32 à rede Wi-Fi definida nas constantes.
<br>
Etapas principais:
<ul>
    <li>Mostra no monitor serial que a conexão está sendo estabelecida.</li>
    <li>Tenta conectar repetidamente até obter sucesso.</li>
    <li>Exibe o IP atribuído ao ESP32.</li>
</ul>

<br>

```c
void initMQTT() {
    MQTT.setServer(default_BROKER_MQTT, default_BROKER_PORT);
    MQTT.setCallback(mqtt_callback);
}
```
Descrição: Configura o cliente MQTT com o broker especificado e define a função de callback para lidar com mensagens recebidas.
<br>
Etapas principais:
<ul>
    <li>Configura o endereço do broker e a porta.</li>
    <li>Associa a função de callback mqtt_callback().</li>
</ul>

<br>

```c
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
```
Descrição: Processa mensagens recebidas via MQTT.
<br>
Como funciona:
<ul>
    <li>Constrói a mensagem recebida transformando o payload em uma string.</li>
    <li>Verifica o tópico da mensagem recebida:</li>
    <ul>
        <li>/TEF/gaco001/attrs/t: Atualiza a temperatura e controla o ar-condicionado.</li>
        <li>/TEF/gaco001/attrs/h: Atualiza a umidade e controla o umidificador.</li>
        <li>/TEF/gaco001/attrs/p: Atualiza a proximidade e controla a lâmpada.</li>
        <li>/TEF/gaco001/attrs/l: Atualiza a luminosidade e controla o tipo de energia.</li>
    </ul>
</ul>

<br>

```c
void VerificaConexoesWiFIEMQTT() {
    if (!MQTT.connected()) reconnectMQTT();
    reconectWiFi();
}
```
Descrição: Garante que o ESP32 esteja conectado ao Wi-Fi e ao broker MQTT.
<br>
Como funciona:
<ul>
    <li>Se o Wi-Fi estiver desconectado, tenta reconectar chamando reconectWiFi().</li>
    <li>Se o MQTT estiver desconectado, chama reconnectMQTT().</li>
</ul>

<br>

```c
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
```
Descrição: Tenta reconectar ao broker MQTT até obter sucesso.
<br>
Etapas principais:
<ul>
    <li>Tenta conectar com o ID MQTT configurado.</li>
    <li>Após reconectar, reinscreve-se nos tópicos necessários.</li>
    <li>Exibe mensagens de status no monitor serial.</li>
</ul>

<br>

































<hr>
 
<center>Este projeto encontra sob a <a href="path">MIT License.</a></center>
