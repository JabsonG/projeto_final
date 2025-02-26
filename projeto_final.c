#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "hardware/pwm.h"
#include "hardware/irq.h"

// Definição de pinos
#define LED_PELTIER 12   // LED Azul (simula o Peltier)
#define LED_LAMPADA 13   // LED Vermelho (simula a lâmpada/relé)
#define BOTAO 5          // Botão para alternar entre modos (GPIO5)
#define UART_ID uart0    // UART usada (GPIO 0 e 1)
#define BAUD_RATE 115200 // Taxa de transmissão

float temperatura_simulada = 15.0;  // Temperatura inicial
bool aquecendo = true;              // Estado do sistema (aquecendo ou resfriando)
bool modo_manual = false;           // Estado do modo (automático ou manual)
float temperatura_desejada = 25.0;  // Temperatura desejada no modo manual

// Protótipos das funções
void configurar_hardware();
void ajustar_temperatura();
void controlar_temperatura();
void botao_pressionado(uint gpio, uint32_t events);
void ler_entrada_usuario();

// Configuração do hardware
void configurar_hardware() {
    stdio_init_all();

    // Configura os pinos dos LEDs como PWM
    gpio_set_function(LED_PELTIER, GPIO_FUNC_PWM); // Configura GPIO12 como PWM
    gpio_set_function(LED_LAMPADA, GPIO_FUNC_PWM); // Configura GPIO13 como PWM

    // Obtém os slices PWM associados aos pinos
    uint slice_peltier = pwm_gpio_to_slice_num(LED_PELTIER); // Slice para GPIO12
    uint slice_lampada = pwm_gpio_to_slice_num(LED_LAMPADA); // Slice para GPIO13

    // Configura o wrap (valor máximo do contador PWM) para ambos os slices
    pwm_set_wrap(slice_peltier, 100); // Wrap para o slice do Peltier
    pwm_set_wrap(slice_lampada, 100); // Wrap para o slice da lâmpada

    // Configura os níveis iniciais dos canais PWM
    pwm_set_chan_level(slice_peltier, PWM_CHAN_A, 0);   // Inicia com o Peltier desligado
    pwm_set_chan_level(slice_lampada, PWM_CHAN_B, 100); // Inicia com a lâmpada ligada

    // Habilita os slices PWM
    pwm_set_enabled(slice_peltier, true); // Habilita o slice do Peltier
    pwm_set_enabled(slice_lampada, true); // Habilita o slice da lâmpada

    // Configura a UART para comunicação serial
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(0, GPIO_FUNC_UART); // TX
    gpio_set_function(1, GPIO_FUNC_UART); // RX

    // Configura o botão com debounce
    gpio_init(BOTAO);
    gpio_set_dir(BOTAO, GPIO_IN);
    gpio_pull_up(BOTAO); // Habilita o resistor de pull-up interno
    gpio_set_irq_enabled_with_callback(BOTAO, GPIO_IRQ_EDGE_FALL, true, &botao_pressionado);

    uart_puts(UART_ID, "Sistema iniciado no modo automático.\r\n");
}

// Função de interrupção para o botão
void botao_pressionado(uint gpio, uint32_t events) {
    static uint32_t ultimo_tempo = 0;
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());

    // Debounce: ignora pulsos muito rápidos
    if (tempo_atual - ultimo_tempo > 200) {
        modo_manual = !modo_manual; // Alterna entre modos

        if (modo_manual) {
            uart_puts(UART_ID, "Modo manual ativado.\r\n");
            uart_puts(UART_ID, "Digite a temperatura desejada (2 dígitos, ex: 15, 25): ");
            ler_entrada_usuario(); // Lê a entrada do usuário
        } else {
            uart_puts(UART_ID, "Modo automático ativado.\r\n");
            temperatura_simulada = 15.0; // Reinicia a temperatura no modo automático
            aquecendo = true;            // Reinicia o estado de aquecimento
        }

        ultimo_tempo = tempo_atual;
    }
}

