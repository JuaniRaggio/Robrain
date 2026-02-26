#include <Arduino.h>

const int  EMG_PIN     = A0;
const long INTERVAL_US = 200000;
const int  VENTANA     = 20;
const int  TOP_N       = 10;

int val_min = 512;
int val_max = 512;

int muestras[VENTANA] = {0};
int idx = 0;

unsigned long last_sample_us = 0;

// ── CALIBRACION ─────────────────────────────────────────────────
void calibrar() {
    int tops[TOP_N]    = {0};
    int bottoms[TOP_N] = {1023, 1023, 1023, 1023, 1023,
                          1023, 1023, 1023, 1023, 1023};

    // REPOSO
    Serial.println("=== CALIBRACION ===");
    Serial.println("1) Relaja el musculo...");
    delay(3000);

    for (int i = 0; i < 1500; i++) {
        int val = analogRead(EMG_PIN);

        int max_idx = 0;
        for (int j = 1; j < TOP_N; j++)
            if (bottoms[j] > bottoms[max_idx]) max_idx = j;
        if (val < bottoms[max_idx]) bottoms[max_idx] = val;

        delayMicroseconds(INTERVAL_US);
    }

    long suma = 0;
    for (int i = 0; i < TOP_N; i++) suma += bottoms[i];
    val_min = suma / TOP_N;
    Serial.print("Reposo: "); Serial.println(val_min);

    // CONTRACCION
    Serial.println("2) Contrae al MAXIMO...");
    delay(3000);

    int tops2[TOP_N] = {0};
    for (int i = 0; i < 1500; i++) {
        int val = analogRead(EMG_PIN);

        int min_idx = 0;
        for (int j = 1; j < TOP_N; j++)
            if (tops2[j] < tops2[min_idx]) min_idx = j;
        if (val > tops2[min_idx]) tops2[min_idx] = val;

        delayMicroseconds(INTERVAL_US);
    }

    suma = 0;
    for (int i = 0; i < TOP_N; i++) suma += tops2[i];
    val_max = suma / TOP_N;
    Serial.print("Contraccion: "); Serial.println(val_max);
    Serial.println("=== LISTO! ===");
}

// ── MEDIANA TRUNCADA ─────────────────────────────────────────────
int mediana_promedio() {
    int temp[VENTANA];
    for (int i = 0; i < VENTANA; i++) temp[i] = muestras[i];

    // Ordenar de menor a mayor
    for (int i = 0; i < VENTANA-1; i++)
        for (int j = i+1; j < VENTANA; j++)
            if (temp[j] < temp[i]) {
                int t = temp[i]; temp[i] = temp[j]; temp[j] = t;
            }

    // Descartar 20% de cada lado y promediar el centro
    int descartar = VENTANA / 5;  // 4 de cada lado
    long suma = 0;
    for (int i = descartar; i < VENTANA - descartar; i++) suma += temp[i];
    return suma / (VENTANA - 2 * descartar);
}

// ── SETUP ────────────────────────────────────────────────────────
void setup() {
    Serial.begin(1000000);
    pinMode(EMG_PIN, INPUT);
    calibrar();
    for (int i = 0; i < VENTANA; i++) muestras[i] = val_min;
}

// ── LOOP ─────────────────────────────────────────────────────────
void loop() {
    unsigned long now = micros();

    if (now - last_sample_us >= (unsigned long)INTERVAL_US) {
        last_sample_us = now;

        int raw = analogRead(EMG_PIN);

        // Guardar en ventana circular
        muestras[idx] = raw;
        idx = (idx + 1) % VENTANA;

        // Mediana truncada para eliminar picos
        int promedio = mediana_promedio();

        // Normalizar entre val_min y val_max → 0 a 100
        int normalizado = (int)((float)(promedio - val_min) / (val_max - val_min) * 100.0);
        if (normalizado < 0)   normalizado = 0;
        if (normalizado > 100) normalizado = 100;

        Serial.print("EMG:");
        Serial.println(normalizado);
    }
}
