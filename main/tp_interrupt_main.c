/*
    Acadêmicos: Eduardo Orsi e Gabriel Bodenmuller
    Disciplina: Sistemas em Tempo Real 02.2022
    Professor: MSC Felipe Viel
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/touch_pad.h"
#include "soc/rtc_periph.h"
#include "soc/sens_periph.h"
#include "freertos/semphr.h"

static const char *TAG = "Touch pad";

#define TOUCH_THRESH_NO_USE   (0)
#define TOUCH_THRESH_PERCENT  (80)
#define TOUCHPAD_FILTER_TOUCH_PERIOD (10)

static bool s_pad_activated[TOUCH_PAD_MAX];
static uint32_t s_pad_init_val[TOUCH_PAD_MAX];

portMUX_TYPE mutual_exclusion = portMUX_INITIALIZER_UNLOCKED;
SemaphoreHandle_t mutual_exclusion_mutex;

/* ABS */
bool abs_ativado = false;
bool roda_travada = false;
/* Vidro */
bool vidro_fechado = false;
/* Airbag */
bool airbag = false;
/* Cinto de Segurança */
bool passageiro_sem_cinto_de_seguranca = false;
/* Farol */
bool farol_ligado = false;
/* Travas */
bool carro_travado = false;
/* Injeção Eletrônica*/
bool injecao_eletronica = false;
/* Temperatura */
int temperatura = 0;
/* Consumo*/
float consumo = 20.01;
bool mostrar_consumo = false;
int aux = 0;

void controlador_abs(bool forca_do_pedal) {
    if (forca_do_pedal == true) {
        vTaskDelay(0.005 / portTICK_PERIOD_MS);
        abs_ativado = true;
        printf("\nABS Ativado\n");
        abs_ativado = false;
        roda_travada = false;
    }
}

void sensor_roda() {
    uint64_t eus, eus2;
    while(1) {
        xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);
        eus = esp_timer_get_time();
        if(roda_travada == true){
            vTaskDelay(0.001 / portTICK_PERIOD_MS);
            controlador_abs(roda_travada);
            eus2 = esp_timer_get_time();
            printf("Tempo de execucao: %llu ms\nValor de Ticks em 1 ms: %u\n", (eus2 - eus), portTICK_PERIOD_MS);
        }
        xSemaphoreGive(mutual_exclusion_mutex);
        vTaskDelay(100/ portTICK_PERIOD_MS);
        
    }
}

void controlador_airbag(bool comando) {
    if (comando == true) {
        vTaskDelay(0.005 / portTICK_PERIOD_MS);
        airbag = false;
        printf("\nCarro Batido!\n");
    }
}

void sensor_airbag() {
    uint64_t eus, eus2;
    while(1) {
        
        xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);
        eus = esp_timer_get_time();
        if(airbag == true){
            vTaskDelay(0.001 / portTICK_PERIOD_MS);
            controlador_airbag(airbag);
            eus2 = esp_timer_get_time();
            printf("Tempo de execucao: %llu ms\nValor de Ticks em 1 ms: %u\n", (eus2 - eus), portTICK_PERIOD_MS);
        }
        xSemaphoreGive(mutual_exclusion_mutex);

        vTaskDelay(100/ portTICK_PERIOD_MS);
        
    }
}

void controlador_cinto_de_seguranca(bool comando) {
    if (comando == true) {
        vTaskDelay(0.005 / portTICK_PERIOD_MS);
        printf("\nCinto acionado\n");
        passageiro_sem_cinto_de_seguranca = false;
    }
}

void sensor_cinto_de_seguranca() {
    uint64_t eus, eus2;
    while(1) {
        
        xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);
        eus = esp_timer_get_time();
        if(passageiro_sem_cinto_de_seguranca == true){
            vTaskDelay(0.001 / portTICK_PERIOD_MS);
            controlador_cinto_de_seguranca(passageiro_sem_cinto_de_seguranca);
            eus2 = esp_timer_get_time();
            printf("Tempo de execucao: %llu ms\nValor de Ticks em 1 ms: %u\n", (eus2 - eus), portTICK_PERIOD_MS);
        }
        xSemaphoreGive(mutual_exclusion_mutex);

        vTaskDelay(100/ portTICK_PERIOD_MS);
        
    }
}

void controlador_farol(bool comando) {
    if (comando == true) {
        vTaskDelay(0.005 / portTICK_PERIOD_MS);
        printf("\nFarol acionado\n");
        farol_ligado = false;
    }
}

void controlador_vidros(bool comando) {
    if (comando == true) {
        vTaskDelay(0.005 / portTICK_PERIOD_MS);
        printf("\nVidro acionado\n");
        vidro_fechado = false;
    }
}

