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
 
<br>
 
## Dependências 📦
<ul>
    <li>WiFi</li>
    <li>PubSubClient</li>
    <li>DHT Sensor Library</li>
</ul>
 
<br>

## Funcionalidades 🛠️
<ul>
    <li>Controle de Energia: Determina o tipo de energia utilizado (solar, eólica ou elétrica) com base na luminosidade.</li>
    <li>Automação de Iluminação: Liga/desliga uma lâmpada com base na proximidade detectada.</li>
    <li>Controle de Temperatura: Liga/desliga o ar-condicionado com base na temperatura do ambiente.</li>
    <li>Controle de Umidade: Liga/desliga o umidificador com base na umidade do ambiente.</li>
    <li>Monitoramento em Tempo Real: Temperatura e umidade; Luminosidade; Proximidade</li> 
    <li>Conectividade MQTT: Publicação de dados dos sensores; Inscrição em tópicos para receber comandos.</li>
</ul>

<br>

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
<br>

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
<br>

## Explicando o <a href="path">Código</a> 🧑‍💻
 








<hr>
 
<center>Este projeto encontra sob a <a href="path">MIT License.</a></center>
