#include <catch/catch.hpp>

#include <world/core.h>

using namespace world;


TEST_CASE("WorldFile", "[serialize]") {

    WorldFile wf;
    wf.addString("1", "1s");
    wf.addInt("2", 1);
    wf.addFloating("3", 3.2);
    wf.addBool("4", true);

    WorldFile a1, a2, a3, a4;

    a1.addString("0", "0s");
    a2.addString("0", "1s");
    // wf.addArray("5", std::vector<WorldFile>{std::move(a1), std::move(a2)});

    a3.addString("0", "2s");
    a4.addString("0", "3s");
    wf.addToArray("6", a3);
    wf.addToArray("6", a4);

    WorldFile c1;

    SECTION("Test read") {
        CHECK(wf.readString("1") == "1s");
        CHECK(wf.readInt("2") == 1);
        CHECK(wf.readFloat("3") == Approx(3.2));
        CHECK(wf.readDouble("3") == Approx(3.2));
        CHECK(wf.readBool("4"));

        CHECK_THROWS(wf.readString("2"));
        CHECK_THROWS(wf.readFloat("2"));
        CHECK_THROWS(wf.readInt("3"));
        CHECK_THROWS(wf.readString("100"));
    }

    SECTION("Test jsonify") {
        WorldFile wf2;
        wf2.fromJson(wf.toJson());

        CHECK(wf2.readString("1") == "1s");
        CHECK(wf2.readInt("2") == 1);
        CHECK(wf2.readFloat("3") == Approx(3.2));
        CHECK(wf2.readBool("4"));
    }

    SECTION("Test read and save") {
        world::createDirectories("unittests");

        std::string filename("unittests/wf_test.json");
        wf.write(filename);

        WorldFile wf2;
        wf2.read(filename);

        CHECK(wf2.readString("1") == "1s");
        CHECK(wf2.readInt("2") == 1);
        CHECK(wf2.readFloat("3") == Approx(3.2));
        CHECK(wf2.readBool("4"));
    }

    SECTION("Move ctor") {
        WorldFile wfc(std::move(wf));

        CHECK(wfc.readBool("4"));
    }
}

class BaseClass1 : public ISerializable {
    WORLD_REGISTER_BASE_CLASS(BaseClass1)
};

class ChildClass1 : public BaseClass1 {};

class ChildClass2 : public BaseClass1 {};

WORLD_REGISTER_CHILD_CLASS(BaseClass1, ChildClass1, "1")
WORLD_REGISTER_CHILD_CLASS(BaseClass1, ChildClass2, "2")

TEST_CASE("ISerializable", "[serialize]") {
    SECTION("Polymorphism") {

        WorldFile wf;
        ChildClass1 c1;
        c1.write(wf);

        BaseClass1 *r = BaseClass1::readSubclass(wf);
        CHECK(dynamic_cast<ChildClass1 *>(r) != nullptr);
        CHECK(dynamic_cast<ChildClass2 *>(r) == nullptr);
    }
}