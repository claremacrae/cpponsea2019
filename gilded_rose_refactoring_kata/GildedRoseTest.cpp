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

TEST(GildedRoseTest, TestUpdateQuality)
{
    vector<Item> items;
    items.push_back(Item("Foo", 1, 1));
    GildedRose app(items);
    app.updateQuality();
    EXPECT_EQ("Foo", app.items[0].name);
    EXPECT_EQ(0, app.items[0].sellIn);
    EXPECT_EQ(0, app.items[0].quality);
}

/*
 * Things to note here:
 * 
 * - 52% still coverage of GildedRose.cpp
 * - Now testing the actual calculation result (new quality)
 */