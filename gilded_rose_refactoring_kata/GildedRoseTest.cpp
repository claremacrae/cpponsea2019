#include <gtest/gtest.h>

#include "GildedRose.h"

TEST(GildedRoseTest, Foo)
{
    vector<Item> items;
    items.push_back(Item("Foo", 0, 0));
    GildedRose app(items);
    app.updateQuality();
    EXPECT_EQ("Foo", app.items[0].name);
}

/*
 * Things to note here:
 * 
 * - 52% coverage of GildedRose.cpp
 * - The actual calculation result (new quality) is not tested
 */