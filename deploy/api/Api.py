from flask import Flask, jsonify, request
import base64
import json
import socket

def sentoServer(msg):
    # Crear un socket UDP y enviar el mensaje al servidor
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server_address = ('127.0.0.1', 6666)
    sock.sendto(msg, server_address)

    # Esperar la respuesta del servidor
    data, server = sock.recvfrom(4096)
    response = data.decode()
    return (response)  # Debería imprimir "Recibido"

def codificar_base64_json(data):
    texto= json.dumps(data)
    texto_bytes = texto.encode('utf-8')
    base64_bytes = base64.b64encode(texto_bytes)
    base64_texto = base64_bytes.decode('utf-8')
    return base64_bytes

def decodificar_base64(base64_texto):
    # Decodificamos los datos en base64
    ##decoded_data = base64.b64decode(data)
    # Convertimos los datos decodificados en formato JSON
    ##texto = json.loads(decoded_data)
    ##return texto

    base64_bytes = base64_texto.encode('utf-8')
    texto_bytes = base64.b64decode(base64_bytes)
    texto = texto_bytes.decode('utf-8')
    data = json.loads(texto)
    return data

app = Flask(__name__)

juegos = {}  # diccionario para almacenar los juegos

@app.route('/CrazyChess/start-game', methods=['POST'])
def crear_juego():
    # Obtenemos los datos enviados en la petición
    data = request.json

    # Obtenemos los datos en formato json
    data_json = codificar_base64_json(data)

    #respuesta = sentoServer(data_json)
    #data = decodificar_base64(respuesta)
    
    
    return jsonify(data), 201

@app.route('/CrazyChess/update', methods=['GET'])
def obtener_movimientos():
    # Obtenemos los datos enviados en la petición
    data = request.json

    # Obtenemos los datos en formato json
    data_json = codificar_base64_json(data)

    #respuesta = sentoServer(data_json)
    #data = decodificar_base64(respuesta)
    
    return jsonify(data), 201

@app.route('/CrazyChess/move-piece', methods=['PUT'])
def aplicar_movimiento():
    # Obtenemos los datos enviados en la petición
    data = request.json

    # Obtenemos los datos en formato json
    data_json = codificar_base64_json(data)

    #respuesta = sentoServer(data_json)
    #data = decodificar_base64(respuesta)
    
    return jsonify(data), 201


@app.route('/CrazyChess/create-game', methods=['GET'])
def create_game():
    return jsonify({}), 201

@app.route('/CrazyChess/join-game', methods=['GET'])
def join_game():
     # Obtenemos los datos enviados en la petición
    data = request.json

    # Obtenemos los datos en formato json
    data_json = codificar_base64_json(data)

    #respuesta = sentoServer(data_json)
    #data = decodificar_base64(respuesta)
    
    return jsonify(data), 201

if __name__ == '__main__':
    app.run(host='127.0.0.2', port=80)