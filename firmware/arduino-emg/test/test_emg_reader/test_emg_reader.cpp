#include "Arduino.h"
#include <emg.h>
#include <unity.h>

// stream_size = 32 samples (uint16_t), buffer = 64 bytes
constexpr uint8_t STREAM_SIZE = 32;
constexpr uint8_t BUFFER_BYTES = STREAM_SIZE * 2;

void setUp() { mock_reset(); }

void tearDown() {}

void test_add_reader_ok() {
  emg::Reader reader;
  TEST_ASSERT_TRUE(reader.add_reader(emg::Muscle::LeftBicep, A0));
  TEST_ASSERT_TRUE(reader.add_reader(emg::Muscle::RightBicep, A1));
}

void test_add_reader_duplicate() {
  emg::Reader reader;
  TEST_ASSERT_TRUE(reader.add_reader(emg::Muscle::LeftBicep, A0));
  TEST_ASSERT_FALSE(reader.add_reader(emg::Muscle::LeftBicep, A1));
}

void test_read_all_increments_count() {
  emg::Reader reader;
  reader.add_reader(emg::Muscle::LeftBicep, A0);

  TEST_ASSERT_EQUAL_UINT8(0, reader.get_count(emg::Muscle::LeftBicep));

  mock_analog_values[A0] = 100;
  reader.read_all();
  TEST_ASSERT_EQUAL_UINT8(1, reader.get_count(emg::Muscle::LeftBicep));

  mock_analog_values[A0] = 200;
  reader.read_all();
  TEST_ASSERT_EQUAL_UINT8(2, reader.get_count(emg::Muscle::LeftBicep));
}

void test_read_all_and_get_data() {
  emg::Reader reader;
  reader.add_reader(emg::Muscle::LeftBicep, A0);
  reader.add_reader(emg::Muscle::RightBicep, A1);

  mock_analog_values[A0] = 512;
  mock_analog_values[A1] = 1023;
  reader.read_all();

  uint8_t out[BUFFER_BYTES] = {};
  reader.get_data(emg::Muscle::LeftBicep, out);
  uint16_t *samples = reinterpret_cast<uint16_t *>(out);
  TEST_ASSERT_EQUAL_UINT16(512, samples[0]);

  memset(out, 0, sizeof(out));
  reader.get_data(emg::Muscle::RightBicep, out);
  samples = reinterpret_cast<uint16_t *>(out);
  TEST_ASSERT_EQUAL_UINT16(1023, samples[0]);
}

void test_get_data_multiple_samples() {
  emg::Reader reader;
  reader.add_reader(emg::Muscle::LeftBicep, A0);

  uint16_t values[] = {100, 200, 300};
  for (uint16_t v : values) {
    mock_analog_values[A0] = v;
    reader.read_all();
  }

  uint8_t out[BUFFER_BYTES] = {};
  reader.get_data(emg::Muscle::LeftBicep, out);
  uint16_t *samples = reinterpret_cast<uint16_t *>(out);

  TEST_ASSERT_EQUAL_UINT16(100, samples[0]);
  TEST_ASSERT_EQUAL_UINT16(200, samples[1]);
  TEST_ASSERT_EQUAL_UINT16(300, samples[2]);
}

void test_is_full() {
  emg::Reader reader;
  reader.add_reader(emg::Muscle::LeftBicep, A0);

  // Escribir STREAM_SIZE - 1 muestras: last_idx llega a STREAM_SIZE - 1
  for (uint8_t i = 0; i < STREAM_SIZE - 1; i++) {
    mock_analog_values[A0] = i;
    reader.read_all();
  }
  TEST_ASSERT_TRUE(reader.is_full(emg::Muscle::LeftBicep));
  TEST_ASSERT_TRUE(reader.is_full());

  // Una lectura mas: last_idx wrappea a 0
  reader.read_all();
  TEST_ASSERT_FALSE(reader.is_full(emg::Muscle::LeftBicep));
}

void test_ring_buffer_wraps() {
  emg::Reader reader;
  reader.add_reader(emg::Muscle::LeftBicep, A0);

  // Escribir STREAM_SIZE + 5 muestras
  for (uint16_t i = 0; i < STREAM_SIZE + 5; i++) {
    mock_analog_values[A0] = i;
    reader.read_all();
  }

  // last_idx = (STREAM_SIZE + 5) % STREAM_SIZE == 5
  TEST_ASSERT_EQUAL_UINT8(5, reader.get_count(emg::Muscle::LeftBicep));

  uint8_t out[BUFFER_BYTES] = {};
  reader.get_data(emg::Muscle::LeftBicep, out);
  uint16_t *samples = reinterpret_cast<uint16_t *>(out);

  // Posiciones 0-4 sobreescritas con valores STREAM_SIZE..STREAM_SIZE+4
  for (uint8_t i = 0; i < 5; i++) {
    TEST_ASSERT_EQUAL_UINT16(STREAM_SIZE + i, samples[i]);
  }
  // Posiciones 5-31 conservan sus valores originales (5..31)
  for (uint8_t i = 5; i < STREAM_SIZE; i++) {
    TEST_ASSERT_EQUAL_UINT16(i, samples[i]);
  }
}

void test_inactive_muscle() {
  emg::Reader reader;
  // Sin registrar ningun musculo
  TEST_ASSERT_EQUAL_UINT8(0, reader.get_count(emg::Muscle::LeftBicep));
  TEST_ASSERT_FALSE(reader.is_full(emg::Muscle::LeftBicep));
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_add_reader_ok);
  RUN_TEST(test_add_reader_duplicate);
  RUN_TEST(test_read_all_increments_count);
  RUN_TEST(test_read_all_and_get_data);
  RUN_TEST(test_get_data_multiple_samples);
  RUN_TEST(test_is_full);
  RUN_TEST(test_ring_buffer_wraps);
  RUN_TEST(test_inactive_muscle);
  return UNITY_END();
}
