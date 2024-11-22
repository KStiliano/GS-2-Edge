from flask import Flask, render_template_string
from flask_socketio import SocketIO
import paho.mqtt.client as mqtt
import json

# Constantes para MQTT
MQTT_BROKER = "23.22.163.175"
MQTT_PORT = 1883
MQTT_TOPIC_L = "/TEF/gaco001/attrs/l"
MQTT_TOPIC_P = "/TEF/gaco001/attrs/p"
MQTT_TOPIC_H = "/TEF/gaco001/attrs/h"
MQTT_TOPIC_T = "/TEF/gaco001/attrs/t"

# Inicializa o aplicativo Flask e o SocketIO
app = Flask(__name__)
socketio = SocketIO(app)

# Variáveis globais para armazenar os últimos valores
ultimo_valor_l = None
ultimo_valor_p = None
ultimo_valor_h = None
ultimo_valor_t = None

# Funções de callback do MQTT
def on_connect(client, userdata, flags, rc):
    print("Conectado com código de resultado " + str(rc))
    client.subscribe(MQTT_TOPIC_L)
    client.subscribe(MQTT_TOPIC_P)
    client.subscribe(MQTT_TOPIC_H)
    client.subscribe(MQTT_TOPIC_T)

def on_message(client, userdata, msg):
    global ultimo_valor_l, ultimo_valor_p, ultimo_valor_h, ultimo_valor_t
    try:
        payload = msg.payload.decode('utf-8')

        if msg.topic == MQTT_TOPIC_L:
            ultimo_valor_l = float(payload)
            socketio.emit('novo_dado_l', {'valor': ultimo_valor_l})
            print("Luminosidade recebida:", ultimo_valor_l)

        elif msg.topic == MQTT_TOPIC_P:
            ultimo_valor_p = float(payload)
            socketio.emit('novo_dado_p', {'valor': ultimo_valor_p})
            print("Proximidade recebida:", ultimo_valor_p)

        elif msg.topic == MQTT_TOPIC_H:
            ultimo_valor_h = float(payload)
            socketio.emit('novo_dado_h', {'valor': ultimo_valor_h})
            print("Umidade recebida:", ultimo_valor_h)

        elif msg.topic == MQTT_TOPIC_T:
            ultimo_valor_t = float(payload)
            socketio.emit('novo_dado_t', {'valor': ultimo_valor_t})
            print("Temperatura recebida:", ultimo_valor_t)

    except Exception as e:
        print(f"Erro no processamento da mensagem: {e}")

# Configura o cliente MQTT e conecta
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect(MQTT_BROKER, MQTT_PORT, 60)
client.loop_start()

@app.route('/')
def index():
    return render_template_string('''
        <!DOCTYPE html>
        <html lang="pt-BR">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Visualizador de Dados do Sensor</title>
            <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css">
            <script src="https://code.jquery.com/jquery-3.5.1.min.js"></script>
            <script src="https://cdn.socket.io/4.0.0/socket.io.min.js"></script>
            <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
            <style>
                .chart-container {
                    display: flex;
                    justify-content: space-around;
                    align-items: center;
                    flex-wrap: wrap;
                }
                canvas {
                    max-width: 300px;
                    margin: 20px;
                }
                #valor-l, #valor-p, #valor-h, #valor-t {
                    font-size: 18px;
                    font-weight: bold;
                    text-align: center;
                    margin-top: 10px;
                }
            </style>
        </head>
        <body>
            <div class="container">
                <h1 class="mt-5 text-center">Sensores</h1>
                <div class="chart-container">
                    <div class="text-center">
                        <h3>Luminosidade</h3>
                        <canvas id="gauge-l" width="300" height="300"></canvas>
                        <div id="valor-l">Aguardando dados...</div>
                    </div>
                    <div class="text-center">
                        <h3>Proximidade</h3>
                        <canvas id="gauge-p" width="300" height="300"></canvas>
                        <div id="valor-p">Aguardando dados...</div>
                    </div>
                    <div class="text-center">
                        <h3>Umidade</h3>
                        <canvas id="gauge-h" width="300" height="300"></canvas>
                        <div id="valor-h">Aguardando dados...</div>
                    </div>
                    <div class="text-center">
                        <h3>Temperatura</h3>
                        <canvas id="gauge-t" width="300" height="300"></canvas>
                        <div id="valor-t">Aguardando dados...</div>
                    </div>
                </div>
            </div>
            <script>
                var gaugeChartLuminosidade = new Chart(document.getElementById('gauge-l').getContext('2d'), {
                    type: 'doughnut',
                    data: {
                        labels: ['Luminosidade', 'Restante'],
                        datasets: [{
                            data: [0, 100],
                            backgroundColor: ['#FF4D00', '#E0E0E0']
                        }]
                    },
                    options: { responsive: true, cutout: '70%' }
                });

                var gaugeChartProximidade = new Chart(document.getElementById('gauge-p').getContext('2d'), {
                    type: 'doughnut',
                    data: {
                        labels: ['Proximidade', 'Restante'],
                        datasets: [{
                            data: [0, 400],
                            backgroundColor: ['#FFDE21', '#E0E0E0']
                        }]
                    },
                    options: { responsive: true, cutout: '70%' }
                });

                var gaugeChartUmidade = new Chart(document.getElementById('gauge-h').getContext('2d'), {
                    type: 'doughnut',
                    data: {
                        labels: ['Umidade', 'Restante'],
                        datasets: [{
                            data: [0, 100],
                            backgroundColor: ['#00FFFF', '#E0E0E0']
                        }]
                    },
                    options: { responsive: true, cutout: '70%' }
                });

                var gaugeChartTemperatura = new Chart(document.getElementById('gauge-t').getContext('2d'), {
                    type: 'doughnut',
                    data: {
                        labels: ['Temperatura', 'Restante'],
                        datasets: [{
                            data: [0, 80],
                            backgroundColor: ['#008000', '#E0E0E0']
                        }]
                    },
                    options: { responsive: true, cutout: '70%' }
                });

                $(document).ready(function() {
                    var socket = io.connect('http://' + document.domain + ':' + location.port);

                    socket.on('novo_dado_l', function(data) {
                        $('#valor-l').text('Luminosidade: ' + data.valor + ' %');
                        gaugeChartLuminosidade.data.datasets[0].data = [data.valor, 100 - data.valor];
                        gaugeChartLuminosidade.update();
                    });

                    socket.on('novo_dado_p', function(data) {
                        $('#valor-p').text('Proximidade: ' + data.valor);
                        gaugeChartProximidade.data.datasets[0].data = [data.valor, 400 - data.valor];
                        gaugeChartProximidade.update();
                    });

                    socket.on('novo_dado_h', function(data) {
                        $('#valor-h').text('Umidade: ' + data.valor + ' %');
                        gaugeChartUmidade.data.datasets[0].data = [data.valor, 100 - data.valor];
                        gaugeChartUmidade.update();
                    });

                    socket.on('novo_dado_t', function(data) {
                        $('#valor-t').text('Temperatura: ' + data.valor + ' °C');
                        gaugeChartTemperatura.data.datasets[0].data = [data.valor, 80 - data.valor];
                        gaugeChartTemperatura.update();
                    });
                });
            </script>
        </body>
        </html>
    ''')

if __name__ == '__main__':
    socketio.run(app, debug=True, allow_unsafe_werkzeug=True)
