#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>

using namespace std;

// Funcion para cargar el archivo conocimiento.txt en un mapa
// Ignora lineas vacias o lineas que empiezan con '#'
void cargarConocimiento(map<string, string>& conocimiento, const string& nombreArchivo) {
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cout << "Error al abrir el archivo de conocimiento." << endl;
        return;
    }

    string linea;
    while (getline(archivo, linea)) {
        if (linea.empty() || linea[0] == '#') {
            continue;
        }

        size_t separador = linea.find('|');
        if (separador != string::npos) {
            string pregunta = linea.substr(0, separador);
            string respuesta = linea.substr(separador + 1);
            conocimiento[pregunta] = respuesta;
        }
    }

    archivo.close();
}

// Funcion para realizar una busqueda exacta
// Compara directamente la pregunta del usuario con las preguntas del mapa
string buscarExacto(const map<string, string>& conocimiento, const string& pregunta) {
    auto it = conocimiento.find(pregunta);
    if (it != conocimiento.end()) {
        return it->second;
    }
    else {
        return "";
    }
}

// Funcion mejorada para realizar una busqueda por palabras clave (sin distinguir mayusculas)
string buscarPorPalabrasClave(const map<string, string>& conocimiento, const string& preguntaUsuario) {
    vector<string> palabrasPregunta;
    stringstream ss(preguntaUsuario);
    string palabra;

    // Convertir la pregunta del usuario a minusculas
    string preguntaMinusculas = preguntaUsuario;
    transform(preguntaMinusculas.begin(), preguntaMinusculas.end(), preguntaMinusculas.begin(), ::tolower);

    // Separar las palabras
    stringstream ss2(preguntaMinusculas);
    while (ss2 >> palabra) {
        palabrasPregunta.push_back(palabra);
    }

    // Buscar en cada pregunta del mapa
    for (const auto& par : conocimiento) {
        string preguntaBase = par.first;

        // Convertir la pregunta de la base a minusculas
        transform(preguntaBase.begin(), preguntaBase.end(), preguntaBase.begin(), ::tolower);

        for (const auto& palabraPregunta : palabrasPregunta) {
            if (preguntaBase.find(palabraPregunta) != string::npos) {
                return par.second;
            }
        }
    }

    return "";
}
// Funcion para calcular la distancia de Levenshtein entre dos cadenas
int calcularDistanciaLevenshtein(const string& a, const string& b) {
    int n = a.size();
    int m = b.size();
    vector<vector<int>> dp(n + 1, vector<int>(m + 1));

    for (int i = 0; i <= n; i++) dp[i][0] = i;
    for (int j = 0; j <= m; j++) dp[0][j] = j;

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            if (a[i - 1] == b[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            }
            else {
                dp[i][j] = 1 + min({ dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1] });
            }
        }
    }

    return dp[n][m];
}

// Funcion para buscar la respuesta mas similar usando distancia de Levenshtein
string buscarPorSimilaridad(const map<string, string>& conocimiento, const string& preguntaUsuario) {
    string mejorRespuesta = "";
    int mejorDistancia = 1000; // Un numero ridiculamente grande al inicio

    for (const auto& par : conocimiento) {
        int distancia = calcularDistanciaLevenshtein(preguntaUsuario, par.first);
        if (distancia < mejorDistancia) {
            mejorDistancia = distancia;
            mejorRespuesta = par.second;
        }
    }

    // Si la mejor distancia es muy alta, significa que no se parece a nada
    if (mejorDistancia > 20) {
        return "";
    }

    return mejorRespuesta;
}


int main() {
    map<string, string> conocimiento;

    // Cargar el conocimiento
    cargarConocimiento(conocimiento, "conocimiento.txt");

    cout << "Te doy la bienvenida a este chatbot amante de los videojuegos. Prometo darte recomendaciones segun tus gustos" << endl;
    cout << "Puedes escribir  Salir para terminar el programa" << endl;

    string preguntaUsuario;
    while (true) {
        cout << "\nJugador: ";
        getline(cin, preguntaUsuario);

        if (preguntaUsuario == "salir") {
            cout << "te deseo un buen viaje y espero te diviertas con mis recomendaciones" << endl;
            break;
        }

        // Buscar por una pregunta exacta
        string respuesta = buscarExacto(conocimiento, preguntaUsuario);

        // Si no encuentra coincidencia exacta, buscar por palabras clave
        if (respuesta.empty()) {
            respuesta = buscarPorPalabrasClave(conocimiento, preguntaUsuario);
        }

        // Si sigue sin encontrar, buscar por similaridad
        if (respuesta.empty()) {
            respuesta = buscarPorSimilaridad(conocimiento, preguntaUsuario);
        }

        // Si aun asi no encuentra
        if (respuesta.empty()) {
            respuesta = "Lamentablemente no tengo una respuesta para esta pregunta pero te juro que hare lo posible para darte una recomendacion de otro tipo si la necesitas";
        }


        // Mostrar la respuesta correctamente
        cout << "GBot: " << respuesta << endl;
    }

    return 0;
}
