document.addEventListener("DOMContentLoaded", function () {
    const flowInput = document.getElementById("flow");
    const pressureInput = document.getElementById("pressure");
    const ultrasonicInput = document.getElementById("ultrasonic");
    const connectBtn = document.getElementById("connect");
    const sendBtn = document.getElementById("send-command");
    const commandInput = document.getElementById("command");
    const logOutput = document.getElementById("log");
    const configOutput = document.getElementById("config-output");
    const updateCredentialsBtn = document.getElementById("update-credentials");

    function updateLog(message) {
        logOutput.value += message + "\n";
        logOutput.scrollTop = logOutput.scrollHeight; // Auto-scroll al final
    }

    connectBtn.addEventListener("click", function () {
        if (!ws || ws.readyState === WebSocket.CLOSED) {
            ws = new WebSocket(WS_URL);

            ws.onopen = function () {
                console.log("Conectado al WebSocket");
                connectBtn.textContent = "Desconectar";
            };

            ws.onmessage = function (event) {
                try {
                    const data = JSON.parse(event.data);
                    console.log("Datos recibidos:", data); // Ver en consola los datos recibidos

                    if (Array.isArray(data)) {
                        // Si la respuesta es un arreglo, mostrarlo en el log
                        updateLog("Datos de sensores: " + JSON.stringify(data, null, 2));
                    } else if (data.sensors) {
                        // Actualizar los valores de los sensores
                        flowInput.value = data.sensors.flow;
                        pressureInput.value = data.sensors.pressure;
                        ultrasonicInput.value = data.sensors.ultrasonic;
                    } else if (data.status === "success" && data.wifi) {
                        // Mostrar la configuración en el textarea de configuración
                        configOutput.value = JSON.stringify(data, null, 2);
                    }
                } catch (error) {
                    updateLog("Error al procesar mensaje: " + event.data);
                    console.error("Error procesando mensaje:", error);
                }
            };

            ws.onclose = function () {
                console.log("Desconectado del WebSocket");
                connectBtn.textContent = "Conectar";
            };

            ws.onerror = function (error) {
                console.error("WebSocket error:", error);
            };
        } else {
            ws.close();
        }
    });

    sendBtn.addEventListener("click", function () {
        const command = commandInput.value.trim();
        if (ws && ws.readyState === WebSocket.OPEN && command) {
            try {
                const commandJson = JSON.parse(command);
                ws.send(JSON.stringify(commandJson));
                console.log("Enviado:", commandJson);
            } catch (e) {
                console.error("Error al parsear el comando JSON:", e);
                alert("El comando no es un JSON válido.");
            }
        }
    });

    updateCredentialsBtn.addEventListener("click", function () {
        const username = document.getElementById("username").value.trim();
        const password = document.getElementById("password").value.trim();

        if (ws && ws.readyState === WebSocket.OPEN && username && password) {
            const credentialsMessage = {
                command: "update_credentials",
                value: { username, password }
            };
            ws.send(JSON.stringify(credentialsMessage));
            console.log("Enviado:", credentialsMessage);
        }
    });
});