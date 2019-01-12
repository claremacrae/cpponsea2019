#include <ApprovalTests.hpp>
#include <gtest/gtest.h>

#include "GildedRose.h"

std::ostream& operator<<(std::ostream& os, const Item& obj)
{
    return os
        << "name: " << obj.name
        << ", sellIn: " << obj.sellIn
        << ", quality: " << obj.quality;
}

TEST(GildedRoseApprovals, TestOneValue)
{
    vector<Item> items;
    items.push_back(Item("Foo", 0, 0));
    GildedRose app(items);
    app.updateQuality();
    Approvals::verify(app.items[0]);
}

/* Things to note
 * 1. If this is run without adding a main() for Approvals, it will give:
 *     unknown file: error: SEH exception with code 0xc0000005 thrown in the test body.
 * 2. The "approved" file will contain this output: Note how it easily allows us to test 
 *    the Item's whole content, rather than needing one test per value.
 *     name: Foo, sellIn: -1, quality: 0
 *    i.e. the actual calculation result (new quality) is now tested
 * 3. If we are testing a class that has its own output stream operator, Approvals
 *    would use that automatically. As Item doesn't have one, we provide
 *    an implementation here in the testsuite.
 * 3. Still only 52% coverage of GildedRose.cpp
 */
