#include <catch/catch.hpp>

#include <worldcore.h>

using namespace world;

TEST_CASE("Test StringOps.h", "[utilities]") {

    SECTION("split") {
        std::string fullstr("I,,am,splitted,by,comas,");
        REQUIRE(split(fullstr, ',') == std::vector<std::string>{"I", "", "am", "splitted", "by", "comas", ""});
        REQUIRE(split(fullstr, ',', true) == std::vector<std::string>{"I", "am", "splitted", "by", "comas"});
        REQUIRE(split("a,,b", ',') == std::vector<std::string>{"a", "", "b"});
    }

    SECTION("trimSpaces") {
        REQUIRE(trimSpaces("  \t trim spaces ! \t\t") == "trim spaces !");
        REQUIRE(trimSpaces("\n \ntrim spaces\n") == "\n \ntrim spaces\n");
    }

    SECTION("startsWith") {
        REQUIRE(startsWith("bbaabba", "bba"));
        REQUIRE_FALSE(startsWith("babaabba", "bba"));
        REQUIRE_FALSE(startsWith("baba", "bababa"));
        REQUIRE(startsWith("a", ""));
        REQUIRE(startsWith(".png", ".png"));
    }

    SECTION("endsWith") {
        REQUIRE(endsWith("extension.png.png", ".png"));
        REQUIRE_FALSE(endsWith("extension.png", ".png.png"));
        REQUIRE_FALSE(endsWith(".png", ".png.png"));
        REQUIRE(endsWith("a", ""));
        REQUIRE(endsWith(".png", ".png"));
    }
}
