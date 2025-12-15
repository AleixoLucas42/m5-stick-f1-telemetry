# M5Stick F1 Telemetry - F1 2024

Este projeto implementa um sistema de telemetria (focado em *Shift Lights*) utilizando um **M5Stick** para o jogo **Formula 1 2024**.

![game](game.gif)

(Por algum motivo o gif ficou em camera lenta lol)

## 📖 Introdução

O objetivo deste projeto é permitir que você tenha um display físico de telemetria (luzes de troca de marcha/RPM) externo ao jogo, utilizando o hardware M5Stick. Ele recebe os dados de telemetria do jogo e acende os LEDs do dispositivo para indicar o momento ideal de troca de marcha, melhorando a imersão e o tempo de reação.

**Atenção:** Este código foi desenvolvido e testado especificamente para o jogo **F1 2024**.

## ⚙️ Pré-requisitos

* Dispositivo M5Stick (ex: M5StickC Plus).
* Jogo **Formula 1 2024**.
* Aplicativo **SimDashboard** (Android/iOS) para realizar o *UDP Forwarding*.
* Arduino IDE (com as bibliotecas do M5Stick instaladas) para compilar e enviar o código.

## ⚠️ Configuração Obrigatória (Wi-Fi)

**Antes de compilar e carregar o código para o seu M5Stick, você deve alterar as credenciais de rede.**

1. Abra o arquivo `ShiftLight.ino`.
2. Procure pelas linhas onde são definidas as constantes/variáveis de conexão Wi-Fi:
```cpp
   const char* ssid = "NOME_DA_SUA_REDE";
   const char* password = "SENHA_DA_SUA_REDE";
````

3.  Substitua `"NOME_DA_SUA_REDE"` e `"SENHA_DA_SUA_REDE"` pelo nome e senha exatos do seu Wi-Fi.
    > **Nota:** O M5Stick e o dispositivo rodando o SimDashboard (seu celular/tablet) devem estar na mesma rede Wi-Fi.

## 🚀 Como Usar (UDP Forwarding via SimDashboard)

Este projeto não se conecta diretamente ao console/PC via USB, ele utiliza a rede Wi-Fi para receber dados. Para isso, utilizamos a função de reencaminhamento de dados (UDP Forwarding) do aplicativo **SimDashboard**.

1.  **No M5Stick:**

      * Após carregar o código, ligue o M5Stick.
      * Ele irá conectar no seu Wi-Fi e mostrará o **endereço IP** na tela. Anote este número.

2.  **No Jogo (F1 2024):**

      * Vá nas configurações de Telemetria.
      * Ative a telemetria UDP.
      * Configure o SimDashboard para o Formula 1 2024.

3.  **No aplicativo SimDashboard:**

      * Procure a opção de **UDP Forwarding** (Reencaminhamento UDP).
      * Adicione uma nova regra de encaminhamento para o `127.0.0.1` na porta `9876`

4.  **Aplicativo intermediario detelemetria**

      * Abra o aplicativo .exe que voce pode encontrar nas [releases](https://github.com/AleixoLucas42/m5-stick-f1-telemetry/releases/tag/v1)
      * Permita as solicitações de segurança do windows
      * Siga os passos no aplicativo para a configuração de ip do M5

> Atenção, o aplicativo precisa ficar aberto durante o uso

## 🛠️ Compilação

1.  Abra o projeto na Arduino IDE.
2.  Certifique-se de ter selecionado a placa correta (M5Stick-C Plus ou similar).
3.  Verifique se todas as bibliotecas necessárias estão instaladas.
4.  Clique em **Upload**.

-----

## Compilar o executavel do windows:
> pyinstaller --onefile --icon=f1.ico --name="F1_M5" f1_telemetry.py

*Desenvolvido por [AleixoLucas42](https://www.google.com/search?q=https://github.com/AleixoLucas42)*