void controlador_travas(bool comando) {
    if (comando == true) {
        vTaskDelay(0.005 / portTICK_PERIOD_MS);
        printf("\nTravas acionadas\n");
        carro_travado = false;
    }
}

void sensor_travas_vidros_farol() {
    uint64_t eus, eus2;
    while(1) {
        
        xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);
        eus = esp_timer_get_time();
        if(carro_travado == true){
            vTaskDelay(0.001 / portTICK_PERIOD_MS);
            controlador_travas(carro_travado);
            eus2 = esp_timer_get_time();
            printf("Tempo de execucao: %llu ms\nValor de Ticks em 1 ms: %u\n", (eus2 - eus), portTICK_PERIOD_MS);
        }
        xSemaphoreGive(mutual_exclusion_mutex);

        xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);
        eus = esp_timer_get_time();
        if(farol_ligado == true){
            vTaskDelay(0.001 / portTICK_PERIOD_MS);
            controlador_farol(farol_ligado);
            eus2 = esp_timer_get_time();
            printf("Tempo de execucao: %llu ms\nValor de Ticks em 1 ms: %u\n", (eus2 - eus), portTICK_PERIOD_MS);
        }
        xSemaphoreGive(mutual_exclusion_mutex);

        xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);
        eus = esp_timer_get_time();
        if(vidro_fechado == true){
            vTaskDelay(0.001 / portTICK_PERIOD_MS);
            controlador_vidros(vidro_fechado);
            eus2 = esp_timer_get_time();
            printf("Tempo de execucao: %llu ms\nValor de Ticks em 1 ms: %u\n", (eus2 - eus), portTICK_PERIOD_MS);
        }
        xSemaphoreGive(mutual_exclusion_mutex);

        vTaskDelay(100/ portTICK_PERIOD_MS);
        
    }
}

void controlador_temperatura(int comando) {
    if (comando >= 100) {
        uint64_t eus1, eus3;
        eus1 = esp_timer_get_time();
        vTaskDelay(0.005 / portTICK_PERIOD_MS);
        temperatura = temperatura - 2;
        printf("\nArrefecimento acionado e resfriando o motor\n");
        eus3 = esp_timer_get_time();
        printf("Tempo de execucao: %llu ms\nValor de Ticks em 1 ms: %u\n", (eus3 - eus1), portTICK_PERIOD_MS);
    }
}

void controlador_injecao_eletronica(bool comando) {
    if (comando == true) {
        vTaskDelay(0.001 / portTICK_PERIOD_MS);
        printf("\nCarro acelerando\n");
        injecao_eletronica = false;
    }
}

void sensor_injecao_eletronica() {
    uint64_t eus, eus2, eus1, eus3;
    while(1) {
        xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);
        eus = esp_timer_get_time();
        if(injecao_eletronica == true){
            vTaskDelay(0.001 / portTICK_PERIOD_MS);
            controlador_injecao_eletronica(injecao_eletronica);
            aux++;
            temperatura++;
            controlador_temperatura(temperatura);
            if(aux % 10 == 0) {
                if (consumo > 6.0) {
                    xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);
                    consumo = consumo - 0.01;
                    xSemaphoreGive(mutual_exclusion_mutex);
                }
            }
            eus2 = esp_timer_get_time();
            printf("Tempo de execucao: %llu ms\nValor de Ticks em 1 ms: %u\n", (eus2 - eus), portTICK_PERIOD_MS);
        }
        xSemaphoreGive(mutual_exclusion_mutex);

        vTaskDelay(100/ portTICK_PERIOD_MS);
    }
}

void controlador_consumo() {
    vTaskDelay(0.005 / portTICK_PERIOD_MS);
    printf("\nConsumo médio: %f\n", consumo);
}

void sensor_consumo() {
    uint64_t eus, eus2;
    while(1) {
        xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);
        eus = esp_timer_get_time();
        if (mostrar_consumo == true) {
            vTaskDelay(0.001 / portTICK_PERIOD_MS);
            controlador_consumo();
            
            mostrar_consumo = false;
            eus2 = esp_timer_get_time();
            printf("Tempo de execucao: %llu ms\nValor de Ticks em 1 ms: %u\n", (eus2 - eus), portTICK_PERIOD_MS);
            
        }
        xSemaphoreGive(mutual_exclusion_mutex);

        vTaskDelay(100/ portTICK_PERIOD_MS);
    }
}

