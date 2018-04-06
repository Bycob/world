#pragma once
#include "core/WorldConfig.h"

#include <armadillo/armadillo>

#include "core/WorldTypes.h"

namespace world {
	class PrivateImage;
	class Image;

	enum class WORLDAPI_EXPORT ImageType {
		RGB = 0,
		RGBA = 1,
		GREYSCALE = 2
	};

	class WORLDAPI_EXPORT ImageStream {
	public:
		ImageStream(const Image &image);

		int remaining();

		int read(char* buffer, int count);
	private:
		int _position = 0;
		const Image &_image;
	};

	class WORLDAPI_EXPORT ConstPixel {
	public:
		u8 getAlpha() const;
		u8 getRed() const;
		u8 getGreen() const;
		u8 getBlue() const;

        double getAlphaf() const;
        double getRedf() const;
        double getGreenf() const;
        double getBluef() const;

	protected:
		friend class Image;

		ConstPixel(int width, int height, Image *ref);

		u8 getComponent(int id) const;

		int _x; int _y;
		Image * _ref;
	};

	class WORLDAPI_EXPORT Pixel : public ConstPixel {
	public:
		void set(u8 r, u8 g = 0, u8 b = 0, u8 a = 255);
		void setf(double r, double g = 0, double b = 0, double a = 1);

		void setAlpha(u8 a);
		void setRed(u8 r);
		void setGreen(u8 g);
		void setBlue(u8 b);

		void setAlphaf(double a);
		void setRedf(double r);
		void setGreenf(double g);
		void setBluef(double b);

        /** Sets the pixel to grey with given intensity. */
		void setLevel(u8 l);

        /** Sets the pixel to grey with given intensity. */
		void setLevelf(double l);

	private:
		friend class Image;

		Pixel(int width, int height, Image *ref);

		void setComponent(int id, u8 type);
	};

	class WORLDAPI_EXPORT Image {
	public:
		Image(int width, int height, const ImageType &type);
		Image(const arma::Cube<double> & data);
		Image(const arma::Mat<double> & data);
		Image(const std::string & filename);
		Image(const char *filename);
		// move constructor
		Image(Image && img);
		// copy constructor
		Image(const Image & img);
		~Image();

		// infos
		ImageType type() const;
		int width() const;
		int height() const;

		// access
		Pixel at(int x, int y);
		const ConstPixel at(int x, int y) const;

		// IO
		void write(const std::string &file) const;
	private:
		ImageType _type;

		PrivateImage *_private;
		
		friend class Pixel;
		friend class ConstPixel;
		friend class ImageStream;
	};
}

