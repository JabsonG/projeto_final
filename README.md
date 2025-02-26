# Controle de Temperatura com PWM e Interrupções no Raspberry Pi Pico

Este projeto simula um sistema de controle de temperatura utilizando o Raspberry Pi Pico. O sistema alterna entre modos automático e manual, controlando LEDs que simulam um módulo Peltier (resfriamento) e uma lâmpada de cerâmica (aquecimento) por meio de PWM (Modulação por Largura de Pulso). O código também utiliza interrupções para detectar o pressionamento de um botão e alternar entre os modos de operação.

## Funcionalidades Principais

### Controle de Temperatura:
- **Modo Automático:** O sistema alterna entre 15°C e 25°C.
- **Modo Manual:** O usuário define a temperatura desejada, e o sistema ajusta a temperatura simulada para alcançar esse valor.

### PWM (Modulação por Largura de Pulso):
- Os LEDs que simulam o módulo Peltier e a lâmpada de cerâmica são controlados por PWM.
- O duty cycle do PWM é ajustado para simular o aquecimento e resfriamento.

### Interrupções:
- Um botão é configurado com interrupção para alternar entre os modos automático e manual.
- A interrupção inclui debounce para evitar leituras incorretas.

### Comunicação Serial (UART):
- Mensagens são enviadas pela UART para informar o estado do sistema e solicitar entradas do usuário.

## Estrutura do Código

### Funções Principais

#### `configurar_hardware()`
- Configura os pinos dos LEDs como saídas PWM.
- Inicializa a UART para comunicação serial.
- Configura o botão com interrupção e debounce.

#### `botao_pressionado(uint gpio, uint32_t events)`
- Função de interrupção chamada quando o botão é pressionado.
- Alterna entre os modos automático e manual.
- No modo manual, solicita ao usuário a temperatura desejada.

#### `ler_entrada_usuario()`
- Lê a entrada do usuário via UART.
- Converte a entrada para `float` e valida o valor digitado.

#### `ajustar_temperatura()`
- Simula a variação de temperatura no modo automático.
- Aumenta ou diminui a temperatura simulada em 2°C a cada ciclo.

#### `controlar_temperatura()`
- Controla os LEDs com base na temperatura simulada.
- Ajusta o duty cycle do PWM para simular aquecimento ou resfriamento.

### Variáveis Principais
- `temperatura_simulada`: Armazena a temperatura atual simulada.
- `aquecendo`: Estado do sistema (verdadeiro se estiver aquecendo, falso se estiver resfriando).
- `modo_manual`: Estado do modo de operação (verdadeiro para manual, falso para automático).
- `temperatura_desejada`: Temperatura definida pelo usuário no modo manual.
- `LED_PELTIER`: Pino GPIO que simula o módulo Peltier (resfriamento).
- `LED_LAMPADA`: Pino GPIO que simula a lâmpada de cerâmica (aquecimento).
- `BOTAO`: Pino GPIO que simula o botão para alternar entre modos.

## Configuração de PWM
O PWM é configurado para controlar os LEDs que simulam o módulo Peltier e a lâmpada de cerâmica. Abaixo estão os detalhes da configuração:

### Pinos PWM:
- **LED_PELTIER (GPIO12):** Controla o módulo Peltier.
- **LED_LAMPADA (GPIO13):** Controla a lâmpada de cerâmica.

### Configuração do PWM:
- O wrap (valor máximo do contador PWM) é definido como 100.
- O duty cycle é ajustado para 0% (desligado) ou 100% (ligado) conforme necessário.

### Controle de Aquecimento/Resfriamento:
- No modo manual, o duty cycle é ajustado para alcançar a temperatura desejada.
- No modo automático, o duty cycle alterna entre 0% e 100% para simular o ciclo de aquecimento e resfriamento.

## Interrupções
O botão é configurado com interrupção para detectar o pressionamento e alternar entre os modos de operação. A interrupção inclui debounce para evitar leituras incorretas.

### Configuração do Botão:
- **Pino:** BOTAO (GPIO5).
- **Modo:** Entrada com resistor de pull-up.
- **Interrupção:** Configurada para detectar borda de descida (GPIO_IRQ_EDGE_FALL).

### Debounce:
A interrupção ignora pulsos muito rápidos, garantindo que apenas pressionamentos válidos sejam detectados.

## Uso

### Modo Automático:
- O sistema alterna automaticamente entre 15°C e 25°C.
- O LED da lâmpada de cerâmica é ligado para aquecer, e o LED do módulo Peltier é ligado para resfriar.

### Modo Manual:
- Pressione o botão para ativar o modo manual.
- Digite a temperatura desejada (entre 15.0 e 30.0) no terminal.
- O sistema ajustará a temperatura simulada para alcançar o valor desejado.

### Exemplo de Saída no Terminal:
```text
Sistema iniciado no modo automático.
🔴 Aquecendo (modo automático)...
🌡️ Temperatura atual: 15.0°C
Modo manual ativado.
Digite a temperatura desejada (15.0 a 30.0): 20.0
Temperatura desejada definida para 20.0°C.
🔴 Aquecendo (modo manual)...
🌡️ Temperatura atual: 15.0°C
```

## Requisitos

### Hardware:
- Raspberry Pi Pico.
- LEDs (ou cargas simuladas) conectados aos pinos GPIO12 e GPIO13.
- Botão conectado ao pino GPIO5.

### Software:
- Ambiente de desenvolvimento configurado para Raspberry Pi Pico (VS Code + Wokwi, PlatformIO, etc.).
- Biblioteca pico-sdk instalada.

## Como Executar
1. Conecte os componentes conforme descrito na seção **Requisitos**.
2. Compile e carregue o código no Raspberry Pi Pico.
3. Abra o terminal serial para interagir com o sistema.

## Link do vídeo demonstrativo:
