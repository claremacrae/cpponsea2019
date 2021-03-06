#include <ApprovalTests.hpp>
#include <gtest/gtest.h>

#include "GildedRose.h"

using namespace std;

ostream& operator<<(ostream& os, const Item& obj)
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
    Approvals::verify(app.items[0]); // 52% line coverage
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

TEST(GildedRoseApprovals, TestMultipleValues)
{
    vector<Item> items;
    items.push_back(Item("Foo", 0, 0));
    items.push_back(Item("Aged Brie", 0, 0));
    GildedRose app(items);
    app.updateQuality();
    Approvals::verifyAll(app.items); // 67% line coverage
}

/* Things to note
 * 1. By using Approvals::verifyAll, it is easy to add extra values to test
 * 2. Test coverage up to 67% on GildedRose.cpp
 * 3. The test output now looks like this, showing the indices of the objects
 *    after they have been updated. They don't show the initial values though.

[0] = name: Foo, sellIn: -1, quality: 0
[1] = name: Aged Brie, sellIn: -1, quality: 2

 */

// Helper function to reduce boilerplate code
Item getUpdatedItem(string name, int sellIn, int quality)
{
    vector<Item> items;
    items.push_back(Item(name, sellIn, quality));
    GildedRose app(items);

    app.updateQuality();

    return app.items[0];
}

TEST(GildedRoseApprovalTests, VerifyCombinations)
{
    vector<string> names { "Foo", "Aged Brie" };
    vector<int> sellIns { 0 };
    vector<int> qualities { 0 };

    CombinationApprovals::verifyAllCombinations<
        vector<string>, vector<int>, vector<int>, Item>(
            [](string name, int sellIn, int quality) {
                return getUpdatedItem(name, sellIn, quality);
            },
            names, sellIns, qualities); // 67% line coverage
}

/* Things to note
 * 1. By using Approvals::verifyAllCombinations, it is easy to add many 
 *    extra values to test.
 * 2. Test coverage still 67% on GildedRose.cpp, as inputs are unchanged
 * 3. The test output now looks like this, showing the indices of the objects
 *    after they have been updated. They don't show the initial values though.

(Foo, 0, 0) => name: Foo, sellIn: -1, quality: 0
(Aged Brie, 0, 0) => name: Aged Brie, sellIn: -1, quality: 2

 */

TEST(GildedRoseApprovalTests, VerifyMoreCombinations)
{
    vector<string> names { "Foo", "Aged Brie", "Backstage passes to a TAFKAL80ETC concert" };
    vector<int> sellIns { 0 };
    vector<int> qualities { 0, 1, 2 };

    CombinationApprovals::verifyAllCombinations<
        vector<string>, vector<int>, vector<int>, Item>(
            [](string name, int sellIn, int quality) {
                return getUpdatedItem(name, sellIn, quality);
            },
            names, sellIns, qualities); // 100% line coverage, 83% branch coverage
}

/* Things to note
 * 1. Test coverage at 100% on GildedRose.cpp, according to OpenCppCoverage
 * 2. But there are many values in the source code that are not mentioned
 *    in our tests. We haven't covered all the unwritten 'else' cases.
 * 3. So branch coverage is less than 100%.
 * 4. After I added dummy else blocks to all the if statements in
 *    GildedRose.cpp, coverage was 83%.
 */

TEST(GildedRoseApprovalTests, VerifyEvenMoreCombinations)
{
    vector<string> names { "Foo", "Aged Brie", "Backstage passes to a TAFKAL80ETC concert",
                                "Sulfuras, Hand of Ragnaros"};
    vector<int> sellIns { -1, 0, 11 };
    vector<int> qualities { 0, 1, 2, 49, 50 };

    CombinationApprovals::verifyAllCombinations<
        vector<string>, vector<int>, vector<int>, Item>(
            [](string name, int sellIn, int quality) {
                return getUpdatedItem(name, sellIn, quality);
            },
            names, sellIns, qualities); // 100% line coverage, 100% branch coverage
}

/* Things to note
 * 1. Test coverage at 100% on GildedRose.cpp, according to OpenCppCoverage,
 *    after adding else blocks in that file, to test more boolean combinations
 * 2. So branch coverage is now 100%.
 * 3. We would be able to refactor pretty confidently.
 */

TEST(GildedRoseApprovalTests, VerifyEvenMoreCombinationsAfterMutation)
{
    vector<string> names { "Foo", "Aged Brie", "Backstage passes to a TAFKAL80ETC concert",
                                "Sulfuras, Hand of Ragnaros"};
    vector<int> sellIns { -1, 0, 2, 6, 11 };
    vector<int> qualities { 0, 1, 2, 49, 50 };

    CombinationApprovals::verifyAllCombinations<
        vector<string>, vector<int>, vector<int>, Item>(
            [](string name, int sellIn, int quality) {
                return getUpdatedItem(name, sellIn, quality);
            },
            names, sellIns, qualities); // 100% line coverage, 100% branch coverage
}

/* Things to note
 * 1. Using manual mutation testing, found two more cases that were not
 *    detected, needing extra tests
 * 2. We would be able to refactor even more confidently.
 */
