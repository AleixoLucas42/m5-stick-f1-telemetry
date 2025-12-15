import socket
import struct
import time
import sys
import os

DEFAULT_PORT = 9876

user_docs = os.path.expanduser("~/Documents")

CONFIG_FOLDER = os.path.join(user_docs, "F1_M5")
CONFIG_FILE = os.path.join(CONFIG_FOLDER, "config.txt")

def load_config():
    """
    Gerencia a configuração na pasta Documentos/F1_M5
    """
    print("⚙️  Verificando configurações...")
    
    if not os.path.exists(CONFIG_FOLDER):
        try:
            os.makedirs(CONFIG_FOLDER)
            print(f"📂 Pasta criada: {CONFIG_FOLDER}")
        except Exception as e:
            print(f"❌ Erro ao criar pasta: {e}")
            return None

    if os.path.exists(CONFIG_FILE):
        with open(CONFIG_FILE, "r") as f:
            stored_ip = f.read().strip()
        
        if len(stored_ip) > 6 and "." in stored_ip:
            print(f"✅ Config carregada de: {CONFIG_FILE}")
            print(f"👉 IP Alvo: {stored_ip}")
            time.sleep(1)
            return stored_ip

    print("\n" + "="*60)
    print("👋 Olá! Bem-vindo ao F1 Bridge.")
    print(f"Vou salvar suas configurações em: {CONFIG_FOLDER}")
    print("Digite o IP do seu M5Stick (ex: 192.168.0.15)")
    print("="*60)
    
    new_ip = input("👉 IP do M5Stick: ").strip()
    
    try:
        with open(CONFIG_FILE, "w") as f:
            f.write(new_ip)
        print("💾 IP salvo com sucesso!")
        time.sleep(1)
    except Exception as e:
        print(f"❌ Erro ao salvar arquivo: {e}")
    
    return new_ip

M5_STICK_IP = load_config()

def print_banner():
    os.system('cls' if os.name == 'nt' else 'clear')
    print(r"""
    =========================================
       🏎️  F1 TELEMETRY BRIDGE FINAL  🏎️
    =========================================
       Status:  ONLINE
       Porta:   9876 (SimDashboard Fix)
       Destino: {}
       Config:  Meus Documentos\F1_M5
    =========================================
    """.format(M5_STICK_IP))

sock_in = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock_out = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

try:
    sock_in.bind(("0.0.0.0", DEFAULT_PORT))
except OSError:
    print("\n❌ ERRO CRÍTICO: A porta 9876 já está ocupada.")
    print("Feche outros scripts ou o SimDashboard no PC.")
    input("\nPressione ENTER para sair...")
    sys.exit()

print_banner()
print("⏳ Aguardando jogo... (Detectando ID automaticamente)")

def run():
    connected = False
    last_id = -1
    
    while True:
        try:
            data, addr = sock_in.recvfrom(2048)
            if len(data) != 1352: continue
            
            # Detecção Automática (Offset 27)
            player_index = data[27]
            if player_index > 21: continue

            base = 29 + (player_index * 60)
            
            try:
                gear = struct.unpack_from("<b", data, base + 15)[0]
                rpm = struct.unpack_from("<H", data, base + 16)[0]
            except:
                continue

            if rpm > 16000 or gear < -1 or gear > 8: continue

            if not connected and rpm > 0:
                print(f"\n✅ SINAL RECEBIDO! Piloto ID: {player_index}")
                connected = True
            
            # Envio
            if rpm > 0:
                rpm_pc = min(rpm / 13500.0, 1.0)
                rpm_pc = max(0.0, rpm_pc)
                
                msg = f"RPMPC:{rpm_pc:.2f};GEAR:{gear}"
                sock_out.sendto(msg.encode(), (M5_STICK_IP, DEFAULT_PORT))
                
                bar = "█" * int(rpm_pc * 20)
                print(f"\r🏎️  ID:{player_index:<2} | RPM: {rpm:<5} | Gear: {gear:<2} | {bar:<20}", end="", flush=True)

        except KeyboardInterrupt:
            break
        except Exception:
            continue

if __name__ == "__main__":
    try:
        run()
    except Exception:
        pass