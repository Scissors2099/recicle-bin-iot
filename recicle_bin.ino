#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <Servo.h>

// --- CONFIGURACIÓN DE PINES ---
const int PIN_IR = 14;      // D5
const int PIN_SERVO = 12;    // D6
const int PIN_BUZZER = 13;   // D7

const int DETECTADO = LOW;

// --- ÁNGULOS Y CONFIGURACIÓN ---
const int TAPA_CERRADA = 0;
const int TAPA_ABIERTA = 90; 

unsigned long TIEMPO_ABIERTO = 3000; 

Servo tapa;
bool abierta = false;
unsigned long momentoApertura = 0;
int contadorAperturas = 0;
unsigned long segundosRestantes = 0;

// --- CONFIGURACIÓN WI-FI Y PORTAL CAUTIVO ---
const char* ssid = "Tacho_Inteligente_iot";
const char* password = "123456";

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
ESP8266WebServer servidor(80);

// --- DASHBOARD WEB (HTML5 / CSS3 / JS) ---
const char paginaHTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Tacho Inteligente IoT - Dashboard Pro</title>
    <style>
        :root {
            --bg-color: #0b0f19;
            --card-bg: rgba(22, 31, 48, 0.85);
            --border-color: rgba(255, 255, 255, 0.08);
            --primary: #38bdf8;
            --success: #10b981;
            --text-main: #f8fafc;
            --text-sub: #94a3b8;
        }

        * { box-sizing: border-box; margin: 0; padding: 0; }

        body {
            font-family: 'Segoe UI', system-ui, -apple-system, sans-serif;
            background: radial-gradient(circle at top right, #1e1b4b, var(--bg-color));
            color: var(--text-main);
            min-height: 100vh;
            padding: 20px;
            display: flex;
            justify-content: center;
            align-items: center;
        }

        .dashboard {
            width: 100%;
            max-width: 900px;
            display: flex;
            flex-direction: column;
            gap: 20px;
        }

        .header-card {
            background: var(--card-bg);
            backdrop-filter: blur(16px);
            border: 1px solid var(--border-color);
            border-radius: 20px;
            padding: 25px;
            display: flex;
            justify-content: space-between;
            align-items: center;
            box-shadow: 0 10px 30px rgba(0,0,0,0.5);
        }

        .header-title h1 { font-size: 1.4rem; color: var(--primary); display: flex; align-items: center; gap: 10px; }
        .header-title p { font-size: 0.8rem; color: var(--text-sub); margin-top: 4px; }

        .live-tag {
            background: rgba(16, 185, 129, 0.15);
            color: var(--success);
            padding: 6px 14px;
            border-radius: 30px;
            font-size: 0.8rem;
            font-weight: 600;
            border: 1px solid rgba(16, 185, 129, 0.3);
            display: flex;
            align-items: center;
            gap: 8px;
        }

        .pulse {
            width: 8px;
            height: 8px;
            background-color: var(--success);
            border-radius: 50%;
            box-shadow: 0 0 10px var(--success);
            animation: blink 1.5s infinite;
        }

        @keyframes blink { 0%, 100% { opacity: 1; } 50% { opacity: 0.3; } }

        .card {
            background: var(--card-bg);
            backdrop-filter: blur(16px);
            border: 1px solid var(--border-color);
            border-radius: 20px;
            padding: 25px;
            box-shadow: 0 8px 25px rgba(0,0,0,0.3);
        }

        .card-title { font-size: 0.85rem; color: var(--text-sub); text-transform: uppercase; letter-spacing: 1px; font-weight: 600; margin-bottom: 15px; display: flex; align-items: center; gap: 8px;}

        .status-display {
            text-align: center;
            padding: 10px 0 20px 0;
        }

        .status-badge {
            display: inline-block;
            font-size: 1.3rem;
            font-weight: 700;
            padding: 12px 35px;
            border-radius: 50px;
            transition: all 0.4s ease;
        }

        .status-badge.cerrado { background: rgba(148, 163, 184, 0.1); color: var(--text-sub); border: 2px solid rgba(148, 163, 184, 0.2); }
        .status-badge.abierto { background: rgba(16, 185, 129, 0.2); color: var(--success); border: 2px solid var(--success); box-shadow: 0 0 20px rgba(16, 185, 129, 0.4); }

        .btn-action {
            width: 100%;
            background: linear-gradient(135deg, #0ea5e9, #2563eb);
            color: white;
            border: none;
            padding: 15px;
            font-size: 1rem;
            font-weight: bold;
            border-radius: 12px;
            cursor: pointer;
            transition: all 0.3s ease;
            box-shadow: 0 4px 15px rgba(14, 165, 233, 0.3);
        }
        .btn-action:hover { opacity: 0.9; transform: translateY(-2px); }

        .controls-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 15px;
            margin-top: 15px;
        }

        .btn-secondary {
            background: rgba(255, 255, 255, 0.05);
            border: 1px solid var(--border-color);
            color: var(--text-main);
            padding: 12px;
            border-radius: 12px;
            font-weight: 600;
            cursor: pointer;
            transition: all 0.2s;
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 8px;
        }
        .btn-secondary:hover { background: rgba(255, 255, 255, 0.1); border-color: var(--primary); }

        .slider-container {
            margin-top: 15px;
        }
        .slider-header {
            display: flex;
            justify-content: space-between;
            font-size: 0.9rem;
            margin-bottom: 8px;
            color: var(--text-sub);
        }
        .slider-header span { color: var(--primary); font-weight: bold; }
        
        input[type=range] {
            width: 100%;
            height: 6px;
            background: rgba(255, 255, 255, 0.1);
            border-radius: 5px;
            outline: none;
            -webkit-appearance: none;
        }
        input[type=range]::-webkit-slider-thumb {
            -webkit-appearance: none;
            width: 18px;
            height: 18px;
            border-radius: 50%;
            background: var(--primary);
            cursor: pointer;
            box-shadow: 0 0 10px var(--primary);
        }

        .stats-row {
            display: flex;
            justify-content: space-between;
            font-size: 0.9rem;
            color: var(--text-sub);
            margin-top: 10px;
            border-top: 1px solid var(--border-color);
            padding-top: 10px;
        }
        .stats-row span { color: var(--primary); font-weight: bold; }
    </style>
</head>
<body>

    <div class="dashboard">
        <!-- Encabezado -->
        <div class="header-card">
            <div class="header-title">
                <h1>🗑️ Tacho Inteligente IoT</h1>
                <p>Gateway ESP8266 • Portal Cautivo</p>
            </div>
            <div class="live-tag">
                <div class="pulse"></div> ONLINE
            </div>
        </div>

        <!-- Estado en Tiempo Real -->
        <div class="card">
            <div class="card-title">🌐 Estado en Tiempo Real</div>
            <div class="status-display">
                <div id="badgeEstado" class="status-badge cerrado">CERRADO</div>
                <div id="msgTimer" class="stat-sub" style="margin-top: 12px; color: var(--text-sub);">Sistema en reposo / Sensor listo</div>
            </div>
            <button class="btn-action" onclick="simularMano()">🖐️ Simular Detección de Mano (IR)</button>
        </div>

        <!-- Configuración de Servomotor y Acciones -->
        <div class="card">
            <div class="card-title">⚙️ Configuración de Servomotor</div>
            
            <div class="slider-container">
                <div class="slider-header">
                    <span>Tiempo de Apertura</span>
                    <span id="lblSliderVal">3 Segundos</span>
                </div>
                <input type="range" id="tiempoSlider" min="1" max="10" value="3" oninput="actualizarSliderVal(this.value)" onchange="cambiarTiempoAPI(this.value)">
            </div>

            <div class="controls-grid">
                <button class="btn-secondary" onclick="probarBuzzer()">🔊 Probar Buzzer</button>
                <button class="btn-secondary" onclick="forzarGiro()">🔄 Forzar Giro (90°)</button>
            </div>

            <div class="stats-row">
                <div>Aperturas totales: <span id="cntAperturas">0</span></div>
                <div>Última activación: <span id="lblHora">--:--:--</span></div>
            </div>
        </div>
    </div>

    <script>
        let contadorPrevio = 0;

        function obtenerHoraLocal() {
            return new Date().toLocaleTimeString();
        }

        function actualizarSliderVal(val) {
            document.getElementById('lblSliderVal').innerText = val + (val == 1 ? " Segundo" : " Segundos");
        }

        function cambiarTiempoAPI(val) {
            fetch('/tiempo?val=' + val).then(res => res.text());
        }

        function simularMano() {
            fetch('/simular').then(res => res.text());
        }

        function probarBuzzer() {
            fetch('/buzzer').then(res => res.text());
        }

        function forzarGiro() {
            fetch('/forzar').then(res => res.text());
        }

        function actualizarDashboard() {
            fetch('/status')
                .then(res => res.json())
                .then(data => {
                    const badge = document.getElementById('badgeEstado');
                    const msgTimer = document.getElementById('msgTimer');
                    const cntAperturas = document.getElementById('cntAperturas');
                    const lblHora = document.getElementById('lblHora');

                    cntAperturas.innerText = data.contador;

                    if (data.contador > contadorPrevio) {
                        const horaActual = obtenerHoraLocal();
                        lblHora.innerText = horaActual;
                        contadorPrevio = data.contador;
                    }

                    if (data.estado === "abierto") {
                        badge.className = "status-badge abierto";
                        badge.innerText = "ABIERTO";
                        msgTimer.innerText = "Cierre automático en " + data.restante + "s (Configurado a " + data.tiempoConfig + "s)";
                    } else {
                        badge.className = "status-badge cerrado";
                        badge.innerText = "CERRADO";
                        msgTimer.innerText = "Sistema en reposo / Sensor listo";
                    }
                })
                .catch(err => console.error("Error al consultar gateway:", err));
        }

        setInterval(actualizarDashboard, 800);
    </script>
</body>
</html>
)rawliteral";

