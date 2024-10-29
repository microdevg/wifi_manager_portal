#ifndef _PAGE_HTML_H




// HTML page to capture Wi-Fi credentials
const char html_page[] =
    "<html>"
    "<head>"
    "<meta charset=\"UTF-8\">"
    "<style>"
    "body { font-family: Arial, sans-serif; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; background-color: #f2f2f2; }"
    ".container { text-align: center; max-width: 400px; }"
    "h1 { color: #333333; font-size: 24px; margin-bottom: 10px; }"
    "p { color: #555555; font-size: 14px; margin-bottom: 20px; }"
    "form { background-color: #ffffff; padding: 20px; border-radius: 8px; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); text-align: left; }"
    "input[type=text], input[type=password] { width: 100%; padding: 8px; margin: 8px 0; border: 1px solid #ccc; border-radius: 4px; box-sizing: border-box; }"
    "input[type=submit] { background-color: #4CAF50; color: white; padding: 10px 15px; border: none; border-radius: 4px; cursor: pointer; width: 100%; font-size: 16px; }"
    "input[type=submit]:hover { background-color: #45a049; }"
    "</style>"
    "</head>"
    "<body>"
    "<div class=\"container\">"
    "<h1>Configuración WiFi</h1>"
    "<p>Ingrese las credenciales de la red WiFi a la cual desea conectarse</p>"
    "<form action=\"/c\" method=\"POST\">"
    "Nombre de Red WiFi: <input type=\"text\" name=\"ssid\"><br>"
    "Contraseña de WiFi: <input type=\"password\" name=\"password\"><br>"
    "<input type=\"submit\" value=\"Conectar\">"
    "</form>"
    "</div>"
    "</body>"
    "</html>";

#define PAGE_HTML_H
#endif