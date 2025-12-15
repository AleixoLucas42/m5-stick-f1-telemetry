#include <M5StickCPlus2.h>
#include <WiFi.h>
#include <WiFiUdp.h>

// --- CONFIGURAÇÕES DE WI-FI (ALTERE AQUI!) ---
const char* ssid = "";
const char* password = "";

// --- CONFIGURAÇÕES UDP ---
WiFiUDP udp;

const int localPort = 9876; 
char packetBuffer[255]; // Buffer para receber os dados

// --- VARIÁVEIS DE DADOS ---
float rpm_percent = 0.0;
int gear = 0;
int prev_gear = -99;
float prev_rpm = -1.0;

// Cores do Shift Light
#define COLOR_LOW     TFT_GREEN
#define COLOR_MED     TFT_YELLOW
#define COLOR_HIGH    0xFD20 // Laranja
#define COLOR_SHIFT   TFT_RED
#define COLOR_FLASH   TFT_BLUE

void setup() {
    auto cfg = M5.config();
    M5.begin(cfg);
    
    // Rotação 3: Modo Paisagem (Botão 'M5' à direita)
    M5.Lcd.setRotation(3); 
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextSize(2);
    
    // --- CONEXÃO WI-FI ---
    M5.Lcd.setCursor(10, 10);
    M5.Lcd.println("Conectando WiFi...");
    
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        M5.Lcd.print(".");
    }
    
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(5, 5);
    M5.Lcd.println("WiFi OK!");
    M5.Lcd.setTextColor(TFT_YELLOW);
    M5.Lcd.println(WiFi.localIP());
    M5.Lcd.setTextColor(TFT_WHITE);
    M5.Lcd.print("Porta: ");
    M5.Lcd.println(localPort);
    
    udp.begin(localPort);
    
    delay(3000); 
    
    // Desenha a interface inicial
    M5.Lcd.fillScreen(BLACK);
    drawInterface();
}

void loop() {
    int packetSize = udp.parsePacket();
    if (packetSize) {
        int len = udp.read(packetBuffer, 255);
        if (len > 0) {
            packetBuffer[len] = 0;
        }
        
        // Parseia a string que vem do Python: "RPMPC:0.50;GEAR:4"
        parseData(packetBuffer);
        
        updateDisplay();
    }
}

void parseData(char* data) {
    // Usa sscanf para extrair o float e o int da string formatada
    sscanf(data, "RPMPC:%f;GEAR:%d", &rpm_percent, &gear);
}

void drawInterface() {
    M5.Lcd.drawRect(0, 0, M5.Lcd.width(), M5.Lcd.height(), TFT_DARKGREY);
}

void updateDisplay() {
    int w = M5.Lcd.width();
    int h = M5.Lcd.height();
    
    // Se for Shift Light Crítico (> 96%), pisca a tela toda!
    if (rpm_percent >= 0.96) {
       static bool flash = false;
       flash = !flash;
       M5.Lcd.fillScreen(flash ? COLOR_SHIFT : COLOR_FLASH);
       
       // Redesenha a marcha em cor contrastante
       M5.Lcd.setTextColor(WHITE);
       M5.Lcd.setTextDatum(MC_DATUM); 
       M5.Lcd.setTextSize(6);
       String g = (gear == 0) ? "N" : (gear == -1 ? "R" : String(gear));
       M5.Lcd.drawString(g, w/2, h/2);
       
       // Força redesenho do fundo na próxima vez que sair do shift light
       prev_gear = -99; 
       return; 
    }

    // Se a marcha mudou (ou voltamos do shift light), limpa a área central
    if (gear != prev_gear) {
        M5.Lcd.fillScreen(BLACK); // Limpa tudo para garantir
        drawInterface();
        prev_gear = gear;
        
        // Desenha a Marcha
        M5.Lcd.setTextColor(WHITE, BLACK);
        M5.Lcd.setTextDatum(MC_DATUM); // Centro
        M5.Lcd.setTextSize(6); // Fonte Grande
        
        String gearStr = String(gear);
        if (gear == 0) gearStr = "N";
        if (gear == -1) gearStr = "R";
        
        M5.Lcd.drawString(gearStr, w/2, (h/2) - 10);
    }

    // Só atualiza a barra se o RPM mudou significativamente para economizar processamento
    if (abs(rpm_percent - prev_rpm) > 0.01) {
        int barHeight = 25;
        int barY = h - barHeight - 5; // 5px de margem inferior
        int maxBarWidth = w - 10;     // 5px de margem lateral
        int currentBarWidth = (int)(maxBarWidth * rpm_percent);
        
        // Cor dinâmica
        uint16_t color = COLOR_LOW;
        if (rpm_percent > 0.60) color = COLOR_MED;
        if (rpm_percent > 0.85) color = COLOR_HIGH;
    
        // Desenha o fundo da barra (cinza escuro) onde a barra "não está"
        // Isso "apaga" a barra antiga sem limpar a tela toda
        M5.Lcd.fillRect(5 + currentBarWidth, barY, maxBarWidth - currentBarWidth, barHeight, 0x18E3); // Cinza
        
        // Desenha a parte cheia da barra
        M5.Lcd.fillRect(5, barY, currentBarWidth, barHeight, color);
        
        prev_rpm = rpm_percent;
    }
}