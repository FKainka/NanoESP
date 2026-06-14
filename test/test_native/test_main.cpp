// Native unit tests for the pure string/byte helpers of NanoESP_MQTT.
// These run on the desktop host via `pio test -e native`, using the stub
// Arduino.h / NanoESP.h under test/native/mocks so the real NanoESP_MQTT.cpp
// compiles without any hardware dependency.
#include <unity.h>

#include "NanoESP_MQTT.h"

static NanoESP nano;
static NanoESP_MQTT mqtt(nano);

void setUp(void) {}
void tearDown(void) {}

// ----------------------------- topicCompare --------------------------------
// topicCompare(filter, topic): filter is the subscription, topic the incoming.

void test_topic_exact_match(void) {
  TEST_ASSERT_TRUE(mqtt.topicCompare("a/b/c", "a/b/c"));
}

void test_topic_exact_mismatch(void) {
  TEST_ASSERT_FALSE(mqtt.topicCompare("a/b/c", "a/b/d"));
}

void test_topic_empty_filter_is_false(void) {
  TEST_ASSERT_FALSE(mqtt.topicCompare("", "a/b/c"));
}

void test_topic_single_plus_match(void) {
  TEST_ASSERT_TRUE(mqtt.topicCompare("a/+/c", "a/b/c"));
}

void test_topic_single_plus_mismatch_tail(void) {
  TEST_ASSERT_FALSE(mqtt.topicCompare("a/+/c", "a/b/d"));
}

void test_topic_single_plus_mismatch_head(void) {
  TEST_ASSERT_FALSE(mqtt.topicCompare("a/+/c", "x/b/c"));
}

void test_topic_multi_plus_match(void) {
  // '+' may appear in several middle segments; the last segment is literal.
  TEST_ASSERT_TRUE(mqtt.topicCompare("a/+/c/+/e", "a/b/c/d/e"));
}

void test_topic_hash_match(void) {
  TEST_ASSERT_TRUE(mqtt.topicCompare("a/#", "a/b/c"));
}

void test_topic_hash_mismatch_prefix(void) {
  TEST_ASSERT_FALSE(mqtt.topicCompare("a/#", "x/b/c"));
}

// --------------------------------- utf8 ------------------------------------
// utf8(input, output): output[0]=0, output[1]=len, output[2..]=payload bytes.

void test_utf8_encodes_length_and_payload(void) {
  unsigned char out[4] = {0xFF, 0xFF, 0xFF, 0xFF};
  mqtt.utf8("Hi", out);
  TEST_ASSERT_EQUAL_UINT8(0, out[0]);
  TEST_ASSERT_EQUAL_UINT8(2, out[1]);
  TEST_ASSERT_EQUAL_UINT8('H', out[2]);
  TEST_ASSERT_EQUAL_UINT8('i', out[3]);
}

void test_utf8_empty_string(void) {
  unsigned char out[2] = {0xFF, 0xFF};
  mqtt.utf8("", out);
  TEST_ASSERT_EQUAL_UINT8(0, out[0]);
  TEST_ASSERT_EQUAL_UINT8(0, out[1]);
}

// -------------------------------- charAdd ----------------------------------
// charAdd(A, lenA, B, lenB, out): concatenates A followed by B into out.

void test_charadd_concatenates_in_order(void) {
  unsigned char a[3] = {1, 2, 3};
  unsigned char b[2] = {4, 5};
  unsigned char out[5] = {0};
  mqtt.charAdd(a, 3, b, 2, out);
  const unsigned char expected[5] = {1, 2, 3, 4, 5};
  TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, out, 5);
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_topic_exact_match);
  RUN_TEST(test_topic_exact_mismatch);
  RUN_TEST(test_topic_empty_filter_is_false);
  RUN_TEST(test_topic_single_plus_match);
  RUN_TEST(test_topic_single_plus_mismatch_tail);
  RUN_TEST(test_topic_single_plus_mismatch_head);
  RUN_TEST(test_topic_multi_plus_match);
  RUN_TEST(test_topic_hash_match);
  RUN_TEST(test_topic_hash_mismatch_prefix);
  RUN_TEST(test_utf8_encodes_length_and_payload);
  RUN_TEST(test_utf8_empty_string);
  RUN_TEST(test_charadd_concatenates_in_order);
  return UNITY_END();
}
