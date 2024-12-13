#include <WiFi.h> 
#include <HTTPClient.h>

// Definição das portas para controle dos LEDs
#define led_azul 46
#define led_verde 2
#define led_vermelho 40
#define led_amarelo 9

// Definição do pino do botão
const int BOTAO = 18;

// Definição do pino do sensor LDR
const int LDR_PIN = 4;

// Valor de limiar para leitura do LDR (determina se está claro ou escuro)
const int LIMIAR_LDR = 600;

// Variável de estado do botão
int estadoBotao = 0;

// variável para armazenar o status de leitura do LDR
int statusLDR = 0;

void setup() {
  // Configuração dos pinos dos LEDs como saídas
  pinMode(led_azul, OUTPUT);
  pinMode(led_verde, OUTPUT);
  pinMode(led_vermelho, OUTPUT);
  pinMode(led_amarelo, OUTPUT);

  // Configuração do pino do botão como entrada
  pinMode(BOTAO, INPUT);

  // Inicialização dos LEDs apagados
  digitalWrite(led_azul, LOW);
  digitalWrite(led_verde, LOW);
  digitalWrite(led_vermelho, LOW);
  digitalWrite(led_amarelo, LOW);

  // Inicialização do monitor serial para debug
  Serial.begin(9600);

  // Conexão ao WiFi
  WiFi.begin("Wokwi-GUEST", ""); // Conexão à rede WiFi aberta com SSID Wokwi-GUEST

  while (WiFi.status() != WL_CONNECT_FAILED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("Conectado ao WiFi com sucesso!"); // Considerando que saiu do loop acima, o ESP32 agora está conectado ao WiFi (outra opção é colocar este comando dentro do if abaixo)

  // Verifica estado do botão
  estadoBotao = digitalRead(buttonPin);
  if (estadoBotao == HIGH) {
    Serial.println("Botão pressionado!");
  } else {
    Serial.println("Botão não pressionado!");
  }

  if(WiFi.status() == WL_CONNECTED){ // Se o ESP32 estiver conectado à Internet
    HTTPClient http;

    String serverPath = "http://www.google.com.br/"; // Endpoint da requisição HTTP

    http.begin(serverPath.c_str());

    int httpResponseCode = http.GET(); // Código do Resultado da Requisição HTTP

    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
      }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      }
      http.end();
    }

  else {
    Serial.println("WiFi Disconnected");
  }
}

void loop() {
  // Leitura do valor analógico do LDR
  statusLDR = analogRead(LDR_PIN);

  // Verifica se está escuro
  if (statusLDR <= LIMIAR_LDR) {
    Serial.println("Modo noturno: está escuro.");
    piscaLedAmarelo();
  } else {
    Serial.println("Modo convencional: está claro.");
    semaforoConvencional();
  }
}

// piscar o LED amarelo no modo noturno
void piscaLedAmarelo() {
  digitalWrite(led_amarelo, HIGH);
  delay(1000);
  digitalWrite(led_amarelo, LOW);
  delay(1000);
}

// controle do semáforo em modo convencional
void semaforoConvencional() {
  // LED verde aceso por 3 segundos
  digitalWrite(led_verde, HIGH);
  delay(3000);
  digitalWrite(led_verde, LOW);

  // LED amarelo aceso por 2 segundos
  digitalWrite(led_amarelo, HIGH);
  delay(2000);
  digitalWrite(led_amarelo, LOW);

  // LED vermelho aceso por 5 segundos
  digitalWrite(led_vermelho, HIGH);
  delay(5000);

  // checa se o botão foi pressionado enquanto o LED vermelho estava aceso
  estadoBotao = digitalRead(BOTAO);
  if (estadoBotao == HIGH) {
    Serial.println("Botão pressionado: abrindo semáforo em 1 segundo.");
    delay(1000);
    digitalWrite(led_vermelho, LOW); // apaga o LED vermelho
    digitalWrite(led_verde, HIGH);  // liga o LED verde
    delay(3000); // semáforo aberto por 3 segundos
    digitalWrite(led_verde, LOW);  // apaga o LED verde
  } else {
    digitalWrite(led_vermelho, LOW); // apaga o LED vermelho se o botão não foi pressionado
  }
}