// --- ENDPOINTS Y FUNCIONES DEL SERVIDOR ---
void manejarRaiz() {
  servidor.send_P(200, "text/html", paginaHTML);
}

// Redirección universal para Portal Cautivo (cualquier dominio desconocido redirige al índice)
void manejarRedirect() {
  servidor.sendHeader("Location", String("http://") + apIP.toString(), true);
  servidor.send(302, "text/plain", "");
}

void manejarStatus() {
  if (abierta) {
    unsigned long transcurrido = millis() - momentoApertura;
    if (transcurrido < TIEMPO_ABIERTO) {
      segundosRestantes = (TIEMPO_ABIERTO - transcurrido) / 1000 + 1;
    } else {
      segundosRestantes = 0;
    }
  } else {
    segundosRestantes = 0;
  }

  String json = "{";
  json += "\"estado\":\"" + String(abierta ? "abierto" : "cerrado") + "\",";
  json += "\"contador\":" + String(contadorAperturas) + ",";
  json += "\"tiempoConfig\":" + String(TIEMPO_ABIERTO / 1000) + ",";
  json += "\"restante\":" + String(segundosRestantes);
  json += "}";
  
  servidor.send(200, "application/json", json);
}

void manejarSimulacion() {
  if (!abierta) {
    tapa.write(TAPA_ABIERTA);
    tone(PIN_BUZZER, 2000, 150);
    abierta = true;
    momentoApertura = millis();
    contadorAperturas++;
  }
  servidor.send(200, "text/plain", "OK");
}