// Função para ler a entrada do usuário via UART
void ler_entrada_usuario() {
    char buffer[3]; // Buffer para armazenar 2 dígitos + terminador
    uint8_t index = 0;

    while (index < 2) { // Lê exatamente 2 caracteres
        if (uart_is_readable(UART_ID)) {
            char c = uart_getc(UART_ID); // Lê um caractere da UART

            if (c >= '0' && c <= '9') { // Verifica se é um dígito válido
                buffer[index++] = c; // Armazena o caractere no buffer
            }
        }
    }
    buffer[index] = '\0'; // Adiciona o terminador de string

    // Converte a string para float
    temperatura_desejada = atof(buffer);

    // Valida a temperatura desejada
    if (temperatura_desejada < 15.0 || temperatura_desejada > 30.0) {
        uart_puts(UART_ID, "Valor inválido! Usando 25.0°C como padrão.\r\n");
        temperatura_desejada = 25.0; // Valor padrão
    } else {
        char msg[50];
        sprintf(msg, "Temperatura desejada definida para %.1f°C.\r\n", temperatura_desejada);
        uart_puts(UART_ID, msg);
    }
}

// Simula a variação de temperatura no modo automático
void ajustar_temperatura() {
    if (!modo_manual) {
        if (aquecendo) {
            temperatura_simulada += 2.0; // Aumenta a temperatura em 2°C
            if (temperatura_simulada >= 25.0) {
                temperatura_simulada = 25.0;
                aquecendo = false; // Muda para resfriamento
            }
        } else {
            temperatura_simulada -= 2.0; // Diminui a temperatura em 2°C
            if (temperatura_simulada <= 15.0) {
                temperatura_simulada = 15.0;
                aquecendo = true; // Muda para aquecimento
            }
        }
    }
}

// Controla os LEDs para simular aquecimento/resfriamento
void controlar_temperatura() {
    // Obtém os slices PWM associados aos pinos
    uint slice_peltier = pwm_gpio_to_slice_num(LED_PELTIER); // Slice para GPIO12
    uint slice_lampada = pwm_gpio_to_slice_num(LED_LAMPADA); // Slice para GPIO13

    if (modo_manual) {
        // Modo manual: ajusta para alcançar a temperatura desejada
        if (temperatura_simulada < temperatura_desejada) {
            pwm_set_chan_level(slice_peltier, PWM_CHAN_A, 0);   // Desliga Peltier
            pwm_set_chan_level(slice_lampada, PWM_CHAN_B, 100); // Liga lâmpada
            uart_puts(UART_ID, "🔴 Aquecendo (modo manual)...\r\n");
        } else if (temperatura_simulada > temperatura_desejada) {
            pwm_set_chan_level(slice_peltier, PWM_CHAN_A, 100); // Liga Peltier
            pwm_set_chan_level(slice_lampada, PWM_CHAN_B, 0);  // Desliga lâmpada
            uart_puts(UART_ID, "🔵 Resfriando (modo manual)...\r\n");
        } else {
            pwm_set_chan_level(slice_peltier, PWM_CHAN_A, 0);   // Desliga Peltier
            pwm_set_chan_level(slice_lampada, PWM_CHAN_B, 0);   // Desliga lâmpada
            uart_puts(UART_ID, "✅ Temperatura desejada alcançada.\r\n");
        }
    } else {
        // Modo automático
        if (aquecendo) {
            pwm_set_chan_level(slice_peltier, PWM_CHAN_A, 0);   // Desliga Peltier
            pwm_set_chan_level(slice_lampada, PWM_CHAN_B, 100); // Liga lâmpada
            uart_puts(UART_ID, "🔴 Aquecendo (modo automático)...\r\n");
        } else {
            pwm_set_chan_level(slice_peltier, PWM_CHAN_A, 100); // Liga Peltier
            pwm_set_chan_level(slice_lampada, PWM_CHAN_B, 0);   // Desliga lâmpada
            uart_puts(UART_ID, "🔵 Resfriando (modo automático)...\r\n");
        }
    }

    // Exibe a temperatura atual
    char msg[50];
    sprintf(msg, "🌡️ Temperatura atual: %.1f°C\r\n", temperatura_simulada);
    uart_puts(UART_ID, msg);
}

int main() {
    configurar_hardware();

    while (true) {
        ajustar_temperatura();      // Ajusta a temperatura simulada
        controlar_temperatura();    // Controla os LEDs com base na temperatura
        sleep_ms(2000);             // Espera 2 segundos
    }
}