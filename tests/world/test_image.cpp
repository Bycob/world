#include <catch/catch.hpp>

#include <world/core.h>

using namespace world;

TEST_CASE("Image - general test case", "[image]") {
    Image img(16, 16, ImageType::RGBA);

    SECTION("dimensions") {
        REQUIRE(img.width() == 16);
        REQUIRE(img.height() == 16);
    }

    SECTION("type") { REQUIRE(img.type() == ImageType::RGBA); }

    SECTION("set and get color - u8") {
        img.rgba(0, 0).set(52, 65, 128, 254);
        auto &pix = img.rgba(0, 0);
        REQUIRE(pix.getRed() == 52);
        REQUIRE(pix.getGreen() == 65);
        REQUIRE(pix.getBlue() == 128);
        REQUIRE(pix.getAlpha() == 254);
    }

    const double eps = 1 / 255.0;

    SECTION("set and get color - double") {
        img.rgba(1, 0).setf(0.3, 0.5, 0.7, 0.9);
        auto &pix = img.rgba(1, 0);
        REQUIRE(abs(pix.getRedf() - 0.3) < eps);
        REQUIRE(abs(pix.getGreenf() - 0.5) < eps);
        REQUIRE(abs(pix.getBluef() - 0.7) < eps);
        REQUIRE(abs(pix.getAlphaf() - 0.9) < eps);
    }

    SECTION("set and get color - set u8, get double") {
        img.rgba(1, 1).set(63, 127, 191, 229);
        auto &pix = img.rgba(1, 1);
        REQUIRE(abs(pix.getRedf() - 0.25) < eps);
        REQUIRE(abs(pix.getGreenf() - 0.5) < eps);
        REQUIRE(abs(pix.getBluef() - 0.75) < eps);
        REQUIRE(abs(pix.getAlphaf() - 0.9) < eps);
    }

    SECTION("set and get color - set double, get u8") {
        img.rgba(1, 0).setf(0.25, 0.5, 0.75, 0.9);
        auto &pix = img.rgba(1, 0);
        REQUIRE(pix.getRed() == 63);
        REQUIRE(pix.getGreen() == 127);
        REQUIRE(pix.getBlue() == 191);
        REQUIRE(pix.getAlpha() == 229);
    }

    Image greyscale(16, 16, ImageType::GREYSCALE);

    // To be continued
}

TEST_CASE("Image - armadillo interoperability", "[image]") {
    Image refGrey(4, 4, ImageType::GREYSCALE);
    refGrey.grey(0, 1).setLevel(255);
    refGrey.grey(1, 0).setLevel(127);

    SECTION("creating image from arma::mat") {
        arma::mat matrix(4, 4, arma::fill::zeros);
        matrix(0, 1) = 1;
        matrix(1, 0) = .5;
        Image img(matrix);
        REQUIRE(img.type() == ImageType::GREYSCALE);
        REQUIRE(img.grey(0, 1).getLevel() == refGrey.grey(0, 1).getLevel());
        REQUIRE(img.grey(1, 0).getLevel() == refGrey.grey(1, 0).getLevel());
    }

    Image refRGB(4, 4, ImageType::RGB);
    refRGB.rgb(0, 1).set(255, 127, 255);
    refRGB.rgb(1, 0).set(127, 255, 127);

    SECTION("creating image from arma::cube") {
        arma::cube cube(4, 4, 4);
        cube(0, 1, 0) = 1;
        cube(0, 1, 1) = .5;
        cube(0, 1, 2) = 1;
        cube(1, 0, 0) = .5;
        cube(1, 0, 1) = 1;
        cube(1, 0, 2) = .5;
        Image img(cube);
        REQUIRE(img.type() == ImageType::RGB);
        REQUIRE(img.rgb(0, 1).getRed() == refRGB.rgb(0, 1).getRed());
        REQUIRE(img.rgb(0, 1).getGreen() == refRGB.rgb(0, 1).getGreen());
        REQUIRE(img.rgb(0, 1).getBlue() == refRGB.rgb(0, 1).getBlue());
        REQUIRE(img.rgb(1, 0).getRed() == refRGB.rgb(1, 0).getRed());
        REQUIRE(img.rgb(1, 0).getGreen() == refRGB.rgb(1, 0).getGreen());
        REQUIRE(img.rgb(1, 0).getBlue() == refRGB.rgb(1, 0).getBlue());
    }
}

TEST_CASE("Image - IO", "[image]") {
    REQUIRE_THROWS(Image("unittests/img_not_found.png"));

    // Image img("unittests/test.png");
}

TEST_CASE("Image - ImageStream", "[image]") {
    Image imgRGBA(3, 3, ImageType::RGBA);

    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 3; ++x) {
            imgRGBA.rgba(x, y).set((u8)x, (u8)y, 1);
        }
    }

    ImageStream stream(imgRGBA);

    SECTION("Check stream size for ARGB stream") {
        REQUIRE(stream.remaining() == 3 * 3 * 4);
    }

    // Read the stream
    char *buffer = new char[stream.remaining()];
    stream.read(buffer, stream.remaining());

    SECTION("Check that data are in row-order format, and ARGB") {
        for (int y = 0; y < 3; ++y) {
            for (int x = 0; x < 3; ++x) {
                INFO(std::string("reading ") + std::to_string(x) + ", " +
                     std::to_string(y));
                // row major order, argb stream
                auto ptr = buffer + (y * 3 + x) * 4;
                REQUIRE((u8)ptr[0] == (u8)255);
                REQUIRE((u8)ptr[1] == (u8)x);
                REQUIRE((u8)ptr[2] == (u8)y);
                REQUIRE((u8)ptr[3] == (u8)1);
            }
        }
    }

    delete[] buffer;
}

TEST_CASE("Image - Benchmarks", "[image][!benchmark]") {

    Image imgRGBA(1024, 1024, ImageType::RGBA);
    Image imgGrey(1024, 1024, ImageType::GREYSCALE);

    SECTION("Access") {
        BENCHMARK("set all pixels RGBA (1024 * 1024)") {
            for (int y = 0; y < 1024; y++) {
                for (int x = 0; x < 1024; x++) {
                    imgRGBA.rgba(x, y).set(2, 2, 2, 255);
                }
            }
        }

        BENCHMARK("set all pixels Greyscale (1024 * 1024)") {
            for (int y = 0; y < 1024; y++) {
                for (int x = 0; x < 1024; x++) {
                    imgGrey.grey(x, y).setLevel(2);
                }
            }
        }
    }
}