void manejarTiempo() {
  if (servidor.hasArg("val")) {
    int segundos = servidor.arg("val").toInt();
    if (segundos >= 1 && segundos <= 10) {
      TIEMPO_ABIERTO = segundos * 1000;
    }
  }
  servidor.send(200, "text/plain", "OK");
}

void manejarBuzzer() {
  tone(PIN_BUZZER, 2500, 200);
  servidor.send(200, "text/plain", "OK");
}

void manejarForzarGiro() {
  tapa.write(TAPA_ABIERTA);
  tone(PIN_BUZZER, 1800, 100);
  delay(1000);
  tapa.write(TAPA_CERRADA);
  servidor.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_IR, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);

  tapa.attach(PIN_SERVO);
  tapa.write(TAPA_CERRADA);

  WiFi.persistent(false);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid, password);

  // Iniciar servidor DNS en puerto 53 para interceptar todas las peticiones de dominio
  dnsServer.start(DNS_PORT, "*", apIP);

  // Rutas del servidor web
  servidor.on("/", manejarRaiz);
  servidor.on("/status", manejarStatus);
  servidor.on("/simular", manejarSimulacion);
  servidor.on("/tiempo", manejarTiempo);
  servidor.on("/buzzer", manejarBuzzer);
  servidor.on("/forzar", manejarForzarGiro);

  // Capturar cualquier otra URL o intento de conexión y redirigir al dashboard
  servidor.onNotFound(manejarRedirect);

  servidor.begin();
  Serial.println("\n🚀 Portal Cautivo Smart Bin Activo");
}

void loop() {
  dnsServer.processNextRequest(); // Procesa las peticiones DNS del celular
  servidor.handleClient();        // Atiende la web y los botones

  int lectura = digitalRead(PIN_IR);

  if (!abierta && lectura == DETECTADO) {
    tapa.write(TAPA_ABIERTA);
    tone(PIN_BUZZER, 2000, 150);
    abierta = true;
    momentoApertura = millis();
    contadorAperturas++; 
  }

  if (abierta && (millis() - momentoApertura >= TIEMPO_ABIERTO)) {
    tapa.write(TAPA_CERRADA);
    abierta = false;
  }

  delay(10);
}