// Define os valores iniciais das sensores
static void tp_example_set_thresholds(void)
{
    uint16_t touch_value;
    for (int i = 0; i < TOUCH_PAD_MAX; i++) {
        touch_pad_read_filtered(i, &touch_value);
        s_pad_init_val[i] = touch_value;
        ESP_ERROR_CHECK(touch_pad_set_thresh(i, touch_value * 2 / 3));

    }
}

static void tp_example_read_task(void *pvParameter)
{
    while(1) {
        //interrupt mode, enable touch interrupt
        touch_pad_intr_enable();
        for (int i = 0; i < TOUCH_PAD_MAX; i++) {
            if (s_pad_activated[i] == true) {
                if (i == 3) {
                    xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);
                    if (vidro_fechado == false) {
                        vidro_fechado = true;
                    }
                    xSemaphoreGive(mutual_exclusion_mutex);
                }
                else if (i == 2) {
                    xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);
                    roda_travada = true;
                    xSemaphoreGive(mutual_exclusion_mutex);
                }
                else if (i == 0) {
                    xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);
                    airbag = true;
                    xSemaphoreGive(mutual_exclusion_mutex);
                }
                else if(i == 5) {
                    xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);
                    farol_ligado = true;
                    xSemaphoreGive(mutual_exclusion_mutex);
                }
                else if (i == 6) {
                    xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);
                    if (carro_travado == false) {
                        carro_travado = true;
                    }
                    xSemaphoreGive(mutual_exclusion_mutex);
                } 
                else if (i == 4) {
                    xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);
                    if (passageiro_sem_cinto_de_seguranca == false) {
                        passageiro_sem_cinto_de_seguranca = true;
                    }
                    xSemaphoreGive(mutual_exclusion_mutex);
                }
                else if (i == 7) {
                    xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);
                    injecao_eletronica = true;
                    xSemaphoreGive(mutual_exclusion_mutex);
                }
                else if (i == 9) {
                    xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);
                    mostrar_consumo = true;
                    xSemaphoreGive(mutual_exclusion_mutex);
                }
                vTaskDelay(100 / portTICK_PERIOD_MS);
                xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);
                s_pad_activated[i] = false;
                xSemaphoreGive(mutual_exclusion_mutex);
            }
        }
        vTaskDelay(100/ portTICK_PERIOD_MS);
    }
}

// Indentifica o pino que foi acionado e salva no array
static void tp_example_rtc_intr(void *arg)
{
    uint32_t pad_intr = touch_pad_get_status();
    touch_pad_clear_status();
    for (int i = 0; i < TOUCH_PAD_MAX; i++) {
        if ((pad_intr >> i) & 0x01) {
            xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);
            s_pad_activated[i] = true;
            xSemaphoreGive(mutual_exclusion_mutex);
        }
    }
}

// Incia o touchpad
static void tp_example_touch_pad_init(void)
{
    for (int i = 0; i < TOUCH_PAD_MAX; i++) {
        touch_pad_config(i, TOUCH_THRESH_NO_USE);
    }
}

void app_main(void)
{
    // Initialize touch pad peripheral, it will start a timer to run a filter
    ESP_LOGI(TAG, "Initializing touch pad");
    ESP_ERROR_CHECK(touch_pad_init());

    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);
    tp_example_touch_pad_init();

    touch_pad_filter_start(TOUCHPAD_FILTER_TOUCH_PERIOD);
    tp_example_set_thresholds();

    touch_pad_isr_register(tp_example_rtc_intr, NULL);

    mutual_exclusion_mutex = xSemaphoreCreateCounting(3,3);

    if( mutual_exclusion_mutex != NULL ){
       printf("Mutex criado\n");
    }

    xTaskCreate(&tp_example_read_task, "touch_pad_read_task", 512, NULL, 25, NULL);
    xTaskCreate(&sensor_airbag, "sensor_airbag", 2048, NULL, 24, NULL);
    xTaskCreate(&sensor_roda, "sensor_freio", 2048, NULL, 20, NULL);
    xTaskCreate(&sensor_travas_vidros_farol, "sensor_travas_vidros_farol", 2048, NULL, 5, NULL);
    xTaskCreate(&sensor_cinto_de_seguranca, "sensor_cinto_de_seguranca", 2048, NULL, 10, NULL);
    xTaskCreate(&sensor_injecao_eletronica, "sensor_injecao_eletronica", 2048, NULL, 20, NULL);
    //xTaskCreate(&sensor_temperatura, "sensor_temperatura", 2500, NULL, 23, NULL);
    xTaskCreate(&sensor_consumo, "sensor_consumo", 2048, NULL, 15, NULL);
    //xTaskCreate(&sensores, "sensores", 2048, NULL, 15, NULL);
}
