#include <catch/catch.hpp>

#include <worldcore.h>

using namespace world;

TEST_CASE("Image - general test case", "[image]") {
    Image img(16, 16, ImageType::RGBA);

    SECTION("dimensions") {
        REQUIRE(img.width() == 16);
        REQUIRE(img.height() == 16);
    }

    SECTION("type") {
        REQUIRE(img.type() == ImageType::RGBA);
    }

    SECTION("set and get color - u8") {
        img.at(0, 0).set(52, 65, 128, 254);
        auto pix = img.at(0, 0);
        REQUIRE(pix.getRed() == 52);
        REQUIRE(pix.getGreen() == 65);
        REQUIRE(pix.getBlue() == 128);
        REQUIRE(pix.getAlpha() == 254);
    }

    const double eps = 1 / 255.0;

    SECTION("set and get color - double") {
        img.at(1, 0).setf(0.3, 0.5, 0.7, 0.9);
        auto pix = img.at(1, 0);
        REQUIRE(abs(pix.getRedf() - 0.3) < eps);
        REQUIRE(abs(pix.getGreenf() - 0.5) < eps);
        REQUIRE(abs(pix.getBluef() - 0.7) < eps);
        REQUIRE(abs(pix.getAlphaf() - 0.9) < eps);
    }

    SECTION("set and get color - set u8, get double") {
        img.at(1, 1).set(63, 127, 191, 229);
        auto pix = img.at(1, 1);
        REQUIRE(abs(pix.getRedf() - 0.25) < eps);
        REQUIRE(abs(pix.getGreenf() - 0.5) < eps);
        REQUIRE(abs(pix.getBluef() - 0.75) < eps);
        REQUIRE(abs(pix.getAlphaf() - 0.9) < eps);
    }

    SECTION("set and get color - set double, get u8") {
        img.at(1, 0).setf(0.25, 0.5, 0.75, 0.9);
        auto pix = img.at(1, 0);
        REQUIRE(pix.getRed() == 63);
        REQUIRE(pix.getGreen() == 127);
        REQUIRE(pix.getBlue() == 191);
        REQUIRE(pix.getAlpha() == 229);
    }

    Image greyscale(16, 16, ImageType::GREYSCALE);

    // To be continued
}

TEST_CASE("Image - IO", "[image]") {
    REQUIRE_THROWS(Image("img_not_found.png"));

    //Image img("test.png");
}

TEST_CASE("Image - Benchmarks", "[image][!benchmark]") {

    Image imgRGBA(1024, 1024, ImageType::RGBA);
    Image imgGrey(1024, 1024, ImageType::GREYSCALE);

    SECTION("Access") {
        BENCHMARK("set all pixels RGBA (1024 * 1024)") {
            for (int x = 0; x < 1024; x++) {
                for (int y = 0; y < 1024; y++) {
                    imgRGBA.at(x, y).set(2, 2, 2, 255);
                }
            }
        }

        BENCHMARK("set all pixels Greyscale (1024 * 1024)") {
            for (int x = 0; x < 1024; x++) {
                for (int y = 0; y < 1024; y++) {
                    imgGrey.at(x, y).set(2);
                }
            }
        }
    }
}