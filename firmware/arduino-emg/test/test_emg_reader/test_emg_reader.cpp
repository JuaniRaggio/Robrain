#include "emg_reader.h"
#include <unity.h>

void setUp() {
  mock_reset();
}

void tearDown() {}

void test_add_reader_ok() {
  emg::Reader reader;
  TEST_ASSERT_TRUE(reader.add_reader(emg::Muscle::LeftBicep, A0));
  TEST_ASSERT_TRUE(reader.add_reader(emg::Muscle::RightBicep, A1));
  // Sin lecturas, get devuelve 0
  TEST_ASSERT_EQUAL_UINT16(0, reader.get(emg::Muscle::LeftBicep));
  TEST_ASSERT_EQUAL_UINT16(0, reader.get(emg::Muscle::RightBicep));
}

void test_add_reader_duplicate() {
  emg::Reader reader;
  TEST_ASSERT_TRUE(reader.add_reader(emg::Muscle::LeftBicep, A0));
  TEST_ASSERT_FALSE(reader.add_reader(emg::Muscle::LeftBicep, A1));
}

void test_read_all_and_get() {
  emg::Reader reader;
  reader.add_reader(emg::Muscle::LeftBicep, A0);
  reader.add_reader(emg::Muscle::RightBicep, A1);

  mock_analog_values[A0] = 512;
  mock_analog_values[A1] = 1023;
  reader.read_all();

  TEST_ASSERT_EQUAL_UINT16(512, reader.get(emg::Muscle::LeftBicep));
  TEST_ASSERT_EQUAL_UINT16(1023, reader.get(emg::Muscle::RightBicep));
}

void test_get_history() {
  emg::Reader reader;
  reader.add_reader(emg::Muscle::LeftBicep, A0);

  uint16_t values[] = {100, 200, 300};
  for (uint16_t v : values) {
    mock_analog_values[A0] = v;
    reader.read_all();
  }

  uint16_t out[3] = {};
  uint8_t got = reader.get(emg::Muscle::LeftBicep, 3, out);
  TEST_ASSERT_EQUAL_UINT8(3, got);
  // Newest first
  TEST_ASSERT_EQUAL_UINT16(300, out[0]);
  TEST_ASSERT_EQUAL_UINT16(200, out[1]);
  TEST_ASSERT_EQUAL_UINT16(100, out[2]);
}

void test_get_history_clamps_to_count() {
  emg::Reader reader;
  reader.add_reader(emg::Muscle::LeftBicep, A0);

  mock_analog_values[A0] = 42;
  reader.read_all();

  uint16_t out[5] = {};
  uint8_t got = reader.get(emg::Muscle::LeftBicep, 5, out);
  TEST_ASSERT_EQUAL_UINT8(1, got);
  TEST_ASSERT_EQUAL_UINT16(42, out[0]);
}

void test_ring_buffer_wraps() {
  emg::Reader reader;
  reader.add_reader(emg::Muscle::LeftBicep, A0);

  // Escribir HISTORY_SIZE + 5 muestras
  for (uint16_t i = 0; i < emg::HISTORY_SIZE + 5; i++) {
    mock_analog_values[A0] = i;
    reader.read_all();
  }

  // El ultimo valor es HISTORY_SIZE + 4
  TEST_ASSERT_EQUAL_UINT16(emg::HISTORY_SIZE + 4,
                           reader.get(emg::Muscle::LeftBicep));

  // Pedir todo el historial (HISTORY_SIZE valores)
  uint16_t out[emg::HISTORY_SIZE] = {};
  uint8_t got = reader.get(emg::Muscle::LeftBicep, emg::HISTORY_SIZE, out);
  TEST_ASSERT_EQUAL_UINT8(emg::HISTORY_SIZE, got);

  // Verificar que son los ultimos HISTORY_SIZE valores, newest first
  for (uint8_t i = 0; i < emg::HISTORY_SIZE; i++) {
    uint16_t expected = emg::HISTORY_SIZE + 4 - i;
    TEST_ASSERT_EQUAL_UINT16(expected, out[i]);
  }
}

void test_inactive_muscle_returns_zero() {
  emg::Reader reader;
  // No registramos nada
  TEST_ASSERT_EQUAL_UINT16(0, reader.get(emg::Muscle::LeftBicep));

  uint16_t out[3] = {};
  uint8_t got = reader.get(emg::Muscle::LeftBicep, 3, out);
  TEST_ASSERT_EQUAL_UINT8(0, got);
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_add_reader_ok);
  RUN_TEST(test_add_reader_duplicate);
  RUN_TEST(test_read_all_and_get);
  RUN_TEST(test_get_history);
  RUN_TEST(test_get_history_clamps_to_count);
  RUN_TEST(test_ring_buffer_wraps);
  RUN_TEST(test_inactive_muscle_returns_zero);
  return UNITY_END();
}
