#pragma once
#include "world/core/WorldConfig.h"

#include <armadillo/armadillo>

#include "world/core/WorldTypes.h"

namespace world {
class PImage;
class Image;

enum class WORLDAPI_EXPORT ImageType { RGB = 0, RGBA = 1, GREYSCALE = 2 };

class WORLDAPI_EXPORT ImageStream {
public:
    ImageStream(const Image &image);

    int remaining();

    int read(char *buffer, int count);

private:
    int _position = 0;
    const Image &_image;
};

class WORLDAPI_EXPORT GreyPixel {
public:
    /** Sets the pixel to grey with given intensity. */
    void setLevel(u8 l);

    /** Sets the pixel to grey with given intensity. */
    void setLevelf(double l);

    u8 getLevel() const;

    double getLevelf(double l);

private:
    u8 _g;
};

struct WORLDAPI_EXPORT RGBPixel {
public:
    u8 getRed() const;
    u8 getGreen() const;
    u8 getBlue() const;

    double getRedf() const;
    double getGreenf() const;
    double getBluef() const;

    void setRed(u8 r);
    void setGreen(u8 g);
    void setBlue(u8 b);

    void setRedf(double r);
    void setGreenf(double g);
    void setBluef(double b);

    void set(u8 r, u8 g, u8 b);
    void setf(double r, double g, double b);

protected:
    u8 _r, _g, _b;
};

struct WORLDAPI_EXPORT RGBAPixel : public RGBPixel {
public:
    u8 getAlpha() const;

    double getAlphaf() const;

    void setAlpha(u8 a);

    void setAlphaf(double a);

    void set(u8 r, u8 g, u8 b, u8 a = 255);
    void setf(double r, double g, double b, double a = 1);

private:
    u8 _a;
};

class WORLDAPI_EXPORT Image {
public:
    Image(int width, int height, const ImageType &type);
    Image(const arma::Cube<double> &data);
    Image(const arma::Mat<double> &data);
    Image(const std::string &filename);
    Image(const char *filename);
    // move constructor
    Image(Image &&img);
    // copy constructor
    Image(const Image &img);
    // destructor
    ~Image();

    // copy assignment
    Image &operator=(const Image &img);
    // move assignment
    Image &operator=(Image &&img);

    // infos
    ImageType type() const;
    int elemSize() const;
    int width() const;
    int height() const;
    /// Get the total size of the image (width * height * elemSize)
    int size() const;

    // access
    /** Gets a rgba access on the pixel at (x, y). This
     * method only works properly on RGBA image.
     * @param x column of the pixel.
     * @param y line of the pixel. */
    RGBAPixel &rgba(int x, int y);

    /** Gets a rgba access on the pixel at (x, y) (const
     * version). This method only works properly on RGBA
     * image.
     * @param x column of the pixel.
     * @param y line of the pixel. */
    const RGBAPixel &rgba(int x, int y) const;

    /** Gets a rgb access on the pixel at (x, y). This
     * method works on both types RGBA and RGB.
     * @warning behaviour on RGBA type is not confirmed yet.
     * @param x column of the pixel.
     * @param y line of the pixel. */
    RGBPixel &rgb(int x, int y);

    /** Gets a rgb access on the pixel at (x, y) (const version).
     * This method works on both types RGBA and RGB.
     * @warning behaviour on RGBA type is not confirmed yet.
     * @param x column of the pixel.
     * @param y line of the pixel. */
    const RGBPixel &rgb(int x, int y) const;

    /** Gets a greyscale access on the pixel at (x, y). The
     * behaviour on images other than greyscale type is
     * undefined.
     * @param x column of the pixel.
     * @param y line of the pixel. */
    GreyPixel &grey(int x, int y);

    /** Gets a greyscale access on the pixel at (x, y) (const
     * version). The behaviour on images other than greyscale
     * type is undefined.
     * @param x column of the pixel.
     * @param y line of the pixel. */
    const GreyPixel &grey(int x, int y) const;

    /** Set the value of the pixel at (x, y). The number of elements
     * copied from the array depends on the type of the image.
     * @param values a float array holding the values to be set. */
    void setf(int x, int y, const float *values);

    /** Get the value of the pixel at (x, y). The number of elements
     * copied to the array depends on the type of the image.
     * @param values pixel data are copied to this array. */
    void getf(int x, int y, float *values) const;

    // IO
    /** Writes the image at the specified location. The
     * extension of the file is used to determine the format
     * of the written image.
     * @param file a relative or absolute pathname to
     * the file.*/
    void write(const std::string &file) const;

private:
    PImage *_internal;

    ImageType _type;

    friend class ImageStream;
};
} // namespace world
