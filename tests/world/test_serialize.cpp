#include <catch/catch.hpp>

#include <world/core.h>

using namespace world;


TEST_CASE("WorldFile", "[serialize]") {
    SECTION("Test add and read") {}
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