# 🗑️ Tacho Inteligente IoT (Smart Bin Gateway)

<p align="center">
  <b>Dispositivo IoT educativo basado en ESP8266 con Portal Cautivo, automatización por infrarrojos y un Dashboard Web en tiempo real.</b>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/ESP8266-NodeMCU-blue?style=for-the-badge&logo=espressif" alt="ESP8266">
  <img src="https://img.shields.io/badge/C++-Arduino-orange?style=for-the-badge&logo=c%2B%2B" alt="Arduino">
  <img src="https://img.shields.io/badge/HTML5%2FJS-Dashboard-green?style=for-the-badge&logo=html5" alt="Dashboard">
  <img src="https://img.shields.io/badge/Status-Functional-success?style=for-the-badge" alt="Status">
</p>

---

## 🌟 Resumen del Proyecto

Este proyecto implementa un contenedor de basura inteligente automatizado que opera de forma autónoma mediante un microcontrolador **ESP8266**. Al detectar la presencia de una mano u objeto a través de un sensor infrarrojo (IR), el sistema acciona mecánicamente la tapa por medio de un servomotor y emite una alerta sonora con un buzzer activo. 

Adicionalmente, el dispositivo crea un **Access Point Wi-Fi con Portal Cautivo**, permitiendo que cualquier smartphone o computadora cercana se conecte y visualice un **Dashboard Web interactivo** para monitorear aperturas en tiempo real, simular detecciones, probar actuadores y configurar parámetros dinámicos sin necesidad de conexión a internet.

---

## ⚙️ Características Principales

* **Automatización sin contacto:** Detección de proximidad mediante sensor infrarrojo acoplado al pin digital D5.
* **Control Mecánico Preciso:** Movimiento de la tapa mediante servomotor calibrado en ángulos de $0^\circ$ (cerrado) y $90^\circ$ (abierto).
* **Portal Cautivo Integrado:** Servidor DNS local (puerto 53) que intercepta las peticiones y redirige automáticamente al usuario al panel de control al conectarse a la red `Tacho_Inteligente_iot`.
* **Dashboard Pro (Glassmorphism):** Interfaz web optimizada con HTML5, CSS3 y JavaScript asíncrono (`fetch` API), que actualiza el estado del sistema cada 800 ms.
* **Telemetría y Control Remoto:** 
  * Contador dinámico de aperturas totales.
  * Control deslizante (*slider*) para ajustar el tiempo de apertura de 1 a 10 segundos en caliente.
  * Botones interactivos para forzar giros, probar el buzzer y simular detecciones remotas.

---

## 🛠️ Componentes y Arquitectura de Pines

| Componente | Pin ESP8266 | Pin Físico | Función |
| :--- | :--- | :--- | :--- |
| **Sensor Infrarrojo (IR)** | `GPIO 14` | D5 | Entrada digital para detección de presencia. |
| **Servomotor** | `GPIO 12` | D6 | Control de posición angular de la tapa ($0^\circ$ / $90^\circ$). |
| **Buzzer Activo** | `GPIO 13` | D7 | Emisión de avisos acústicos de confirmación. |

---

## 🚀 Guía de Instalación y Despliegue

### 1. Requisitos Previos en el IDE de Arduino
Asegúrate de tener instalada la placa **ESP8266** en tu IDE de Arduino y las siguientes librerías oficiales/estándar:
* `ESP8266WiFi`
* `ESP8266WebServer`
* `DNSServer`
* `Servo`

### 2. Configuración de Red por Defecto
El firmware levanta un punto de acceso (*Access Point*) con los siguientes parámetros configurados en el código:
* **SSID:** `Tacho_Inteligente_iot`
* **Contraseña:** `123456`
* **IP del Gateway:** `192.168.4.1`

### 3. Código Fuente del Firmware
El código completo que gestiona tanto la lógica del microcontrolador como el servidor web embebido se encuentra en el archivo principal del repositorio (`.ino`). 

---

## 📊 Vista Previa de la Interfaz Web

El dashboard integrado ofrece una experiencia visual moderna con indicadores de estado dinámicos:
* **Estado del Sistema:** Muestra de forma gráfica si el tacho se encuentra en reposo (`CERRADO`) o ejecutando una apertura (`ABIERTO`) con cuenta regresiva en vivo.
* **Paneles de Control:** Ajustes deslizantes y gatillos manuales para depuración rápida directamente desde el navegador móvil.

# ☕ Apoya el Desarrollo del Proyecto
Si este firmware unificado te sirvió para tus auditorías o te ahorró horas en el banco de pruebas, ¡puedes invitarme un café de forma anónima para bancar los insumos del taller!
[![Invitame un café en cafecito.app](https://cdn.cafecito.app/imgs/buttons/button_1.svg)](https://cafecito.app/scissor)


---

## 💡 Autor

Desarrollado como proyecto escolar de automatización e Internet de las Cosas (IoT).
