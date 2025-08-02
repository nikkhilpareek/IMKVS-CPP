#include <gtest/gtest.h>
#include "KeyValueStore.h"

// Test fixture for creating a fresh KeyValueStore for each test case
class KeyValueStoreTest : public ::testing::Test {
protected:
    KeyValueStore kvs;
};

// Test case to verify basic SET and GET operations
TEST_F(KeyValueStoreTest, SetAndGet) {
    kvs.set("name", "Nikhil");
    ASSERT_TRUE(kvs.get("name").has_value());
    EXPECT_EQ(kvs.get("name").value(), "Nikhil"); 

    kvs.set("age", "20");
    EXPECT_EQ(kvs.get("age").value(), "20");
}

// Test case for the REMOVE operation
TEST_F(KeyValueStoreTest, Remove) {
    kvs.set("key_to_remove", "some_value");
    ASSERT_TRUE(kvs.get("key_to_remove").has_value());

    kvs.remove("key_to_remove");
    EXPECT_FALSE(kvs.get("key_to_remove").has_value());
}

// Test case for the COUNT operation
TEST_F(KeyValueStoreTest, Count) {
    EXPECT_EQ(kvs.count(), 0);
    kvs.set("key1", "val1");
    EXPECT_EQ(kvs.count(), 1);
    kvs.set("key2", "val2");
    EXPECT_EQ(kvs.count(), 2);
    kvs.remove("key1");
    EXPECT_EQ(kvs.count(), 1);
}

// Test case for basic INCR functionality
TEST_F(KeyValueStoreTest, Increment) {
    auto result1 = kvs.incr("counter");
    ASSERT_TRUE(result1.has_value());
    EXPECT_EQ(result1.value(), 1);

    auto result2 = kvs.incr("counter");
    ASSERT_TRUE(result2.has_value());
    EXPECT_EQ(result2.value(), 2);
}

// Test case for basic transaction rollback
TEST_F(KeyValueStoreTest, TransactionRollback) {
    kvs.set("status", "initial");
    kvs.begin();
    kvs.set("status", "modified");
    EXPECT_EQ(kvs.get("status").value(), "modified");
    kvs.rollback();
    EXPECT_EQ(kvs.get("status").value(), "initial");
}